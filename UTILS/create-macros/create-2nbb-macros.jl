#= create-2nbb-macros.jl
 =
 = script to write MaGe macro files for each detector (active volume
 = or dead layer) starting for generic .tmac template files for
 =
 = 2nbbLV data
 = 2nbb   data
 =
 = Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 11/12/2017
 =#
using JSON

# check if everything is ok

if size(ARGS,1) < 2
    println("USAGE: julia create-2nbb-macros.jl [2nbb|2nbbLV] [000|001|002|...]")
    exit()
end
(ARGS[1] == "2nbb" || ARGS[1] == "2nbbLV") || error("Please specify the [2nbb|2nbbLV] option!")

isfile("./create-2nbb-macros.jl") || error("Please run this script from where create-2nbb-macros.jl is!")

for f in [ 
          "../../gedet/intrinsic/$(ARGS[1])/edep/log/raw-gedet-intrinsic-$(ARGS[1])-edep-%DET.tmac",
          "../det-data/ged-mapping.json",
          "../det-data/ged-parameters.json"
          ]
    isfile(f) || error("$f not found!")
end

# get dictionaries for gedet
gedMap = JSON.parsefile("../det-data/ged-mapping.json")["mapping"]
gedPar = JSON.parsefile("../det-data/ged-parameters.json")
# get template macro file
temLines = readlines(open("../../gedet/intrinsic/$(ARGS[1])/edep/log/raw-gedet-intrinsic-$(ARGS[1])-edep-%DET.tmac"))

# calculate total mass
tot_mass = 0.
for (det, info) in gedPar
    tot_mass += info["mc_mass"]
end

# loop over detectors in ged-mapping file with a (key,value) pair
for (det, info) in gedMap
   CHANNEL = info["channel"]
   VOLID = info["volume"]
   NEV = convert(Int64,round(1E08 * gedPar[det]["mc_mass"] / tot_mass))

   # substitute $ variables in template file
   macLines = temLines
   macLines = replace.(macLines, "\$CHANNEL", CHANNEL) 
   macLines = replace.(macLines, "\$ID", VOLID)
   macLines = replace.(macLines, "\$NEV", NEV)

   # add message
   unshift!(macLines, "")
   unshift!(macLines, "#This macro was auto-generated by the create-2nbb-macros.jl script")

   writedlm(open("../../gedet/intrinsic/$(ARGS[1])/edep/log/raw-gedet-intrinsic-$(ARGS[1])-edep-ch$CHANNEL-$(ARGS[2]).mac", "w"), macLines, '\n')
   println(" created ../../gedet/intrinsic/$(ARGS[1])/edep/log/raw-gedet-intrinsic-$(ARGS[1])-edep-ch$CHANNEL-$(ARGS[2]).mac")
end
