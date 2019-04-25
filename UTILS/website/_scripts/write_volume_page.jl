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

function write_volume_page(page_name::String)

    @debug "writing page for volume $page_name"

    # check input
    try
        global meta = JSON.parsefile("$gerda_ms/$page_name/metadata.json")
    catch
        @debug "metadata.json not found for '$page_name', skipping..."
        return
    end

    table = ["part" "description" "mass" "volume/surface" "Density" "MaGe volumes"]

    for (key, val) in meta[page_name]
        key == "description" && continue

        @debug "processing $key =>" val

        # description
        if typeof(val["description"]) <: Array
            desc = join(val["description"], ' ')
        else
            desc = val["description"]
        end

        # mass
        if haskey(val, "simulated-properties")
            if typeof(val["simulated-properties"]) <: Dict
                matches = collect(filter(x -> occursin(r"mass-\w", x.first), val["simulated-properties"]))
                mass = !isempty(matches) ? schop(matches[1].second, digits) * get_units(matches[1].first) : "---"

                # volume
                matches = collect(filter(x -> occursin(r"volume-\w", x.first) ||
                                              occursin(r"surface-\w", x.first), val["simulated-properties"]))
                volume = !isempty(matches) ? schop(matches[1].second, digits) * get_units(matches[1].first) : "---"

                # density
                matches = collect(filter(x -> occursin(r"density-\w", x.first), val["simulated-properties"]))
                density = !isempty(matches) ? schop(matches[1].second, digits) * get_units(matches[1].first) : "---"
            else
                mass = volume = density = val["simulated-properties"]
            end
        else
            mass = volume = density = "---"
        end

        # cook up MaGe volumes list
        if haskey(val, "mage-volumes")
            if typeof(val["mage-volumes"]) <: Array
                vlist = join(["`$x`" for x in val["mage-volumes"]], ", ")
            else
                vlist = val["mage-volumes"]
            end
        else
            vlist = "---"
        end

        table = vcat(table, [key desc mass volume density vlist])
    end

    lines = """
    ---
    layout: page
    ---

    # $page_name

    $(meta[page_name]["description"]).

    """

    lines = lines * write_row(table[1, :])
    lines = lines * write_row(["--" for x in 1:size(table,2)])
#    lines = lines * write_row(["--:"; ["--" for x in 2:size(table,2)]])
    for i in 2:size(table, 1)
        lines = lines * write_row(table[i, :])
    end

    # images
    lines = lines * "\n<p align=\"center\">\n"
    images = readdir_regex(r"\w\.png", "$gerda_ms/$page_name")
    for img in images
        cp(img, "$gerda_ms/UTILS/website/images/$(basename(img))", force=true)
        lines = lines * "<img src=\"/images/$(basename(img))\" width=\"400\"/>\n"
    end
    lines = lines * "<p/>\n"

    open("$gerda_ms/UTILS/website/volumes/$page_name.md", "w") do io
        write(io, lines)
    end
end
