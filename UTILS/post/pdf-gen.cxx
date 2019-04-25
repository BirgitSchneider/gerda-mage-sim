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

enum logLevel {debug, info, warning, error};
std::ostream& glog(logLevel lvl);

int main( int argc, char** argv ) {

    auto usage = [](){
        std::cout << "Create official pdfs from tier4rized files.\n"
                  << "USAGE: pdf-gen --config <pdf-gen-settings.json> --destdir "
                  << "<gerda-pdfs/cycle> <volume>/<part>/<isotope>\n";
        exit(1);
    };

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);

    if (args.size() < 5) usage();

    std::string configs;
    std::string destdir;

    auto result = std::find(args.begin(), args.end(), "--config");
    if (result != args.end()) configs = *(result+1);
    else usage();

    result = std::find(args.begin(), args.end(), "--destdir");
    if (result != args.end()) destdir = *(result+1);
    else usage();

    auto path_to_isotope = args[args.size()-1];

    Json::Value cfg;
    std::ifstream fconfigs(configs.c_str());
    fconfigs >> cfg;

    auto gms_path          = cfg["gerda-mage-sim"].asString();
    auto gerda_meta        = cfg["gerda-metadata"].asString();
    bool verbose           = cfg.get("debug", false).asBool();
    auto mapping_file      = cfg["ged-mapping"].asString();
    auto calib_pdfs_file   = cfg["calib-pdf-settings"].asString();
    bool incNatCoax        = cfg.get("include-nat-coax-in-M2-spectra", false).asBool();
    bool applyLArVetoCut   = cfg.get("apply-LAr-veto-cut", false).asBool();
    bool computeM2spectra  = cfg.get("compute-M2-spectra", true).asBool();

    if (verbose) {
        glog(debug) << "gerda-mage-sim: " << gms_path << std::endl;
        glog(debug) << "destination: " << destdir << std::endl;
        glog(debug) << "selected simulation: " << path_to_isotope << std::endl;
        glog(debug) << "include natCoax in M2 Spectra: " << (incNatCoax ? "yes" : "no")  << std::endl;
        glog(debug) << "apply LAr veto cut: " << (applyLArVetoCut ? "yes" : "no") << std::endl;
        glog(debug) << "compute M2 spectra: " << (computeM2spectra ? "yes" : "no") << std::endl;
    }

    if (verbose) glog(debug) << "paths found in JSON config:\n";
    for (auto s : {&gerda_meta, &calib_pdfs_file, &mapping_file}) {
        if (s->front() != '/') *s = gms_path + "/" + *s;
        if (verbose) glog(debug) << "  " << *s << std::endl;
    }

    if (path_to_isotope.find("chanwise") != std::string::npos) {
        glog(error) << "These t4z files won't be processed because the original simulations "
                    << "are separated in channels. This will create problems in PDFs building. "
                    << "Aborting...";
        return 1;
    }
    // lambda function to get t4z-*.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { glog(error) << foldName << ": invalid or empty directory path!\n"; return filelist; }
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 and r; ) {
            if (entry.d_type == 8 and
                std::string(entry.d_name).find("t4z-") != std::string::npos and
                std::string(entry.d_name).find(".root") != std::string::npos) {
                filelist.push_back(foldName + "/" + std::string(entry.d_name));
                if (verbose) glog(debug) << "  " << std::string(entry.d_name) << std::endl;
            }
        }
        return filelist;
    };

    // get t4z- edep files
    auto fold = destdir + '/' + path_to_isotope + "/edep";
    if (verbose) glog(debug) << "edep t4z- files found in " << fold << " :\n";
    auto edepFilelist = GetContent(fold);
    if (edepFilelist.empty()) {
        glog(error) << "there were problems in reading the t4z-edep files. Aborting...\n";
        return 1;
    }
    // join all t4z- files in same tree
    TChain edepCh("fTree");
    for ( auto& f : edepFilelist ) edepCh.Add(f.c_str());

    // get t4z- coin file
    fold = destdir + '/' + path_to_isotope + "/coin";
    if (verbose) glog(debug) << "coin t4z- files found in " << fold << " :\n";
    auto coinFilelist = GetContent(fold);
    bool processCoin = true;
    if (coinFilelist.empty()) {
        glog(warning) << "there were problems in reading the t4z- coin files, They won't be processed\n";
        processCoin = false;
    }
    // join all t4z- files in same tree
    TChain coinCh("fTree");
    for ( auto& f : coinFilelist ) coinCh.Add(f.c_str());

    // strip out folders in path_to_isotope to build up final pdf- filename
    std::vector<std::string> items;
    std::string tmp = path_to_isotope;
    for (int j = 0; j < 2; j++ ) {
        items.push_back(tmp.substr(tmp.find_last_of('/')+1));
        tmp.erase(tmp.find_last_of('/'), tmp.back());
    }
    items.push_back(tmp);

    // read in detector mapping
    Json::Value mapping;
    std::ifstream fGedMap(mapping_file);
    if (!fGedMap.is_open()) {glog(error) << "ged mapping json file not found!\n"; return 1;}
    fGedMap >> mapping; fGedMap.close();
    std::map<int,std::string> det;
    Json::Value::Members detinfo = mapping["mapping"].getMemberNames();
    for (const auto & d : detinfo) {
        det[mapping["mapping"][d]["channel"].asInt()] = d;
    }
    // if (verbose) {
    //     glog(debug) << "detectors:\n";
    //     for (const auto& i : det) glog(debug) << "ch" << i.first << '\t' << i.second << std::endl;
    // }

    processCoin *= computeM2spectra;

    // build M1 spectra
    if (verbose) glog(debug) << "building M1 spectra...\n";
    std::vector<TH1D> energy_ch;
    for ( int i = 0; i < 40; ++i ) {
        energy_ch.emplace_back(Form("M1_ch%i", i), ("edep, M=1 (" + det[i] + ")").c_str(), 8000, 0, 8000);
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
    TTreeReaderValue<int>    isLArVetoed (reader, "isLArVetoed");

    unsigned int M1_datasetMinusOne = 0; // M1: count events thrown away due to dataset -1
    unsigned int M2_datasetMinusOne = 0; // M2: -"-

    // loop
    glog(info) << "processing edep... " << edepCh.GetEntries() << " entries\n" << std::flush;
    edepCh.LoadTree(0);
    reader.SetTree(&edepCh);
    while ( reader.Next() ) {
        // apply LAr veto cut
        if ( applyLArVetoCut ) { if ( *isLArVetoed > 0 ) continue; }
        // select correct multiplicity
        if ( *multiplicity == 1 ) {
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
    if (verbose) glog(debug) << "There were " << M1_datasetMinusOne << " multiplicity 1 events in datasetID = -1 \n";

    // getting number of primaries
    if (verbose) glog(debug) << "Getting number of primaries...\n";
    bool problems = false;
    Long64_t nPrimEdep = 0;
    for (const auto& f : edepFilelist) {
        TFile froot(f.c_str());
        if (froot.GetListOfKeys()->Contains("NumberOfPrimaries")) {
            auto nPrimEdep_loc = dynamic_cast<TParameter<Long64_t>*>(froot.Get("NumberOfPrimaries"))->GetVal();
            if (nPrimEdep_loc == 0) {
                glog(warning) << "number of primaries in " << f << " is zero!\n";
                problems = true;
            }
            nPrimEdep += nPrimEdep_loc;
        }
        else {
            glog(warning) << "NumberOfPrimaries not found in " << f << "!\n";
            problems = true;
        }
    }
    // set number of primaries to zero in case of failures
    if (problems) {
        glog(warning) << "there where some problems, setting number of primaries to zero...\n";
        nPrimEdep = 0;
    }
    if (verbose) glog(debug) << "Number of edep primaries: " << nPrimEdep << std::endl;

    // build M2 spectra
    if (verbose) glog(debug) << "building M2 spectra...\n";
    std::string m2_det = "enrAll"; if (incNatCoax) m2_det = "all"; const char * m2_detectors = m2_det.c_str();
    TH2D M2_enrE1vsE2  ("M2_enrE1vsE2",   Form("edep with smaller detID, other edep, M=2 (%s)",m2_detectors), 8000, 0, 8000, 8000, 0, 8000);
    TH1D M2_enrE1plusE2("M2_enrE1plusE2", Form("edep1 + edep2, M=2 (%s)",m2_detectors),                       8000, 0, 8000);
    TH1D M2_enrE1andE2 ("M2_enrE1andE2",  Form("edep1 and edep2, M=2 (%s)",m2_detectors),                     8000, 0, 8000);

    TH2D M2_ID1vsID2_1525("M2_ID1vsID2_1525", Form("ID1 with edep1+edep2 in range = 1525 +- 6 keV, ID2, M=2 (%s)",m2_detectors),   40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_1461("M2_ID1vsID2_1461", Form("ID1 with edep1+edep2 in range = 1461 +- 6 keV, ID2, M=2 (%s)",m2_detectors),   40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_full("M2_ID1vsID2_full", Form("ID1 with edep1+edep2 in range = [250,3000] keV, ID2, M=2 (%s)",m2_detectors),  40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_S1  ("M2_ID1vsID2_S1",   Form("ID1 with edep1+edep2 in range = [1405,1450] keV, ID2, M=2 (%s)",m2_detectors), 40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_S2  ("M2_ID1vsID2_S2",   Form("ID1 with edep1+edep2 in range = [1470,1515] keV, ID2, M=2 (%s)",m2_detectors), 40, 0, 40, 40, 0, 40);
    TH2D M2_ID1vsID2_S3  ("M2_ID1vsID2_S3",   Form("ID1 with edep1+edep2 in range = [1535,1580] keV, ID2, M=2 (%s)",m2_detectors), 40, 0, 40, 40, 0, 40);

    TH1D M2_ID1andID2_1525("M2_ID1andID2_1525", Form("ID1 and ID2 with edep1+edep2 in range = 1525 +- 6 keV, M=2 (%s)",m2_detectors),   40, 0, 40);
    TH1D M2_ID1andID2_1461("M2_ID1andID2_1461", Form("ID1 and ID2 with edep1+edep2 in range = 1461 +- 6 keV, M=2 (%s)",m2_detectors),   40, 0, 40);
    TH1D M2_ID1andID2_full("M2_ID1andID2_full", Form("ID1 and ID2 with edep1+edep2 in range = [250,3000] keV, M=2 (%s)",m2_detectors),  40, 0, 40);
    TH1D M2_ID1andID2_S1  ("M2_ID1andID2_S1",   Form("ID1 and ID2 with edep1+edep2 in range = [1405,1450] keV, M=2 (%s)",m2_detectors), 40, 0, 40);
    TH1D M2_ID1andID2_S2  ("M2_ID1andID2_S2",   Form("ID1 and ID2 with edep1+edep2 in range = [1470,1515] keV, M=2 (%s)",m2_detectors), 40, 0, 40);
    TH1D M2_ID1andID2_S3  ("M2_ID1andID2_S3",   Form("ID1 and ID2 with edep1+edep2 in range = [1535,1580] keV, M=2 (%s)",m2_detectors), 40, 0, 40);

    Long64_t nPrimCoin = 0;  // number of primaries for coincidences
    int badevents = 0;  // counter for events with multiplicity 2 but only 0 or 1 energy deposition
    if (processCoin) {
        // object to store events as a (det_id,energy) pair
        std::map<int,double> evMap;
        glog(info) << "processing coin... " << coinCh.GetEntries() << "entries\n" << std::flush;
        coinCh.LoadTree(0);
        reader.SetTree(&coinCh);
        while ( reader.Next() ) {
            // clear object from previous loop iteration
            evMap.clear();

            // apply LAr veto cut
            if ( applyLArVetoCut ) { if ( *isLArVetoed > 0 ) continue; }
            // select multiplicity
            if ( *multiplicity == 2 ) {
                // loop over detector ids
                for ( int i = 0; i < 40; ++i ) {
                    // skip if detector is in dataset -1
                    if ( datasetID[i] == -1 ) { M2_datasetMinusOne++; continue; }

                    // select event if it has a valid energy
                    if ( energy[i] > 0 ) evMap.insert(std::make_pair(i, energy[i]));
                }
                // in case one detector has dataset -1, evMap has less than 2 entires
                if ( evMap.size() != 2 ) {
                    //if (verbose) glog(debug) << "WARNING: Found " << evMap.size() << " events instead of 2! This should not happen!\n";
                    //if (verbose) glog(debug) << "WARNING: But can happen for coincidence events in GD02D!\n";
                    badevents++;
                    continue;
                }
                // comfortable referencies
                auto& ID1 = (*evMap.begin())    .first;
                auto& ID2 = (*(++evMap.begin())).first;
                auto& E1  = (*evMap.begin())    .second;
                auto& E2  = (*(++evMap.begin())).second;
                /*
                glog(debug) << "Multiplicity: " << *multiplicity << std::endl;
                glog(debug) << ID1 << '\t' << E1 << std::endl;
                glog(debug) << ID2 << '\t' << E2 << std::endl;
                glog(debug) << "-------------\n";
                */
                // do not include events that contain a trigger in an AC channel
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
        if (verbose) glog(debug) << "there were " << badevents
                                 << " events with multiplicity = 2 but a different number "
                                 << "of edeps > 0 found in the tree\n";
        if (verbose) glog(debug) << "there were " << M2_datasetMinusOne
                                 << " multiplicity 2 events in datasetID = -1 \n";

        if (verbose) glog(debug) << "getting number of primaries...\n";
        problems = false;
        for (const auto& f : coinFilelist) {
            TFile froot(f.c_str());
            if (froot.GetListOfKeys()->Contains("NumberOfPrimaries")) {
                auto nPrimCoin_loc = dynamic_cast<TParameter<Long64_t>*>(froot.Get("NumberOfPrimaries"))->GetVal();
                if (nPrimCoin_loc == 0) {
                    glog(warning) << "number of primaries in " << f << " is zero!\n";
                    problems = true;
                }
                nPrimCoin += nPrimCoin_loc;
            }
           else {
               glog(warning) << "NumberOfPrimaries not found in " << f << "!\n";
               problems = true;
           }
        }
        if (problems) {
            glog(warning) << "there where some problems, setting number of primaries to zero...\n";
            nPrimCoin = 0;
        }
        if (verbose) glog(debug) << "number of coin primaries: " << nPrimCoin << std::endl;
    }

    // Save!
    // build output filename
    std::string outName = destdir + '/' + path_to_isotope + '/' +
        "pdf-" + items[2] + '-' + items[1] + '-' + items[0];
    // spacial naming for calib PDFs
    if (items[2] == "calib") {
        Json::Value calcfg;
        std::ifstream fcal(calib_pdfs_file.c_str());
        fcal >> calcfg;
        auto pos = items[1].substr(items[1].find_last_of('_')+1);
        auto src = items[1].substr(items[1].find_first_of('_')+1, 2);
        outName += "-" + calcfg["calib"][items[0]][src][pos]["id"].asString();
    }
    // special naming if larveto
    if (applyLArVetoCut) outName += "-larveto";
    outName += ".root";
    TFile outfile(outName.c_str(), "RECREATE");

    if (verbose) glog(debug) << "saving to disk...\n";
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
    glog(info) << "output saved in: " << outName << std::endl;

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
