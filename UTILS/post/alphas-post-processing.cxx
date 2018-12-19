/*
 * Author : K.v.Sturm based on a script of V.Wagner
 * Note   : See /lfs/l3/gerda/vwagner/Simulations/AlphaSimPhaseII/
 * Usage  : ./alphas-post-processing -i <indir> -o <outfile>
 *
 */

// c/c++
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <getopt.h>

// root
#include "TChain.h"
#include "TH1D.h"
#include "TFile.h"

// own
#include "ReadDir.h"
#include "ProgressBar.h"


using namespace std;

void Usage();

int main( int argc, char* argv[] )
{
    float Ele      = 0; // adcenergy in keV
    float Ehe      = 8000.;
    float Ebinning = 1;
    int nEbins     = (int) (Ehe-Ele)/Ebinning;

    float sourcepos = 1.4895; // in cm
    float deadlayer = 100; // in nm

    string  inputDir; bool indir_defined = false;
    TString outputFile = "default-alpha-post.root";

    // read in options from commandline
    int choice = 0;

    static struct option long_options[] =
    {
        { "help",    no_argument,       0,   'h' },
        { "indir",   required_argument, 0,   'i' },
        { "outfile", required_argument, 0,   'o' },
        { 0, 0, 0, 0 }
    };

    int option_index = 0;

    while( (choice = getopt_long(argc, argv, "hi:o:", long_options, &option_index)) != -1 )
    {
        switch (choice)
        {
            case 'h':
                Usage();
                break;
            case 'i':
                inputDir      = optarg;
                indir_defined = true;
                break;
            case 'o':
                outputFile = optarg;
                break;
            default:
                cout << "Unknown option: " << choice << endl;
                Usage();
                break;
        }
    }

    if( !indir_defined )
    {
        cout << "No input directory defined. Terminating program." << endl;
        Usage();
        exit( EXIT_FAILURE );
    }

    ReadDir::GetContent_R(inputDir, "*.root");
    ReadDir::SetVerbose(true);
    auto filelist = ReadDir::GetFiles();
    int nfiles = filelist.size();

    TChain* ch = new TChain("fTree");
    for ( int f = 0; f < nfiles; f++ ) ch -> Add( filelist.at(f).c_str() );

    cout << "Added " << nfiles << " MC files"         << endl;

    float Etot[11]    = {0.};
    float adcEnergy   = 0;
    float ene_vol[12] = {0.};

    ch->ResetBranchAddresses();
    ch->SetBranchAddress("adcEnergy",        &adcEnergy  );
    ch->SetBranchAddress("ene_activeCrystalLogical", &ene_vol[11]);
    ch->SetBranchAddress("ene_pplus",        &ene_vol[10]);
    ch->SetBranchAddress("ene_dl100nm",          &ene_vol[0] );
    ch->SetBranchAddress("ene_dl200nm",          &ene_vol[1] );
    ch->SetBranchAddress("ene_dl300nm",          &ene_vol[2] );
    ch->SetBranchAddress("ene_dl400nm",          &ene_vol[3] );
    ch->SetBranchAddress("ene_dl500nm",          &ene_vol[4] );
    ch->SetBranchAddress("ene_dl600nm",          &ene_vol[5] );
    ch->SetBranchAddress("ene_dl700nm",          &ene_vol[6] );
    ch->SetBranchAddress("ene_dl800nm",          &ene_vol[7] );
    ch->SetBranchAddress("ene_dl900nm",          &ene_vol[8] );
    ch->SetBranchAddress("ene_dl1000nm",         &ene_vol[9] );

    TH1D** hist = new TH1D*[11];
    for ( int j = 0; j < 11; j++ )
        hist[j] = new TH1D(Form("hist_dl%gnm",j*deadlayer), "", nEbins, Ele, Ehe);

    int nentries = ch->GetEntries();

    cout << "Processing " << nentries << " events... this may take a while..." << endl;

    ProgressBar bar(nentries, '#', false);

    for ( int i = 0; i < nentries; i++ )
    {
        bar.Update();
        ch->GetEntry(i);

        if (!adcEnergy) continue;

        for ( int ii = 0; ii < 11; ii++ )
        {
            Etot[ii] = 0;
            for ( int j = ii; j < 12; j++ ) Etot[ii] += ene_vol[j]*1000;
            hist[ii] -> Fill(Etot[ii]);
        }
    }// end of loop over events

    TFile* fout = new TFile( outputFile,"RECREATE" );
    for ( int i = 0; i < 11; i++ ) hist[i]->Write();
    fout->Close();

    return 0;
}


void Usage()
{
    cout << "\n*****************************************************************\n\n";
    cout << "Post processing of alpha p+ contact simulations: "               << endl;
    cout << "\tUsage: ./alphas-post-processing.cxx -i <indir> -o <outfile>"   << endl;
    cout << "\t-i: input directory"                                           << endl;
    cout << "\t-o: output file"                                               << endl;
    cout << "\n*****************************************************************\n\n";
    return;
}
