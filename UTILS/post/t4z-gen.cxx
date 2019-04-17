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
#include <string>
#include <memory>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <math.h>

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
    auto usage = [](){ std::cout << "Split simulations in a tier4ized file for each run.\n"
                                 << "USAGE: ./t4z-gen [OPTIONS] [DIR-WITH-RAW]\n\n"
                                 << "OPTIONS:\n"
                                 << "  required: --metadata <gerda-metadata-location>\n"
                                 << "            --srcdir <gerda-mage-sim-dir>\n"
                                 << "            --destdir <destination-dir-post-processed>\n"
                                 << "            --livetime-file <json-file-from-livetime-calc-ph2>\n"
                                 << "            --runlist-file <json-file-with-runlist>\n"
                                 << "  optional: -v : verbose mode\n\n"
                                 << "NOTES: Please use absolute paths!"
                                 << std::endl;};

    // get & check arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 12) {usage(); return 1;}
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
    std::string livetimeFile;
    result = std::find(args.begin(), args.end(), "--livetime-file");
    if (result != args.end()) livetimeFile = *(result+1);
    else {usage(); return 1;}
    std::ifstream flivetimes(livetimeFile);
    if (!flivetimes.is_open()) {glog(debug) << "Invalid livetime file! Aborting..."; return 1;}
    std::string runlistFile;
    result = std::find(args.begin(), args.end(), "--runlist-file");
    if (result != args.end()) runlistFile = *(result+1);
    else {usage(); return 1;}
    std::ifstream frunlist(runlistFile);
    if (!frunlist.is_open()) {glog(debug) << "Invalid run-list file! Aborting..."; return 1;}
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    auto dirWithRaw = *(args.end()-1);

    if (dirWithRaw.find("chanwise") != std::string::npos) {
        glog(debug) << "These simulations won't be processed because they are separated in channels. This will create problems in PDFs building. Aborting..."; return 1;
    }

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

    // get raw-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { glog(error) << "Invalid or empty directory path!\n"; return filelist; }
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
        if (verbose) {
            glog(debug) << "sorted raw- files:\n";
            for ( const auto & f : filelist ) glog(debug) << '\t' << f << std::endl;
        }

        return filelist;
    };

    // join all raw- files in same tree and get number of primaries
    auto filelist = GetContent(dirWithRaw);
    if (filelist.empty()) {glog(error) << "There were problems reading in the raw- files. Aborting...\n"; return 1;}
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

    // read json file with run list
    Json::Value runlist;
    frunlist >> runlist;

    // read json file with livetimes
    Json::Value livetimes;
    flivetimes >> livetimes;

    // calculate total livetime
    int totLivetime = 0;
    for (auto r : runlist["run-list"]) {
        auto runID = "run" + r.asString();
        if (!livetimes[runID]["livetime_in_s"]) {
            glog(error) << runID + " not found in livetime file! rerun livetime-calc-ph2! Aborting...";
            return 1;
        }
        if (verbose) glog(debug) << runID + ": " << livetimes[runID]["livetime_in_s"].asInt() << " s\n";
        totLivetime += livetimes[runID]["livetime_in_s"].asInt();
    }
    if (verbose) glog(debug) << "total livetime: " << totLivetime << std::endl;

    int first = 0; // first event to be processed in tree
    // loop over runIDs in json file
    for ( auto r : runlist["run-list"] ) {
        auto runID = "run" + r.asString();
        glog(info) << "==> " << runID << std::endl;
        // strip out folders in dir to build up final t4z- filename
        std::vector<std::string> items;
        std::string dircopy = dirWithRaw;
        for (int j = 0; j < 4; j++ ) {
            items.push_back(dircopy.substr(dircopy.find_last_of('/')+1));
            dircopy.erase(dircopy.find_last_of('/'), dircopy.back());
        }
        auto& it = items;
        auto filedir = destDirPath + '/' + it[3] + '/' + it[2] + '/' + it[1] + '/' + it[0];
        auto filename = filedir + '/' + "t4z-" + it[3] + '-' + it[2] + '-' + it[1] + '-' + it[0] + "-" +
                        runID + ".root";
        if (verbose) glog(debug) << "post-production folder: " << destDirPath << std::endl;
        if (verbose) glog(debug) << "tz4- file name: " << filename << std::endl;

        // make destdir
        std::system(c_str("mkdir -p " + filedir));

        // calculate fraction of events
        auto nentries = ch.GetEntries();
        int fraction = std::round(nentries * livetimes[runID]["livetime_in_s"].asInt() *1. / totLivetime);
        Long64_t primFraction = std::round(totPrimaries * livetimes[runID]["livetime_in_s"].asInt() *1. / totLivetime);
        if (verbose) glog(debug) << "Events fraction: " << nentries << " * "
                                 << livetimes[runID]["livetime_in_s"].asInt() << " *1. / "
                                 << totLivetime << " = " << fraction << std::endl;
        if (verbose) glog(debug) << "Primaries fraction: " << totPrimaries << " * "
                                 << livetimes[runID]["livetime_in_s"].asInt() << " *1. / "
                                 << totLivetime << " = " << primFraction << std::endl;

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

        config.LoadRunConfig((ULong64_t)livetimes[runID]["timestamp"].asUInt64());
        if(verbose) glog(debug) << "RunConfig for timestamp " << livetimes[runID]["timestamp"].asUInt64()
                                << " loaded" << std::endl;

        gada::T4SimHandler handler(&ch, &config, filename);

        // run!
        glog(info) << "running production... first event = " << first << std::endl;
        if ( first + fraction > nentries ) fraction = nentries - first;
        handler.RunProduction( first, first + fraction );

        // save number of primaries
        TFile f(filename.c_str(),"UPDATE");
        TParameter<Long64_t> numberOfPrimaries("NumberOfPrimaries",primFraction);
        numberOfPrimaries.Write();
        f.Close();

        glog(info) << "done. " << filename << " created" << std::endl;

        // update first event
        first += fraction;
    }

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
