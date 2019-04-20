using DelimitedFiles, JSON

for arg in ARGS
    println(arg)
    lines = readdlm(arg, '\n')

    lines = filter(l -> l[1] == '|' && l[end] == '|', lines)
    lines = lines[3:end]

    out = Dict()

    for l in lines
        it = strip.(split(l, '|', keepempty=false))

        volume = split(arg, '/')[end-2]
        part = split(arg, '/')[end-1]
        type = it[1][findfirst(isequal('['), it[1])+1:end-2]
        iso = it[1][1:findfirst(isequal(' '), it[1])-1]

        rev_a = []
        rev = ""
        cont = ""
        if it[4] != ""
            rev_a = split(it[4], ' ')
            rev = strip(rev_a[findfirst(isequal("MaGe:"), rev_a)+1], '`')
            cont = strip(rev_a[findfirst(isequal("container:"), rev_a)+1], '`')
        else
            rev = cont = ""
        end

        prim_a = split(it[3], ('*', 'x'))
        prim_a = strip.(strip.(prim_a), '`')
        prim = 1
        for f in prim_a
            if occursin('^', f)
                po = [parse(Float64, a) for a in split(f, '^')]
                prim *= po[1]^po[2]
            else
            prim *= parse(Float64, f)
            end
        end

        tmp = Dict(volume => Dict(part => Dict(
                   iso => Dict(
                                 type => Dict(
                                              "contact" => it[2],
                                              "primaries" => prim,
                                              "mage-version" => Dict(
                                                                     "revision" => rev,
                                                                     "gerda-sw-all" => cont
                                                                    )
                                             )
                                )
                  )))
        if it[end] != ""
            tmp[volume][part][iso][type]["notes"] = it[end]
        end

        if isempty(out)
            merge!(out, tmp)
        elseif !haskey(out[volume][part], iso)
            merge!(out[volume][part], tmp[volume][part])
        else
            merge!(out[volume][part][iso], tmp[volume][part][iso])
        end
    end

    open(dirname(arg) * "/metadata.json", "w") do f
        JSON.print(f, out, 4)
    end
end
