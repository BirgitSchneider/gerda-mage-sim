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

// jsoncpp
#include "json/json.h"

// cern-root
#include "TFile.h"
#include "TChain.h"

// gerda-ada
#include "gerda-ada/T4SimHandler.h"
#include "gerda-ada/T4SimConfig.h"

int main(int argc, char** argv) {

    // utilities
    auto c_str = [&](std::string s){return s.c_str();};
    auto usage = [](){ std::cout << "USAGE: ./t4z-gen [OPTIONS] [DIR-WITH-RAW]\n\n"
                                << "OPTIONS:\n"
                                << "  required: --metadata <gerda-metadata-location>\n"
                                << "            --destdir <destination-dir-post-processed>\n"
                                << "            --livetime-file <json-file-from-livetime-calc-ph2>\n"
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
    std::string livetimeFile;
    result = std::find(args.begin(), args.end(), "--livetime-file");
    if (result != args.end()) livetimeFile = *(result+1);
    else {usage(); return 1;}
    std::ifstream flivetimes(livetimeFile);
    if (!flivetimes.is_open()) {std::cout << "Invalid livetime file! Aborting..."; return 1;}
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    auto dirWithRaw = *(args.end()-1);

    // strip off trailing '/' character
    if (verbose) std::cout << "Paths:\n";
    if (gerdaMetaPath.back() == '/') gerdaMetaPath.pop_back(); if (verbose) std::cout << gerdaMetaPath << std::endl;
    if (destDirPath.back()   == '/') destDirPath.pop_back();   if (verbose) std::cout << destDirPath   << std::endl;
    if (dirWithRaw.back()    == '/') dirWithRaw.pop_back();    if (verbose) std::cout << dirWithRaw    << std::endl;

    // get raw-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { std::cout << "Invalid or empty directory path!\n"; return filelist; }
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 && r; ) {
            if (entry.d_type == 8 &&
                std::string(entry.d_name).find("raw-") != std::string::npos &&
                std::string(entry.d_name).find(".root") != std::string::npos) {
                filelist.push_back(foldName + "/" + std::string(entry.d_name));
                if (verbose) std::cout << '\t' << std::string(entry.d_name) << std::endl;
            }
        }
        std::sort (filelist.begin(), filelist.end());
        if (verbose) { std::cout << "\nraw- files sorted:\n"; for ( const auto & f : filelist ) std::cout << '\t' << f << std::endl; }

        return filelist;
    };

    // join all raw- files in same tree
    if (verbose) std::cout << "\nraw- files found:\n";
    auto filelist = GetContent(dirWithRaw);
    if (edepFilelist.empty()) {std::cout << "There were problems in reading the raw- files. Aborting...\n"; return 1;}
    TChain ch("fTree");
    for ( auto& f : filelist ) ch.Add(f.c_str());

    // read json file with livetimes
    Json::Value livetimes;
    flivetimes >> livetimes;

    Json::Value::Members runs = livetimes.getMemberNames();

    // calculate total livetime
    int totLivetime = 0;
    for ( auto r : runs ) {
        if (verbose) std::cout << '\n' << r << ": " << livetimes[r]["livetime_in_s"].asInt() << " s";
        totLivetime += livetimes[r]["livetime_in_s"].asInt();
    }
    if (verbose) std::cout << "\nTotal livetime:" << totLivetime << std::endl;

    int first = 0; // first event to be processed in tree
    // loop over runIDs in json file
    for ( auto r : runs ) {
        if (verbose) std::cout << "\nRun" << r << std::endl;
        // strip out folders in dir to build up final t4z- filename
        std::vector<std::string> items;
        std::string dircopy = dirWithRaw;
        for (int j = 0; j < 4; j++ ) {
            items.push_back(dircopy.substr(dircopy.find_last_of('/')+1));
            dircopy.erase(dircopy.find_last_of('/'), dircopy.back());
        }
        auto& it = items;
        auto filedir = destDirPath + '/' + it[3] + '/' + it[2] + '/' + it[1] + '/' + it[0];
        auto filename = filedir + '/' + "t4z-" + it[3] + '-' + it[2] + '-' + it[1] + '-' + it[0] + "-run" +
                        livetimes[r]["ID"].asString() + ".root";
        if (verbose) std::cout << destDirPath << std::endl;
        if (verbose) std::cout << "Path to t4z- file: " << filedir << std::endl;
        if (verbose) std::cout << "tz4- file name: " << filename << std::endl;

        // make destdir
        std::system(c_str("mkdir -p " + filedir));

        // calculate fraction of events
        auto nentries = ch.GetEntries();
        int fraction = (int)(nentries * livetimes[r]["livetime_in_s"].asInt() *1. / totLivetime);
        if (verbose) std::cout << nentries << " * " << livetimes[r]["livetime_in_s"].asInt() << " *1. / " << totLivetime << " = " << fraction << std::endl;

        // set up tier4izer
        gada::T4SimConfig config;
        config.LoadMapping(gerdaMetaPath + "/detector-data/mapping-default-depr.json");
            if(verbose) std::cout << "Mapping loaded" << std::endl;
        config.LoadGedSettings(gerdaMetaPath + "/detector-data/ged-settings-default.json");
            if(verbose) std::cout << "Ged settings loaded" << std::endl;
//        config.LoadLArSettings(gerdaMetaPath + "/detector-data/lar-settings-default.json");
//            if(verbose) std::cout << "LAr settings loaded" << std::endl;
        config.LoadGedResolutions(gerdaMetaPath + "/detector-data/ged-resolution-default.json");
            if(verbose) std::cout << "Ged resolutions loaded" << std::endl;
        config.LoadRunConfig(gerdaMetaPath + "/config/_aux/geruncfg/" + livetimes[r]["runconfig"].asString());
            if(verbose) std::cout << "RunConfig loaded" << std::endl;

        gada::T4SimHandler handler(&ch, &config, filename);
            if (verbose) std::cout << "Running production... first event = " << first << std::endl;
        // run
        handler.RunProduction( first, first + fraction );

        // update first event
        first += fraction;
    }

    return 0;
}
