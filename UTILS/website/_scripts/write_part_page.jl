#= write_volume-page.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Tue 21 Aug 2018
 =#

using JSON

include("$scripts/fs_tools.jl")
include("$scripts/units_tools.jl")

function write_row(a::Array{<:Any,1})
    r = "| $(a[1])"
    [r = "$r | $e" for e in a[2:end]]
    return "$r |\n"
end

digits = 4

function write_part_page(volume::String, part::String)

    @debug "writing page for part $part"

    # check input
    try
        global meta = JSON.parsefile("$gerda_ms/$volume/$part/metadata.json")
    catch
        @debug "metadata.json not found for '$volume/$part', skipping..."
        return
    end

    table = ["isotope [type]" "primaries" "software" "notes" "contact"]

    for (iso, val) in meta[volume][part]
        for (type, vall) in val

            @debug "processing $iso/$type =>" vall

            prim = vall["primaries"]
            sw = "MaGe: `$(vall["mage-version"]["revision"])` (gerda-sw-all: `$(vall["mage-version"]["gerda-sw-all"])`)"
            notes = haskey(vall, "notes") ? vall["notes"] : ""
            contact = vall["contact"]

            table = vcat(table, ["$iso [$type]" prim sw notes contact])
        end
    end

    lines = """
    ---
    layout: page
    ---

    # $part

    """

    lines = lines * write_row(table[1, :])
    lines = lines * write_row(["--" for x in 1:size(table,2)])
#    lines = lines * write_row(["--:"; ["--" for x in 2:size(table,2)]])
    for i in 2:size(table, 1)
        lines = lines * write_row(table[i, :])
    end

    open("$gerda_ms/UTILS/website/volumes/parts/$part.md", "w") do io
        write(io, lines)
    end
end
