/* gen-spectra.cxx
 *
 * Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 * Created: Tue 12 Nov 2017, 12:03 a.m.
 *
 * Compile with:
 * $ g++ $(root-config --cflags --libs) -lTreePlayer -o gen-spectra gen-spectra.cxx
 *
 */

// stl
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TH1D.h"
#include "TString.h"
#include "TParameter.h"

// other
#include "progressbar/ProgressBar.h"

int main( int argc, char** argv ) {

    int M = 100;
    int max_kev = 7500;
    bool silent = true;

    if ( argc == 1 ) {
        std::cout << "Create default energy spectra from a tier4rized file.\n"
                  << "USAGE: $ ./gen-spectra [OPTIONS] [T4IZED FILE]\n"
                  << "\n"
                  << "OPTIONS: -m <max_multiplicity>   : set the upper cut value for\n"
                  << "                                   the multiplicity of one event\n"
                  << "         -v                      : display nice progress bar\n";
        return -1;
    }

    std::vector<std::string> args;
    for ( int i = 0; i < argc; ++i ) args.emplace_back( argv[i] );

    std::vector<std::string>::iterator result = std::find( args.begin(), args.end(), "-m" );
    if ( result != args.end() ) M = std::stoi(*(result+1));
 
    result = std::find( args.begin(), args.end(), "-v" );
    if ( result != args.end() ) silent = false;

    std::string name;
    for( int i = 1; i < argc; ++i ) {
        if( args[i].find("t4z-") != std::string::npos ) name = args[i];
    }

    TFile infile( name.c_str(), "READ" );
    TTree* fTree = dynamic_cast<TTree*>(infile.Get("fTree"));

    name.replace(name.rfind('/')+1, 4, "spc-");
    TFile outfile(name.c_str(), "RECREATE");

    std::vector<std::string> ch_name = { "GD91A", "GD35B", "GD02B", "GD00B", "GD61A", "GD89B",
                                         "GD02D", "GD91C", "ANG5" , "RG1"  , "ANG3" , "GD02A",
                                         "GD32B", "GD32A", "GD32C", "GD89C", "GD61C", "GD76B",
                                         "GD00C", "GD35C", "GD76C", "GD89D", "GD00D", "GD79C",
                                         "GD35A", "GD91B", "GD61B", "ANG2" , "RG2"  , "ANG4",
                                         "GD00A", "GD02C", "GD79B", "GD91D", "GD32D", "GD89A",
                                         "ANG1" , "GTF112", "GTF32", "GTF45" };

    std::vector<TH1D> energy_ch;
    for ( int i = 0; i < 40; ++i ) {
        energy_ch.emplace_back(Form("ch%i", i), ch_name[i].c_str(), max_kev, 0, max_kev);
    }
    TH1D energyBEGe   ("BEGe"   , "BEGe"   , max_kev, 0, max_kev);
    TH1D energyEnrCOAX("enrCOAX", "enrCOAX", max_kev, 0, max_kev);
    TH1D energyNatCOAX("natCOAX", "natCOAX", max_kev, 0, max_kev);
    TH1D energyEnrAll ("enrAll",  "enrAll",  max_kev, 0, max_kev);

    TTreeReader treereader; treereader.SetTree(fTree);
    TTreeReaderValue<int>    multiplicity(treereader, "multiplicity");
    TTreeReaderArray<double> energy      (treereader, "energy");

    ProgressBar bar(fTree->GetEntries());
    std::cout << "Processing: " << std::endl;
    while(treereader.Next()) {
    if (!silent) bar.Update();
    if( *multiplicity <= M && *multiplicity > 0 ) {
            for ( int i = 0; i < 40; ++i ) {
                if ( energy[i] < 10000 and energy[i] > 0 ) {
                    energy_ch[i].Fill(energy[i]);
                }
            }
        }
    }

    for ( int i = 0; i < 40; ++i ) {
        if ( ch_name[i].substr(0, 2) == "GD"  ) {
            energyBEGe.Add(&energy_ch[i]);
            energyEnrAll.Add(&energy_ch[i]);
        }
        if ( ch_name[i].substr(0, 3) == "ANG" or
             ch_name[i].substr(0, 2) == "RG"  ) {
            energyEnrCOAX.Add(&energy_ch[i]);
            energyEnrAll.Add(&energy_ch[i]);
        }
        if ( ch_name[i].substr(0, 3) == "GTF" ) energyNatCOAX.Add(&energy_ch[i]);
    }

    // set number of primaries in first bin
    if (infile.GetListOfKeys()->Contains("NumberOfPrimaries")) {
        auto nPrim = dynamic_cast<TParameter<long>*>(infile.Get("NumberOfPrimaries"));
        for ( auto h : energy_ch ) h.SetBinContent(1, nPrim->GetVal());
        for ( auto h : { energyBEGe, energyEnrCOAX, energyNatCOAX, energyEnrAll } ) h.SetBinContent(1, nPrim->GetVal());
    }
    else std::cout << "WARNING: NumberOfPrimaries not found in t4z- file! Please use a more recent version of gerda-ada." << std::endl;

    for ( auto h : energy_ch ) h.Write();
    energyBEGe.Write();
    energyEnrCOAX.Write();
    energyNatCOAX.Write();
    energyEnrAll.Write();

    std::cout << "Output saved in: " << name << std::endl;

    return 0;
}
