# Crosscheck of isotopes in Geant4 and decay0 primary generation

## Isotopes

* Ac228
* Bi207
* K40
* K42
* Tl208


## K42

<p align="center">
  <img src="K42.png" width="600"/>
</p>



For the simulation the Dario detector and rootscheme was used

/tracking/verbose                               0

/MG/manager/mglog                               trace

/MG/manager/seedWithDevRandom

/MG/processes/realm                             BBdecay

/MG/processes/lowenergy                         true

/MG/processes/useNoHadPhysics                   true

/MG/geometry/detector                           Dario

/MG/geometry/database                           false

/MG/eventaction/reportingfrequency              10000

/MG/eventaction/rootschema                      Dario

/MG/eventaction/rootfilename               	Out_Dario.root

/run/initialize

/MG/generator/confine                           volume

/MG/generator/volume                            crystal

---

/MG/generator/select                       	decay0

/MG/generator/decay0/filename             	../../decay0files/K42_1E7.dk0

---

/tracking/verbose                          	0

/run/beamOn                                	1000000

