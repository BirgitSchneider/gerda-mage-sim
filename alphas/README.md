# alphas
**Remember to update also `UTILS/sim-parameters-all.json`**!

In this directory the simulations of alpha decays on the p+ contact and in the LAr close to the p+ are stored.

The original files, macros and info by Vici can be found at mpik in /lfs/l3/gerda/vwagner/Simulations/AlphaSimPhaseII

## geometry

The file ./geometry/GeometryBEGe_Ge91C_0nm.def contains everything which is needed for simulations varying the pplus contact thickness between 0 and 1000nm

## analytic

Here functions are stored to describe the form of the alpha spectrum alaytically. They are not simulated but for completeness saved here.

## volumes description

| volume name | mc name(s)          | mc mass [kg] | mc volume [cm^3] | density [g/cm^3] | volume description                                           | notes |
| ----------- | ------------------- | ------------ | ---------------- | ---------------- | ------------------------------------------------------------ | ----- |
| lar_pplus   | see below `LAr p+`  |              |                  |                  | cylinder volume of `height=1cm` and `diam=1.8cm` close to p+ |       |
| pplus       | see below `p+`      |              |                  |                  | circular plane on p+ contact with `diam=1.8cm`               |       |

### p+

/gps/pos/centre               0 0 1.4895
/gps/pos/type                 Plane
/gps/pos/shape                Circle
/gps/pos/radius               0.9
/gps/ang/type                 iso

### LAr p+

/gps/pos/centre               0 0 1.9895
/gps/pos/type                 Volume
/gps/pos/shape                Cylinder
/gps/pos/halfz                0.5
/gps/pos/radius               0.9
/gps/ang/type                 iso

