/* pdf-gen.cxx
 *
 * Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 * Created: Sun 28 Jan 2018
 *
 */

// stl
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <dirent.h>
#include <map>

// jsoncpp
#include "json/json.h"

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TParameter.h"
#include "TChain.h"

int main( int argc, char** argv ) {

    // utilities
    auto c_str = [&](std::string s) {return s.c_str();};
    auto usage = []() {
        std::cout << "Create official pdfs from tier4rized files.\n"
                  << "USAGE: ./pdf-gen [OPTIONS] [<volume>/<part>/<isotope>]\n\n"
                  << "OPTIONS:\n"
                  << "  required:  --destdir <path>     : path to the top of the directory tree\n"
                  << "                                    with the t4z- files\n"
                  << "             --ged-mapping <file> : ged-mapping.json file included\n"
                  << "                                    in gerda-mage-sim\n\n"
                  << "  optional:  --inc-natcoax        : post process including natural Coax\n"
                  << "                                    detectors in M2 spectra (default:false)\n\n"
                  << "NOTES: Please use absolute paths!\n";
    };

    // get command line arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 6) {usage(); return 1;}
    bool verbose = false;
    auto result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    std::string pathToTop;
    result = std::find(args.begin(), args.end(), "--destdir");
    if (result != args.end()) pathToTop = *(result+1);
    else {usage(); return 1;}
    std::string gedMapFile;
    result = std::find(args.begin(), args.end(), "--ged-mapping");
    if (result != args.end()) gedMapFile = *(result+1);
    else {usage(); return 1;}
    bool incNatCoax = false;
    result = std::find(args.begin(), args.end(), "--inc-natcoax");
    if (result != args.end()) incNatCoax = true;
    // strip off trailing '/' character
    if (pathToTop.back() == '/') pathToTop.pop_back();
    if (verbose) std::cout << "Top directory tree: " << pathToTop << std::endl;
    auto pathToIsotope = *(args.end()-1);

    if (pathToIsotope.find("chanwise") != std::string::npos) {
        std::cout << "These t4z files won't be processed because the original simulations are separated in channels. This will create problems in PDFs building. Aborting..."; return 1;
    }
    // lambda function to get t4z-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { std::cout << "Invalid or empty directory path!\n"; return filelist; }
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 and r; ) {
            if (entry.d_type == 8 and
                std::string(entry.d_name).find("t4z-") != std::string::npos and
                std::string(entry.d_name).find(".root") != std::string::npos) {
                filelist.push_back(foldName + "/" + std::string(entry.d_name));
                if (verbose) std::cout << '\t' << std::string(entry.d_name) << std::endl;
            }
        }
        return filelist;
    };

    // get t4z- edep files
    auto fold = pathToTop + '/' + pathToIsotope + "/edep";
    if (verbose) std::cout << "\nedep t4z- files found in " << fold << " :\n";
    auto edepFilelist = GetContent(fold);
    if (edepFilelist.empty()) {std::cout << "There were problems in reading the t4z-edep files. Aborting...\n"; return 1;}
    // join all t4z- files in same tree
    TChain edepCh("fTree");
    for ( auto& f : edepFilelist ) edepCh.Add(f.c_str());

    // get t4z- coin file
    fold = pathToTop + '/' + pathToIsotope + "/coin";
    if (verbose) std::cout << "\ncoin t4z- files found in " << fold << " :\n";
    auto coinFilelist = GetContent(fold);
    bool processCoin = true;
    if (coinFilelist.empty()) {std::cout << "There were problems in reading the t4z- coin files, They won't be processed\n"; processCoin = false;}
    // join all t4z- files in same tree
    TChain coinCh("fTree");
    for ( auto& f : coinFilelist ) coinCh.Add(f.c_str());

    // strip out folders in pathToIsotope to build up final pdf- filename
    std::vector<std::string> items;
    std::string tmp = pathToIsotope;
    for (int j = 0; j < 2; j++ ) {
        items.push_back(tmp.substr(tmp.find_last_of('/')+1));
        tmp.erase(tmp.find_last_of('/'), tmp.back());
    }
    items.push_back(tmp);

    // read in detector mapping
    Json::Value root;
    std::ifstream fGedMap(gedMapFile);
    if (!fGedMap.is_open()) {std::cout << "ged mapping json file not found!"; return 1;}
    fGedMap >> root;
    std::map<int,std::string> det;
    Json::Value::Members detinfo = root["mapping"].getMemberNames();
    for ( const auto & d : detinfo ) {
        det[root["mapping"][d]["channel"].asInt()] = d;
    }
    if (verbose) {
        std::cout << "\nDetectors:\n";
        for (const auto& i : det) std::cout << "ch" << i.first << '\t' << i.second << std::endl;
    }

    // build M1 spectra
    if (verbose) std::cout << "\nBuilding M1 spectra...\n";
    std::vector<TH1D> energy_ch;
    for ( int i = 0; i < 40; ++i ) {
        energy_ch.emplace_back(Form("M1_ch%i", i), c_str("edep, M=1 (" + det[i] + ")"), 8000, 0, 8000);
    }
    TH1D energyBEGe   ("M1_enrBEGe", "edep, M=1 (enrBEGe)", 8000, 0, 8000);
    TH1D energyEnrCoax("M1_enrCoax", "edep, M=1 (enrCOAX)", 8000, 0, 8000);
    TH1D energyNatCoax("M1_natCoax", "edep, M=1 (natCOAX)", 8000, 0, 8000);

    TH1D M1_all_1525("M1_all_1525", "ID with edep in range = 1525 +- 4 keV, M=1 (all)",  40, 0, 40);
    TH1D M1_all_1461("M1_all_1461", "ID with edep in range = 1461 +- 4 keV, M=1 (all)",  40, 0, 40);
    TH1D M1_all_full("M1_all_full", "ID with edep in range = [565,5500] keV, M=1 (all)", 40, 0, 40);

    TH1D M1_all_S1  ("M1_all_S1",   "ID with edep in range = [1405,1450] keV, M=1 (all)", 40, 0, 40);
    TH1D M1_all_S2  ("M1_all_S2",   "ID with edep in range = [1470,1515] keV, M=1 (all)", 40, 0, 40);
    TH1D M1_all_S3  ("M1_all_S3",   "ID with edep in range = [1535,1580] keV, M=1 (all)", 40, 0, 40);

    // set up the TTree reader object
    TTreeReader reader;
    TTreeReaderValue<int>    multiplicity(reader, "multiplicity");
    TTreeReaderArray<double> energy      (reader, "energy");
    TTreeReaderArray<int>    datasetID   (reader, "datasetID");

    unsigned int M1_datasetMinusOne = 0; // M1: count events thrown away due to dataset -1
    unsigned int M2_datasetMinusOne = 0; // M2: -"-

    // loop
    std::cout << "Processing edep... " << edepCh.GetEntries() << " entries\n" << std::flush;
    edepCh.LoadTree(0);
    reader.SetTree(&edepCh);
    while(reader.Next()) {
        // select correct multiplicity
        if( *multiplicity <= 1 and *multiplicity > 0 ) {
            // loop over detector ids
            for ( int i = 0; i < 40; ++i ) {
                // skip if detector is in dataset -1
                if ( datasetID[i] == -1 ) { M1_datasetMinusOne++; continue; }

                // keep only valid energies
                if ( energy[i] < 10000 and energy[i] > 0 ) {
                    energy_ch[i].Fill(energy[i]);
                    if ( energy[i] >= 1521 and energy[i] < 1529 ) M1_all_1525.Fill(i);
                    if ( energy[i] >= 1457 and energy[i] < 1465 ) M1_all_1461.Fill(i);
                    if ( energy[i] >= 565  and energy[i] < 5500 ) M1_all_full.Fill(i);
                    if ( energy[i] >= 1405 and energy[i] < 1450 ) M1_all_S1.Fill(i);
                    if ( energy[i] >= 1470 and energy[i] < 1515 ) M1_all_S2.Fill(i);
                    if ( energy[i] >= 1535 and energy[i] < 1580 ) M1_all_S3.Fill(i);
                    if ( datasetID[i] == 0 or datasetID[i] == 4 ) energyBEGe.Fill(energy[i]);
                    if ( datasetID[i] == 1 ) energyEnrCoax.Fill(energy[i]);
                    if ( datasetID[i] == 3 ) energyNatCoax.Fill(energy[i]);
                }
            }
        }
    }
    if (verbose) std::cout << "There were " << M1_datasetMinusOne << " multiplicity 1 events in datasetID = -1 \n";

    // getting number of primaries
    if (verbose) std::cout << "Getting number of primaries...\n";
    bool problems = false;
    Long64_t nPrimEdep = 0;
    for (const auto& f : edepFilelist) {
        TFile froot(f.c_str());
        if (froot.GetListOfKeys()->Contains("NumberOfPrimaries")) {
            auto nPrimEdep_loc = dynamic_cast<TParameter<Long64_t>*>(froot.Get("NumberOfPrimaries"))->GetVal();
            if (nPrimEdep_loc == 0) {
                std::cout << "WARNING: number of primaries in " << f << " is zero!\n";
                problems = true;
            }
            nPrimEdep += nPrimEdep_loc;
        }
        else {
            std::cout << "WARNING: NumberOfPrimaries not found in " << f << "!\n";
            problems = true;
        }
    }
    // set number of primaries to zero in case of failures
    if (problems) {
        std::cout << "WARNING: there where some problems, setting number of primaries to zero...\n";
        nPrimEdep = 0;
    }
    if (verbose) std::cout << "Number of edep primaries: " << nPrimEdep << std::endl;

    // build M2 spectra
    if (verbose) std::cout << "\nBuilding M2 spectra...\n";
    TH2D M2_enrE1vsE2  ("M2_enrE1vsE2",   "edep with smaller detID, other edep, M=2 (enrAll)", 8000, 0, 8000, 8000, 0, 8000);
    TH1D M2_enrE1plusE2("M2_enrE1plusE2", "edep1 + edep2, M=2 (enrAll)",                       8000, 0, 8000);
    TH1D M2_enrE1andE2 ("M2_enrE1andE2",  "edep1 and edep2, M=2 (enrAll)",                     8000, 0, 8000);

    TH2D M2_ID1vsID2_1525("M2_ID1vsID2_1525", "ID1 with edep1+edep2 in range = 1525 +- 6 keV, ID2, M=2 (enrAll)",   40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_1461("M2_ID1vsID2_1461", "ID1 with edep1+edep2 in range = 1461 +- 6 keV, ID2, M=2 (enrAll)",   40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_full("M2_ID1vsID2_full", "ID1 with edep1+edep2 in range = [250,3000] keV, ID2, M=2 (enrAll)",  40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_S1  ("M2_ID1vsID2_S1",   "ID1 with edep1+edep2 in range = [1405,1450] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_S2  ("M2_ID1vsID2_S2",   "ID1 with edep1+edep2 in range = [1470,1515] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_S3  ("M2_ID1vsID2_S3",   "ID1 with edep1+edep2 in range = [1535,1580] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);

    TH1D M2_ID1andID2_1525("M2_ID1andID2_1525", "ID1 and ID2 with edep1+edep2 in range = 1525 +- 6 keV, M=2 (enrAll)",   40, 0, 40);
    TH1D M2_ID1andID2_1461("M2_ID1andID2_1461", "ID1 and ID2 with edep1+edep2 in range = 1461 +- 6 keV, M=2 (enrAll)",   40, 0, 40);
    TH1D M2_ID1andID2_full("M2_ID1andID2_full", "ID1 and ID2 with edep1+edep2 in range = [250,3000] keV, M=2 (enrAll)",  40, 0, 40);
    TH1D M2_ID1andID2_S1  ("M2_ID1andID2_S1",   "ID1 and ID2 with edep1+edep2 in range = [1405,1450] keV, M=2 (enrAll)", 40, 0, 40);
    TH1D M2_ID1andID2_S2  ("M2_ID1andID2_S2",   "ID1 and ID2 with edep1+edep2 in range = [1470,1515] keV, M=2 (enrAll)", 40, 0, 40);
    TH1D M2_ID1andID2_S3  ("M2_ID1andID2_S3",   "ID1 and ID2 with edep1+edep2 in range = [1535,1580] keV, M=2 (enrAll)", 40, 0, 40);

    Long64_t nPrimCoin = 0;  // number of primaries for coincidences
    int  badevents = 0;  // counter for events with multiplicity 2 but only 0 or 1 energy deposition
    if (processCoin) {
        // object to store events as a (det_id,energy) pair
        std::map<int,double> evMap;
        std::cout << "Processing coin... " << coinCh.GetEntries() << "entries\n" << std::flush;
        coinCh.LoadTree(0);
        reader.SetTree(&coinCh);
        while(reader.Next()) {
            // clear object from previous loop iteration
            evMap.clear();

            // select multiplicity
            if (*multiplicity == 2) {
                // loop over detector ids
                for ( int i = 0; i < 40; ++i ) {
                    // skip if detector is in dataset -1
                    if ( datasetID[i] == -1 ) { M2_datasetMinusOne++; continue; }

                    // select event if it has a valid energy
                    if ( energy[i] > 0 ) evMap.insert(std::make_pair(i, energy[i]));
                }
                // in case one detector has dataset -1, evMap has less than 2 entires
                if (evMap.size() != 2) {
                    //if (verbose) std::cout << "WARNING: Found " << evMap.size() << " events instead of 2! This should not happen!\n";
                    //if (verbose) std::cout << "WARNING: But can happen for coincidence events in GD02D!\n";
                    badevents++;
                    continue;
                }
                // comfortable referencies
                auto& ID1 = (*evMap.begin())    .first;
                auto& ID2 = (*(++evMap.begin())).first;
                auto& E1  = (*evMap.begin())    .second;
                auto& E2  = (*(++evMap.begin())).second;
/*                std::cout << "Multiplicity: " << *multiplicity << std::endl;
                std::cout << ID1 << '\t' << E1 << std::endl;
                std::cout << ID2 << '\t' << E2 << std::endl;
                std::cout << "-------------\n";
*/
                //do not include events that contain a trigger in an AC channel
                if ( E1 == 10000 or E2 == 10000 ) continue;

                auto sumE = E1 + E2;
                // only add datasets 0 (BEGe), 4 (BEGe noPSD) and 1 (enrCoax)
                // if option --inc-natcoax is given include also natural Coax detectors : 3 (natCoax)
                bool goodDataSet = (datasetID[ID1] == 0 or datasetID[ID1] == 4 or datasetID[ID1] == 1) and
                                   (datasetID[ID2] == 0 or datasetID[ID2] == 4 or datasetID[ID2] == 1);
                if( incNatCoax )
                     goodDataSet = (datasetID[ID1] == 0 or datasetID[ID1] == 4 or datasetID[ID1] == 1 or datasetID[ID1] == 3) and
                                   (datasetID[ID2] == 0 or datasetID[ID2] == 4 or datasetID[ID2] == 1 or datasetID[ID2] == 3);

                if ( goodDataSet ) {
                    M2_enrE1vsE2.Fill(E1, E2);
                    M2_enrE1plusE2.Fill(sumE);
                    M2_enrE1andE2.Fill(E1); M2_enrE1andE2.Fill(E2);

                    if ( sumE >= 1519 and sumE < 1531 ) {
                        M2_ID1vsID2_1525.Fill(ID1, ID2);
                        M2_ID1andID2_1525.Fill(ID1);
                        M2_ID1andID2_1525.Fill(ID2);
                    }
                    if ( sumE >= 1455 and sumE < 1467 ) {
                        M2_ID1vsID2_1461.Fill(ID1, ID2);
                        M2_ID1andID2_1461.Fill(ID1);
                        M2_ID1andID2_1461.Fill(ID2);
                    }
                    if ( sumE >= 250  and sumE < 3000 ) {
                        M2_ID1vsID2_full.Fill(ID1, ID2);
                        M2_ID1andID2_full.Fill(ID1);
                        M2_ID1andID2_full.Fill(ID2);
                    }
                    if ( sumE >= 1405 and sumE < 1450 ) {
                        M2_ID1vsID2_S1.Fill(ID1, ID2);
                        M2_ID1andID2_S1.Fill(ID1);
                        M2_ID1andID2_S1.Fill(ID2);
                    }
                    if ( sumE >= 1470 and sumE < 1515 ) {
                        M2_ID1vsID2_S2.Fill(ID1, ID2);
                        M2_ID1andID2_S2.Fill(ID1);
                        M2_ID1andID2_S2.Fill(ID2);
                    }
                    if ( sumE >= 1535 and sumE < 1580 ) {
                        M2_ID1vsID2_S3.Fill(ID1, ID2);
                        M2_ID1andID2_S3.Fill(ID1);
                        M2_ID1andID2_S3.Fill(ID2);
                    }
                }
            }
        }
        if (verbose) std::cout << "There were " << badevents << " events with multiplicity = 2 but a different number of edeps > 0 found in the tree\n";
        if (verbose) std::cout << "There were " << M2_datasetMinusOne << " multiplicity 2 events in datasetID = -1 \n";

        if (verbose) std::cout << "Getting number of primaries...\n";
        problems = false;
        for (const auto& f : coinFilelist) {
            TFile froot(f.c_str());
            if (froot.GetListOfKeys()->Contains("NumberOfPrimaries")) {
                auto nPrimCoin_loc = dynamic_cast<TParameter<Long64_t>*>(froot.Get("NumberOfPrimaries"))->GetVal();
                if (nPrimCoin_loc == 0) {
                    std::cout << "WARNING: number of primaries in " << f << " is zero!\n";
                    problems = true;
                }
                nPrimCoin += nPrimCoin_loc;
            }
           else {
               std::cout << "WARNING: NumberOfPrimaries not found in " << f << "!\n";
               problems = true;
           }
        }
        if (problems) {
            std::cout << "WARNING: there where some problems, setting number of primaries to zero...\n";
            nPrimCoin = 0;
        }
        if (verbose) std::cout << "Number of coin primaries: " << nPrimCoin << std::endl;
    }

    // Save!
    // build output filename
    std::string outName = pathToTop + '/' + pathToIsotope + '/' +
                          "pdf-" + items[2] + '-' + items[1] + '-' + items[0] + ".root";
    TFile outfile(outName.c_str(), "RECREATE");

    if (verbose) std::cout << "Saving to disk...\n";
    TParameter<Long64_t> nPrimCoinPar("NumberOfPrimariesCoin", nPrimCoin);
    TParameter<Long64_t> nPrimEdepPar("NumberOfPrimariesEdep", nPrimEdep);
    for ( auto h : energy_ch ) h.Write();
    energyBEGe.Write();
    energyEnrCoax.Write();
    energyNatCoax.Write();

    M1_all_1525.Write();
    M1_all_1461.Write();
    M1_all_full.Write();

    M1_all_S1.Write();
    M1_all_S2.Write();
    M1_all_S3.Write();

    if (processCoin) {
        M2_enrE1vsE2.Write();
        M2_enrE1plusE2.Write();
        M2_enrE1andE2.Write();

        M2_ID1vsID2_1525.Write();
        M2_ID1vsID2_1461.Write();
        M2_ID1vsID2_full.Write();
        M2_ID1vsID2_S1.Write();
        M2_ID1vsID2_S2.Write();
        M2_ID1vsID2_S3.Write();

        M2_ID1andID2_1525.Write();
        M2_ID1andID2_1461.Write();
        M2_ID1andID2_full.Write();
        M2_ID1andID2_S1.Write();
        M2_ID1andID2_S2.Write();
        M2_ID1andID2_S3.Write();

        nPrimCoinPar.Write();
    }

    nPrimEdepPar.Write();
    std::cout << "Output saved in: " << outName << std::endl;

    return 0;
}
