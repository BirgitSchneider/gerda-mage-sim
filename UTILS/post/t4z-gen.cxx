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

    auto usage = [](){
        std::cout << "Split simulations in a tier4ized file for each run.\n"
                  << "USAGE: t4z-gen --config <t4z-gen-settings.json> --destdir "
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
    auto livetime_file       = cfg["run-livetimes"].asString();
    auto runlist_file        = cfg["runlist"].asString();
    bool verbose             = cfg.get("debug", false).asBool();
    auto calib_file          = cfg["ged-resolution-curves"].asString();
    auto mapping_file        = cfg["channels-mapping"].asString();
    auto ged_settings_file   = cfg["ged-settings"].asString();
    auto ged_parameters_file = cfg["ged-parameters"].asString();
    auto ged_transition_file = cfg["ged-transition"].asString();
    auto larveto_model       = cfg["LAr-veto-model"].asString();
    auto heat_map            = cfg.get("heat-map", "null").asString();

    if (verbose) glog(debug) << "gerda-mage-sim: " << gms_path << std::endl;
    if (verbose) glog(debug) << "destination: " << destdir << std::endl;
    if (verbose) glog(debug) << "raw- files location: " << dir_with_raw << std::endl;

    if (verbose) glog(debug) << "paths found in JSON config:\n";
    for (auto s : {&gerda_meta, &livetime_file, &runlist_file, &calib_file,
                   &mapping_file, &ged_settings_file, &ged_parameters_file,
                   &ged_transition_file, &heat_map}) {
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
            for ( const auto & f : filelist ) glog(debug) << "  " << f << std::endl;
        }

        return filelist;
    };

    // join all raw- files in same tree and get number of primaries
    auto filelist = GetContent(dir_with_raw);
    if (filelist.empty()) {
        glog(error) << "There were problems reading in the raw- files. Aborting...\n";
        return 1;
    }

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
    std::ifstream frunlist(runlist_file);
    Json::Value runlist;
    frunlist >> runlist;

    // read json file with livetimes
    std::ifstream flivetimes(livetime_file);
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
        // if (verbose) glog(debug) << runID + ": " << livetimes[runID]["livetime_in_s"].asInt() << " s\n";
        totLivetime += livetimes[runID]["livetime_in_s"].asInt();
    }
    if (verbose) glog(debug) << "total livetime: " << totLivetime << " s" << std::endl;

    int first = 0; // first event to be processed in tree
    // loop over runIDs in json file
    for ( auto r : runlist["run-list"] ) {
        auto runID = "run" + r.asString();
        glog(info) << "==> " << runID << std::endl;
        // strip out folders in dir to build up final t4z- filename
        std::vector<std::string> items;
        // strip off trailing '/' character, if present
        if (dir_with_raw.back() == '/') dir_with_raw.pop_back();
        std::string dircopy = dir_with_raw;
        for (int j = 0; j < 4; j++ ) {
            items.push_back(dircopy.substr(dircopy.find_last_of('/')+1));
            dircopy.erase(dircopy.find_last_of('/'), dircopy.back());
        }
        auto& it = items;
        auto filedir = destdir + '/' + it[3] + '/' + it[2] + '/' + it[1] + '/' + it[0];
        auto filename = filedir + '/' + "t4z-" + it[3] + '-' + it[2] + '-' + it[1] + '-' + it[0] + "-" +
                        runID + ".root";
        if (verbose) glog(debug) << "post-production folder: " << destdir << std::endl;
        if (verbose) glog(debug) << "tz4- file name: " << filename << std::endl;

        // make destdir
        std::system(("mkdir -p " + filedir).c_str());

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
        TParameter<Long64_t> numberOfPrimaries("NumberOfPrimaries", primFraction);
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
