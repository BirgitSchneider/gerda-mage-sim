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
#include <set>

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
#include "TH1I.h"
#include "TH2I.h"
#include "TString.h"
#include "TParameter.h"
#include "TChain.h"

int main( int argc, char** argv ) {

    // utilities
    auto c_str = [&](std::string s) {return s.c_str();};
    auto usage = []() {
        std::cout << "Create official pdfs from tier4rized files.\n"
                  << "USAGE: ./gen-spectra [OPTIONS] [<volume>/<part>/<isotope>]\n\n"
                  << "OPTIONS:\n"
                  << "  required:  --destdir <path> : path to the top of the directory tree\n"
                  << "                                with the t4z- files\n"
                  << "             --ged-mapping <file> : ged-mapping.json file included\n"
                  << "                                    in gerda-mage-sim\n\n"
                  << "NOTES: Please use absolute paths!\n";
    };

    // get command line arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 5) {usage(); return 1;}
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
    // strip off trailing '/' character
    if (pathToTop.back() == '/') pathToTop.pop_back();
    if (verbose) std::cout << "Top directory tree: " << pathToTop << std::endl;
    auto pathToIsotope = *(args.end()-1);

    // lambda function to get t4z-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
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
    if (verbose) std::cout << "\nedep t4z- files found:\n";
    auto edepFilelist = GetContent(pathToTop + '/' + pathToIsotope + "/edep");
    // join all t4z- files in same tree
    TChain edepCh("fTree");
    for ( auto& f : edepFilelist ) edepCh.Add(f.c_str());

    // get t4z- coin file
    if (verbose) std::cout << "\ncoin t4z- files found:\n";
    auto coinFilelist = GetContent(pathToTop + '/' + pathToIsotope + "/coin");
    // join all t4z- files in same tree
    TChain coinCh("fTree");
    for ( auto& f : coinFilelist ) coinCh.Add(f.c_str());

    // strip out folders in pathToIsotope to build up final pdf- filename
    std::vector<std::string> items;
    std::string tmp = pathToIsotope;
    for (int j = 0; j < 3; j++ ) {
        items.push_back(tmp.substr(tmp.find_last_of('/')+1));
        tmp.erase(tmp.find_last_of('/'), tmp.back());
    }
    std::string outName = pathToTop + '/' + pathToIsotope + '/' +
                          "pdf-" + items[2] + '-' + items[1] + '-' + items[0] + ".root";
    if (verbose) std::cout << "\nOutput filename: " << outName << std::endl;
    TFile outfile(outName.c_str(), "RECREATE");

    Json::Value root;
    std::ifstream fGedMap(gedMapFile);
    if (!fGedMap.is_open()) {std::cout << "ged mapping json file not found!"; return 1;}
    fGedMap >> root;
    std::map<int,std::string> det;
    for (Json::Value::iterator itr = root.begin(); itr != root.end(); ++itr ) {
        det.insert((*itr)["channel"], (*itr)["name"]);
    }
    if (verbose) {
        std::cout << "\nDetectors:\n";
        for (const auto& i : det) std::cout << "ch" << i.first << '\t' << i.second << std::endl;
    }

    // build M1 spectra
    std::vector<TH1D> energy_ch;
    for ( int i = 0; i < 40; ++i ) {
        energy_ch.emplace_back(Form("M1_ch%i", i), c_str("edep, M=1 (" + det[i] + ")"), 8000, 0, 8000);
    }
    TH1D energyBEGe   ("M1_enrBEGe", "edep, M=1 (enrBEGe)", 8000, 0, 8000);
    TH1D energyEnrCoax("M1_enrCoax", "edep, M=1 (enrCOAX)", 8000, 0, 8000);
    TH1D energyNatCoax("M1_natCoax", "edep, M=1 (natCOAX)", 8000, 0, 8000);

    TH1I M1_enrBEGe_1525("M1_enrBEGe_1525", "ID with edep in range = 1525 +- 4 keV, M=1 (enrBEGe)",  40, 0, 40);
    TH1I M1_enrBEGe_1461("M1_enrBEGe_1461", "ID with edep in range = 1461 +- 4 keV, M=1 (enrBEGe)",  40, 0, 40);
    TH1I M1_enrBEGe_full("M1_enrBEGe_full", "ID with edep in range = [565,5500] keV, M=1 (enrBEGe)", 40, 0, 40);
    TH1I M1_enrCoax_1525("M1_enrCoax_1525", "ID with edep in range = 1525 +- 4 keV, M=1 (enrCoax)",  40, 0, 40);
    TH1I M1_enrCoax_1461("M1_enrCoax_1461", "ID with edep in range = 1461 +- 4 keV, M=1 (enrCoax)",  40, 0, 40);
    TH1I M1_enrCoax_full("M1_enrCoax_full", "ID with edep in range = [565,5500] keV, M=1 (enrCoax)", 40, 0, 40);
    TH1I M1_natCoax_1525("M1_natCoax_1525", "ID with edep in range = 1525 +- 4 keV, M=1 (natCoax)",  40, 0, 40);
    TH1I M1_natCoax_1461("M1_natCoax_1461", "ID with edep in range = 1461 +- 4 keV, M=1 (natCoax)",  40, 0, 40);
    TH1I M1_natCoax_full("M1_natCoax_full", "ID with edep in range = [565,5500] keV, M=1 (natCoax)", 40, 0, 40);

    TTreeReader reader; reader.SetTree(&edepCh);
    TTreeReaderValue<int>    multiplicity(reader, "multiplicity");
    TTreeReaderArray<double> energy      (reader, "energy");

    std::cout << "Processing edep... " << std::endl;
    while(reader.Next()) {
    if( *multiplicity <= 1 and *multiplicity > 0 ) {
            for ( int i = 0; i < 40; ++i ) {
                if ( energy[i] < 10000 and energy[i] > 0 ) {
                    energy_ch[i].Fill(energy[i]);

                    if (det[i].substr(0,2) == "GD") {
                        if ( energy[i] >= 1521 and energy[i] < 1529 ) M1_enrBEGe_1525.Fill(i);
                        if ( energy[i] >= 1457 and energy[i] < 1465 ) M1_enrBEGe_1461.Fill(i);
                        if ( energy[i] >= 565  and energy[i] < 5500 ) M1_enrBEGe_full.Fill(i);
                        energyBEGe.Fill(energy[i]);
                    }
                    if ( det[i].substr(0, 3) == "ANG" or
                         det[i].substr(0, 2) == "RG"  ) {
                        if ( energy[i] >= 1521 and energy[i] < 1529 ) M1_enrCoax_1525.Fill(i);
                        if ( energy[i] >= 1457 and energy[i] < 1465 ) M1_enrCoax_1461.Fill(i);
                        if ( energy[i] >= 565  and energy[i] < 5500 ) M1_enrCoax_full.Fill(i);
                        energyEnrCoax.Fill(energy[i]);
                    }
                    if ( det[i].substr(0, 3) == "GTF" ) {
                        if ( energy[i] >= 1521 and energy[i] < 1529 ) M1_natCoax_1525.Fill(i);
                        if ( energy[i] >= 1457 and energy[i] < 1465 ) M1_natCoax_1461.Fill(i);
                        if ( energy[i] >= 565  and energy[i] < 5500 ) M1_natCoax_full.Fill(i);
                        energyNatCoax.Fill(energy[i]);
                    }
                }
            }
        }
    }

    // set number of primaries in first bin
    if (verbose) std::cout << "Setting number of primaries in first bin...\n";
    long nPrim = 0;
    for (const auto& f : edepFilelist) {
        TFile froot(f.c_str());
        if (froot.GetListOfKeys()->Contains("NumberOfPrimaries")) {
            auto n = dynamic_cast<TParameter<long>*>(froot.Get("NumberOfPrimaries"));
            nPrim += n->GetVal();
        }
        else std::cout << "WARNING: NumberOfPrimaries not found in t4z- file! Please use a more recent version of gerda-ada." << std::endl;
    }
    for ( auto h : energy_ch ) h.SetBinContent(1, nPrim);
    for ( auto h : { energyBEGe, energyEnrCoax, energyNatCoax } ) h.SetBinContent(1, nPrim);

    // build M2 spectra
    TH2D M2_enrE1vsE2  ("M2_enrE1vsE2",   "edep with smaller detID, other edep, M=2 (enrAll)", 8000, 0, 8000, 8000, 0, 8000);
    TH1D M2_enrE1plusE2("M2_enrE1plusE2", "edep1 + edep2, M=2 (enrAll)",                       8000, 0, 8000);
    TH1D M2_enrE1andE2 ("M2_enrE1andE2",  "edep1 and edep2, M=2 (enrAll)",                     8000, 0, 8000);

    TH2I M2_ID1vsID2_1525("M2_ID1vsID2_1525", "ID1 with edep1+edep2 in range = 1525 +- 6 keV, ID2, M=2 (enrAll)",   40, 0, 40, 40, 0, 40);
    TH2I M2_ID1vsID2_1461("M2_ID1vsID2_1461", "ID1 with edep1+edep2 in range = 1461 +- 6 keV, ID2, M=2 (enrAll)",   40, 0, 40, 40, 0, 40);
    TH2I M2_ID1vsID2_full("M2_ID1vsID2_full", "ID1 with edep1+edep2 in range = [250,3000] keV, ID2, M=2 (enrAll)",  40, 0, 40, 40, 0, 40);
    TH2I M2_ID1vsID2_S1  ("M2_ID1vsID2_S1",   "ID1 with edep1+edep2 in range = [1405,1450] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);
    TH2I M2_ID1vsID2_S2  ("M2_ID1vsID2_S2",   "ID1 with edep1+edep2 in range = [1470,1515] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);
    TH2I M2_ID1vsID2_S3  ("M2_ID1vsID2_S3",   "ID1 with edep1+edep2 in range = [1535,1580] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);

    TH1I M2_ID1andID2_1525("M2_ID1andID2_1525", "ID1 and ID2 with edep1 and edep2 in range = 1525 +- 6 keV, M=2 (enrAll)",   40, 0, 40);
    TH1I M2_ID1andID2_1461("M2_ID1andID2_1461", "ID1 and ID2 with edep1 and edep2 in range = 1461 +- 6 keV, M=2 (enrAll)",   40, 0, 40);
    TH1I M2_ID1andID2_full("M2_ID1andID2_full", "ID1 and ID2 with edep1 and edep2 in range = [250,3000] keV, M=2 (enrAll)",  40, 0, 40);
    TH1I M2_ID1andID2_S1  ("M2_ID1andID2_S1",   "ID1 and ID2 with edep1 and edep2 in range = [1405,1450] keV, M=2 (enrAll)", 40, 0, 40);
    TH1I M2_ID1andID2_S2  ("M2_ID1andID2_S2",   "ID1 and ID2 with edep1 and edep2 in range = [1470,1515] keV, M=2 (enrAll)", 40, 0, 40);
    TH1I M2_ID1andID2_S3  ("M2_ID1andID2_S3",   "ID1 and ID2 with edep1 and edep2 in range = [1535,1580] keV, M=2 (enrAll)", 40, 0, 40);

    reader.SetTree(&coinCh);
    std::map<int,double> evMap;
    std::cout << "Processing edep... " << std::endl;
    while(reader.Next()) {
        if (*multiplicity == 2) {
            for ( int i = 0; i < 40; ++i ) {
                if ( energy[i] < 10000 and energy[i] > 0 ) evMap.insert(std::make_pair(i, energy[i]));
            }
            auto& ID1 = (*evMap.begin()).first;
            auto& ID2 = (*(evMap.begin()++)).first;
            auto sumE = evMap[0] + evMap[1];
            if ( det[ID1].substr(0,3) != "GTF" and
                 det[ID2].substr(0,3) != "GTF" ) {
                M2_enrE1vsE2.Fill(evMap[0], evMap[1]);
                M2_enrE1plusE2.Fill(sumE);
                M2_enrE1andE2.Fill(evMap[0]); M2_enrE1andE2.Fill(evMap[1]);

                if ( sumE >= 1519 and sumE < 1531 ) M2_ID1vsID2_1525.Fill(ID1, ID2);
                if ( sumE >= 1455 and sumE < 1467 ) M2_ID1vsID2_1461.Fill(ID1, ID2);
                if ( sumE >= 250  and sumE < 3000 ) M2_ID1vsID2_full.Fill(ID1, ID2);
                if ( sumE >= 1405 and sumE < 1450 ) M2_ID1vsID2_S1  .Fill(ID1, ID2);
                if ( sumE >= 1470 and sumE < 1515 ) M2_ID1vsID2_S2  .Fill(ID1, ID2);
                if ( sumE >= 1535 and sumE < 1580 ) M2_ID1vsID2_S3  .Fill(ID1, ID2);

                if ( evMap[0] >= 1519 and evMap[0] < 1531 and
                     evMap[1] >= 1519 and evMap[1] < 1531 ) {M2_ID1andID2_1525.Fill(ID1); M2_ID1andID2_1525.Fill(ID2);}
                if ( evMap[0] >= 1455 and evMap[0] < 1467 and
                     evMap[1] >= 1455 and evMap[1] < 1467 ) {M2_ID1andID2_1461.Fill(ID1); M2_ID1andID2_1461.Fill(ID2);}
                if ( evMap[0] >= 250  and evMap[0] < 3000 and
                     evMap[1] >= 250  and evMap[1] < 3000 ) {M2_ID1andID2_full.Fill(ID1); M2_ID1andID2_full.Fill(ID2);}
                if ( evMap[0] >= 1405 and evMap[0] < 1450 and
                     evMap[1] >= 1405 and evMap[1] < 1450 ) {M2_ID1andID2_S1  .Fill(ID1); M2_ID1andID2_S1  .Fill(ID2);}
                if ( evMap[0] >= 1470 and evMap[0] < 1515 and
                     evMap[1] >= 1470 and evMap[1] < 1515 ) {M2_ID1andID2_S2  .Fill(ID1); M2_ID1andID2_S2  .Fill(ID2);}
                if ( evMap[0] >= 1535 and evMap[0] < 1580 and
                     evMap[1] >= 1535 and evMap[1] < 1580 ) {M2_ID1andID2_S3  .Fill(ID1); M2_ID1andID2_S3  .Fill(ID2);}
            }
        }
    }

    // Save!
    if (verbose) std::cout << "Saving to disk...\n";
    for ( auto h : energy_ch ) h.Write();
    energyBEGe.Write();
    energyEnrCoax.Write();
    energyNatCoax.Write();

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

    std::cout << "Output saved in: " << outName << std::endl;

    return 0;
}
/*
    std::vector<std::string> ch_name = { "GD91A", "GD35B", "GD02B", "GD00B", "GD61A", "GD89B",
                                         "GD02D", "GD91C", "ANG5" , "RG1"  , "ANG3" , "GD02A",
                                         "GD32B", "GD32A", "GD32C", "GD89C", "GD61C", "GD76B",
                                         "GD00C", "GD35C", "GD76C", "GD89D", "GD00D", "GD79C",
                                         "GD35A", "GD91B", "GD61B", "ANG2" , "RG2"  , "ANG4",
                                         "GD00A", "GD02C", "GD79B", "GD91D", "GD32D", "GD89A",
                                         "ANG1" , "GTF112", "GTF32", "GTF45" };
*/
