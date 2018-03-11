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
    std::string srcDirPath;
    result = std::find(args.begin(), args.end(), "--srcdir");
    if (result != args.end()) srcDirPath = *(result+1);
    else {usage(); return 1;}
    std::string livetimeFile;
    result = std::find(args.begin(), args.end(), "--livetime-file");
    if (result != args.end()) livetimeFile = *(result+1);
    else {usage(); return 1;}
    std::ifstream flivetimes(livetimeFile);
    if (!flivetimes.is_open()) {std::cout << "Invalid livetime file! Aborting..."; return 1;}
    std::string runlistFile;
    result = std::find(args.begin(), args.end(), "--runlist-file");
    if (result != args.end()) runlistFile = *(result+1);
    else {usage(); return 1;}
    std::ifstream frunlist(runlistFile);
    if (!frunlist.is_open()) {std::cout << "Invalid run-list file! Aborting..."; return 1;}
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    auto dirWithRaw = *(args.end()-1);

    if (dirWithRaw.find("chanwise") != std::string::npos) {
        std::cout << "These simulations won't be processed because they are separated in channels. This will create problems in PDFs building. Aborting..."; return 1;
    }

    setenv("MU_CAL", (gerdaMetaPath + "/config/_aux/geruncfg").c_str(), 1);

    // strip off trailing '/' character, if present
    if (verbose) std::cout << "Paths:\n";
    if (gerdaMetaPath.back() == '/') gerdaMetaPath.pop_back(); if (verbose) std::cout << gerdaMetaPath << std::endl;
    if (destDirPath.back()   == '/') destDirPath.pop_back();   if (verbose) std::cout << destDirPath   << std::endl;
    if (srcDirPath.back()   == '/')  srcDirPath.pop_back();    if (verbose) std::cout << srcDirPath   << std::endl;
    if (dirWithRaw.back()    == '/') dirWithRaw.pop_back();    if (verbose) std::cout << dirWithRaw    << std::endl;

    // get raw-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { std::cout << "Invalid or empty directory path!\n"; return filelist; }
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
        if (verbose) { std::cout << "\nsorted raw- files:\n"; for ( const auto & f : filelist ) std::cout << '\t' << f << std::endl; }

        return filelist;
    };

    // join all raw- files in same tree and get number of primaries
    auto filelist = GetContent(dirWithRaw);
    if (filelist.empty()) {std::cout << "There were problems reading in the raw- files. Aborting...\n"; return 1;}
    TChain ch("fTree");
    bool problems = false;
    long totPrimaries = 0;
    for ( auto& f : filelist ) {
        TFile file(f.c_str());
        // check first if the object exists
        if (file.GetListOfKeys()->Contains("NumberOfPrimaries")) {
            auto primaries = dynamic_cast<TParameter<long>*>(file.Get("NumberOfPrimaries"))->GetVal();
            if (primaries == 0) {
                std::cout << "WARNING: NumberOfPrimaries is zero in " << f << "!\n";
                problems = true;
            }
            totPrimaries += primaries;
        }
        else {
            std::cout << "WARNING: NumberOfPrimaries not found in " << f << "! It will be set to zero!\n";
            problems = true;
        }
        file.Close();
        ch.Add(f.c_str());
    }
    // if at least one object does not exist set primaries to zero to indicate a failure
    if (problems) {
        std::cout << "WARNING: there were some problems, setting totPrimaries = 0...\n";
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
            std::cout << "\nERROR: " + runID + " not found in livetime file! rerun livetime-calc-ph2! Aborting...";
            return 1;
        }
        if (verbose) std::cout << "\n" + runID + ": " << livetimes[runID]["livetime_in_s"].asInt() << " s";
        totLivetime += livetimes[runID]["livetime_in_s"].asInt();
    }
    if (verbose) std::cout << "\nTotal livetime: " << totLivetime << std::endl;

    int first = 0; // first event to be processed in tree
    // loop over runIDs in json file
    for ( auto r : runlist["run-list"] ) {
        auto runID = "run" + r.asString();
        if (verbose) std::cout << std::endl;
        std::cout << "==> " << runID << std::endl;
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
        if (verbose) std::cout << "Post-production folder: " << destDirPath << std::endl;
        if (verbose) std::cout << "tz4- file name: " << filename << std::endl;

        // make destdir
        std::system(c_str("mkdir -p " + filedir));

        // calculate fraction of events
        auto nentries = ch.GetEntries();
        int fraction = std::round(nentries * livetimes[runID]["livetime_in_s"].asInt() *1. / totLivetime);
        Long64_t primFraction = std::round(totPrimaries * livetimes[runID]["livetime_in_s"].asInt() *1. / totLivetime);
        if (verbose) std::cout << "Events fraction: " << nentries << " * " << livetimes[runID]["livetime_in_s"].asInt() << " *1. / " << totLivetime << " = " << fraction << std::endl;
        if (verbose) std::cout << "Primaries fraction: " << totPrimaries << " * " << livetimes[runID]["livetime_in_s"].asInt() << " *1. / " << totLivetime << " = " << primFraction << std::endl;

        // set up tier4izer
        gada::T4SimConfig config;
        config.LoadMapping(gerdaMetaPath + "/detector-data/mapping-default-depr.json");
            if(verbose) std::cout << "Mapping loaded" << std::endl;
        config.LoadGedSettings(srcDirPath + "/UTILS/post/settings/ged-settings-custom.json");
            if(verbose) std::cout << "Ged settings loaded" << std::endl;
//        config.LoadLArSettings(gerdaMetaPath + "/detector-data/lar-settings-default.json");
//            if(verbose) std::cout << "LAr settings loaded" << std::endl;
        config.LoadGedResolutions(srcDirPath + "/UTILS/post/settings/ged-resolution-custom.json", "Zac");
            if(verbose) std::cout << "Ged resolutions loaded" << std::endl;
        config.LoadRunConfig((ULong64_t)livetimes[runID]["timestamp"].asUInt64());
            if(verbose) std::cout << "RunConfig loaded" << std::endl;

        gada::T4SimHandler handler(&ch, &config, filename);
            if (verbose) std::cout << "Running production... first event = " << first << std::endl;
        // run!
        if ( first + fraction > nentries ) fraction = nentries - first;
        handler.RunProduction( first, first + fraction );

        // save number of primaries
        TFile f(filename.c_str(),"UPDATE");
        TParameter<Long64_t> numberOfPrimaries("NumberOfPrimaries",primFraction);
        numberOfPrimaries.Write();
        f.Close();

        // update first event
        first += fraction;
    }

    return 0;
}
