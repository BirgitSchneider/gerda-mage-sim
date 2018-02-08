/* livetime-calc-ph2.cxx
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
#include <fstream>
#include <sstream>
#include <map>

// jsoncpp
#include <json/json.h>

// ROOT
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// gerda-ada
#include "gerda-ada/FileMap.h"
#include "gerda-ada/DataLoader.h"
//#include "gerda-ada/GADatasetManager.h"

// gelatio
#include "GETRunConfiguration.hh"
#include "GERunConfigurationManager.hh"

int main(int argc, char** argv) {

    // utilities
    auto c_str = [](std::string s){ return s.c_str(); };
    auto usage = [](){ std::cout << "USAGE: livetime-calc-ph2 [OPTION] [VALUE]\n\n"
                                 << "OPTIONS:\n"
                                 << "  required:\n"
                                 << "      --metadata <gerda-metadata-location>\n"
                                 << "      --data <gerda-data-location>\n"
                                 << "  optional:"
                                 << "      --output <output-file>\n"
                                 << "      -v\n"
                                 << std::endl; return;};

    // retrieve arguments
    std::vector<std::string> args;
    for ( int i = 0; i < argc; ++i ) args.emplace_back(argv[i]);
    if ( argc < 5 ) {usage(); return 1;}
    std::string gerdaMetaPath;
    auto result = std::find(args.begin(), args.end(), "--metadata");
    if ( result != args.end()) gerdaMetaPath = *(result+1);
    else {usage(); return 1;}
    std::string gerdaDataPath;
    result = std::find(args.begin(), args.end(), "--data");
    if ( result != args.end()) gerdaDataPath = *(result+1);
    else {usage(); return 1;}
    std::string outputFile = "run-livetime.json";
    result = std::find(args.begin(), args.end(), "--output");
    if ( result != args.end()) outputFile = *(result+1);
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if ( result != args.end()) verbose = true;

    // *CHANGE ME* list of runs to process
    auto runList = { 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
                     63, 64, 65, /*66,*/ 67, /*68,*/ 69, 70, 71, 72,
                     73, 74, 75, 76, 77, 78, 79, /*80, 81, 82,*/
                     83, 84, 85, 86, 87, 88, 89 };

    // get runconfig list from runconfiguration.db
    std::vector<std::pair<std::string,int>> confList;
    std::ifstream dbfile(gerdaMetaPath + "/config/_aux/geruncfg/runconfiguration.db");
    std::stringstream datastream;
    std::string datastring, name;
    int tstamp;

    getline(dbfile, datastring);
    getline(dbfile, datastring);

    // A safe way to read data using stringstream
    // and to avoid duplication of the last line
    while (getline(dbfile, datastring)) {

        // Skip empty lines
        if (datastring.empty()) continue;

        // Read the line and stream the name and the time stamp
        datastream.clear();
        datastream.str(datastring);
        datastream >> name;
        datastream >> tstamp;

        confList.push_back(std::make_pair(name, tstamp));
    }
    if (verbose) for (auto& i : confList) std::cout << i.first << '\t' << i.second << std::endl;

    // lambda function to retrieve GETRunConfiguration and filename for timestamp
    auto FindRunConfiguration = [&](unsigned long long timestamp) {
        if (verbose) std::cout << "Getting RunConfiguration\n";
        // find filename
        std::string rcfilename;
        for (int k = (int)confList.size()-1; k >= 0; k--) {
            if (timestamp >= confList[k].second) {
                rcfilename = confList[k].first;
                break;
            }
        }
        // open and read it
        TFile gtrFile(c_str(gerdaMetaPath + "/config/_aux/geruncfg/" + rcfilename));
        std::unique_ptr<GETRunConfiguration> runconfig(dynamic_cast<GETRunConfiguration*>(gtrFile.Get("RunConfiguration")));

        return std::make_pair(std::move(runconfig), rcfilename);
    };

    TTreeReader reader;
    TTreeReaderValue<int>                isTP(reader, "isTP.isTP");
    TTreeReaderValue<unsigned long long> timestamp(reader, "timestamp");
//    TTreeReaderArray<bool>               psdIsEval(reader, "psdIsEval");

    Json::Value root;
//    GADatasetManager theDatasetManager;
    for ( auto& id : runList ) {
        if (verbose) std::cout << "RUN" << id << std::endl
                               << "Looking for data files...\n";

        // get official data tree for runID
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
        reader.SetTree(loader.GetSharedMasterChain());
//        bool hasPsdIsEval = true;
//        if (...) hasPsdEval = false;

        // we need the timestamp to get the RunConfiguration
        // NOTE: this assumes that we don't have RunConfiguration changes during a run
        reader.Next();
        auto gtr = FindRunConfiguration(*timestamp);

        // count test pulses to get the correct livetime
        int nTP = 0;
        reader.SetEntry(0);
        if (verbose) std::cout << "Scanning tree...\n";
        while (reader.Next()) {
            if (*isTP) {
/*                for (int i = 0; i < (int)gtr->GetNDetectors(); ++i) {
                    int idataset = theDatasetManager->FindDataset(timestamp,
                                                                  (int)iDet,
                                                                  hasPsdIsEval ? psdIsEval.at(iDet) : false);

                    //This event does not belong to any dataset
                    if (idataset < 0)
                    continue;
                }
*/                nTP++;
            }
        }

        int livetime = nTP*1. / gtr.first->GetPulserRate();
        std::cout << "Livetime: " << livetime << " s\n";
        root["run" + std::to_string(id)]["ID"] = id;
        root["run" + std::to_string(id)]["livetime_in_s"] = livetime;
        root["run" + std::to_string(id)]["runconfig"] = gtr.second;
    }

    // write down jsonfile
    std::ofstream fout(outputFile);
    fout << root;

    return 0;
}
