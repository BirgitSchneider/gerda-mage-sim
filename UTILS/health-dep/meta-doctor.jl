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
    !isfile("$v/metadata.json") && error("missing $v/metadata.json")

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

        p == "surf_chanwise" && continue

        # check part metadata
        !isfile("$v/$p/metadata.json") && error("missing $v/$p/metadata.json")
        try
            isometa = JSON.parse(open("$v/$p/metadata.json"))
        catch
            error("$v/$p/metadata.json seems to be uncorrectly encoded")
        end

        isoexclude = ["ver"]
        isotopes = filter(e -> isdir("$v/$p/$e") && e[1] != '.' && !(e in isoexclude), readdir("$v/$p"))
        @debug "detected isotopes" isotopes

        isometa = isometa[v][p]
        for i in isotopes
            !haskey(isometa, i) && error("missing '$i' entry in $v/$p/metadata.json")

            types = filter(e -> isdir("$v/$p/$i/$e") && e[1] != '.', readdir("$v/$p/$i"))
            @debug "detected types" types

            for t in types
                !haskey(isometa[i], t) && error("missing '$i/$t' entry in $v/$p/metadata.json")
                for k in ["mage-version", "primaries", "contact"]
                    !haskey(isometa[i][t], k) && error("missing '$i/$t/$k' field in $v/$p/metadata.json")
                end
                for k in ["revision", "gerda-sw-all"]
                    !haskey(isometa[i][t]["mage-version"], k) && error("missing '$i/$t/mage-version/$k' field in $v/$p/metadata.json")
                end
            end
        end
    end
end
