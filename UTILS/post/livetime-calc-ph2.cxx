/* livetime-calc-ph2.cxx
 *
 * Author: Luigi Pertoldi: luigi.pertoldi@pd.infn.it
 * Created: 24 Jan 2018
 *
 * g++ -std=c++11 $(root-config --cflags) -lTreePlayer $(gerda-ada-config --cflags) $(gelatio-config --cflags) $(mgdo-config --cflags)
 * -I../jsoncpp/ ../jsoncpp/jsoncpp.cpp
 * $(root-config --libs) $(gerda-ada-config --libs) $(gelatio-config --libs) $(mgdo-config --libs) livetime-calc-ph2.cxx -o livetime-calc-ph2
 */
// stl
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

// jsoncpp
#include <json/json.h>

// ROOT
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// gerda-ada
#include "gerda-ada/FileMap.h"
#include "gerda-ada/DataLoader.h"

// gelatio
#include "GETRunConfiguration.hh"
#include "GERunConfigurationManager.hh"

int main(int argc, char** argv) {

    auto usage = [](){ std::cout << "USAGE: livetime-calc-ph2 [OPTION] [VALUE]\n\n"
                                 << "OPTIONS: --metadata <gerda-metadata-location>\n"
                                 << "         --data     <gerda-data-location>\n"
                                 << "         -v         verbose mode\n"
                                 << std::endl; return 1; };

    std::vector<std::string> args;
    for ( int i = 0; i < argc; ++i ) args.emplace_back(argv[i]);

    if ( argc < 4 ) usage();
    std::string gerdaMetaPath;
    auto result = std::find(args.begin(), args.end(), "--metadata");
    if ( result != args.end()) gerdaMetaPath = *(result+1);
    else usage();
    std::string gerdaDataPath;
    result = std::find(args.begin(), args.end(), "--data");
    if ( result != args.end()) gerdaDataPath = *(result+1);
    else usage();
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if ( result != args.end()) verbose = true;

    auto runList = { 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
                     63, 64, 65, /*66,*/ 67, /*68,*/ 69, 70, 71, 72,
                     73, 74, 75, 76, 77, 78, 79, 80, 82, 82,
                     83, 84, 85, 86, 87, 88, 89 };

    auto FindRunConfiguration = [&](unsigned long long timestamp) {
        if (verbose) std::cout << "Getting RunConfiguration\n";
        GERunConfigurationManager theRunConfManager;
        theRunConfManager.AllowRunConfigurationSwitch(true); // This is important!
        if (verbose) theRunConfManager.SetVerbosity(1);
        return std::unique_ptr<GETRunConfiguration>(theRunConfManager.GetRunConfiguration(timestamp));
    };

    TTreeReader reader;
    TTreeReaderValue<int>                isTP(reader, "isTP.isTP");
    TTreeReaderValue<unsigned long long> timestamp(reader, "timestamp");

    Json::Value root;

    for ( auto& id : runList ) {
        if (verbose) std::cout << "RUN" << id << std::endl
                               << "Looking for data files...\n";
        gada::FileMap myMap;
        myMap.SetRootDir(gerdaDataPath);
        std::string pathToListOfKeys = gerdaMetaPath + "/data-sets/phy/run00" + std::to_string(id) + "-phy-analysis.txt";
        std::ifstream ftmp(pathToListOfKeys.c_str());
        if ( !ftmp.is_open() ) { std::cerr << pathToListOfKeys << " does not exist!\n"; continue; }
        myMap.BuildFromListOfKeys(pathToListOfKeys);

        if (verbose) std::cout << "Loading trees...\n";
        gada::DataLoader loader;
        loader.AddFileMap(&myMap);
        loader.BuildTier3();
        reader.SetTree(loader.GetUniqueMasterChain());

        reader.Next();
        auto gtr = FindRunConfiguration(*timestamp);

        int nTP = 0;
        reader.SetEntry(-1);
        if (verbose) std::cout << "Scanning tree...\n";
        while (reader.Next()) if (*isTP) nTP++;

        int livetime = nTP*1. / gtr->GetPulserRate();
        std::cout << "Livetime: " << livetime << " s\n";
        root["run" + std::to_string(id)]["ID"] = id;
        root["run" + std::to_string(id)]["livetime_in_s"] = livetime;
        root["run" + std::to_string(id)]["runconfig"] = std::string(gtr->GetConfigurationName());
    }

    std::ofstream fout("run-livetime.json");
    fout << root;

    return 0;
}
