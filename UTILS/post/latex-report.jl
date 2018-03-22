#= latex-report.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: 14 Mar 2018
 =#
using JSON

(length(ARGS) == 0) && (println("USAGE: julia latex-report.jl [sim-parameters-all.json]"); exit())
const sims = JSON.parsefile(ARGS[1])

preamble =
    raw"""
    \documentclass[a4paper]{article}
    \usepackage[utf8]{inputenc}
    \usepackage[T1]{fontenc}
    \usepackage[english]{babel}
    \usepackage{microtype}
    \usepackage{booktabs}
    \usepackage{multirow}
    \usepackage{caption}
    \captionsetup[table]{position=top}
    \usepackage{xcolor}
    \usepackage{colortbl}
    \newcommand{\rclb}{\cellcolor{gray!30}}
    \usepackage{soul}
    \usepackage{enumitem}
    \title{\texttt{gerda-mage-sim} status report}
    \date{\today}
    \author{by \texttt{gerda-mage-sim/UTILS/post/report-latex.jl}}
    \begin{document}
    \maketitle
    """

#================================= INTRODUCTORY LIST ====================================#
# get run list
rl = sims["general"]["run-list"]
rl_printed = []
for i in [rl[1]:rl[end]...]
    if i in rl
        push!(rl_printed, "$i")
    else
        push!(rl_printed, "\\st{$i}")
    end
end

# get revision control info
git_tag    = readchomp(`git describe --tags --abbrev=0`)
git_commit = readchomp(pipeline(`git rev-parse HEAD`, ` cut -c1-7`))

# first list with general parameters
list =
    """
    \\begin{description}
    \\item[tag/commit] \\texttt{$git_tag/$git_commit}
        \\item[run list] \\{ $(join(rl_printed, ", ")) \\}
        \\item[total livetime] $(@sprintf("%.3f",sims["general"]["total-livetime-yr"])) yr
    \\end{description}
    """

#================================= TABLE WITH MASSES ====================================#
heading =
    raw"""
    \begin{table}
        \centering
        \caption{\textsc{Gerda} components for which background contaminations are simulated. Mass, volume and density, as implemented in MaGe, are also reported.}
        \begin{tabular}{llccc}
            \toprule
            volume & part & mass [g] & volume [cm$^3$] & density [g/cm$^3$] \\
            \midrule
    """

body = ""

closing =
    raw"""
            \bottomrule
        \end{tabular}
    \end{table}
    """

# function to write a line in the masses table
function write_line(volume_name, part_name, mc_mass_kg, mc_volume_cm3, mc_density_kgcm3, is_surface = false)
    # indent
    line = "        "
    # write volume name and part name
    line = line * @sprintf("%-25s", volume_name) * "& "
    line = line * @sprintf("%-30s", part_name) * "& "
    # write the mass
    !isa(mc_mass_kg, String) ?
        line = line * @sprintf(" %-10.3f", mc_mass_kg) * "& " :
        line = line * @sprintf(" %-10s", mc_mass_kg) * "& "
    # write the volume
    !isa(mc_volume_cm3, String) ?
        (!is_surface ?
            line = line * @sprintf("%-14.3f", mc_volume_cm3) * "& " :
            line = line * @sprintf("%-7.3f", mc_volume_cm3) * " cm\$^2\$" * "& ") :
        (!is_surface ?
            line = line * @sprintf("%-14s", mc_volume_cm3) * "& " :
            line = line * @sprintf("%-7s", mc_volume_cm3) * " cm\$^2\$ " * "& ")
    # write the density
    !isa(mc_density_kgcm3, String) ?
        line = line * @sprintf("%-10.3f", mc_density_kgcm3) * "\\\\\n" :
        line = line * @sprintf("%-10s", mc_density_kgcm3) * "\\\\\n"

    return line
end

# array of strings, keys in the dictionary
all_keys = collect(keys(sims))
# loop over volumes
for vol in all_keys
    (vol == "general") && continue

    # dictionary associated to this volume
    parts = copy(sims[vol])

    # delete non-parts from this dictionary
    for (p,val) in sims[vol]
        if !isa(val, Dict) # it should be a dictionary
            delete!(parts, p)
        elseif (!haskey(val, "mc-volume-cm3") && # it should always contain the volume or the surface
                !haskey(val, "mc-surface-cm2"))
            delete!(parts, p)
        end
    end

    first = true;
    for (p,val) in parts
        # in LaTeX you have to escape _ characters
        v = replace("\\texttt{$vol}", "_", "\\_")
        p = replace("\\texttt{$p}", "_", "\\_")

        # check if the number is there and WRITE!
        body = body * write_line(
            first ? v : " ",
            p,
            haskey(val, "mc-mass-kg") ? val["mc-mass-kg"] : "--",
            haskey(val, "mc-volume-cm3") ? val["mc-volume-cm3"] : val["mc-surface-cm2"],
            haskey(val, "mc-density-kgcm3") ? 1E03*val["mc-density-kgcm3"] : "--",
            haskey(val, "mc-surface-cm2")
        )
        first = false
    end

    # write row with sum for volume, if present
    if (haskey(sims[vol], "mc-mass-kg") ||
        haskey(sims[vol], "mc-volume-cm3") ||
        haskey(sims[vol], "mc-density-kgcm3"))
        body = body * write_line(
            "\\rclb\\textsc{total}",
            " ",
            haskey(sims[vol], "mc-mass-kg") ? sims[vol]["mc-mass-kg"] : " ",
            haskey(sims[vol], "mc-volume-cm3") ? sims[vol]["mc-volume-cm3"] : " ",
            haskey(sims[vol], "mc-density-kgcm3") ? 1E03*sims[vol]["mc-density-kgcm3"] : " ",
        )
    end

    # separate parts with midrule
    if (vol != all_keys[end])
        body = body * "        \\midrule\n"
    end
end

# whoami
user = readchomp(`whoami`)

# write to file
open("/tmp/gerda-mage-sim-report-$user.tex", "w") do f
    write(f, preamble * list * '\n' * heading * body * closing)
end

#================================= TABLE WITH PRIMARIES ====================================#

# non-isotopes: there's a much portable way to do this, but I don't want to code it...
non_iso = ["mc-mass-kg", "mc-density-kgcm3", "mc-volume-cm3", "mc-surface-cm2", "mean-enrichment-fraction"]

# let's do a table for each volume
for vol in all_keys
    (vol == "general") && continue

    parts = copy(sims[vol])
    # delete non-parts from parts array
    for (p,val) in sims[vol]
        if !isa(val, Dict)
            delete!(parts, p)
        elseif (!haskey(val, "mc-volume-cm3") &&
                !haskey(val, "mc-surface-cm2"))
            delete!(parts, p)
        end
    end

    # get all the isotopes in that volume
    iso_all = []
    for p in collect(keys(parts))
        for is in collect(keys(sims[vol][p]))
            if !(is in non_iso)
                push!(iso_all, is)
            end
        end
    end

    # delete redundancies
    iso = unique(iso_all)
    # sort alphabetically
    sort!(iso)

    # this is needed to set the tabular environment
    c_c = "";
    for _ in iso
        c_c = c_c * 'c'
    end

    # in LaTeX you have to escape _ characters
    v = replace("\\texttt{$vol}", "_", "\\_")
    heading =
        """
        \\begin{table}
            \\caption{Number of simulated events for each isotope in the $v \\textsc{Gerda} volume. First row refers to \\texttt{edep}, second to \\texttt{coin}. The number of primaries is divided by \$10^8\$.}
            \\centerline{%
            \\begin{tabular}{ll$c_c}
                \\toprule
                \\multirow{2}{*}{part} & \\multirow{2}{*}{type} & \\multicolumn{$(length(iso))}{c}{primaries/\$10^8\$} \\\\
                \\cmidrule{3-$(length(iso)+2)}
                     &      & \\texttt{$(join(iso, "} & \\texttt{"))} \\\\
                \\midrule
        """

    body = ""

    closing =
        raw"""
                \bottomrule
            \end{tabular}%
            }
        \end{table}
        """

    # function to write a row in the primaries table
    function write_line_prim(part_dict, part_name, iso_array)
        line = ""
        for t in ["edep", "coin"]
            # line with edep, indent
            line = line * "        "
            line = line * @sprintf("%-30s", t == "edep" ? part_name : " ") * " & "
            line = line * @sprintf("%-5s", "\\texttt{$t}") * " & "
            for i in iso_array
                if (haskey(part_dict, i) &&
                    haskey(part_dict[i], t) &&
                    haskey(part_dict[i][t], "primaries"))
                    line = line * @sprintf("%8.2f", part_dict[i][t]["primaries"]/1E8)
                else
                    line = line * "   --"
                end
                line = line * (i != iso_array[end] ? " & " : " \\\\\n")
            end
        end
        return line
    end

    # WRITE
    for p in sort(collect(keys(parts)))
        # in LaTeX you have to escape _ characters
        pn = replace("\\texttt{$p}", "_", "\\_")
        body = body * write_line_prim(sims[vol][p], pn, iso)
    end
    # write total if present
    if (haskey(sims[vol], "mc-mass-kg") ||
        haskey(sims[vol], "mc-volume-cm3") ||
        haskey(sims[vol], "mc-density-kgcm3"))
        body = body * write_line_prim(sims[vol], "\\rclb\\textsc{total}", iso)
    end

    # write-append
    open("/tmp/gerda-mage-sim-report-$user.tex", "a") do f
        write(f, '\n' * heading * body * closing)
    end
end

# There's the \end{document} line missing!
open("/tmp/gerda-mage-sim-report-$user.tex", "a") do f
    write(f, "\n\\end{document}")
end
#====================================== COMPILE ============================================#

try
    readchomp(`which pdflatex`)
catch
    warn("pdflatex executable not found, saving only the .tex...")
    rm("./gerda-mage-sim-report.tex")
    run(`mv /tmp/gerda-mage-sim-report-$user.tex ./gerda-mage-sim-report.tex`)
    chmod("./gerda-mage-sim-report.tex",0o660)
    exit()
end

run(`pdflatex -output-directory=/tmp -interaction=nonstopmode /tmp/gerda-mage-sim-report-$user.tex`)
rm("./gerda-mage-sim-report.pdf")
run(`cp /tmp/gerda-mage-sim-report-$user.pdf ./gerda-mage-sim-report.pdf`)

# change file permissions to rw-rw----
chmod("./gerda-mage-sim-report.pdf",0o660)

#=
if is_linux()
    run(`display gerda-mage-sim-report.pdf`)
elseif is_apple()
    run(`open gerda-mage-sim-report.pdf`)
end
=#
