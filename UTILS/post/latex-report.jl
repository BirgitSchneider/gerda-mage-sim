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
    \usepackage{caption}
    \captionsetup[table]{position=top}
    \usepackage{xcolor}
    \usepackage{soul}
    \usepackage{enumitem}
    \title{\texttt{gerda-mage-sim} status report}
    \date{\today}
    \author{\tt gerda-mage-sim/UTILS/post/report-latex.jl}
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

git_tag    = readchomp(`git describe --tags --abbrev=0`)
git_commit = readchomp(pipeline(`git rev-parse HEAD`, ` cut -c1-7`))

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

closing =
    raw"""
            \bottomrule
        \end{tabular}
    \end{table}
    """

body = ""
# loop over volumes
all_keys = collect(keys(sims))
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

    first = true;
    for (p,val) in parts
        # in LaTeX you have to escape _ characters
        v = replace("\\texttt{$vol}", "_", "\\_")
        p = replace("\\texttt{$p}", "_", "\\_")
        # indent
        body = body * "        "
        # write volume name if first row, write the part name
        first ?
            body = body * @sprintf("%-25s& %-30s &", v, p) :
            body = body * @sprintf("%-25s& %-30s &", ' ', p)
        # write the mass
        body = body * "$(haskey(val, "mc-mass-kg") ?
            @sprintf(" %-10.3f", val["mc-mass-kg"]) :
            @sprintf(" %-10s","N.A.")) &"
        # write the volume
        body = body * "$(haskey(val, "mc-volume-cm3") ?
            @sprintf(" %-13.3f", val["mc-volume-cm3"]) :
            @sprintf(" %-6.3f", val["mc-surface-cm2"]) * " cm\$^2\$") &"
        # write the density
        body = body * "$(haskey(val, "mc-density-kgcm3") ?
            @sprintf(" %-10.3f", 1E03*val["mc-density-kgcm3"]) :
            @sprintf(" %-10s","N.A.")) \\\\\n"

        first = false
    end

    # write row with sum, if present
    if (haskey(sims[vol], "mc-mass-kg") ||
        haskey(sims[vol], "mc-volume-cm3") ||
        haskey(sims[vol], "mc-density-kgcm3"))
        # write first two columns
        body = body * "        "
        body = body * @sprintf("%-25s& %-30s &", ' ', "\\textsc{total}")
        # write the mass
        body = body * "$(haskey(sims[vol], "mc-mass-kg") ?
            @sprintf(" %-10.3f", sims[vol]["mc-mass-kg"]) :
            @sprintf(" %-10s",' ')) &"
        # write the volume
        body = body * "$(haskey(sims[vol], "mc-volume-cm3") ?
            @sprintf(" %-13.3f", sims[vol]["mc-volume-cm3"]) :
            @sprintf(" %-13.3f", ' ')) &"
        # write the density
        body = body * "$(haskey(sims[vol], "mc-density-kgcm3") ?
            @sprintf(" %-10.3f", 1E03*sims[vol]["mc-density-kgcm3"]) :
            @sprintf(" %-10s",' ')) \\\\\n"
    end
    if (vol != all_keys[end])
        body = body * "        \\midrule\n"
    end
end

massestable = heading * body * closing

# write
open("/tmp/gerda-mage-sim-report.tex", "w") do f
    write(f, preamble * list * '\n' * massestable)
end

#================================= TABLE WITH PRIMARIES ====================================#

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
            if (is != "mc-mass-kg"    && is != "mc-density-kgcm3" &&
                is != "mc-volume-cm3" && is != "mc-surface-cm2" && is != "mean-enrichment-fraction")
                push!(iso_all, is)
            end
        end
    end

    iso = unique(iso_all)
    sort!(iso)

    c_c = "";
    for _ in iso
        c_c = c_c * 'c'
    end

    # in LaTeX you have to escape _ characters
    v = replace("\\texttt{$vol}", "_", "\\_")
    heading =
        """
        \\begin{table}
            \\centering
            \\caption{Number of simulated events for each isotope in the $v \\textsc{Gerda} volume.}
            \\begin{tabular}{l$c_c}
                \\toprule
                part & \\texttt{$(join(iso, "} & \\texttt{"))} \\\\
                \\midrule
        """

    closing =
        raw"""
                \bottomrule
            \end{tabular}
        \end{table}
        """

    body = ""

    for (p,val) in parts
        # in LaTeX you have to escape _ characters
        p = replace("\\texttt{$p}", "_", "\\_")
        # indent
        body = body * "        "
        body = body * @sprintf("%-30s", p) * " & "
        for i in iso
            if (haskey(val, i) &&
                haskey(val[i], "edep") &&
                haskey(val[i]["edep"], "primaries"))
                body = "$body $(val[i]["edep"]["primaries"])"
            else
                body = body * "      "
            end
            i != iso[end] ? (body = body * " & ") : (body = body * " \\\\\n")
        end
        # indent
        body = body * "        "
        body = body * @sprintf("%-30s", ' ') * " & "
        for i in iso
            if (haskey(val, i) &&
                haskey(val[i], "coin") &&
                haskey(val[i]["coin"], "primaries"))
                body = "$body $(val[i]["coin"]["primaries"])"
            else
                body = body * "      "
            end
            i != iso[end] ? (body = body * " & ") : (body = body * " \\\\\n")
        end
    end
    if (haskey(sims[vol], "mc-mass-kg") ||
        haskey(sims[vol], "mc-volume-cm3") ||
        haskey(sims[vol], "mc-density-kgcm3"))
        # write first two columns
        body = body * "        "
        body = body * @sprintf("%-30s &", "\\textsc{total}")
        for i in iso
            body = "$body $(haskey(sims[vol][i], "edep") ? sims[vol][i]["edep"]["primaries"] : "    ")"
            i != iso[end] ? (body = body * " & ") : (body = body * " \\\\\n")
        end
        body = body * "        "
        body = body * @sprintf("%-30s &", ' ')
        for i in iso
            body = "$body $(haskey(sims[vol][i], "coin") ? sims[vol][i]["coin"]["primaries"] : "   ")"
            i != iso[end] ? (body = body * " & ") : (body = body * " \\\\\n")
        end
    end

    # write-append
    open("/tmp/gerda-mage-sim-report.tex", "a") do f
        write(f, '\n' * heading * body * closing)
    end
end

open("/tmp/gerda-mage-sim-report.tex", "a") do f
    write(f, "\n\\end{document}")
end
#====================================== COMPILE ============================================#

try
    readchomp(`which pdflatex`)
catch
    warn("pdflatex executable not found, saving only the .tex...")
    run(`mv /tmp/gerda-mage-sim-report.tex .`)
    exit()
end

run(`pdflatex -output-directory=/tmp -interaction=nonstopmode /tmp/gerda-mage-sim-report.tex`)
run(`cp /tmp/gerda-mage-sim-report.pdf .`)
#=
if is_linux()
    run(`display gerda-mage-sim-report.pdf`)
elseif is_apple()
    run(`open gerda-mage-sim-report.pdf`)
end
=#
