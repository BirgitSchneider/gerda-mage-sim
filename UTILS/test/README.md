# Crosscheck of isotopes in Geant4 and decay0 primary generation

## Isotopes

* [Ac228](#ac228)
* [Bi207](#bi207)
* [K40](#k40)
* [K42](#k42)
* [Tl208](#tl208)
* [Co60](#co60)


## Ac228

<p align="center">
  <img src="Ac228.png" width="400"/>
  <img src="Ac228_Zoom.png" width="400"/>
</p>

* Lines of G4RadioactiveDecay in accordance with recommended data.
* Do not use decay0 to generate the primaries

## K40

<p align="center">
  <img src="K40.png" width="400"/>
  <img src="K40_Zoom.png" width="400"/>
</p>


## K42

<p align="center">
  <img src="K42.png" width="400"/>
  <img src="K42_Zoom.png" width="400"/>
</p>
<p align="center">
  <img src="K42_Zoom2.png" width="400"/>
  <img src="K42_Zoom3.png" width="400"/>
</p>
<p align="center">
  <img src="K42_Zoom4.png" width="400"/>
  <img src="K42_Zoom5.png" width="400"/>
</p>
<p align="center">
  <img src="K42_Zoom6.png" width="400"/>
  <img src="K42_Zoom7.png" width="400"/>
</p>

* None of the two primary generators is really in accordance with NuDat

312.60 (25) </br>
692.0 (8) </br>
899.7 (4) </br>
1021.2 (9) </br>
1228.0 (15) </br>
1524.6 (3) </br>
1920.8 (10) </br>
2424.3 (7) </br>

## Co60

<p align="center">
  <img src="Co60.png" width="400"/>
  <img src="Co60_Zoom.png" width="400"/>
</p>
<p align="center">
  <img src="Co60_Zoom2.png" width="400"/>
  <img src="Co60_Zoom3.png" width="400"/>
</p>


## Tl208

<p align="center">
  <img src="Tl208.png" width="400"/>
  <img src="Tl208_Zoom.png" width="400"/>
</p>
<p align="center">
  <img src="Tl208_Zoom2.png" width="400"/>
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

