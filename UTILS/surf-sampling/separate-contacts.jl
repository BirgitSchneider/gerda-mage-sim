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

for f in [ ["../det-data/ged-mapping.json", "../det-data/ged-parameters.json"]; ["../../gedet/surf/ver/ver-gedet-surf-ch$i.root" for i = 0:39] ]
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
    void do_ROOT_job(bool upside_down, int ch, double x0, double y0, double r, double l) {

        TFile inFile(Form("../../gedet/surf/ver/ver-gedet-surf-ch%i.root", ch));
        TFile nFile(Form("../../gedet/nplus/ver/ver-gedet-nplus-ch%i.root", ch), "RECREATE");
        TFile pFile(Form("../../gedet/pplus/ver/ver-gedet-pplus-ch%i.root", ch), "RECREATE");

        auto tree = dynamic_cast<TTree*>(inFile.Get("GSSTree"));
        auto pTree = tree->CloneTree(0);
        auto nTree = tree->CloneTree(0);

        double x, y, z;
        tree->SetBranchAddress("x_cm", &x);
        tree->SetBranchAddress("y_cm", &y);
        tree->SetBranchAddress("z_cm", &z);

        int N = tree->GetEntries();
        if (upside_down) {
            for (int i = 0; i < N; ++i) {
                tree->GetEntry(i);
                if ( (x0-x)*(x0-x)+(y0-y)*(y0-y) <= r*r and z > l ) pTree->Fill();
                else                                                nTree->Fill();
            }
        }

        else {
            for (int i = 0; i < N; ++i) {
                tree->GetEntry(i);
                if ( (x0-x)*(x0-x)+(y0-y)*(y0-y) <= r*r and z < l ) pTree->Fill();
                else                                                nTree->Fill();
            }
        }

        pFile.WriteTObject(pTree);
        nFile.WriteTObject(nTree);
    }
"""

for i in 0:39
    det = chDict[i]
    x0  = parse(Float32, gedPar[det]["detcenter_x"])
    y0  = parse(Float32, gedPar[det]["detcenter_y"])
    z0  = parse(Float32, gedPar[det]["detcenter_z"])
    dz  = parse(Float32, gedPar[det]["detdim_z"])
    pl  = parse(Float32, gedPar[det]["pc_length"])
    # add 1mm to set less restrictive cuts for coax
    r   = parse(Float32, gedPar[det]["pc_radius"]) + 0.1

    gedPar[det]["upside_down"] ?
        l = z0 + dz - pl :
        l = z0 - dz + pl

    println("Processing detector $det..."); flush(STDOUT)
    @cxx do_ROOT_job(gedPar[det]["upside_down"], i, x0, y0, r, l)
end
