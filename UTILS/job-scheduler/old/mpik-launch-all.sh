#!/bin/bash

for l in gedet; do
    for p in nplus pplus lar_pplus; do
        for i in K42; do
	    for m in edep; do
		START=0
		STOP=39

	        A="$( printf %d $(($START + 1)) )"
        	B="$( printf %d $(($STOP  + 1)) )"

		OUT=mpik-qsub-$l-$p-$i-$m.sh

	        cat mpik-qsub.sh | sed "s/\$1/"$l"/g" \
				 | sed "s/\$2/"$p"/g" \
				 | sed "s/\$3/"$i"/g" \
				 | sed "s/\$4/"$m"/g" \
				 | sed "s/\$5/$A/g" \
				 | sed "s/\$6/$B/g" \
				 > $OUT
		qsub $OUT
	    done
	done
    done
done

