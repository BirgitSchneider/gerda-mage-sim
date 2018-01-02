// C/c++  includes
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
/*
#include <cmath>
#include <map>
*/
// root cern includes
#include "TSystem.h"
#include "TRegexp.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TFile.h"
/*
#include "TH2D.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
*/

// boost filesystem lib
// #include "boost/filesystem.hpp"

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

	// check number of arguments
	if( argc < 2 )
	{
		cout << "Too few arguments" << endl;
		Usage();
		return -1;
	}
	else if( argc == 2 ) DIR = argv[1];
	else if( argc == 3 ) { DIR = argv[1]; PATTERN = argv[2]; }
	else if( argc > 3 )
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

	// parse dir name for location, part, isotope and multiplicity
	string LOCATION, PART, ISOTOPE, MULTI;
	ParseDirName( DIR, LOCATION, PART, ISOTOPE, MULTI );

	// plot x-y z-x, z-y distribiutions of vertices and hits
	string title = Form("controlplots-%s-%s-%s-%s", LOCATION.c_str(), PART.c_str(), ISOTOPE.c_str(), MULTI.c_str() );
	TCanvas * can = new TCanvas( "cplots", title.c_str(), 1500, 500 );
	can -> Divide(3,1);
	can -> cd(1);
	c -> Draw( "vertex_xpos:vertex_ypos","","COLZ" );
	c -> Draw( "hits_xpos:hits_ypos","","same" );
	can -> cd(2);
	c -> Draw( "vertex_zpos:vertex_xpos","","COLZ" );
	c -> Draw( "hits_zpos:hits_xpos","","same" );
	can -> cd(3);
	c -> Draw( "vertex_zpos:vertex_ypos","","COLZ" );
	c -> Draw( "hits_zpos:hits_ypos","","same" );

	// open output file
	string outfilename = DIR; outfilename += title; outfilename += ".root";
	TFile * file = new TFile( outfilename.c_str() );
	can -> Write();
	file -> Close();

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

	LOCATION = parts.at( n - 4 );
	PART = parts.at( n - 3 );
	ISOTOPE = parts.at( n - 2 );
	MULTI = parts.at( n - 1 );

	return;
}


/**********************************************************************/
vector<string> cutString( string stringToCut, char c )
{
	string STC;
	vector<string> parts;

	int index = STC.find("/");

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
	cout << "./check-simulation DIR [PATTERN]" << endl;
	cout << "**********************************" << endl;

	return;
}
