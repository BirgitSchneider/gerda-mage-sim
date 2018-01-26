/* t4z-gen.cxx
 *
 * Author: Luigi Pertoldi: luigi.pertoldi@pd.infn.it
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

// jsoncpp
#include "json/json.h"

// cern-root
#include "TFile.h"
#include "TChain.h"

// gerda-ada
#include "gerda-ada/T4SimHandler.h"
#include "gerda-ada/T4SimConfig.h"

int main(int argc, char** argv) {

    auto usage = [](){ std::cout << "USAGE: ./t4z-gen [OPTIONS] [DIR-WITH-RAW]\n\n"
                                << "OPTIONS: --metadata <gerda-metadata-location>\n"
                                << "         --destdir <destination-dir-post-processed>\n"
                                << "         -v : verbose mode\n"
                                << std::endl; return 1; };

    // get arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 1) usage();
    std::string gerdaMetaPath;
    auto result = std::find(args.begin(), args.end(), "--metadata");
    if (result != args.end()) gerdaMetaPath = *(result+1);
    else usage();
    std::string destDirPath;
    result = std::find(args.begin(), args.end(), "--destdir");
    if (result != args.end()) destDirPath = *(result+1);
    else usage();
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    auto dir = *(args.end()-1);

    // remove trailing '/'
    if (verbose) std::cout << "Paths:\n";
    for (auto s : {gerdaMetaPath, destDirPath, dir}) {
        s = std::string(realpath(s.c_str(), nullptr));
        if (s.back() == '/') s.pop_back();
        if (verbose) std::cout << s << std::endl;
    }

    // get raw-*.root files in directory
    if (verbose) std::cout << "\nraw- files found:\n";
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 && r; ) {
            if (entry.d_type == 8 &&
                std::string(entry.d_name).find("raw-") != std::string::npos &&
                std::string(entry.d_name).find(".root") != std::string::npos) {
                filelist.push_back(foldName + "/" + std::string(entry.d_name));
                if (verbose) std::cout << "  " << std::string(entry.d_name) << std::endl;
            }
        }
        return filelist;
    };

    // join all raw- files in same tree
    auto filelist = GetContent(dir);
    TChain ch("fTree");
    for ( auto& f : filelist ) ch.Add(f.c_str());

    // open json file with livetimes
    std::ifstream flivetimes("run-livetime.json");
    Json::Value livetimes;
    flivetimes >> livetimes;

    // calculate total livetime
    int totLivetime = 0;
    for ( int i = 0; i < livetimes.size(); ++i ) {
        totLivetime += livetimes[i]["ID"].asInt();
    }
    if (verbose) std::cout << "\nTotal livetime:" << totLivetime << std::endl;
    int first = 0; // first event to be processed in tree
    // loop over runIDs in json file
    for ( int i = 0; i < livetimes.size(); ++i ) {
        // strip out folders in dir to build up final t4z- filename
        std::vector<std::string> items;
        for (int j = 0; j < 4; j++ ) {
            dir.erase(dir.find_last_of('/'), dir.back());
            items.push_back(dir.substr(dir.find_last_of('/')+1));
        }
        auto& it = items;
        auto filename = destDirPath + '/' + it[3] + '/' + it[2] + '/' + it[1] + '/' + it[3] + '/' +
                        "t4z-" + it[3] + '-' + it[2] + '-' + it[1] + '-' + it[0] + "-run" +
                        livetimes[i]["ID"].asString() + ".root";
        if (verbose) std::cout << filename << std::endl;

        // calculate fraction of events
        auto nentries = ch.GetEntries();
        int fraction = (int)(nentries * livetimes[i]["ID"].asInt() *1. / totLivetime);
        if (verbose) std::cout << nentries << " * " << livetimes[i]["ID"].asInt() << " *1. / " << totLivetime << " = " << fraction << std::endl;

        // set up tier4izer
        gada::T4SimConfig config;
        config.LoadMapping(gerdaMetaPath + "/detector-data/mapping-default-depr.json");
        config.LoadGedSettings(gerdaMetaPath + "/detector-data/ged-resolution-default.json");
        config.LoadLArSettings(gerdaMetaPath + "/detector-data/lar-settings-default.json");
        config.LoadGedResolutions(gerdaMetaPath + "/detector-data/ged-resolution-default.json");
        config.LoadRunConfig(gerdaMetaPath + "/config/_aux/geruncfg/" + livetimes[i]["runconfig"].asString());
        gada::T4SimHandler handler(dynamic_cast<TChain*>(ch.CopyTree("", "", fraction,first)), &config, filename);
        if (verbose) std::cout << "Running production... first event = " << first << std::endl;
        // run
        handler.RunProduction();

        // update first event
        first += fraction;
    }

    return 0;
}
