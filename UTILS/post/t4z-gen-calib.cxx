/* t4z-gen.cxx
 *
 * Authors: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 *          Katharina von Sturm - vonsturm@pd.infn.it
 * Created: 24 Jan 2018
 *
 */
// stl
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <ctime>

// jsoncpp
#include "json/json.h"

// cern-root
#include "TFile.h"
#include "TChain.h"
#include "TParameter.h"

// gerda-ada
#include "gerda-ada/T4SimHandler.h"
#include "gerda-ada/T4SimConfig.h"

enum logLevel {debug, info, warning, error};
std::ostream& glog(logLevel lvl);

int main(int argc, char** argv) {

    // utilities
    auto c_str = [&](std::string s){return s.c_str();};
    auto usage = [](){ std::cout << "Tier4ize calibration simulations.\n"
                                 << "USAGE: ./t4z-gen [OPTIONS] [DIR-WITH-RAW]\n\n"
                                 << "OPTIONS:\n"
                                 << "  required: --metadata <gerda-metadata-location>\n"
                                 << "            --srcdir <gerda-mage-sim-dir>\n"
                                 << "            --destdir <destination-dir-post-processed>\n"
                                 << "  optional: -v : verbose mode\n\n"
                                 << "NOTES: Please use absolute paths!"
                                 << std::endl;};

    // get & check arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 8) {usage(); return 1;}
    std::string gerdaMetaPath;
    auto result = std::find(args.begin(), args.end(), "--metadata");
    if (result != args.end()) gerdaMetaPath = *(result+1);
    else {usage(); return 1;}
    std::string destDirPath;
    result = std::find(args.begin(), args.end(), "--destdir");
    if (result != args.end()) destDirPath = *(result+1);
    else {usage(); return 1;}
    std::string gms_path;
    result = std::find(args.begin(), args.end(), "--srcdir");
    if (result != args.end()) gms_path = *(result+1);
    else {usage(); return 1;}
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    auto dirWithRaw = *(args.end()-1);

    setenv("MU_CAL", (gerdaMetaPath + "/config/_aux/geruncfg").c_str(), 1);

    // strip off trailing '/' character, if present
    if (gerdaMetaPath.back() == '/') gerdaMetaPath.pop_back();
    if (verbose) glog(debug) << "gerda-metadata: " << gerdaMetaPath << std::endl;
    if (destDirPath.back() == '/') destDirPath.pop_back();
    if (verbose) glog(debug) << "destination: " << destDirPath << std::endl;
    if (gms_path.back() == '/')  gms_path.pop_back();
    if (verbose) glog(debug) << "gerda-mage-sim: " << gms_path << std::endl;
    if (dirWithRaw.back() == '/') dirWithRaw.pop_back();
    if (verbose) glog(debug) << "raw- files location: " << dirWithRaw << std::endl;

    // strip out folders in dir to build up final t4z- filename
    std::vector<std::string> items;
    std::string dircopy = dirWithRaw;
    for (int j = 0; j < 4; j++ ) {
        items.push_back(dircopy.substr(dircopy.find_last_of('/')+1));
        dircopy.erase(dircopy.find_last_of('/'), dircopy.back());
    }

    if (items[3] != "calib") { glog(error) << "not a valid 'calib' directory, aborting...\n"; return 1; }

    // get raw-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { glog(error) << "invalid or empty raw- directory path!\n"; return filelist; }
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 && r; ) {
            // this means "is a regular file", DT_REG = 8
            if (entry.d_type == 8 &&
                std::string(entry.d_name).find("raw-") != std::string::npos &&
                std::string(entry.d_name).find(".root") != std::string::npos) {
                filelist.push_back(foldName + "/" + std::string(entry.d_name));
            }
        }
        std::sort(filelist.begin(), filelist.end());
        if (verbose) { glog(debug) << "sorted raw- files:\n"; for ( const auto & f : filelist ) glog(debug) << "  " << f << std::endl; }

        return filelist;
    };

    // join all raw- files in same tree and get number of primaries
    auto filelist = GetContent(dirWithRaw);
    if (filelist.empty()) { glog(error) << "there were problems reading in the raw- files. Aborting...\n"; return 1; }
    TChain ch("fTree");
    bool problems = false;
    long totPrimaries = 0;
    for ( auto& f : filelist ) {
        TFile file(f.c_str());
        // check first if the object exists
        if (file.GetListOfKeys()->Contains("NumberOfPrimaries")) {
            auto primaries = dynamic_cast<TParameter<long>*>(file.Get("NumberOfPrimaries"))->GetVal();
            if (primaries == 0) {
                glog(warning) << "NumberOfPrimaries is zero in " << f << "!\n";
                problems = true;
            }
            totPrimaries += primaries;
        }
        else {
            glog(warning) << "NumberOfPrimaries not found in " << f << "! It will be set to zero!\n";
            problems = true;
        }
        file.Close();
        ch.Add(f.c_str());
    }
    // if at least one object does not exist set primaries to zero to indicate a failure
    if (problems) {
        glog(warning) << "there were some problems, setting totPrimaries = 0...\n";
        totPrimaries = 0;
    }

    // read json file with calibs configs
    Json::Value calibs;
    std::ifstream fcalibcfg(gms_path + "/UTILS/post/settings/calib-pdf-settings.json");
    if (!fcalibcfg.is_open()) {glog(error) << "invalid calib config file! Aborting...\n"; return 1;}
    fcalibcfg >> calibs;
    auto& cfg = calibs["calib"];

    // check if we're asked to do something
    auto& isotope = items[1];
    if (verbose) glog(debug) << "isotope: " << isotope << std::endl;
    auto part = items[2];
    if (verbose) glog(debug) << "part: " << part << std::endl;
    auto type_str = part.substr(0, part.find_first_of("_"));
    bool is_multi = false;
    if      (type_str == "multi" ) is_multi = true;
    else if (type_str == "single" ) is_multi = false;
    else {
        glog(error) << "could not determine if simulation is 'multi' or 'single', please check the folder name\n";
        return 1;
    }
    if (verbose) glog(debug) << "is multi: " << (is_multi ? "yes" : "no") << std::endl;
    part.erase(0, type_str.size()+1);
    if (part.size() != 7) {
        glog(error) << "incorrect source_position name, not in the form sX_XXXX, please check the folder name\n";
        return 1;
    }
    auto source = part.substr(0, 2);
    auto pos = part.substr(3);
    if (source != "s1" and source != "s2" and source != "s3") {
        glog(error) << "could not determine source number, please check the folder name\n";
        return 1;
    }
    if (verbose) glog(debug) << "source: " << source << std::endl;
    if (pos.size() != 4) {
        glog(error) << "could not determine source position (4 digit number), please check the folder name\n";
        return 1;
    }
    if (verbose) glog(debug) << "Position: " << pos << std::endl;

    bool found = false;

    if (cfg[isotope][source][pos][type_str + "-mode"]) {
        if (cfg[isotope][source][pos][type_str + "-mode"].asBool() == true) {
            cfg = cfg[isotope][source][pos];
            found = true;
        }
    }
    // try Th228 if Bi212/Tl208
    else if (isotope == "Bi212" or isotope == "Tl208") {
        if (cfg["Th228"][source][pos][type_str + "-mode"]) {
            if (cfg["Th228"][source][pos][type_str + "-mode"].asBool() == true) {
                cfg = cfg["Th228"][source][pos];
                found = true;
            }
        }
    }

    if (!found) {
        glog(error) << "could not find any entry for " << items[2] << "/" << items[1]
                  << " in the JSON file. Exiting gracefully...\n";
        return 1;
    }

    // build output filename
    auto& it = items;
    auto filedir = destDirPath + '/' + it[3] + '/' + it[2] + '/' + it[1] + '/' + it[0];
    auto filename = filedir + '/' + "t4z-" + it[3] + '-' + it[2] + '-' + it[1] + '-' + it[0];
    if (cfg["id"] and cfg["id"].asString() != "") filename += "-" + cfg["id"].asString();
    filename += ".root";
    if (verbose) {
        glog(debug) << "post-production folder: " << destDirPath << std::endl;
        glog(debug) << "tz4- file name: " << filename << std::endl;
    }

    // look for the keylist file
    if (!cfg["keylist"]) { glog(error) << "keylist file not specified, aborting\n"; return 1; }
    auto keylist = cfg["keylist"].asString();
    std::ifstream flist;  flist.open(gerdaMetaPath + "/data-sets/cal/" + keylist);
    if (!flist.is_open()) flist.open(gerdaMetaPath + "/data-sets/cal/" + keylist + ".txt");
    if (!flist.is_open()) flist.open(gerdaMetaPath + "/data-sets/pca/" + keylist);
    if (!flist.is_open()) flist.open(gerdaMetaPath + "/data-sets/pca/" + keylist + ".txt");
    if (!flist.is_open()) {
        glog(error) << "could not find keylist " << keylist << "in gerda-metadata\n";
        return 1;
    }

    // get timestamp range
    std::string start, stop;
    flist >> start;
    while (flist >> stop) { continue; }
    start.erase(start.find_last_of('-'), start.back());
    start = start.substr(start.find_last_of('-')+1);
    auto startkey = start;
    // start.pop_back();
    // start.erase(8,1);
    // auto tstart = std::stoul(start);
    // stop.erase(stop.find_last_of('-'), stop.back());
    // stop = stop.substr(stop.find_last_of('-')+1);
    // stop.pop_back();
    // stop.erase(8,1);
    // auto tstop = std::stoul(stop);
    // if (verbose) glog(debug) << "timestamp range: " << tstart << " -> " << tstop << std::endl;

    /*
    // reorganize gerda-calibrations.jsonl...
    std::ifstream fgcal(gerdaMetaPath + "/calib/gerda-calibrations.jsonl");
    if (!fgcal.is_open()) { glog(debug) << "Could not find gerda-calibrations.jsonl! Aborting...\n"; return 1; }
    std::map<unsigned long,std::string> gcal_map;

    Json::Value gcal;
    for (std::string line; std::getline(fgcal, line); ) {
        std::stringstream stream(line);
        stream >> gcal;
        auto _stamp = gcal["valid"]["ged"].asString();
        _stamp.pop_back();
        _stamp.erase(8,1);
        gcal_map.emplace(std::stoul(_stamp), gcal["key"].asString());
    }

    // get right calibration curves
    std::string calib_file = "";
    for (auto& p : gcal_map) {
        if (p.first <= tstart) {
            auto run = p.second.substr(6,7);
            calib_file = gerdaMetaPath + "/calib/" + run + "/" + p.second + "-cal-ged-tier3-calib.json";
        }
    }
    if (calib_file.empty()) {
        glog(error) << "Could not identify calib file for time " + std::to_string(tstart) + " in gerda-calibrations.jsonl\n";
        return 1;
    }
    */
    // if (verbose) glog(debug) << "found calib file: " << calib_file << std::endl;

    // make destdir
    std::system(c_str("mkdir -p " + filedir));

    // set up tier4izer
    gada::T4SimConfig config;

    auto calib_file        = gms_path + "/UTILS/post/settings/ged-resolution.json";
    auto mapping_file      = gms_path + "/UTILS/post/settings/mapping.json";
    auto ged_settings_file = gms_path + "/UTILS/post/settings/ged-settings.json";
    auto heat_map          = gms_path + "/UTILS/post/settings/gerda-larmap.root";

    config.LoadMapping(mapping_file);
    if(verbose) glog(debug) << "channel mapping " << mapping_file << " loaded" << std::endl;

    config.LoadGedSettings(ged_settings_file);
    if(verbose) glog(debug) << "ged threshold settings " << ged_settings_file << " loaded" << std::endl;

    // retrieve and set LAr detection probability map
    TFile _f(heat_map.c_str());
    if (_f.IsZombie()) return 1;
    config.LoadLArMap(heat_map, "LAr_prob_map");
    if(verbose) glog(debug) << "LAr probability map " << heat_map << " loaded" << std::endl;

    config.LoadGedResolutions(calib_file, "Zac");
    if(verbose) glog(debug) << "ged resolution curves " << calib_file << " loaded" << std::endl;

    std::tm tt;
    strptime(startkey.c_str(), "%Y%m%dT%H%M%SZ", &tt);
    config.LoadRunConfig((ULong64_t)mktime(&tt));
    if(verbose) glog(debug) << "RunConfig for timestamp " << mktime(&tt) << " loaded" << std::endl;

    gada::T4SimHandler handler(&ch, &config, filename);

    // run!
    glog(info) << "running production..." << std::endl;
    handler.RunProduction();

    // save number of primaries
    TFile fout(filename.c_str(), "update");
    TParameter<Long64_t> numberOfPrimaries("NumberOfPrimaries", totPrimaries);
    numberOfPrimaries.Write();
    fout.Close();

    glog(info) << "done. " << filename << " created" << std::endl;

    return 0;
}

std::ostream& glog(logLevel lvl) {
    if (lvl == debug) {
        std::cout << "[DEBUG] ";
        return std::cout;
    }
    if (lvl == info) {
        std::cout << "[INFO] ";
        return std::cout;
    }
    if (lvl == warning) {
        std::cerr << "[WARNING] ";
        return std::cerr;
    }
    if (lvl == error) {
        std::cerr << "[ERROR] ";
        return std::cerr;
    }
}
