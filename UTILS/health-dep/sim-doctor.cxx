/* sim-doctor.cxx
 *
 * Author  : K.v.Sturm and L.Pertoldi
 * Date    : 14.2.2018
 *
 */


// C/C++
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ReadDir.h"
#include "ProgressBar.h"

// cern root
#include "TFile.h"
#include "TROOT.h"
#include "TError.h"
#include "TEnv.h"

int main(int argc, char** argv) {

    if (argc > 1) {
        if (std::string(argv[1]) == "-h" or std::string(argv[1]) == "--help") {
            std::cout << "USAGE: " << std::string(argv[0]) << " [-h] [--help] <dir>\n";
            return 1;
        }
    }
    std::string dir = argc > 1 ? argv[1] : ".";
    ReadDir::GetContent_R(dir, "*.root");
    ReadDir::SetVerbose(false);
    auto rootfilelist = ReadDir::GetFiles();

    ProgressBar bar(rootfilelist.size(), '#', false);
    int nIsOrphanSim = 0;
    int nIsZombie = 0;
    int nHasToBeRecovered = 0;

    //gEnv->SetValue("TFile.Recover", 0);
    gErrorIgnoreLevel = kError;

    std::cout << "Checking ROOT files... ";
    for (const auto& f : rootfilelist) {
        bar.Update();

        if (f.find("alpha") != std::string::npos) continue;

        // check if there's the corresponding macro file
        auto f_tmp = f;
        auto filename = f_tmp.substr(f_tmp.find_last_of('/')+1);
        auto path     = f_tmp.substr(0, f_tmp.find_last_of('/'));
        std::ifstream fs(path + "/log/" + filename.erase(filename.size()-5) + ".mac");
        if (filename.find("ver") == std::string::npos and path.find("dk0") == std::string::npos) {
            if (!fs.is_open()) {
                std::cout << f << " is an orphan!\n";
                nIsOrphanSim++;
            }
        }

        // check properties
        TFile file(f.c_str(), "READ");
        if (file.TestBit(TFile::kRecovered)) {
            std::cout << f << " needs recovery, probably truncated or simulation still ongoing.\n";
            nHasToBeRecovered++;
            continue;
        }
        if (file.IsZombie()) {
            std::cout << std::endl << f << " IsZombie!\n";
            nIsZombie++;
        }
   }

    ReadDir::Clear();
    ReadDir::GetContent_R(dir, "*.mac");
    ReadDir::SetVerbose(false);
    auto macfilelist = ReadDir::GetFiles();

    int nIsOrphanMac = 0;
    bar.Reset();

    std::cout << "Checking macro files... ";
    for (const auto& f : macfilelist) {
        bar.Update();

        if (f.find("preamble") != std::string::npos or f.find("alpha") != std::string::npos) continue;

        // check if there's the corresponding root file
        auto f_tmp = f;
        auto filename = f_tmp.substr(f_tmp.find_last_of('/')+1);
        auto path     = f_tmp.substr(0, f_tmp.find_last_of('/'));
        std::ifstream fs(path + "/../" + filename.erase(filename.size()-4) + ".root");
        if (filename.find("ver") == std::string::npos and path.find("dk0") == std::string::npos) {
            if (!fs.is_open()) {
                std::cout << f << " is an orphan!\n";
                nIsOrphanMac++;
            }
        }
    }
    std::cout << "\n\n"
              << "========= SUMMARY =========" << std::endl
              << nIsZombie         << '/' << rootfilelist.size() << " zombie ROOT files" << std::endl
              << nHasToBeRecovered << '/' << rootfilelist.size() << " ROOT files that need recovery" << std::endl
              << nIsOrphanSim      << '/' << rootfilelist.size() << " orphan ROOT files" << std::endl
              << nIsOrphanMac      << '/' << macfilelist.size()  << " orphan macro files" << std::endl
              << "===========================" << std::endl;

    return 0;
}
