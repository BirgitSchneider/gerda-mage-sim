#= build_site.jl
 =
 = Author: Luigi Pertoldi - pertoldi@pd.infn.it
 = Created: Tue 21 Aug 2018
 =#

gerda_ms = abspath("$(@__DIR__)/../../..")
scripts = "$gerda_ms/UTILS/website/_scripts"

include("$scripts/write_toc.jl")
include("$scripts/write_volume_page.jl")
include("$scripts/write_part_page.jl")

@info "writing TOC..."
write_toc()

@info "writing volume pages..."
for v in readdir(gerda_ms)
    !isdir("$gerda_ms/$v") && continue
    write_volume_page(v)
    for p in readdir("$gerda_ms/$v")
        write_part_page(v, p)
    end
end
