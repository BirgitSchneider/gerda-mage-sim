# make.jl
#
# Author: Luigi Pertoldi - pertoldi@pd.infn.it
# Created: Fri 12 Apr 2019
#
using Logging, ProgressMeter

# parse command line arguments
if "-h" in ARGS || "--help" in ARGS
    println("""
            Run the production of the MaGe simulations by sending jobs to a SGE
            cluster.  The usage of the '--dry-run' option is recommended

            USAGE: julia make.jl [OPTIONS]

            OPTIONS: -h, --help   : pring usage and exit
                     --dry-run    : do everything normally but do not actually send jobs
                     --only <dir> : restrict search only to dir
                     --uuid <id>  : optionally append string <id> to job names

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

fake = "--dry-run" in ARGS ? true : false
fake && @warn "starting a dry run, no jobs will be actually sent"

only = get_argument("--only", optional=true)
only != nothing && @info "looking only in dir $only"

uuid = get_argument("--uuid", optional=true)
uuid != nothing && @info "using uuid \"$uuid\""

cd(@__DIR__)
gerda_ms = rstrip(abspath(pwd()), '/')
@debug "gerda-mage-sim: $gerda_ms"

# function to check if the job already exist in queue
# NOTE: the list of running jobs is fetched only once in the following
qstat_r = read(pipeline(`qstat -r`, `grep "Full jobname:"`), String)
joblist = sort(split(qstat_r)[3:3:end])
job_exists(jobname) = jobname in joblist

# function that tries to send a mage job to the SGE queue
function spawn_mage(mmacro::String; dryrun::Bool=false)
    job = replace(basename(mmacro), ".mac" => "") * (uuid != nothing ? "-$uuid" : "")
    if job_exists(job)
        @debug "job $job already in queue"
        return 2
    end

    qscript = ["$gerda_ms/UTILS/job-scheduler/mpik-run-mage.qsub", mmacro]

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

# check if needed aux files are there
@debug "checking presence of auxiliary files"
isfile("$gerda_ms/UTILS/container/gerda-sw-all_active.simg") ||
    error("could not find UTILS/container/gerda-sw-all_active.simg")

scandir = rstrip(only == nothing ? gerda_ms : "$gerda_ms/$only", '/')

prog = ProgressUnknown(dt=2, desc="[ Progress: scanned files:")

for (root, dirs, files) in walkdir(String(scandir))
    for mac in filter(x -> occursin(r"raw-.+-.+-.+-.+\.mac", x), files)
        ProgressMeter.next!(prog)
        rootfile = replace("$root/$mac", ".mac" => ".root")
        rootfile = replace(rootfile, "log/" => "")
        if !isfile(rootfile) || mtime(mac) >= ctime(rootfile)
            @debug "$rootfile must be (re)done"
            spawn_mage("$root/$mac", dryrun=fake)
        end
    end
end

ProgressMeter.finish!(prog)
