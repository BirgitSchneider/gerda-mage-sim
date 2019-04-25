#= units_tools.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Wed 22 Aug 2018
 =#

using Unitful

conv_table = Dict(
                  "mg"    => u"mg",
                  "g"     => u"g",
                  "kg"    => u"kg",

                  "cm2"   => u"cm^2",
                  "m2"    => u"m^2",
                  "gcm2"  => u"g/cm^2",
                  "kgcm2" => u"kg/cm^2",

                  "cm3"   => u"cm^3",
                  "m3"    => u"m^3",
                  "gcm3"  => u"g/cm^3",
                  "kgcm3" => u"kg/cm^3"
                 )

function get_units(s::String)
    if findlast(isequal('-'), s) != nothing
        u = s[findlast(isequal('-'), s)+1:end]
        return isempty(u) ? nothing : conv_table[u]
    else
        return nothing
    end
end

function schop(num, digits)
    if num == 0.0 then
        return num
    else
        e = ceil(log10(abs(num)))
        scale = 10^(digits - e)
        return trunc(num * scale) / scale
    end
end
