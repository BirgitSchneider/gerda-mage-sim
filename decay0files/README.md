# decay0 files

Put here your decay0 files. Inside each file you can find info about the initial random seed, the isotope and generated transition. Syntax: `ISOTOPE-NEVENTS.dk0`

```shell
$ ls -Xsh1 --hide="*.md"
total 17G
741M 2nbbLV_1E7.dk0
138M 2nbb_1E6.dk0
1.4G 2nbb_1E7.dk0
2.6G Ac228_1E7.dk0
3.2G Bi207_1E7.dk0
1.9G Co60_1E7.dk0
871M K40_1E7.dk0
916M K42_1E7.dk0
814M Kr85_1e7.dk0
840M Pa234m_1E7.dk0
1.7G Pb214_1E7.dk0
2.2G Tl208_1E7.dk0
```

## generate decay0 files

In `UTILS/job-scheduler/decay0gen` you find script which will send your decay0 job directly in the queue.
Consult the `README` for decay0 input options or just launch decay0 interactively to figure out
what you need. 
