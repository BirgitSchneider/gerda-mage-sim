# alphas

In this directory the simulations of alpha decays on the p+ contact and in the LAr close to the p+ are stored.

# simulations

| isotope   | contact    | primaries | software tag \[branch/commmit\]  | notes            |
| -------   | ---------- | --------- | :------------------------------: | ---------------- |
| Po210     | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Ra226     | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Rn222     | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Po218     | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Po214     | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Ra226_LAr | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Rn222_LAr | K.v.Sturm  | 1e9       | ? | simulation V.Wagner |
| Po218_LAr | K.v.Sturm  | 2e8       | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.1` | |
| Po214_LAr | K.v.Sturm  | 2e8       | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.1` | |

# macros

The original files, macros and info by Vici can be found at mpik in /lfs/l3/gerda/vwagner/Simulations/AlphaSimPhaseII

## geometry

The file ./geometry/GeometryBEGe_Ge91C_0nm.def contains everything which is needed

## p+

/gps/pos/centre               0 0 1.4895  
/gps/pos/type                 Plane  
/gps/pos/shape                Circle  
/gps/pos/radius               0.9  
/gps/ang/type                 iso  

## LAr p+

/gps/pos/centre               0 0 1.9895  
/gps/pos/type                 Volume  
/gps/pos/shape                Cylinder  
/gps/pos/halfz                0.5  
/gps/pos/radius               0.9  
/gps/ang/type                 iso  

