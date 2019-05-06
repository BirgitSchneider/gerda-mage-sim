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

    auto usage = [](){
        std::cout << "Generate tier4ized files for calibration simulations.\n"
                  << "USAGE: t4z-gen-calib --config <t4z-gen-settings.json> --destdir "
                  << "<gerda-pdfs/cycle> <dir-with-mage-files>\n";
        exit(1);
    };

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);

    if (args.size() != 5) usage();

    std::string configs;
    std::string destdir;

    auto result = std::find(args.begin(), args.end(), "--config");
    if (result != args.end()) configs = *(result+1);
    else usage();

    result = std::find(args.begin(), args.end(), "--destdir");
    if (result != args.end()) destdir = *(result+1);
    else usage();

    auto dir_with_raw = args[args.size()-1];

    Json::Value cfg;
    std::ifstream fconfigs(configs.c_str());
    fconfigs >> cfg;

    auto gms_path            = cfg["gerda-mage-sim"].asString();
    auto gerda_meta          = cfg["gerda-metadata"].asString();
    bool verbose             = cfg.get("debug", false).asBool();
    auto calib_file          = cfg["ged-resolution-curves"].asString();
    auto mapping_file        = cfg["channels-mapping"].asString();
    auto ged_settings_file   = cfg["ged-settings"].asString();
    auto ged_parameters_file = cfg["ged-parameters"].asString();
    auto ged_transition_file = cfg["ged-transition"].asString();
    auto larveto_model       = cfg["LAr-veto-model"].asString();
    auto heat_map            = cfg.get("heat-map", "null").asString();
    auto calib_pdfs_file     = cfg["calib-pdf-settings"].asString();

    if (verbose) glog(debug) << "gerda-mage-sim: " << gms_path << std::endl;
    if (verbose) glog(debug) << "destination: " << destdir << std::endl;
    if (verbose) glog(debug) << "raw- files location: " << dir_with_raw << std::endl;

    if (verbose) glog(debug) << "paths found in JSON config:\n";
    for (auto s : {&gerda_meta, &calib_file, &mapping_file, &ged_settings_file,
                   &ged_parameters_file, &ged_transition_file, &heat_map, &calib_pdfs_file}) {
        if (s->front() != '/') *s = gms_path + "/" + *s;
        if (verbose) glog(debug) << "  " << *s << std::endl;
    }

    if (verbose) glog(debug) << "LAr veto model: " << larveto_model << std::endl;
    if (larveto_model != "calorimetric" and larveto_model != "heat-map") {
        glog(error) << "'LAr-veto-model' should be either 'calorimetric' or 'heat-map'\n";
        return 1;
    }

    if (dir_with_raw.find("chanwise") != std::string::npos) {
        glog(warning) << "these simulations won't be processed because they are separated "
                      << "in channels. This will create problems in PDFs building.\n";
        return 1;
    }

    setenv("MU_CAL", (gerda_meta + "/config/_aux/geruncfg").c_str(), 1);

    // strip out folders in dir to build up final t4z- filename
    std::vector<std::string> items;
    if (dir_with_raw.back() == '/') dir_with_raw.pop_back();
    std::string dircopy = dir_with_raw;
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
    auto filelist = GetContent(dir_with_raw);
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
    std::ifstream fcalibcfg(calib_pdfs_file.c_str());
    if (!fcalibcfg.is_open()) {glog(error) << "invalid calib config file! Aborting...\n"; return 1;}
    fcalibcfg >> calibs;
    auto& ccfg = calibs["calib"];

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

    if (ccfg[isotope][source][pos][type_str + "-mode"]) {
        if (ccfg[isotope][source][pos][type_str + "-mode"].asBool() == true) {
            ccfg = ccfg[isotope][source][pos];
            found = true;
        }
    }
    // try Th228 if Bi212/Tl208
    else if (isotope == "Bi212" or isotope == "Tl208") {
        if (ccfg["Th228"][source][pos][type_str + "-mode"]) {
            if (ccfg["Th228"][source][pos][type_str + "-mode"].asBool() == true) {
                ccfg = ccfg["Th228"][source][pos];
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
    auto filedir = destdir + '/' + it[3] + '/' + it[2] + '/' + it[1] + '/' + it[0];
    auto filename = filedir + '/' + "t4z-" + it[3] + '-' + it[2] + '-' + it[1] + '-' + it[0];
    if (ccfg["id"] and ccfg["id"].asString() != "") filename += "-" + ccfg["id"].asString();
    filename += ".root";
    if (verbose) {
        glog(debug) << "post-production folder: " << destdir << std::endl;
        glog(debug) << "tz4- file name: " << filename << std::endl;
    }

    // look for the keylist file
    if (!ccfg["keylist"]) { glog(error) << "keylist file not specified, aborting\n"; return 1; }
    auto keylist = ccfg["keylist"].asString();
    std::ifstream flist;  flist.open(gerda_meta + "/data-sets/cal/" + keylist);
    if (!flist.is_open()) flist.open(gerda_meta + "/data-sets/cal/" + keylist + ".txt");
    if (!flist.is_open()) flist.open(gerda_meta + "/data-sets/pca/" + keylist);
    if (!flist.is_open()) flist.open(gerda_meta + "/data-sets/pca/" + keylist + ".txt");
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
    std::ifstream fgcal(gerda_meta + "/calib/gerda-calibrations.jsonl");
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
            calib_file = gerda_meta + "/calib/" + run + "/" + p.second + "-cal-ged-tier3-calib.json";
        }
    }
    if (calib_file.empty()) {
        glog(error) << "Could not identify calib file for time " + std::to_string(tstart) + " in gerda-calibrations.jsonl\n";
        return 1;
    }
    */
    // if (verbose) glog(debug) << "found calib file: " << calib_file << std::endl;

    // make destdir
    std::system(("mkdir -p " + filedir).c_str());

    // set up tier4izer
    gada::T4SimConfig config;

    config.LoadMapping(mapping_file);
    if(verbose) glog(debug) << "channel mapping " << mapping_file << " loaded" << std::endl;

    config.LoadGedSettings(ged_settings_file);
    if(verbose) glog(debug) << "ged threshold settings " << ged_settings_file << " loaded" << std::endl;

    // retrieve and set LAr detection probability map
    if (larveto_model == "heat-map") {
        if (heat_map == "null") {
            std::cout << "missing heat map file specification.\n";
            return 1;
        }
        TFile _f(heat_map.c_str());
        if (_f.IsZombie()) return 1;
        config.LoadLArMap(heat_map, "LAr_prob_map");
        if(verbose) glog(debug) << "LAr probability map " << heat_map << " loaded" << std::endl;
    }

    config.LoadGedResolutions(calib_file, "Zac");
    if(verbose) glog(debug) << "ged resolution curves " << calib_file << " loaded" << std::endl;

    config.LoadGedTransitions(ged_transition_file, ged_parameters_file);
    if(verbose) glog(debug) << "ged transition layers " << ged_transition_file << " loaded" << std::endl;

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
