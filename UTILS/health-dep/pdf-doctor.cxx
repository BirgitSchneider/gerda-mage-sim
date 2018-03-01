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
#include "TParameter.h"
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
    ReadDir::SetVerbose(true);
    auto filelist = ReadDir::GetFiles();

    ProgressBar bar(filelist.size(), '#', false);
    int nT4zFiles = 0;
    int nPdfFiles = 0;
    int nIsZombie = 0;
    int nHasToBeRecovered = 0;
    int nHasPrimaries = 0;
    int nHasZeroPrimaries = 0;

    //gEnv->SetValue("TFile.Recover", 0);
    gErrorIgnoreLevel = kError;

    std::cout << "Checking... ";
    for (const auto& f : filelist) {
        bar.Update();

        // check properties
        TFile file(f.c_str(), "READ");
        if (file.TestBit(TFile::kRecovered)) {
            std::cout << std::endl << f << " needs recovery, probably truncated or production still ongoing.\n";
            nHasToBeRecovered++;
        }
        else if (file.IsZombie()) {
            std::cout << std::endl << f << " IsZombie!\n";
            nIsZombie++;
        }
        else {
            if (f.find("t4z-") != std::string::npos) {
                nT4zFiles++;
                if (!file.GetListOfKeys()->Contains("NumberOfPrimaries")) {
                    std::cout << std::endl << f << " does not contain NumberOfPrimaries!\n";
                    nHasPrimaries++;
                }
                else {
                    if (dynamic_cast<TParameter<long>*>(file.Get("NumberOfPrimaries"))->GetVal() == 0) {
                        std::cout << std::endl << f << " has NumberOfPrimaries = 0!\n";
                        nHasZeroPrimaries++;
                    }
                }
            }
            if (f.find("pdf-") != std::string::npos) {
                nPdfFiles++;
                if (!file.GetListOfKeys()->Contains("NumberOfPrimariesEdep")) {
                    std::cout << std::endl << f << " does not contain NumberOfPrimariesEdep!\n";
                    nHasPrimaries++;
                }
                else {
                    if (dynamic_cast<TParameter<long>*>(file.Get("NumberOfPrimariesEdep"))->GetVal() == 0) {
                        std::cout << std::endl << f << " has NumberOfPrimariesEdep = 0!\n";
                        nHasZeroPrimaries++;
                    }
                }
                if (!file.GetListOfKeys()->Contains("NumberOfPrimariesCoin")) {
                    //std::cout << std::endl << f << " does not contain NumberOfPrimariesCoin!\n";
                    //nHasPrimaries++;
                }
                else {
                    if (dynamic_cast<TParameter<long>*>(file.Get("NumberOfPrimariesCoin"))->GetVal() == 0) {
                        std::cout << std::endl << f << " has NumberOfPrimariesCoin = 0!\n";
                        nHasZeroPrimaries++;
                    }
                }
            }
        }
    }

    std::cout << "\n\n"
              << "========= SUMMARY =========" << std::endl
              << nIsZombie         << '/' << filelist.size() << " zombie ROOT files" << std::endl
              << nHasToBeRecovered << '/' << filelist.size() << " ROOT files that need recovery" << std::endl
              << nHasPrimaries     << '/' << filelist.size() << " ROOT files without a NumberOfPrimaries object" << std::endl
              << nHasZeroPrimaries << '/' << filelist.size() << " ROOT files with a zero number of primaries" << std::endl
              << "===========================" << std::endl;

    return 0;
}
