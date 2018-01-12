#!/bin/bash
#
# USAGE: multiply.sh <volume>-<part>-<isotope>-<type> <factor> <nfiles>

for i in `seq 1 $(($2-1))`; do 
    for j in `seq 0 $(($3-1))`; do
        if [[ $((j+$3*i)) -lt 10 ]]; then
            cp raw-$1-00${j}.mac raw-$1-00$((j+$3*i)).mac
        else
            cp raw-$1-00${j}.mac raw-$1-0$((j+$3*i)).mac
        fi
    done
done
for i in `seq $3 9`;            do vim -E -c "%s/$1-00.*.root/$1-00${i}.root/g | x" raw-$1-00${i}.mac; done
for i in `seq 10 $(($3*$2-1))`; do vim -E -c "%s/$1-00.*.root/$1-0${i}.root/g | x" raw-$1-0${i}.mac; done
