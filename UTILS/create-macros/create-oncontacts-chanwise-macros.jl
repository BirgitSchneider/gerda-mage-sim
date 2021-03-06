#= create-oncontacts-chanwise-macros.jl
 =
 = script to write MaGe macro files for each detector
 = starting for generic .tmac template files for simulations
 = on p+ and n+ contacts using pre-sampled points (ver- files)
 =
 = Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 14/12/2017
 =#
using JSON

# check if everything is ok

if size(ARGS,1) < 3
    println("USAGE: julia create-oncontacts-chanwise-macros.jl [nplus|pplus] [ISOTOPE] [edep|coin]")
    exit()
end
(ARGS[1] == "nplus" || ARGS[1] == "pplus") || error("Please specify the [nplus|pplus] option first!")
(ARGS[3] == "edep"  || ARGS[3] == "coin")  || error("Please specify the [edep|coin] option last!")

isfile("./create-oncontacts-chanwise-macros.jl") || error("Please run this script from where create-contacts-chanwise-macros.jl is!")

for f in [ 
          "../../gedet/$(ARGS[1])_chanwise/$(ARGS[2])/$(ARGS[3])/log/raw-gedet-$(ARGS[1])_chanwise-$(ARGS[2])-$(ARGS[3])-%DET.tmac",
          "../det-data/ged-mapping.json",
          ]
    isfile(f) || error("$f not found!")
end

pplus_entries = [38776, 33081, 38778, 35554, 35237, 34652, 35265, 39442, 412445, 509146,
                 511514, 39297, 37252, 44626, 36788, 35605, 35354, 55891, 33663, 35628,
                 33313, 37262, 32928, 31477, 34744, 38683, 33790, 431371, 401773, 487503,
                 39767, 34209, 33216, 37598, 36485, 40917, 520459, 267732, 226337, 225298]
nplus_entries = [2860829, 3105966, 3056926, 3048089, 2944514, 2870800, 2889773, 3106659, 3547402, 3580697,
                 3567461, 2762628, 3157533, 2866477, 3184654, 2885262, 2946378, 3149863, 3158832, 2947692,
                 3145194, 2828355, 3110914, 3022305, 2937970, 3112722, 3053433, 3550707, 3594823, 3522830,
                 2567914, 3137209, 3003590, 3153977, 3150370, 2730112, 3471847, 3593565, 3546794, 3580640]

# get dictionaries for gedet
gedMap = JSON.parsefile("../det-data/ged-mapping.json")["mapping"]
# get template macro file
temLines = readlines(open("../../gedet/$(ARGS[1])_chanwise/$(ARGS[2])/$(ARGS[3])/log/raw-gedet-$(ARGS[1])_chanwise-$(ARGS[2])-$(ARGS[3])-%DET.tmac"))

# loop over detectors in ged-mapping file with a (key,value) pair
for (det, info) in gedMap
   CHANNEL = info["channel"]

   # substitute $ variables in template file
   macLines = temLines
   macLines = replace.(macLines, "\$CHANNEL", CHANNEL)
   macLines = replace.(macLines, "\$NEV", ARGS[1] == "nplus" ? nplus_entries[CHANNEL+1] : pplus_entries[CHANNEL+1])

   # add message
   unshift!(macLines, "")
   unshift!(macLines, "#This macro was auto-generated by the create-oncontacts-macros.jl script")

   writedlm(open("../../gedet/$(ARGS[1])_chanwise/$(ARGS[2])/$(ARGS[3])/log/raw-gedet-$(ARGS[1])_chanwise-$(ARGS[2])-$(ARGS[3])-ch$CHANNEL.mac", "w"), macLines, '\n')
   println(" created ../../gedet/$(ARGS[1])_chanwise/$(ARGS[2])/$(ARGS[3])/log/raw-gedet-$(ARGS[1])_chanwise-$(ARGS[2])-$(ARGS[3])-ch$CHANNEL.mac")
end
