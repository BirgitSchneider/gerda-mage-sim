#= mac-doctor.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Tue 1 May 2018
 =#
using JSON

# check arguments
(length(ARGS) < 1) && error("USAGE: julia mac-doctor.jl <macro-file> [...]")

exit_code = true
for fn in ARGS
    # check file existence
    mac = fn
    open(mac) do f
        global lines = readlines(f)
    end

    # build up database of fields from sim-parameters-all.json
    sim_par = JSON.parsefile(@__DIR__() * "/../sim-parameters-all.json")
    db = []
    for (vol,v1) in sim_par
        (vol == "general") && continue
        push!(db, vol)
        for (part,v2) in v1
            push!(db, part)
            for (iso,v3) in v2
                !occursin(iso, "mc-") && push!(db, iso)
            end
        end
    end
    push!(db, "coin", "edep")
    db = unique(db)

    # function to check for line in macro
    function check_line(lines, command, value)
        index = findfirst(x -> occursin(Regex(command), x), lines)
        words = split(lines[index], ['\t',' '], keepempty = false)

        if (index == 0)
            println("ERROR: $command command not found in macro!")
            global exit_code = false
            return
        end

        if (length(words) > 2)
            println("ERROR: too much words at line " * index)
            global exit_code = false
        end

        if (words[2] != value)
            println("ERROR: wrong value of $command")
            global exit_code = false
        end
    end

    # if it's a regular macro
    if occursin(r"raw-\w+-\w+-\w+-\w+-\w+\.mac", basename(mac))

        # extract fields
        fields = split(basename(mac), ['-', '.'])

        for i in [2:5]
            # check for presence of fields in database
            if (fields[i] in db)
                println("ERROR: $(fields[i]) not found in sim-parameters-all.json!")
                global exit_code = false
            end
        end

        # check if path matches name
        if !occursin("/$(fields[2])/$(fields[3])/$(fields[4])/$(fields[5])/log/$(basename(mac))", abspath(mac))
            println("ERROR: macro name does not match macro path")
            global exit_code = false
        end

        # check if preamble is correctly loaded
        check_line(lines,
                   "/control/execute",
                   "./$(fields[2])/$(fields[3])/$(fields[4])/$(fields[5])/log/preamble.mac")

        # check root filename
        check_line(lines,
                    "/MG/eventaction/rootfilename",
                    "./$(fields[2])/$(fields[3])/$(fields[4])/$(fields[5])/$(splitext(basename(mac))[1]).root")

    # if it's a preamble
    elseif (basename(mac) == "preamble.mac")
        check_line(lines,
                    "/MG/geometry/detector/geometryfile",
                    "geometry_PhaseII_BEGeFCCDgrowth_CoaxPhaseI.dat")
        check_line(lines,
                    "/MG/geometry/detector/matrixfile",
                    "matrix_phase_ii_StatusDec2015_pos.dat")
    else
        println("ERROR: $fn -> Extraneous file or not in the form raw-<volume>-<part>-<isotope>-<type>-###.mac / preamble.mac")
        global exit_code = false
    end
end

if (!exit_code)
    error("Errors occurred! Check your macros")
end
