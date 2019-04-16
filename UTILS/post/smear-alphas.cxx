/* smear-alphas.cxx
 *
 * Author  : K.v.Sturm vonsturm@pd.infn.it
 * Created : 18.05.2018
 * Usage   :
 * Descrtiption : smear files generated as alpha simulations using super calibration curves
 *
 */

// c/c++
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// jsoncpp
#include <json/json.h>

// ROOT
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TRandom3.h"

using namespace std;

int main()
{
    // read argunments
    string unsmeared = argv[1];
    string curves    = argv[2]; // /nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root
    vector<string> types = { "BEGe", "Coax", "Nat" };

    // get calibration curves from file
    TFile * calfile = new TFile( curves.c_str(), "READ" );
    TF1 * enrBEGe_cal = (TF1*) calfile -> Get( "BEGe" );
    TF1 * enrCoax_cal = (TF1*) calfile -> Get( "Coax" );
    TF1 * natCoax_cal = (TF1*) calfile -> Get( "Nat" );

    // open unsmeared hist file
    TFile * file = new TFile( unsmeared.c_str(), "READ" );

    // loop over data sets
    for( auto type : types )
    {
        TF1 * cal = (TF1*) calfile -> Get( type.c_str() );

        // loop over dl values
	for( int dl = 0; dl <= 1000; dl += 100 )
        {
            // get unsmeared histogram, clone, reset
            string uhistname = "hist_dl"; uhistname += to_string(dl); uhistname += "nm";
            string shistname = uhistname; shistname += "_s";
            TH1D * uhist = (TH1D*) file -> Get( uhistname.c_str() );
            TH1D * shist = (TH1D*) uhist -> Clone( shistname.c_str() ); uhist -> Reset();

            TRandom3 rand;
            rand -> SetSeed(0);

            double dE = rand -> Gaus(  )

            // loop over bins
            int nbins = uhist -> GetNbinsX();
            for( int b = 1; b <= nbins; b++ )
            {

            }
        }

    calfile -> Close();
    file -> Close();

    return 0;
}
