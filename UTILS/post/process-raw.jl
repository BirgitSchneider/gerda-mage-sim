#!/usr/bin/env julia
#= process-raw.jl
 =
 = Julia script to tier4ize raw files and produce default spectra files
 =
 = Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 04/01/2018
 =#

isfile("./process-raw.jl") || error("Please run this script from where process-raw.jl is!")

function usage()
    println("""
            USAGE: julia process-raw.jl [ FILE1 FILE2 ... ]
                   julia process-raw.jl [ DIR ]

            CAVEATS:

            only one t4z- file is produced, all input files must reside in the same folder

            please set the following env variables:
              - GERDA_DETDATA: path to the location of json files needed by the tier4izer
              - TIER4IZER: tier4ize executable (if not present in PATH)
              - GENSPECTRA: gen-spectra executable (if not present in PATH)
              - DESTDIR: path to the external directory that will contain all the
                         processed files, with the same directory structure
            """)
    exit()
end

# check if everything is set up correctly
try
    global GERDA_DETDATA = ENV["GERDA_DETDATA"]
    global DESTDIR = ENV["DESTDIR"]
catch
    println("Missing environment variables!")
    usage()
end

try
    global TIER4IZER = readchomp(`which tier4ize`)
catch
    try
        global TIER4IZER = ENV["TIER4IZER"]
    catch
        println("tier4ize executable not found!")
        usage()
    end
end

try
    global GENSPECTRA = readchomp(`which gen-spectra`)
catch
    if isfile("gen-spectra")
        global GENSPECTRA = abspath("gen-spectra")
    else
        try
            global GENSPECTRA = ENV["GENSPECTRA"]
        catch
            println("""
                    gen-spectra not found! Is it compiled?
                    \$ g++ \$(root-config --cflags --libs) -lTreePlayer -o gen-spectra gen-spectra.cxx
                    """)
            usage()
        end
    end
end

println("""
        GERDA_DETDATA : $GERDA_DETDATA
        TIER4IZER     : $TIER4IZER
        GENSPECTRA    : $GENSPECTRA
        DESTDIR       : $DESTDIR
        """)

# check for arguments
length(ARGS) == 0 && usage()

filelist = Array{String}(0)

# if you provided a directory
if ( length(ARGS) == 1 && isdir(ARGS[1]) )
    templist = readdir(ARGS[1])
    for item in templist
        absitem = abspath(joinpath(ARGS[1],item))
        if isfile(absitem)
            if !ismatch(r"raw.*.root", basename(absitem))
                warn("Non ROOT file discarded: $absitem")
                continue
            end
            push!(filelist, absitem)
        else warn("$absitem discarded")
        end
    end
# if you provided a file list
else
    for item in ARGS
        absitem = abspath(item)
        if isfile(absitem)
            if !ismatch(r"raw.*.root", basename(absitem))
                warn("Non ROOT file discarded: $absitem")
                continue
            end
            push!(filelist, absitem)
        else error("$absitem: not a file")
        end
    end
end

length(filelist) == 0 && error("Empty filelist")

for i in 1:length(filelist)-1
    if dirname(filelist[i]) != dirname(filelist[i+1])
        error("The selected files do not live in the same directory!")
    end
end
BASE = dirname(filelist[1])
s = splitdir
deposit = s(BASE)[2]
isotope = s(s(BASE)[1])[2]
part    = s(s(s(BASE)[1])[1])[2]
volume  = s(s(s(s(BASE)[1])[1])[1])[2]

BASE = "$DESTDIR/$volume/$part/$isotope/$deposit"; mkpath(BASE)

# finally do something
t4zcomm = ```
          $(split(TIER4IZER))
          -m $GERDA_DETDATA/mapping-default-depr.json
          -c $(pwd())/configuration_dummy.root
          -o $BASE/t4z-$volume-$part-$isotope-$deposit.root
          $filelist
          ```
print_with_color(:green, "Running $t4zcomm ...\n")
run(ignorestatus(t4zcomm))
spccomm = `$(split(GENSPECTRA)) $BASE/t4z-$volume-$part-$isotope-$deposit.root`
print_with_color(:green, "Running $spccomm ...\n")
run(spccomm)
