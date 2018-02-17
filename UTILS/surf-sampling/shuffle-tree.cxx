/* shuffle-tree.cxx
 *
 * Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 * Created: 17 Feb 2018
 *
 */
// stl
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <dirent.h>
#include <algorithm>
#include <random>

// jsoncpp
#include "json/json.h"

// cern-root
#include "TFile.h"
#include "TTree.h"

int main(int argc, char** argv) {

    auto usage = [&argv](){ std::cout << "Shuffle a collection of trees (each in a ROOT file) into a final global tree.\n\n"
                                 << "USAGE: " << std::string(argv[0]) << " [DIR-WITH-FILES] [TREE-NAME]\n"
                                 << "  optional: -v : verbose mode\n";
                          };

    // get & check arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 3) {usage(); return 1;}
    bool verbose = false;
    auto result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    auto dirWithRaw = *(args.end()-2);
    auto treeName   = *(args.end()-1);

    // strip off trailing '/' character, if present
    if (dirWithRaw.back() == '/') dirWithRaw.pop_back(); if (verbose) std::cout << dirWithRaw << std::endl;

    // get *.root files in directory
    auto GetContent = [&](std::string foldName) {
        std::vector<std::string> filelist;
        auto p = std::unique_ptr<DIR,std::function<int(DIR*)>>{opendir(foldName.c_str()), &closedir};
        if (!p) { std::cout << "Invalid or empty directory path!\n"; return filelist; }
        dirent entry;
        for (auto* r = &entry; readdir_r(p.get(), &entry, &r) == 0 && r; ) {
            // this means "is a regular file", DT_REG = 8
            if (entry.d_type == 8 &&
                std::string(entry.d_name).find(".root") != std::string::npos) {
                filelist.push_back(foldName + "/" + std::string(entry.d_name));
            }
        }
        std::sort(filelist.begin(), filelist.end());
        if (verbose) { std::cout << "\nsorted files:\n"; for ( const auto & f : filelist ) std::cout << '\t' << f << std::endl; }

        return filelist;
    };

    // Shuffling big trees in ROOT is not simple as it could seem...
    // Keep the input files separate so you can call TTree::LoadBasket
    // for details see: https://root-forum.cern.ch/t/serious-ttree-benchmarking-random-access/11727/4
    //                  https://root-forum.cern.ch/t/shuffling-ttrees-together/24265/6
    auto filelist = GetContent(dirWithRaw);
    if (filelist.empty()) {std::cout << "There were problems reading in the files. Aborting...\n"; return 1;}
    std::vector<TFile*> tfilelist;      // vector with input TFiles
    std::vector<TTree*> ttreelist;      // vector with extracted TTeees
    std::vector<TFile*> outtfilelist;   // vector with TFiles for the cloned trees (so ROOT can flush
                                        // baskets to disk periodically to not overload RAM)
    std::vector<TTree*> clonedTrees;    // vector with cloned trees
    for (int i = 0; i < (int)filelist.size(); ++i) {
        tfilelist.push_back(TFile::Open(filelist[i].c_str()));
        ttreelist.push_back(dynamic_cast<TTree*>(tfilelist.back()->Get(treeName.c_str())));
        if (!ttreelist.back()) {std::cout << treeName << " not found in " << filelist[i] << "!\n"; return 1;}
        // clone the structure of the original tree
        outtfilelist.push_back(TFile::Open(("/tmp/t4ztmp_" + std::to_string(i) + ".root").c_str(), "RECREATE"));
        clonedTrees.push_back(dynamic_cast<TTree*>(ttreelist[0]->CloneTree(0)));
    }

    // first we shuffle events from each input tree between the clone trees
    if (verbose) std::cout << "Splitting (shuffling) global tree into nfiles trees...\n";
    for (int i = 0; i < (int)ttreelist.size(); ++i) {
        auto& t = ttreelist[i];
        if (verbose) std::cout << i+1 << '/' << ttreelist.size() << ' ' << std::flush;
        // link cloned trees to the current tree, so clonedTrees[]->Fill works
        for (auto&& t : clonedTrees) t->CopyAddresses(t);
        std::mt19937 mt(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, ttreelist.size()-1);
        auto localEntries = t->GetEntries();
        for (Long64_t j = 0; j < localEntries; ++j) {
            t->GetEntry(j);
            // randomize filling of cloned trees
            clonedTrees[dist(mt)]->Fill();
        }
        // unlink
        for (auto&& t : clonedTrees) ttreelist[i]->CopyAddresses(t, true);
        // input trees are no more needed
        tfilelist[i]->Close();
    }

    // now we can call TTree::LoadBaskets on each cloned tree and shuffle again in the final one
    if (verbose) std::cout << "\nShuffling each single tree...\n";
    TFile outfile("shuffled-tree.root", "RECREATE");
    // the final tree
    TTree* shTree = clonedTrees[0]->CloneTree(0);
    for (int i = 0; i < (int)clonedTrees.size(); ++i) {
        auto& t = clonedTrees[i];
        if (verbose) std::cout << i+1 << '/' << clonedTrees.size();
        auto localEntries = t->GetEntries();
        if (verbose) std::cout << " Entries: " << localEntries << std::endl;

        // build set of randomised entries
        std::vector<Long64_t> ev(localEntries);
        std::iota(ev.begin(), ev.end(), 0);
        std::shuffle(ev.begin(), ev.end(), std::mt19937{std::random_device{}()});

        // link final tree to the current
        t->CopyAddresses(shTree);
        // load tree in RAM so random access is fast
        t->LoadBaskets();
        for (Long64_t j = 0; j < localEntries; ++j) {
            clonedTrees[i]->GetEntry(ev[j]);
            shTree->Fill();
        }
        // unlink
        t->CopyAddresses(shTree, true);
        // delet cloned trees from disk
        t->Delete();
    }

    std::cout << "Saving output file in shuffled-tree.root\n";
    shTree->Write();
    return 0;
}
