#= write_toc.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Wed 15 Aug 2018
 =#
using JSON

# write TOC in _layout/default.html

function write_toc()
    volumes = []

    if !isfile("$gerda_ms/UTILS/post/gerda-mage-sim.json")
        @info "generating gerda-mage-sim.json"
        include("$gerda_ms/UTILS/post/meta-amalgamator.jl")
    end

    for (key, val) in JSON.parsefile("$gerda_ms/UTILS/post/gerda-mage-sim.json")
        push!(volumes, key)
    end

    sort!(volumes)

    for page in ["default", "page"]
        open("$gerda_ms/UTILS/website/_layouts/$page.html") do f
            global lines = readlines(f, keep=true)
        end

        containstoc(s) = occursin("<!--", s) &&
                         occursin("TOC" , s) &&
                         occursin("-->" , s)
        first = findfirst(containstoc, lines)
        last  =  findlast(containstoc, lines)

        (first == nothing) && error("TOC section not found in $page.html, please",
                                    "put two '<!-- TOC -->' lines in the file.")

        if last > first+1
            for _ in first+1:last-1
                deleteat!(lines, first+1)
            end
        end

        for v in reverse(volumes)
            insert!(lines, first+1, "        <a href=\"{{site.baseurl}}/volumes/$v.html\">$v</a><br>\n")
        end

        open("$gerda_ms/UTILS/website/_layouts/$page.html", "w") do o
            [write(o, l) for l in lines]
        end
    end
end
