/* check-simulation.cxx
 *
 * Author: Katharina von Sturm - vonsturm@pd.infn.it
 * Created: Tue 02 Jan 2018
 *
 * Compile with:
 * $ g++ -I/lfs/l2/gerda/Hades/Analysis/Users/sturm/BAT/progressbar -L/lfs/l2/gerda/Hades/Analysis/Users/sturm/BAT/progressbar -lProgressBar \
 * $     $(root-config --cflags --libs) -lTreePlayer -o check-simulation check-simulation.cxx
 *
 */

// C/c++  includes
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>

// root cern includes
#include "TSystem.h"
#include "TRegexp.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TVirtualTreePlayer.h"
#include "TH2D.h"

// own includes
#include "ProgressBar.h"

using namespace std;

vector<string> GetFileList( string dirName, string pattern );
vector<string> cutString( string stringToCut, char c );
void ParseDirName( string DIR, string & LOCATION, string & PART, string & ISOTOPE, string & MULTI );
void FormatDir( string & dir );
void Usage();

int main( int argc, char * argv[] )
{
	string DIR;
	string PATTERN = "raw*.root";
	Long64_t NEVENTS = TVirtualTreePlayer::kMaxEntries;

	// check number of arguments
	if( argc < 2 )
	{
		cout << "Too few arguments" << endl;
		Usage();
		return -1;
	}
	else if( argc == 2 ) DIR = argv[1];
	else if( argc == 3 ) { DIR = argv[1]; PATTERN = argv[2]; }
	else if( argc == 4 ) { DIR = argv[1]; PATTERN = argv[2]; NEVENTS = atol( argv[3] ); }
	else if( argc > 4 )
	{
		cout << "Too many arguments" << endl;
		Usage();
		return -1;
	}

	// search directory for root files
	FormatDir( DIR );
	vector<string> flist = GetFileList( DIR, PATTERN );

	// construct chain
	TChain * c = new TChain( "fTree" );
	cout << "Add files: " << endl;

	for( auto f : flist )
	{
		cout << "\t" << f << endl;
		c->Add( f.c_str() );
	}

	Long64_t N = c -> GetEntries();
	Long64_t Ntoprocess = min( N, NEVENTS );
	cout << "Events in chain: " << N << endl;
	cout << "Events to be processed: " << Ntoprocess << endl;

	// parse dir name for location, part, isotope and multiplicity
	string LOCATION, PART, ISOTOPE, MULTI;
	ParseDirName( DIR, LOCATION, PART, ISOTOPE, MULTI );

	// Set brnach addresses
	int max = 10000;
	int hits_totnum;
	vector<float> vertex_xpos( max );
	vector<float> vertex_ypos( max );
	vector<float> vertex_zpos( max );
	vector<float> hits_xpos( max );
	vector<float> hits_ypos( max );
	vector<float> hits_zpos( max );

	c -> SetBranchAddress( "hits_totnum", &hits_totnum );
	c -> SetBranchAddress( "vertex_xpos", &vertex_xpos[0] );
	c -> SetBranchAddress( "vertex_ypos", &vertex_ypos[0] );
	c -> SetBranchAddress( "vertex_zpos", &vertex_zpos[0] );
	c -> SetBranchAddress( "hits_xpos", &hits_xpos[0] );
	c -> SetBranchAddress( "hits_ypos", &hits_ypos[0] );
	c -> SetBranchAddress( "hits_zpos", &hits_zpos[0] );

	double xmin = -35, xmax = 35, ymin = -35, ymax = 35, zmin = -50, zmax = 60;
	int xnbins = 70, ynbins = 70, znbins = 110;

	if( LOCATION == "gedet" )
	{
		xmin = -20, xmax = 20, ymin = -20, ymax = 20, zmin = -45, zmax = 45;
		xnbins = 40, ynbins = 40, znbins = 90;
	}
	else if( LOCATION == "larveto" )
	{
		if( PART == "fibers" )
		{
			xmin = -35, xmax = 35, ymin = -35, ymax = 35, zmin = -50, zmax = 60;
			xnbins = 70, ynbins = 70, znbins = 110;
		}
		else if( PART == "pmt_bottom" )
		{
			xmin = -35, xmax = 35, ymin = -35, ymax = 35, zmin = -105, zmax = 45;
			xnbins = 70, ynbins = 70, znbins = 150;

		}
		else if( PART == "pmt_top" )
		{
			xmin = -35, xmax = 35, ymin = -35, ymax = 35, zmin = -45, zmax = 115;
			xnbins = 70, ynbins = 70, znbins = 160;
		}
	}

	TH2D * v_xy = new TH2D( "v_xy", "v_xy", xnbins,xmin,xmax, ynbins,ymin,ymax );
	TH2D * v_xz = new TH2D( "v_xz", "v_xz", xnbins,xmin,xmax, znbins,zmin,zmax );
	TH2D * v_yz = new TH2D( "v_yz", "v_yz", ynbins,ymin,ymax, znbins,zmin,zmax );

	TH2D * h_xy = new TH2D( "h_xy", "h_xy", 2*xnbins,xmin,xmax, 2*ynbins,ymin,ymax );
	TH2D * h_xz = new TH2D( "h_xz", "h_xz", 2*xnbins,xmin,xmax, 2*znbins,zmin,zmax );
	TH2D * h_yz = new TH2D( "h_yz", "h_yz", 2*ynbins,ymin,ymax, 2*znbins,zmin,zmax );

	ProgressBar bar( Ntoprocess, '#', false );

	for( int i = 0; i < Ntoprocess; i++ )
	{
		bar.Update();
		c -> GetEntry( i );

		v_xy -> Fill( vertex_xpos[0], vertex_ypos[0] );
		v_xz -> Fill( vertex_xpos[0], vertex_zpos[0] );
		v_yz -> Fill( vertex_ypos[0], vertex_zpos[0] );

		for( int h = 0; h < hits_totnum; h++ )
		{
			h_xy -> Fill( hits_xpos[h], hits_ypos[h] );
			h_xz -> Fill( hits_xpos[h], hits_zpos[h] );
			h_yz -> Fill( hits_ypos[h], hits_zpos[h] );
		}
	}

	delete c;

	// plot x-y z-x, z-y distribiutions of vertices and hits
	cout << "\nCreate plots..." << endl;

	string title = Form("controlplots-%s-%s-%s-%s", LOCATION.c_str(), PART.c_str(), ISOTOPE.c_str(), MULTI.c_str() );
	TCanvas * can = new TCanvas( "cplots", title.c_str(), 1300, 500 );
	can -> Divide(3,1);
	can -> cd(1);
	v_xy -> Draw("COLZ");
	h_xy -> Draw("same");
	can -> cd(2);
	v_xz -> Draw("COLZ");
	h_xz -> Draw("same");
	can -> cd(3);
	v_yz -> Draw("COLZ");
	h_yz -> Draw("same");

	// open output file and write canvas to file
	string outfilename = DIR; outfilename += title;
	string rootoutfilename = outfilename; rootoutfilename += ".root";
	string pngoutfilename = outfilename; pngoutfilename += ".png";

	cout << "...and write them to file: " << endl;
	cout << "\t" << rootoutfilename << endl;

	TFile * file = new TFile( rootoutfilename.c_str(), "RECREATE" );
	can -> Write();
	file -> Close();

	cout << "\t" << pngoutfilename << endl;

	can -> Print( pngoutfilename.c_str() );

	delete file;
	delete can;

	cout << "DONE" << endl;

	return 0;
}


/**********************************************************************/
vector<string> GetFileList( string dirName, string pattern )
{
        vector<string> flist;

        void * dir = gSystem->OpenDirectory( dirName.c_str() );

        if (!dir)
        {
              cout << "Couldn't open directory '" << dirName << "'" << std::endl;
              return flist;
        }

        const Char_t * c_file;
        TRegexp regexp( pattern.c_str(), true );

        while ( ( c_file = gSystem->GetDirEntry(dir) ) )
        {
                string file = c_file;

                if ( (TString (file)).Index(regexp) >= 0 )
                {
                        file.insert( 0, dirName );
                        flist.push_back( file );
                }
        }

        gSystem->FreeDirectory(dir);

        sort( flist.begin(), flist.end() );

        return flist;
}


/**********************************************************************/
void FormatDir( string & dir )
{
	if( &(dir.at( dir.size()-1 )) != "/" ) dir += "/";

	return;
}


/**********************************************************************/
void ParseDirName( string DIR, string & LOCATION, string & PART, string & ISOTOPE, string & MULTI )
{
	vector<string> parts = cutString( DIR, '/' );

	int n = parts.size();

	if( n < 4 )
	{
		cout << "directory structure not recognized" << endl;
		return;
	}

	LOCATION = parts.at( n - 4 );
	PART = parts.at( n - 3 );
	ISOTOPE = parts.at( n - 2 );
	MULTI = parts.at( n - 1 );

	return;
}


/**********************************************************************/
vector<string> cutString( string stringToCut, char c )
{
	string STC = stringToCut;
	vector<string> parts;

	int index = STC.find( c );

	while( index != string::npos )
	{
		string part = STC.substr(0,index);
		if( part.size() > 0 ) parts.push_back( part );

		string subSTC = STC.substr(index+1, STC.size()-index-1);
		STC = subSTC;
		index = STC.find("/");
	}

	if( STC.size() > 0 ) parts.push_back( STC );

	return parts;
}


/**********************************************************************/
void Usage()
{
	cout << "**********************************" << endl;
	cout << "./check-simulation DIR [PATTERN | raw*.root] [NEVENTS | kMaxEntries]" << endl;
	cout << "**********************************" << endl;

	return;
}
