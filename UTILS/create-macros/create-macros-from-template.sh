#!/bin/bash

### USAGE: ./create-macros-from-template.sh <start> <end> <template-000.mac>
### USAGE: ./create-macros-from-template.sh <template-ch0.mac>

REFID=000

if test "$#" -eq 3; then
	START=$1
	END=$2

	TEMPLATE=$3
	echo TEMPLATE FILE: $TEMPLATE

	if [[ $TEMPLATE == *"raw"*"-${REFID}.mac" ]]; then
		for ID in $(seq -f "%03g" $START $END); do
			MACRO=$(echo $TEMPLATE | sed "s/${REFID}.mac/$ID.mac/g")
			echo $ID: $MACRO
			sed "s/${REFID}.root/$ID.root/g" $TEMPLATE > $MACRO
		done
	fi

elif test "$#" -eq 1; then
	TEMPLATE=$1
	echo TEMPLATE FILE: $TEMPLATE

        if [[ $TEMPLATE == *"raw"*"-ch0.mac" ]]; then
		for ID in $(seq 1 39); do
			MAC=$(echo $TEMPLATE | sed "s/ch0/ch$ID/g");
			echo ch$ID: $MAC
			sed "s/ch0/ch$ID/g" $TEMPLATE > $MAC;
		done
	fi
fi
