/* res-curves-jsonizer.cxx
 *
 * Author: Luigi Pertoldi: luigi.pertoldi@pd.infn.it
 * Created: 5 Mar 2018
 *
 */
// stl
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

// jsoncpp
#include <json/json.h>

// ROOT
#include "TFile.h"
#include "TF1.h"

int main(int argc, char** argv) {

    auto usage = [](){ std::cout << "USAGE: res-curves-jsonizer [OPTIONS] <res-curves-rootfile>\n\n"
                                 << "OPTIONS:\n"
                                 << "  required: --mapping <ged-mapping.json>\n"
                                 << "  optional: -v : verbose mode\n"
                                 << std::endl; return;};

    // retrieve arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
    if (argc < 4) {usage(); return 1;}
    std::string inputFile = *(args.end()-1);
    bool verbose = false;
    auto result = std::find(args.begin(), args.end(), "-v");
    if (result != args.end()) verbose = true;
    std::string gedMappingFileName;
    result = std::find(args.begin(), args.end(), "--mapping");
    if (result != args.end()) gedMappingFileName = *(result+1);
    if (verbose) std::cout << "ged-mapping file name: " << gedMappingFileName << std::endl;

    Json::Value gedMapping;
    std::ifstream gedMappingFile(gedMappingFileName.c_str());
    if (!gedMappingFile.is_open()) {
        std::cout << gedMappingFileName << " does not exist!\n";
        return 1;
    }
    gedMappingFile >> gedMapping;
    auto detectors = gedMapping["mapping"].getMemberNames();

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "    ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    TFile resfile(inputFile.c_str());
    for (auto& d : detectors) {
        TF1* func;
        func = dynamic_cast<TF1*>( resfile.Get( d.c_str() ) );
        if (!func) {
            std::cout << "Calibration curve of " << d << " not found! Skipping...\n";
            break;
        }

        auto& loc_entry = root["Zac"][std::to_string(gedMapping["mapping"][d]["channel"].asInt())];
        loc_entry["func"] = "2.35482 * sqrt( [0] + [1] * x )";
        loc_entry["params"]["0"] = func->GetParameter(0);
        loc_entry["params"]["1"] = func->GetParameter(1);
    }

    // write down jsonfile
    std::ofstream fout("ged-resolution.json");
    writer->write(root, &fout);

    return 0;
}
