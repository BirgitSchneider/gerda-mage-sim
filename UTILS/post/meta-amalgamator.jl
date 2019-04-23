#= meta-amalgamator.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Tue 23 Apr 2019
 =#
import JSON

gerda_ms = abspath("$(@__DIR__)/../..")
cd(gerda_ms)

amalgamated = Dict()

for (root, dirs, files) in walkdir(gerda_ms)
    for f in files
        if f == "metadata.json"
            _src = JSON.parse(open("$root/metadata.json"))
            for (k,v) in _src
                if !haskey(amalgamated, k)
                    merge!(amalgamated, Dict(k => v))
                else
                    for (kk, vv) in _src[k]
                        if !haskey(amalgamated[k], kk)
                            merge!(amalgamated[k], Dict(kk => vv))
                        else
                            for (kkk, vvv) in _src[k][kk]
                                if !haskey(amalgamated[k][kk], kkk)
                                    merge!(amalgamated[k][kk], Dict(kkk => vvv))
                                else
                                    for (kkkk, vvvv) in _src[k][kk][kkk]
                                        if !haskey(amalgamated[k][kk][kkk], kkkk)
                                            merge!(amalgamated[k][kk][kkk], Dict(kkkk => vvvv))
                                        end
                                    end
                                end
                            end
                        end
                    end
                end
            end
        end
    end
end

open("UTILS/post/gerda-mage-sim.json", "w") do f
    JSON.print(f, amalgamated, 4)
end
