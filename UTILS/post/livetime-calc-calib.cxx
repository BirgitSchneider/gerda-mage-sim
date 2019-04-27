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
    auto usage = [](){ std::cout << "USAGE: livetime-calc-calib [OPTION] [VALUE]\n\n"
                                 << "OPTIONS:\n"
                                 << "  required:\n"
                                 << "      --calib-pdf-settings <calib-pdf-settings.json>\n"
                                 << "      --metadata <gerda-metadata-location>\n"
                                 << "      --data <gerda-data-location>\n"
                                 << "  optional:"
                                 << "      --output <output-file>\n"
                                 << "      -v\n"
                                 << std::endl; return;};

    // retrieve arguments
    std::vector<std::string> args;
    for ( int i = 0; i < argc; ++i ) args.emplace_back(argv[i]);
    if ( argc < 7 ) {usage(); return 1;}
    std::string calibSetPath;
    auto result = std::find(args.begin(), args.end(), "--calib-pdf-settings");
    if ( result != args.end()) calibSetPath = *(result+1);
    else {usage(); return 1;}
    std::string gerdaMetaPath;
    result = std::find(args.begin(), args.end(), "--metadata");
    if ( result != args.end()) gerdaMetaPath = *(result+1);
    else {usage(); return 1;}
    std::string gerdaDataPath;
    result = std::find(args.begin(), args.end(), "--data");
    if ( result != args.end()) gerdaDataPath = *(result+1);
    else {usage(); return 1;}
    std::string outputFile = "calib-livetime.json";
    result = std::find(args.begin(), args.end(), "--output");
    if ( result != args.end()) outputFile = *(result+1);
    bool verbose = false;
    result = std::find(args.begin(), args.end(), "-v");
    if ( result != args.end()) verbose = true;

    TTreeReader reader;
    TTreeReaderValue<int>                isTP(reader, "isTP");
    TTreeReaderValue<unsigned long long> timestamp(reader, "timestamp");
//    TTreeReaderArray<bool>               psdIsEval(reader, "psdIsEval");

    Json::Value calibcfg;
    std::ifstream fcalibcfg(calibSetPath);
    fcalibcfg >> calibcfg;

    std::vector<std::string> done;
    Json::Value root;
//    GADatasetManager theDatasetManager;
    for (auto iso = calibcfg["calib"].begin(); iso != calibcfg["calib"].end(); ++iso) {
        for (auto src = (*iso).begin(); src != (*iso).end(); ++src) {
            if (src.key() == "notes") continue;
            for (auto pos = (*src).begin(); pos != (*src).end(); ++pos) {
                if (verbose) std::cout << "Isotope: " << iso.key()
                                       << ", source: " << src.key()
                                       << ", position: " << pos.key() << std::endl;

                // get official data tree for runID
                gada::FileMap myMap;
                myMap.SetRootDir(gerdaDataPath);

                auto single_keylist = (*pos)["keylist"].asString();
                auto keyname = single_keylist;

                if (std::find(done.begin(), done.end(), keyname) != done.end()) continue;
                done.push_back(keyname);

                std::cout << "Keylist: " << keyname << " " << std::flush;
                std::ifstream flist;
                auto test_keylist = gerdaMetaPath + "/data-sets/cal/" + single_keylist;
                flist.open(test_keylist);
                if (!flist.is_open()) {
                    test_keylist = gerdaMetaPath + "/data-sets/cal/" + single_keylist + ".txt";
                    flist.open(test_keylist);
                    if (!flist.is_open()) {
                        test_keylist = gerdaMetaPath + "/data-sets/pca/" + single_keylist;
                        flist.open(test_keylist);
                        if (!flist.is_open()) {
                            test_keylist = gerdaMetaPath + "/data-sets/pca/" + single_keylist + ".txt";
                            flist.open(test_keylist);
                            if (!flist.is_open()) {
                                std::cerr << "could not find keylist " << single_keylist << " in gerda-metadata\n";
                                return 1;
                            }
                        }
                    }
                }
                single_keylist = test_keylist;

                myMap.BuildFromListOfKeys(single_keylist);

                if (verbose) std::cout << "Loading trees...\n";
                gada::DataLoader loader;
                loader.AddFileMap(&myMap);
                loader.BuildTier3();
                auto tree = loader.GetSharedMasterChain();
                tree->SetBranchStatus("*", false);
                tree->SetBranchStatus("isTP", true);
                tree->SetBranchStatus("timestamp", true);
                reader.SetTree(tree);
        //        bool hasPsdIsEval = true;
        //        if (...) hasPsdEval = false;

                // we need the timestamp to get the RunConfiguration
                // NOTE: this assumes that we don't have RunConfiguration changes during a run
                reader.Next();

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

                int livetime = nTP * 20.;
                std::cout << "--> livetime: " << livetime << " s\n";
                root[keyname]["livetime_in_s"] = livetime;
            }
        }
    }

    // write down jsonfile
    std::ofstream fout(outputFile);
    fout << root;

    return 0;
}
