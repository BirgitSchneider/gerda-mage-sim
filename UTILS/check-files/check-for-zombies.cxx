/* Author  : K.v.Sturm
 * Date    : 14.2.2018
 * Compile : g++ $(root-config --cflags) $(root-config --libs) check-for-zombies.cxx -o check-for-zombies
 * Usage   : find gerda-mage-sim -iname "*root" > list.txt; gerda-mage-sim/UTILS/check-files/check-for-zombies list.txt;
*/


// C/C++
#include <cstdlib>
#include <iostream>
#include <fstream>

// cern root
#include "TFile.h"


using namespace std;

int main( int argc, char * argv[] )
{
    string filelistname = argv[1];

    ifstream filelist( filelistname );

    string filename; filelist >> filename;

    ofstream report( "zombie-report.txt" );

    while( !filelist.eof() )
    {
        cout << filename << endl;

        // open file
        TFile file( filename.c_str(), "READ" );

        // check if zombie
        if( file.IsZombie() )
        {
            cout << "\033[1;31m" << filename << "\033[0m\n";
            report << filename << endl;
        }

        // close file
        file.Close();

        filelist >> filename;
    }

    return 0;
}
