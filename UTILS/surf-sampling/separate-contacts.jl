#=
 = separate-contacts.jl
 =
 = Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 7 Dic 2017
 =
 =#
using Cxx, ROOT, JSON
cxxinclude("TFile.h")
cxxinclude("TTree.h")
cxxinclude("TTreeReader.h")
cxxinclude("TTreeReaderValue.h")

# check if everything is ok
isfile("./separate-contacts.jl") || error("Please run this script from where separate-contacts.jl is!")

for f in [ ["../det-data/ged-mapping.json", "../det-data/ged-parameters.json"]; ["../../gedet/surf_chanwise/ver/ver-gedet-surf_chanwise-ch$i.root" for i = 0:39] ]
    isfile(f) || error("$f not found!")
end

# get dictionaries for gedet
gedMap = JSON.parsefile("../det-data/ged-mapping.json")["mapping"]
gedPar = JSON.parsefile("../det-data/ged-parameters.json")

# create useful dictionary
chDict = Dict()
for (det, info) in gedMap
    chDict[info["channel"]] = det
end

cxx"""
    void do_ROOT_job(bool upside_down, int ch, double x0, double y0, double ri, double ro, double l) {

        TFile inFile(Form("../../gedet/surf_chanwise/ver/ver-gedet-surf_chanwise-ch%i.root", ch));
        auto tree = dynamic_cast<TTree*>(inFile.Get("GSSTree"));

        TFile pFile(Form("../../gedet/pplus_chanwise/ver/ver-gedet-pplus_chanwise-ch%i.root", ch), "RECREATE");
        auto pTree = tree->CloneTree(0);
        TFile nFile(Form("../../gedet/nplus_chanwise/ver/ver-gedet-nplus_chanwise-ch%i.root", ch), "RECREATE");
        auto nTree = tree->CloneTree(0);
        //auto lTree = tree->CloneTree(0);

        double x, y, z;
        tree->SetBranchAddress("x_cm", &x);
        tree->SetBranchAddress("y_cm", &y);
        tree->SetBranchAddress("z_cm", &z);

        int n_lost = 0;
        double sqradius;

        int N = tree->GetEntries();
        if (upside_down) {
            for (int i = 0; i < N; ++i) {
                tree->GetEntry(i);
                sqradius = (x0-x)*(x0-x)+(y0-y)*(y0-y);
                if      ( sqradius <= ri*ri and z > l ) pTree->Fill();
                else if ( sqradius <= ro*ro or  z < l ) nTree->Fill();
                else if ( sqradius >= ro*ro           ) nTree->Fill();
                else { n_lost++; /*lTree->Fill();*/ }
            }
        }

        else {
            for (int i = 0; i < N; ++i) {
                tree->GetEntry(i);
                sqradius = (x0-x)*(x0-x)+(y0-y)*(y0-y);
                if      ( sqradius <= ri*ri and z < l ) pTree->Fill();
                else if ( sqradius <= ro*ro and z > l ) nTree->Fill();
                else if ( sqradius >= ro*ro           ) nTree->Fill();
                else { n_lost++; /*lTree->Fill();*/ }
            }
        }

        std::cout << "There were " << n_lost << " discarded vertices.\n";
        pFile.cd(); pTree->Write();
        nFile.cd(); nTree->Write(); //lTree->Write("lost");
    }
"""

for i in 0:39
    det = chDict[i]
    x0  = gedPar[det]["detcenter_x"]
    y0  = gedPar[det]["detcenter_y"]
    z0  = gedPar[det]["detcenter_z"]
    dz  = 0.5gedPar[det]["height"]
    pl  = gedPar[det]["pplus_length_z"] + 1
    ri  = gedPar[det]["groove_inner_radius"] - 0.01
    ro  = gedPar[det]["groove_outer_radius"] + 0.01

    gedPar[det]["upside_down"] ?
        l = z0 + dz - pl :
        l = z0 - dz + pl

    print("Processing detector $det... "); flush(STDOUT)
    @cxx do_ROOT_job(gedPar[det]["upside_down"], i, x0, y0, ri, ro, l)
end
