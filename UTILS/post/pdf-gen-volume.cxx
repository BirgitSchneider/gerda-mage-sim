/* pdf-gen-volume.cxx
 *
 * Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 * Created: Sun 28 Jan 2018
 *
 */

// stl
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <map>
#include <set>

// jsoncpp
#include "json/json.h"

// ROOT
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TParameter.h"

int main( int argc, char** argv ) {

    // utilities
    auto c_str = [&](std::string s) {return s.c_str();};
    auto usage = []() {
        std::cout << "Create official pdfs for an entire volume.\n"
                  << "USAGE: ./pdf-gen [OPTIONS] <volume-name>\n\n"
                  << "OPTIONS:\n"
                  << "  required:  --destdir <path> : path to the top of the directory tree\n"
                  << "                                with the post-processed files\n"
                  << "             --ged-mapping <file> : ged-mapping.json file included\n"
                  << "                                    in gerda-mage-sim\n\n"
                  << "NOTES: Please use absolute paths!\n";
    };

    // get command line arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 6) {usage(); return 1;}
    bool verbose = false;
    auto result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    std::string pathToDestTop;
    result = std::find(args.begin(), args.end(), "--destdir");
    if (result != args.end()) pathToDestTop = *(result+1);
    else {usage(); return 1;}
    std::string gedMapFile;
    result = std::find(args.begin(), args.end(), "--ged-mapping");
    if (result != args.end()) gedMapFile = *(result+1);
    else {usage(); return 1;}
    // strip off trailing '/' character
    if (pathToDestTop.back() == '/') pathToDestTop.pop_back();
    if (pathToDestTop.back() == '/') pathToDestTop.pop_back();
    if (verbose) std::cout << "Top dest directory tree: " << pathToDestTop << std::endl;
    auto volume = *(args.end()-1);

    auto GetDirContent = [&](std::string foldName) {
        std::vector<std::string> dirlist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { std::cout << "Invalid or empty directory path!\n"; return dirlist; }
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 and r; ) {
            if (entry.d_type == 4 and
                std::string(entry.d_name) != "." and
                std::string(entry.d_name) != "..") {
                dirlist.push_back(foldName + "/" + std::string(entry.d_name));
                if (verbose) std::cout << '\t' << std::string(entry.d_name) << std::endl;
            }
        }
        return dirlist;
    };

    // get parts in volume
    if (verbose) std::cout << "\nfolders found in " << pathToDestTop + '/' + volume << " :\n";
    auto partlist = GetDirContent(pathToDestTop + '/' + volume);
    if (partlist.empty()) {std::cout << "There were problems in reading the directories in " << volume << ". Aborting...\n"; return 1;}

    // get isotopes in part
    std::map<std::string,std::vector<std::string>> isoMap;
    std::set<std::string> allIso;
    for (const auto& p : partlist ) {
        if (verbose) std::cout << "\nfolders found in " << p << " :\n";
        auto c = GetDirContent(p);
        isoMap.insert(std::make_pair(p, c));
        for (const auto& i : c ) allIso.insert(i.substr(i.find_last_of('/')+1));
    }
    if (verbose) {for (const auto& i : allIso) std::cout << i << ' '; std::cout << std::endl;}
/*
    // check if we have the isotope in each part
    if (verbose) std::cout << "Check if we have the isotope in each part...\n";
    for (const auto& i : allIso ) {
        for (auto p = isoMap.begin(); p != isoMap.end(); p++) {
            if (std::find(p->second.begin(), p->second.end(), i) == p->second.end()) {
                allIso.erase(i);
                if (verbose) std::cout << "Erased " << i << "!\n";
                break;
            }
        }
    }
 */
    // get detector data
    if (verbose) std::cout << "Retrieving detector data...\n";
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

    // loop over isotopes
    if (verbose) std::cout << "Loop over valid isotopes...\n";
    for (const auto& iso : allIso ) {

        // build final M1 spectra
        std::vector<TH1D> energy_ch;
        for ( int i = 0; i < 40; ++i ) {
            energy_ch.emplace_back(Form("M1_ch%i", i), c_str("edep, M=1 (" + det[i] + ")"), 8000, 0, 8000);
        }
        TH1D energyBEGe   ("M1_enrBEGe", "edep, M=1 (enrBEGe)", 8000, 0, 8000);
        TH1D energyEnrCoax("M1_enrCoax", "edep, M=1 (enrCOAX)", 8000, 0, 8000);
        TH1D energyNatCoax("M1_natCoax", "edep, M=1 (natCOAX)", 8000, 0, 8000);

        TH1I M1_all_1525("M1_all_1525", "ID with edep in range = 1525 +- 4 keV, M=1 (all)",  40, 0, 40);
        TH1I M1_all_1461("M1_all_1461", "ID with edep in range = 1461 +- 4 keV, M=1 (all)",  40, 0, 40);
        TH1I M1_all_full("M1_all_full", "ID with edep in range = [565,5500] keV, M=1 (all)", 40, 0, 40);

        // build final M2 spectra
        TH2D M2_enrE1vsE2  ("M2_enrE1vsE2",   "edep with smaller detID, other edep, M=2 (enrAll)", 8000, 0, 8000, 8000, 0, 8000);
        TH1D M2_enrE1plusE2("M2_enrE1plusE2", "edep1 + edep2, M=2 (enrAll)",                       8000, 0, 8000);
        TH1D M2_enrE1andE2 ("M2_enrE1andE2",  "edep1 and edep2, M=2 (enrAll)",                     8000, 0, 8000);

        TH2I M2_ID1vsID2_1525("M2_ID1vsID2_1525", "ID1 with edep1+edep2 in range = 1525 +- 6 keV, ID2, M=2 (enrAll)",   40, 0, 40, 40, 0, 40);
        TH2I M2_ID1vsID2_1461("M2_ID1vsID2_1461", "ID1 with edep1+edep2 in range = 1461 +- 6 keV, ID2, M=2 (enrAll)",   40, 0, 40, 40, 0, 40);
        TH2I M2_ID1vsID2_full("M2_ID1vsID2_full", "ID1 with edep1+edep2 in range = [250,3000] keV, ID2, M=2 (enrAll)",  40, 0, 40, 40, 0, 40);
        TH2I M2_ID1vsID2_S1  ("M2_ID1vsID2_S1",   "ID1 with edep1+edep2 in range = [1405,1450] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);
        TH2I M2_ID1vsID2_S2  ("M2_ID1vsID2_S2",   "ID1 with edep1+edep2 in range = [1470,1515] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);
        TH2I M2_ID1vsID2_S3  ("M2_ID1vsID2_S3",   "ID1 with edep1+edep2 in range = [1535,1580] keV, ID2, M=2 (enrAll)", 40, 0, 40, 40, 0, 40);

        TH1I M2_ID1andID2_1525("M2_ID1andID2_1525", "ID1 and ID2 with edep1+edep2 in range = 1525 +- 6 keV, M=2 (enrAll)",   40, 0, 40);
        TH1I M2_ID1andID2_1461("M2_ID1andID2_1461", "ID1 and ID2 with edep1+edep2 in range = 1461 +- 6 keV, M=2 (enrAll)",   40, 0, 40);
        TH1I M2_ID1andID2_full("M2_ID1andID2_full", "ID1 and ID2 with edep1+edep2 in range = [250,3000] keV, M=2 (enrAll)",  40, 0, 40);
        TH1I M2_ID1andID2_S1  ("M2_ID1andID2_S1",   "ID1 and ID2 with edep1+edep2 in range = [1405,1450] keV, M=2 (enrAll)", 40, 0, 40);
        TH1I M2_ID1andID2_S2  ("M2_ID1andID2_S2",   "ID1 and ID2 with edep1+edep2 in range = [1470,1515] keV, M=2 (enrAll)", 40, 0, 40);
        TH1I M2_ID1andID2_S3  ("M2_ID1andID2_S3",   "ID1 and ID2 with edep1+edep2 in range = [1535,1580] keV, M=2 (enrAll)", 40, 0, 40);

        // now loop over parts
        bool missingpdf = false;
        bool processCoin = true;
        int nPrimCoin = 0;
        int nPrimEdep = 0;
        for (const auto& prt : partlist) {

            // open input pdf- file
            auto file = prt + '/' + iso + '/' + "pdf-" + volume + '-' + prt.substr(prt.find_last_of('/')+1) + '-' + iso + ".root";
            if (verbose) std::cout << "Opening " << file << "...\n";
            TFile inFile(file.c_str());
            if (!inFile.IsOpen()) {std::cout << file << " seems not to be there. Aborting...\n"; missingpdf = true; break;}

            // retrieve histograms
            std::vector<TH1D*> tmp_energy_ch;
            for ( int i = 0; i < 40; ++i ) {
                tmp_energy_ch.push_back(dynamic_cast<TH1D*>(inFile.Get(Form("M1_ch%i", i))));
                if (!tmp_energy_ch[i]) std::cout << "Problems retrieving M1_ch" << i << "!\n";
            }

            TH1D* tmp_energyBEGe        = dynamic_cast<TH1D*>(inFile.Get("M1_enrBEGe"));        if (!tmp_energyBEGe)                    std::cout << "Problems retrieving M1_energyBEGe!\n";
            TH1D* tmp_energyEnrCoax     = dynamic_cast<TH1D*>(inFile.Get("M1_enrCoax"));        if (!tmp_energyEnrCoax)                 std::cout << "Problems retrieving M1_energyEnrCoax!\n";
            TH1D* tmp_energyNatCoax     = dynamic_cast<TH1D*>(inFile.Get("M1_natCoax"));        if (!tmp_energyNatCoax)                 std::cout << "Problems retrieving M1_energyNatCoax!\n";

            TH1I* tmp_M1_all_1525       = dynamic_cast<TH1I*>(inFile.Get("M1_all_1525"));       if (!tmp_M1_all_1525)                   std::cout << "Problems retrieving M1_all_1525!\n";
            TH1I* tmp_M1_all_1461       = dynamic_cast<TH1I*>(inFile.Get("M1_all_1461"));       if (!tmp_M1_all_1461)                   std::cout << "Problems retrieving M1_all_1461!\n";
            TH1I* tmp_M1_all_full       = dynamic_cast<TH1I*>(inFile.Get("M1_all_full"));       if (!tmp_M1_all_full)                   std::cout << "Problems retrieving M1_all_full!\n";

            TH2D* tmp_M2_enrE1vsE2      = dynamic_cast<TH2D*>(inFile.Get("M2_enrE1vsE2"));      if (verbose and !tmp_M2_enrE1vsE2)      std::cout << "Problems retrieving M2_enrE1vsE2!\n";
            TH1D* tmp_M2_enrE1plusE2    = dynamic_cast<TH1D*>(inFile.Get("M2_enrE1plusE2"));    if (verbose and !tmp_M2_enrE1plusE2)    std::cout << "Problems retrieving M2_enrE1plusE2!\n";
            TH1D* tmp_M2_enrE1andE2     = dynamic_cast<TH1D*>(inFile.Get("M2_enrE1andE2"));     if (verbose and !tmp_M2_enrE1andE2)     std::cout << "Problems retrieving M2_enrE1andE2!\n";
            TH2I* tmp_M2_ID1vsID2_1525  = dynamic_cast<TH2I*>(inFile.Get("M2_ID1vsID2_1525"));  if (verbose and !tmp_M2_ID1vsID2_1525)  std::cout << "Problems retrieving M2_ID1vsID2_1525!\n";
            TH2I* tmp_M2_ID1vsID2_1461  = dynamic_cast<TH2I*>(inFile.Get("M2_ID1vsID2_1461"));  if (verbose and !tmp_M2_ID1vsID2_1461)  std::cout << "Problems retrieving M2_ID1vsID2_1461!\n";
            TH2I* tmp_M2_ID1vsID2_full  = dynamic_cast<TH2I*>(inFile.Get("M2_ID1vsID2_full"));  if (verbose and !tmp_M2_ID1vsID2_full)  std::cout << "Problems retrieving M2_ID1vsID2_full!\n";
            TH2I* tmp_M2_ID1vsID2_S1    = dynamic_cast<TH2I*>(inFile.Get("M2_ID1vsID2_S1"));    if (verbose and !tmp_M2_ID1vsID2_S1)    std::cout << "Problems retrieving M2_ID1vsID2_S1!\n";
            TH2I* tmp_M2_ID1vsID2_S2    = dynamic_cast<TH2I*>(inFile.Get("M2_ID1vsID2_S2"));    if (verbose and !tmp_M2_ID1vsID2_S2)    std::cout << "Problems retrieving M2_ID1vsID2_S2!\n";
            TH2I* tmp_M2_ID1vsID2_S3    = dynamic_cast<TH2I*>(inFile.Get("M2_ID1vsID2_S3"));    if (verbose and !tmp_M2_ID1vsID2_S3)    std::cout << "Problems retrieving M2_ID1vsID2_S3!\n";

            TH1I* tmp_M2_ID1andID2_1525 = dynamic_cast<TH1I*>(inFile.Get("M2_ID1andID2_1525")); if (verbose and !tmp_M2_ID1andID2_1525) std::cout << "Problems retrieving M2_ID1andID2_1525!\n";
            TH1I* tmp_M2_ID1andID2_1461 = dynamic_cast<TH1I*>(inFile.Get("M2_ID1andID2_1461")); if (verbose and !tmp_M2_ID1andID2_1461) std::cout << "Problems retrieving M2_ID1andID2_1461!\n";
            TH1I* tmp_M2_ID1andID2_full = dynamic_cast<TH1I*>(inFile.Get("M2_ID1andID2_full")); if (verbose and !tmp_M2_ID1andID2_full) std::cout << "Problems retrieving M2_ID1andID2_full!\n";
            TH1I* tmp_M2_ID1andID2_S1   = dynamic_cast<TH1I*>(inFile.Get("M2_ID1andID2_S1"));   if (verbose and !tmp_M2_ID1andID2_S1)   std::cout << "Problems retrieving M2_ID1andID2_S1!\n";
            TH1I* tmp_M2_ID1andID2_S2   = dynamic_cast<TH1I*>(inFile.Get("M2_ID1andID2_S2"));   if (verbose and !tmp_M2_ID1andID2_S2)   std::cout << "Problems retrieving M2_ID1andID2_S2!\n";
            TH1I* tmp_M2_ID1andID2_S3   = dynamic_cast<TH1I*>(inFile.Get("M2_ID1andID2_S3"));   if (verbose and !tmp_M2_ID1andID2_S3)   std::cout << "Problems retrieving M2_ID1andID2_S3!\n";

            nPrimCoin += dynamic_cast<TParameter<long>*>(inFile.Get("NumberOfPrimariesCoin"))->GetVal();
            nPrimEdep += dynamic_cast<TParameter<long>*>(inFile.Get("NumberOfPrimariesEdep"))->GetVal();

            if(!tmp_M2_ID1vsID2_1525) processCoin = false;

            // Add!
            for ( int i = 0; i < 40; ++i ) energy_ch[i].Add(tmp_energy_ch[i]);

            energyBEGe     .Add(tmp_energyBEGe);
            energyEnrCoax  .Add(tmp_energyEnrCoax);
            energyNatCoax  .Add(tmp_energyNatCoax);

            M1_all_1525    .Add(tmp_M1_all_1525);
            M1_all_1461    .Add(tmp_M1_all_1461);
            M1_all_full    .Add(tmp_M1_all_full);

            if (processCoin) {
                 M2_enrE1vsE2     .Add(tmp_M2_enrE1vsE2);
                 M2_enrE1plusE2   .Add(tmp_M2_enrE1plusE2);
                 M2_enrE1andE2    .Add(tmp_M2_enrE1andE2);

                 M2_ID1vsID2_1525 .Add(tmp_M2_ID1vsID2_1525);
                 M2_ID1vsID2_1461 .Add(tmp_M2_ID1vsID2_1461);
                 M2_ID1vsID2_full .Add(tmp_M2_ID1vsID2_full);
                 M2_ID1vsID2_S1   .Add(tmp_M2_ID1vsID2_S1);
                 M2_ID1vsID2_S2   .Add(tmp_M2_ID1vsID2_S2);
                 M2_ID1vsID2_S3   .Add(tmp_M2_ID1vsID2_S3);

                 M2_ID1andID2_1525.Add(tmp_M2_ID1andID2_1525);
                 M2_ID1andID2_1461.Add(tmp_M2_ID1andID2_1461);
                 M2_ID1andID2_full.Add(tmp_M2_ID1andID2_full);
                 M2_ID1andID2_S1  .Add(tmp_M2_ID1andID2_S1);
                 M2_ID1andID2_S2  .Add(tmp_M2_ID1andID2_S2);
                 M2_ID1andID2_S3  .Add(tmp_M2_ID1andID2_S3);
            }

        }
        if (missingpdf) break;

        // Save!
        std::string outName = pathToDestTop + '/' + volume + '/' +
                              "pdf-" + volume + '-' + iso + ".root";
        TFile outfile(outName.c_str(), "RECREATE");

        TParameter<long> nPrimCoinT("NumberOfPrimariesCoin",nPrimCoin);
        TParameter<long> nPrimEdepT("NumberOfPrimariesEdep",nPrimEdep);

        if (verbose) std::cout << "Saving to disk...\n";
        for ( auto h : energy_ch ) h.Write();
        energyBEGe.Write();
        energyEnrCoax.Write();
        energyNatCoax.Write();

        M1_all_1525.Write();
        M1_all_1461.Write();
        M1_all_full.Write();

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
             nPrimCoinT.Write();
        }
        nPrimEdepT.Write();
        std::cout << "Output saved in: " << outName << std::endl;
    }
    return 0;
}