#= 
 = surf-calc.jl
 =
 = Computes the area of the p+ and n+ surfaces for each detector
 =
 = Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 17 Dec 2017
 =#
using JSON

dict = Dict()
if isfile("../det-data/ged-parameters.json") 
    dict = JSON.parsefile("../det-data/ged-parameters.json")
else
    error("../det-data/ged-mapping.json not found!")
end

println("det\tp+ [cm^2]  n+ [cm^2]")
println("----------------------------")
for (det,info) in dict
    # is a BEGe
    if contains(det, "GD")
        #= is edged
             ________
           /          \
          /            \ n+
         |    _    _    |
         |___| |__| |___|
                p+
        =#
        if dict[det]["edged"]
            pplus = π * dict[det]["pplus_radius"]^2
            nplus = π * (2dict[det]["radius"]*(dict[det]["height"] - dict[det]["corner_height"])
                         + dict[det]["corner_radius"]^2
                         + dict[det]["radius"]^2 - dict[det]["groove_outer_radius"]^2
                         + (dict[det]["radius"] + dict[det]["corner_radius"])
                           * √(
                               (dict[det]["radius"] - dict[det]["corner_radius"])^2
                               + dict[det]["corner_height"]^2
                              )
                        )
        #= not edged
          ______________
         |              |
         |              | n+
         |    _    _    |
         |___| |__| |___|
                p+
        =#
        else
            pplus = π * dict[det]["pplus_radius"]^2
            nplus = π * (2dict[det]["radius"]*dict[det]["height"]
                         + 2dict[det]["radius"]^2
                         - dict[det]["groove_outer_radius"])
        end
    #= is a coaxial
      __________________
     |                  |
     |        __        | n+
     |       |  |       |
     |       |p+|       |
     |   _   |  |   _   |
     |__| |__|  |__| |__|

    =#
    else
        pplus = π * (2dict[det]["pplus_radius"]dict[det]["pplus_length_z"]
                     + dict[det]["groove_inner_radius"]^2)
        nplus = π * (2dict[det]["radius"]*dict[det]["height"]
                     + 2dict[det]["radius"]^2
                     - dict[det]["groove_outer_radius"])
    end
    pplus /= 1E04
    nplus /= 1E04
    print("$det\t"); @printf("%f   %f\n", pplus, nplus)
end
