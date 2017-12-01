#=
 = create-surf-macros.jl
 =
 = Author:  Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 30 Nov 2017
 =
 =#
import JSON

# check if everything is ok
for f in ["../gedet/surf/ver/log/ver-gedet-surf-%DET.tmac", "ged-mapping.json", "ged-parameters.json"]
    isfile(f) || error("$f not found!")
end

# get dictionaries for gedet
gedMap = JSON.parsefile("ged-mapping.json")["mapping"]
gedPar = JSON.parsefile("ged-parameters.json")
# get template macro file
temLines = readlines(open("../gedet/surf/ver/log/ver-gedet-surf-%DET.tmac"))

# loop over detectors in ged-mapping file with a (key,value) pair
for (det, info) in gedMap
    CHANNEL = info["channel"]
    DET = info["volume"]
    ORIGIN = string(gedPar[det]["detcenter_x"]) * ' ' *
             string(gedPar[det]["detcenter_y"]) * ' ' * 
             string(gedPar[det]["detcenter_z"]) * " cm"

    RADIUS = string(√(parse(Float16, gedPar[det]["detdim_x"])^2
                      + parse(Float16, gedPar[det]["detdim_y"])^2
                      + parse(Float16, gedPar[det]["detdim_z"])^2)) * " mm"

    macLines = temLines
    macLines = replace.(macLines, "\$DET", DET)
    macLines = replace.(macLines, "\$ORIGIN", ORIGIN)
    macLines = replace.(macLines, "\$RADIUS", RADIUS)
    macLines = replace.(macLines, "\$CHANNEL", CHANNEL)

    writedlm(open("../gedet/surf/ver/log/ver-gedet-surf-$CHANNEL.mac", "w"), macLines, '\n')
end
