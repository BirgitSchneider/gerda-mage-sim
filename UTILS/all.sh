#!/bin/bash

GERDA_DETDATA="/lfs/l3/gerda/Hades/sw/gerda-metadata/detector-data"
t4ize="tier4ize -g $GERDA_DETDATA/ged-mapping-default.json -s $GERDA_DETDATA/ged-settings-default.json -l $GERDA_DETDATA/lar-mapping-default.json -r $GERDA_DETDATA/ged-resolution-default.json -t $GERDA_DETDATA/lar-settings-default.json -c /lfs/l2/gerda/Hades/gerda-mage-sim/UTILS/configuration_dummy.root"

$t4ize -o ./larveto/fibers/Bi214/edep/t4z-larveto-fibers-Bi214-edep.root ./larveto/fibers/Bi214/edep/raw*root
$t4ize -o ./larveto/fibers/Tl208/edep/t4z-larveto-fibers-Tl208-edep.root ./larveto/fibers/Tl208/edep/raw*root
$t4ize -o ./larveto/fibers/K40/edep/t4z-larveto-fibers-K40-edep.root ./larveto/fibers/K40/edep/raw*.root
$t4ize -o ./larveto/fibers/Pb214/edep/t4z-larveto-fibers-Pb214-edep.root ./larveto/fibers/Pb214/edep/raw*.root
$t4ize -o ./larveto/fibers/Bi212/edep/t4z-larveto-fibers-Bi212-edep.root ./larveto/fibers/Bi212/edep/raw*.root
$t4ize -o ./larveto/pmt_top/K40/edep/t4z-larveto-pmt_top-K40-edep.root ./larveto/pmt_top/K40/edep/raw*.root
$t4ize -o ./larveto/pmt_bottom/K40/edep/t4z-larveto-pmt_bottom-K40-edep.root ./larveto/pmt_bottom/K40/edep/*.root

./UTILS/gen-spectra ./larveto/fibers/Bi214/edep/t4z-larveto-fibers-Bi214-edep.root
./UTILS/gen-spectra ./larveto/fibers/Tl208/edep/t4z-larveto-fibers-Tl208-edep.root
./UTILS/gen-spectra ./larveto/fibers/K40/edep/t4z-larveto-fibers-K40-edep.root
./UTILS/gen-spectra ./larveto/fibers/Pb214/edep/t4z-larveto-fibers-Pb214-edep.root
./UTILS/gen-spectra ./larveto/fibers/Bi212/edep/t4z-larveto-fibers-Bi212-edep.root
./UTILS/gen-spectra ./larveto/pmt_top/K40/edep/t4z-larveto-pmt_top-K40-edep.root
./UTILS/gen-spectra ./larveto/pmt_bottom/K40/edep/t4z-larveto-pmt_bottom-K40-edep.root
