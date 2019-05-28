# make.jl
#
# Run the GERDA PDF production
#
# Author: Luigi Pertoldi - pertoldi@pd.infn.it
# Created: Thu 11 Apr 2019
#
using JSON, Logging

# parse command line arguments
if "-h" in ARGS || "--help" in ARGS
    println("""
            Run the GERDA PDF production by sending jobs to a SGE cluster.
            The usage of the '--dry-run' option is recommended

            USAGE: julia make.jl [OPTIONS] --cycle <name>

            OPTIONS: -h, --help      : pring usage and exit
                     --dry-run       : do everything normally but do not actually send jobs
                     --cycle <name>  : set the cycle name
                     --dest <dir>    : set the output folder (default
                                       '/lfs/l1/gerda/gerda-simulations/gerda-pdfs')
                     --only <volume> : restrict to selected volume
                     --exclude <volume> : exclude selected volume

            Run with `JULIA_DEBUG=all julia make.jl` to enable debug mode
            """)
    exit(0)
end

function get_argument(str::String; optional=false)
    if !optional
        !in(str, ARGS) && error("option '$str' is mandatory")
        length(ARGS) < findfirst(isequal(str), ARGS)+1 && error("option '$str' is mandatory")
    else
        !in(str, ARGS) && return nothing
        length(ARGS) < findfirst(isequal(str), ARGS)+1 && return nothing
    end
    return ARGS[findfirst(isequal(str), ARGS)+1]
end

# get cycle name
cycle = get_argument("--cycle")

# partial processing?
only = get_argument("--only", optional=true)
excl = get_argument("--exclude", optional=true)

# retrieve path to gerda-mage-sim
cd(@__DIR__)
gerda_ms = rstrip(abspath(pwd() * "/../.."), '/')

# get destination
gerda_pdfs = get_argument("--dest", optional=true)
if gerda_pdfs == nothing
    gerda_pdfs = "/lfs/l1/gerda/gerda-simulations/gerda-pdfs"
end
destdir = "$gerda_pdfs/$cycle"

fake = "--dry-run" in ARGS ? true : false

fake && @warn "starting a dry run, no jobs will be actually sent"
@debug "gerda-mage-sim: $gerda_ms"
@info "processing cycle $cycle in directory $gerda_pdfs"

# function to check if the job already exist in queue
# NOTE: the list of running jobs is fetched only once in the following
qstat_r = read(pipeline(`qstat -r`, `grep "Full jobname:"`), String)
joblist = sort(split(qstat_r)[3:3:end])
job_exists(jobname) = jobname in joblist

# function that tries to send a t4z-gen job to the SGE queue
function spawn_t4z_gen(src::String, dest::String; calib::Bool=false, dryrun::Bool=false)
    it = split(src, "/")
    job = "t4z-$(it[end-3])-$(it[end-2])-$(it[end-1])-$(it[end])-$cycle"
    if job_exists(job)
        @debug "job $job already in queue"
        return 2
    end

    qscript = ["$gerda_ms/UTILS/job-scheduler/mpik-t4z-gen.qsub",
               "--config", "$gerda_ms/UTILS/post/settings/t4z-gen-settings-$(it[end]).json",
               "--destdir", dest]
    calib && push!(qscript, "--calib")
    push!(qscript, src)

    if dryrun
        @info "would send job $job"
        @debug "would execute $(`qsub -N $job $qscript`)"
        return true
    else
        @debug "executing $(`qsub -N $job $qscript`)"
        status = success(run(pipeline(`qsub -N $job $qscript`, stdout=devnull), wait=true))
        status == false && @warn "job $job submission failed"
        @info "job $job requested"
        return status
    end
end

# function that tries to send a pdf-gen job to the SGE queue
function spawn_pdf_gen(src::String, dest::String; larveto::Bool=false, dryrun::Bool=false)
    it = split(src, "/")
    job = "pdf-$(it[end-2])-$(it[end-1])-$(it[end])-$cycle"
    if larveto == true
        job *= "-larveto"
    end
    if job_exists(job)
        @debug "job $job already in queue"
        return 2
    end

    qscript = ["$gerda_ms/UTILS/job-scheduler/mpik-pdf-gen.qsub",
               "--config", "$gerda_ms/UTILS/post/settings/pdf-gen-settings$(larveto ? "-larveto" : "").json",
               "--destdir", dest,
               src]

    parent = "t4z-$(it[end-2])-$(it[end-1])-$(it[end])-edep-$cycle,t4z-$(it[end-2])-$(it[end-1])-$(it[end])-coin-$cycle"

    if dryrun
        @info "would send job $job"
        @debug "would execute $(`qsub -hold_jid $parent -N $job $qscript`)"
        return true
    else
        @debug "executing $(`qsub -hold_jid $parent -N $job $qscript`)"
        status = success(run(
            pipeline(`qsub -hold_jid $parent -N $job $qscript`, stdout=devnull),
            wait=true
        ))
        status == false && @warn "job $job submission failed"
        @info "job $job requested"
        return status
    end
end

# find utility emulation
# returns the path of the found objects *relative to the starting directory path*
function find_files(path::String, reg::Regex="")
    rstrip(path, '/')
    list = String[]
    for (root, dirs, files) in walkdir(path)
        root = replace(root, "$path/" => "")
        append!(list, filter(x -> occursin(reg, x), "$root/" .* dirs))
        append!(list, filter(x -> occursin(reg, x), "$root/" .* files))
    end

    return list
end

 # ============= # 
## START ROUTINE ##
 # ============= #

# check if needed aux files are there
@debug "checking presence of auxiliary files"
isfile("$gerda_ms/UTILS/container/gerda-mage-sim-utils_active.simg") ||
    error("could not find UTILS/container/gerda-mage-sim-utils_active.simg")
isfile("$gerda_ms/UTILS/post/settings/gerda-larmap.root") ||
    error("could not find UTILS/post/settings/gerda-larmap.root, the PDF production won't work")

# read in list of runs
open("settings/run-list.json") do f
    global runs = JSON.parse(f)["run-list"]
end

# copy the configs
@debug "copying configs"
mkpath("$destdir/prod-settings")
for f in readdir("$gerda_ms/UTILS/post/settings/")
    !fake && cp("$gerda_ms/UTILS/post/settings/$f", "$destdir/prod-settings/$f", force=true)
end

# generate amalgamated gerda-mage-sim JSON
@info "generating gerda-mage-sim.json"
include("$gerda_ms/UTILS/post/meta-amalgamator.jl")
!fake && cp("$gerda_ms/UTILS/post/gerda-mage-sim.json", "$destdir/gerda-mage-sim.json", force=true)

# directories to be scanned
maindirs = ["alphas", "cables", "calib", "electronics",
            "ge_holders", "gedet", "lar", "larveto",
            "minishroud"]

if only != nothing
    maindirs = [only]
end

if excl != nothing
   filter!(e->e!=excl,maindirs)
end

# main loop
for volume in maindirs
	!isdir("$gerda_ms/$volume") && continue;

    @info "processing $volume/ directory"

    # special calib folder treatment
    # the following approach consists in looking into the config JSON file
    # rather than starting from simulations available
    if volume == "calib"
        try
            cfg = JSON.parse(open("$gerda_ms/UTILS/post/settings/calib-pdf-settings.json"))["calib"]

            # inspect the settings file and see if we have something to spawn
            for (iso,d1) in cfg for (src,d2) in d1
                length(src) != 2 && src[1] != 's' && continue
                for (pos,d3) in d2
                    for mode in ["single", "multi"]
                        !d3["$mode-mode"] && continue
                        part = "$(mode)_$(src)_$pos"
                        @debug "asked for calib/$part/$iso PDF"
                        if !isdir("$gerda_ms/calib/$part/$iso/edep") && !isdir("$gerda_ms/calib/$part/$iso/coin")
                            @warn "asked for calib/$part/$iso PDF but no edep|coin folder could be found"
                            continue
                        end
                        # see if a t4z-gen job should be spawned
                        t4zspawn = false
                        for type in ["edep", "coin"]
                            if isdir("$gerda_ms/calib/$part/$iso/$type")
                                file = """$destdir/calib/$part/$iso/$type/t4z-calib-$part-$iso-$type-$(d3["id"]).root"""
                                if !isfile(file)
                                    @debug "$file does not exist"
                                    spawn_t4z_gen("$gerda_ms/calib/$part/$iso/$type", destdir, calib=true, dryrun=fake)
                                    t4zspawn = true
                                else
                                    @debug "$file is up-to-date"
                                end
                            end
                        end
                        # see if a pdf-gen job should be spawned
                        for applylar in [false, true]
                            file = """$destdir/calib/$part/$iso/pdf-calib-$part-$iso-$(d3["id"])$(applylar ? "-larveto" : "").root"""
                            if !isfile(file) || t4zspawn
                                @debug "$file must be (re)done"
                                spawn_pdf_gen("$gerda_ms/calib/$part/$iso", destdir, larveto=applylar, dryrun=fake)
                            else
                                @debug "$file is up-to-date"
                            end
                        end
                    end
                end
            end end
        catch ex
            if isa(ex, LoadError)
                @warn "problems reading $gerda_ms/UTILS/post/settings/calib-pdf-settings.json, skipping calib/"
            else
                throw(ex)
            end
        end
        continue
    end

    # special alphas treatment
    if volume == "alphas"
        mkpath("$destdir/alphas")
        for f in find_files("$gerda_ms/alphas", r"pdf-.*\.root$")
            mkpath(dirname("$destdir/alphas/$f"))
            @debug "copying $gerda_ms/alphas/$f"
            !fake && cp("$gerda_ms/alphas/$f", "$destdir/alphas/$f", force=true)
        end
        continue
    end

    # ordinary directory
    # loop on a part/isotope/type list
    for d in unique([s[1:end-5] for s in find_files("$gerda_ms/$volume", r".+/.+/(edep|coin)$")])

        # 'd' is of the form part/isotope
        items = split(d, '/')
        part = items[1]
        isotope = items[2]

        # skip chan-wise simulations
        if occursin("chanwise", part)
            @debug "skipping $volume/$part"
            continue
        end

        @debug "processing $volume/$part/$isotope"

        # see if a t4z-gen job should be spawned
        t4zspawn = false
        for type in ["edep", "coin"]
            if isdir("$gerda_ms/$volume/$d/$type")
                for r in runs
                    file = "$destdir/$volume/$d/$type/t4z-$volume-$part-$isotope-$type-run$r.root"
                    if !isfile(file)
                        @debug "$file does not exist"
                        spawn_t4z_gen("$gerda_ms/$volume/$d/$type", destdir, dryrun=fake)
                        t4zspawn = true
                        break
                    else
                        @debug "$file is up-to-date"
                    end
                end
            end
        end

        # see if a pdf-gen job should be spawned
        for applylar in [false, true]
            file = "$destdir/$volume/$d/pdf-$volume-$part-$isotope$(applylar ? "-larveto" : "").root"
            if !isfile(file) || t4zspawn
                @debug "$file must be (re)done"
                spawn_pdf_gen("$gerda_ms/$volume/$d", destdir, larveto=applylar, dryrun=fake)
            else
                @debug "$file is up-to-date"
            end
        end
    end
end

if !fake
    if !isfile("$gerda_ms/UTILS/container/gerda-tgsend.simg")
        @warn "could not find gerda-tgsend.simg, disabling Telegram notifications"
    else
        @debug "executing qwatch Telegram notification daemon script"
        run(`$gerda_ms/UTILS/post/qwatch --daemon $cycle`, wait=false)
    end
end
