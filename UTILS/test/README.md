# Crosscheck of isotopes in Geant4 and decay0 primary generation

## Isotopes

* Ac228
* Bi207
* K40
* K42
* Tl208


## K42

<p align="center">
  <img src="K42.png" width="400"/>
  <img src="K42_Zoom.png" width="400"/>
</p>


## Macro

For the simulation the Dario detector and rootscheme was used

/tracking/verbose                               0</br>
/MG/manager/mglog                               trace</br>
/MG/manager/seedWithDevRandom</br>
/MG/processes/realm                             BBdecay</br>
/MG/processes/lowenergy                         true</br>
/MG/processes/useNoHadPhysics                   true</br>
/MG/geometry/detector                           Dario</br>
/MG/geometry/database                           false</br>
/MG/eventaction/reportingfrequency              10000</br>
/MG/eventaction/rootschema                      Dario</br>
/MG/eventaction/rootfilename               	    Out_Dario.root</br>

/run/initialize</br>

/MG/generator/confine                           volume</br>
/MG/generator/volume                            crystal</br>
/MG/generator/select                       	    decay0</br>
/MG/generator/decay0/filename             	    decay0files/K42_1E7.dk0</br>
/tracking/verbose                          	    0</br>
/run/beamOn                                	    1000000</br>

