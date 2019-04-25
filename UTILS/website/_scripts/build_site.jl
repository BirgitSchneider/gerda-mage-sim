#= build_site.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Tue 21 Aug 2018
 =#

gerda_ms = abspath("$(@__DIR__)/../../..")
scripts = "$gerda_ms/UTILS/website/_scripts"

include("$scripts/write_toc.jl")
include("$scripts/write_volume_page.jl")

@info "writing TOC..."
write_toc()

@info "writing volume pages..."
for x in readdir(gerda_ms)
    write_volume_page(x)
end
