/*
 * Author: Katharina
 * Date: 07.11.2018
 * Description: Takes a simulated file and splits it in two volumes -> K42 inside and outside the mini-shrouds
 * Copiling: g++ $(root-config --cflags) $(root-config --libs) split-volumes-K42.cxx -o split-volumes-K42
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>

#include "TTree.h"
#include "TFile.h"
#include "TParameter.h"

using namespace std;

struct mini_shroud
{
    double height;
    double radius;
    double center_x;
    double center_y;
    double center_z;
};

mini_shroud ms1 = {434.9, 51.5,  110.,       0., -123.55};
mini_shroud ms2 = {396.9, 51.5,   55.,  95.2628, -104.55};
mini_shroud ms3 = {403.9, 51.5,  -55.,  95.2628, -108.05};
mini_shroud ms4 = {424.9, 51.5, -110.,       0., -118.55};
mini_shroud ms5 = {376.9, 51.5,  -55., -95.2628,  -94.55};
mini_shroud ms6 = {397.9, 51.5,   55., -95.2628, -105.05};
mini_shroud ms7 = {336.9, 56.5,    0.,       0.,  -74.55};

bool point_is_inside_MS( double x, double y, double z, mini_shroud ms );
bool point_is_inside_any_MS( double x, double y, double z );

int main( int argc, char * argv[] )
{
    // read input filename as option
    string filename = argv[1];

    // construct output folder and filename
    string GERDA_MAGE_SIM = "/lfs/l2/gerda/gerda-simulations/gerda-mage-sim/";
    string of_out = GERDA_MAGE_SIM + "lar/outside_ms/K42/";
    string of_in = GERDA_MAGE_SIM + "lar/inside_ms/K42/";
    if( filename.find("-edep-") != string::npos )     { of_out += "edep/"; of_in += "edep/"; }
    else if( filename.find("-coin-") != string::npos) { of_out += "coin/"; of_in += "coin/"; }

    cout << "inside ms: " << of_in << endl;
    cout << "outside ms: " << of_in << endl;

    string of_out_filename = filename;
    string of_in_filename = filename;

    int index = filename.find("sur_array");
    of_out_filename.replace(index,9,"outside_ms");
    of_in_filename.replace(index,9,"inside_ms");

    of_out += of_out_filename;
    of_in += of_in_filename;

    // open file
    TFile file( filename.c_str(), "READ" );
    TTree * fTree = dynamic_cast<TTree*>( file.Get("fTree") );

    // create output files and copy original tree twice
    TFile file_of_out( of_out.c_str(), "RECREATE" );
    TTree * fTree_out = dynamic_cast<TTree*>( fTree->CloneTree(0) );
    TFile file_of_in( of_in.c_str(), "RECREATE" );
    TTree * fTree_in  = dynamic_cast<TTree*>( fTree->CloneTree(0) );

    // link trees to original tree
    fTree->CopyAddresses( fTree_out );
    fTree->CopyAddresses( fTree_in );

    // get vertex position
    vector<Float_t> vertex_xpos(1,0.);
    vector<Float_t> vertex_ypos(1,0.);
    vector<Float_t> vertex_zpos(1,0.);

    fTree->SetBranchAddress("vertex_xpos", &vertex_xpos[0]);
    fTree->SetBranchAddress("vertex_ypos", &vertex_ypos[0]);
    fTree->SetBranchAddress("vertex_zpos", &vertex_zpos[0]);

    int nevents = fTree->GetEntries();

    cout << "Splitting " << nevents << " events..." << endl;

    // loop events and write them in the according tree
    for( int e = 0; e < nevents; e++ )
    {
        fTree->GetEntry(e);
        Float_t x = vertex_xpos[0]*10.;
        Float_t y = vertex_ypos[0]*10.;
        Float_t z = vertex_zpos[0]*10.;

        // here everything has to be in mm!
        if( point_is_inside_any_MS(x,y,z) ) fTree_in->Fill();
        else                                fTree_out->Fill();
    }

    // unlink trees
    fTree->CopyAddresses( fTree_out, true );
    fTree->CopyAddresses( fTree_in, true );

    TString TP_name = (dynamic_cast<TParameter<long>*>( file.Get("NumberOfPrimaries") )) -> GetName();
    long TP_value =  (dynamic_cast<TParameter<long>*>( file.Get("NumberOfPrimaries") )) -> GetVal();

    double in_fraction  = 0.00195451;
    double out_fraction = 0.99804549;

    TParameter<long> * NumberOfPrimaries_out = new TParameter<long>( TP_name, (long)(out_fraction*TP_value) );
    TParameter<long> * NumberOfPrimaries_in  = new TParameter<long>( TP_name, (long) (in_fraction*TP_value) );

    // input file not needed anymore
    file.Close();

    // save trees in their files
    file_of_out.cd();
    NumberOfPrimaries_out->Write();
    fTree_out->Write();
    file_of_out.Close();

    file_of_in.cd();
    NumberOfPrimaries_in->Write();
    fTree_in->Write();
    file_of_in.Close();

    return 0;
}

bool point_is_inside_MS( double x, double y, double z, mini_shroud ms )
{
    // translate coordinate system
    double tr_x = x - ms.center_x;
    double tr_y = y - ms.center_y;
    double tr_z = z - ms.center_z;

    if( abs(tr_z) <= ms.height/2. ) // height inside MS
        if( sqrt(tr_x*tr_x + tr_y*tr_y) <= ms.radius ) // radius inside MS
            return true;

    return false;
}

bool point_is_inside_any_MS( double x, double y, double z )
{
    if(      point_is_inside_MS(x,y,z,ms1) ) return true;
    else if( point_is_inside_MS(x,y,z,ms2) ) return true;
    else if( point_is_inside_MS(x,y,z,ms3) ) return true;
    else if( point_is_inside_MS(x,y,z,ms4) ) return true;
    else if( point_is_inside_MS(x,y,z,ms5) ) return true;
    else if( point_is_inside_MS(x,y,z,ms6) ) return true;
    else if( point_is_inside_MS(x,y,z,ms7) ) return true;
    else return false;
}
