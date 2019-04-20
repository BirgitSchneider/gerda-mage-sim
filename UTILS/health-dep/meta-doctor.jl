#= meta-doctor.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Sat 20 Apr 2019
 =
 =#
using JSON, Logging

gerda_ms = abspath(String(@__DIR__) * "/../..")
cd(gerda_ms)

exclude = ["UTILS", "decay0files", "alphas"]
volumes = []

# check volume metadata
for it in readdir(gerda_ms)
    if isdir(it) && !(it in exclude) && it[1] != '.'
        push!(volumes, it) # path to directories
    end
end

@debug "detected volumes:" volumes

for v in volumes
    @debug "inspecting $v/"
    !isfile("$v/metadata.json") && error("missing metadata.json for $v/")

    parts = filter(e -> isdir("$v/$e") && e[1] != '.', readdir("$v"))
    @debug "detected parts" parts

    try
        meta = JSON.parse(open("$v/metadata.json"))
    catch
        error("$v/metadata.json seems to be uncorrectly encoded")
    end

    !haskey(meta, v) && error("missing '$v' entry in $v/metadata.json")

    meta = meta[v]
    for p in parts
        !haskey(meta, p) && error("missing '$p' entry in $v/metadata.json")
        for k in ["description", "simulated-properties", "mage-volumes"]
            !haskey(meta[p], k) && error("missing '$p/$k' field in $v/metadata.json")
        end
        if !occursin("_chanwise", p)
            props = meta[p]["simulated-properties"]
            if true in occursin.([r"surface-."], collect(keys(props)))
                @debug "$p seems a surface" props
                length(props) < 1 && error("surface area information missing for '$p/simulated-properties' field in $v/metadata.json")
            else
                @debug "$p seems a volume" props
                length(props) < 2 && error("mass and volume information missing for '$p/simulated-properties' field in $v/metadata.json")
            end
        else
            @debug "$p is chanwise, skipping simulated-properties checks"
        end
    end
end
