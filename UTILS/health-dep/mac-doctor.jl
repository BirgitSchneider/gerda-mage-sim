#= mac-doctor.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Tue 1 May 2018
 =#
using JSON

function check_macro(mac)

    myerror(str) = println("$mac: $str")

    ecode = 0
    # check file existence
    open(mac) do f
        global lines = readlines(f)
    end

    # function to check for line in macro
    function check_line(lines, command, values; ispath=false)
        index = findfirst(x -> occursin(Regex(command), x), lines)
        if (index == nothing)
            myerror("$command command not found in macro!")
            ecode = 1
            return
        end

        words = split(lines[index], ['\t',' '], keepempty = false)

        if (length(words) > 2)
            myerror("too much words at line " * index)
            ecode = 1
        end

        for v in values
            if ispath
                relpath(String(words[2])) == relpath(v) && return
            else
                words[2] == v && return
            end
        end

        myerror("wrong value of $command")
        ecode = 1
    end

    # if it's a regular macro
    if occursin(r"raw-.+-.+-.+-.+\.mac", basename(mac))

        # extract fields
        fields = split(basename(mac), ['-', '.'])

        # check if path matches name
        if !occursin("/$(fields[2])/$(fields[3])/$(fields[4])/$(fields[5])/log/$(basename(mac))", abspath(mac))
            myerror("macro name does not match macro path")
            ecode = 1
        end

        # check if preamble is correctly loaded
        check_line(lines,
            "/control/execute",
            ["./$(fields[2])/$(fields[3])/$(fields[4])/$(fields[5])/log/preamble.mac"],
            ispath=true)

        # check root filename
        check_line(lines,
            "/MG/eventaction/rootfilename",
            ["./$(fields[2])/$(fields[3])/$(fields[4])/$(fields[5])/$(splitext(basename(mac))[1]).root"],
            ispath=true)

    # if it's a preamble
    elseif (basename(mac) == "preamble.mac")
        @debug "$mac is a preamble macro"
        check_line(lines,
                    "/MG/geometry/detector/geometryfile",
                    ["geometry_PhaseII_BEGeFCCDgrowth_CoaxPhaseI.dat", "geometry.json"])
        check_line(lines,
                    "/MG/geometry/detector/matrixfile",
                    ["matrix_phase_ii_StatusDec2015_pos.dat", "matrix-PhaseII-dec15.json"])
    else
        myerror("extraneous file or not in the form raw-<volume>-<part>-<isotope>-<type>-###.mac / preamble.mac")
        ecode = 1
    end

    return ecode
end

 ##########
## ROUTINE ##
 ##########

gerda_ms = rstrip(abspath("$(@__DIR__)/../.."), '/')
cd(gerda_ms)

exit_code = 0

for (root, dirs, files) in walkdir(gerda_ms)
    occursin("$gerda_ms/alphas", root) && continue
    for mac in filter(x -> occursin(r"(raw-.+|preamble)\.mac", x), files)
        root = replace(root, "$gerda_ms/" => "")
        @debug "analyzing $root/$mac"
        global exit_code += check_macro("$root/$mac")
    end
end

if exit_code != 0
    error("Errors occurred! Check your macros")
end
