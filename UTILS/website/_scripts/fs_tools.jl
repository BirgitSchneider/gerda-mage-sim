#= fs_tools.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Sun 2 Sep 2018
 =#

function readdir_regex(r::Regex, d::String)
    images = filter(s -> occursin(r, s), readdir(d))
    return ["$d/$img" for img in images]
end
