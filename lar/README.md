# lar
**Remember to update also `UTILS/sim-parameters-all.json`**!

## volumes description

| volume name    | mc name(s) | mc mass [kg] | mc volume [cm^3] | density [g/cm^3]  | volume description                                                                                 | notes |
| -------------- | ---------- | ------------ | ---------------- | ----------------- | -------------------------------------------------------------------------------------------------- | ----- |
| above_array    | N.A.       | 268          | 192480           | 1.39              | Cylinder (radius = 25cm, height = 100cm) positioned over the germanium array (z = 65cm)            |       |
| sur_array      | N.A.       | 10895        | 7837850          | 1.39              | Cylinder (radius = 100cm, height = 250cm) that surrounds the germanium array (z = 0)               |       |
| above_ms1      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 1 starting from ms-top |       |
| above_ms2      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 2 starting from ms-top |       |
| above_ms3      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 3 starting from ms-top |       |
| above_ms4      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 4 starting from ms-top |       |
| above_ms5      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 5 starting from ms-top |       |
| above_ms6      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 6 starting from ms-top |       |
| above_ms7      | N.A.       | 1.091        | 785.375          | 1.39              | Cylinder (radius = 5cm, height = 20cm) positioned over minishroud of string 7 starting from ms-top |       |
| sur_array1     | N.A.       | 2341.2       | 1684340          | 1.39              | Cylinder (radius = 60cm, height = 150cm) surrounding the Ge array (z=-15cm for better centering) | volume to sample energy in Ge of ~600 keV beta with 1e10 sim events |
| sur_array2     | N.A.       | 9149.5       | 6582400          | 1.39              | Cylinder (radius = 100cm, height = 210cm) surrounding the Ge array (z=-15cm for better centering) | volume to sample energy in Ge of ~500 keV gamma with 1e10 sim events |
| inside_ms      | N.A.       |              |                  |                   | post-processed volume splitting of sur_array               |       |
| outside_ms     | N.A.       |              |                  |                   | post-processed volume splitting of sur_array               |       |


## (x,y) string positions
```
string1 ch0-7
/MG/generator/Sampling/volume/offsetCenterXPos         110.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         0.0 mm

string2 ch8-10
/MG/generator/Sampling/volume/offsetCenterXPos         55.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         95.2628 mm

string3 ch11-18
/MG/generator/Sampling/volume/offsetCenterXPos         -55.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         95.2628 mm

string4 ch19-26
/MG/generator/Sampling/volume/offsetCenterXPos         -110.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         0.0 mm

string5 ch27-29
/MG/generator/Sampling/volume/offsetCenterXPos         -55.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         -95.2628 mm

string6 ch30-36
/MG/generator/Sampling/volume/offsetCenterXPos         55.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         -95.2628 mm

string7 ch37-39
/MG/generator/Sampling/volume/offsetCenterXPos         0.0 mm
/MG/generator/Sampling/volume/offsetCenterYPos         0.0 mm
```
