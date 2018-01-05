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

            please set the following env variables:
            - GERDA_DETDATA: path to the location of json files needed by the tier4izer
            - TIER4IZER: tier4ize executable (if not present in PATH)
            """)
    exit()
end

# check if everything is set up correctly
try
    global GERDA_DETDATA = ENV["GERDA_DETDATA"]
catch
    println("GERDA_DETDATA not set!")
    usage()
end

try
    global TIER4IZER = chomp(readstring(`which tier4izer`, stderr = DevNull))
catch
    try
        global TIER4IZER = ENV["TIER4IZER"]
    catch
        println("tier4ize executable not found!")
        usage()
    end
end

println("""
        GERDA_DETDATA : $GERDA_DETDATA
        TIER4IZER     : $TIER4IZER
        """)

# check for gen-spectra
if !isfile("gen-spectra")
    error("""
          gen-spectra not found! Is it compiled?
          \$ g++ \$(root-config --cflags --libs) -lTreePlayer -o gen-spectra gen-spectra.cxx
          """)
end

# check for arguments
size(ARGS,1) == 0 && usage()

filelist = Array{String}

# if you provided a directory
if ( size(ARGS,1) == 1 && isdir(ARGS[1]) )
    templist = readdir(ARGS[1])
    for item in templist
        if isfile(item)
            splitext(item)[2] != ".root" && ( warn("Non ROOT file discarded: $item"); continue )
            push!(filelist, abspath(item))
        else warn("$item discarded")
        end
    end
# if you provided a file list
else
    for item in ARGS
        if isfile(item)
            splitext(item)[2] != ".root" && ( warn("Non ROOT file discarded: $item"); continue )
            push!(filelist, abspath(item))
        else error("$item: not a file")
        end
    end
end

size(filelist,1) == 0 && error("Empty filelist")

for i in size(filelist,1)-1
    if dirname(filelist[i]) != dirname(filelist[i+1])
        error("The selected files do not live in the same directory!")
    end
    global BASE = dirname(filelist[i])
end

deposit = splitdir(BASE)[2]
isotope = splitdir(BASE)[2][2]
part    = splitdir(BASE)[2][2][2]
volume  = splitdir(BASE)[2][2][2][2]

# finally do something
run(```
    $TIER4IZER \
    -g $GERDA_DETDATA/ged-mapping-default.json \
    -s $GERDA_DETDATA/ged-settings-default.json \
    -l $GERDA_DETDATA/lar-mapping-default.json \
    -r $GERDA_DETDATA/ged-resolution-default.json \
    -t $GERDA_DETDATA/lar-settings-default.json \
    -c configuration_dummy.root" \
    -o $BASE/t4z-$volume-$part-$isotope-$deposit.root \
    $filelist

    gen-spectra $BASE/t4z-$volume-$part-$isotope-$deposit.root
    ```)
