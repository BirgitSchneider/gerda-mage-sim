#= process-volume.jl
 =
 = Julia script to join tier4ized files of every part into a global one (+ spc- file)
 = for the entire volume
 =
 = Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
 = Created: 07/01/2018
 =#
using ROOT, Cxx

isfile("./process-volume.jl") || error("Please run this script from where process-volume.jl is!")

function usage()
    println("""
            USAGE: julia process-volume.jl [VOLUME] [ISOTOPE] [TYPE]

            - VOLUME  = [cables|gedet|minishroud|...]
            - ISOTOPE = [K40|Pb214|Co60...]
            - TYPE    = [edep|coin]

            please set (if needed) the following env variables:
              - GENSPECTRA: gen-spectra executable (if not present in PATH)
            """)
    exit()
end

try
    global GENSPECTRA = chomp(readstring(`which gen-spectra`, stderr = DevNull))
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

println("GENSPECTRA : $GENSPECTRA\n")

# check for arguments
(length(ARGS) < 3) && usage()

# get parts
volume = ARGS[1]
parts = Array{String,1}(0)
base = "../../$volume"
tmparts = readdir(base)
for item in tmparts
    isdir("$base/$item") && push!(parts, item)
end

# check if isotope and t4z- file is present in all parts
isotope = ARGS[2]
simtype = ARGS[3]
for item in parts
    if !isdir("$base/$item/$isotope")
        error("$isotope folder not found for part $(item)!")
    end
    if !isfile("$base/$item/$isotope/$simtype/t4z-$volume-$item-$isotope-$simtype.root")
        println("$base/$item/$isotope/$simtype/t4z-$volume-$item-$isotope-$simtype.root not found,")
        print("do you want to process it now? [y/n] "); ans = read(STDIN, Char)
        if ( ans == 'y' || ans == 'Y' ) include("process-raw.jl $base/$item/$isotope/$simtype")
        else (println("Aborting..."); exit())
        end
    end
end

# now get all t4z- files
filelist = Array{String,1}(0)
for item in parts
    push!(filelist, abspath("$base/$item/$isotope/$simtype/t4z-$volume-$item-$isotope-$simtype.root"))
end

# now create the global t4z- file
cxxinclude("TChain.h")
chain = icxx"""new TChain("fTree");"""
for file in filelist
    println("Adding $file...")
    @cxx chain->Add(pointer(file))
end
outfile = abspath("$base/t4z-$volume-$isotope-$simtype.root")
println("Creating $outfile...")
@cxx chain->Merge(pointer(outfile))

spccomm = `$(split(GENSPECTRA)) $outfile`
print_with_color(:green, "Running $spccomm ...\n")
run(spccomm)
