# Examples
Various experiments and examples using Obstacle Shadowing Model.

### Prerequisites
ns-3, Obstacle Shadowing Model patch

### Examples
* _Simple building_: simple example using Obstacle Shadowing Model; building
data is provided with a file.
* _Simple building custom_: like the previous one, but you can also set the model
parameters beta and gamma.
* _Routing Math Department_: vanet-routing-compare example (with Obstacle modification) on Math Department osm data.

### Usage
Copy cc files into scratch folder and input data into main ns-3 folder, than lunch waf.
E.g. if you want to test the basic Obstacle Shadowing Model example:
```
cd simple-building/
cp obstacle-shadowing-model-basic-example.cc  $NS3_HOME/scratch
cp basic-example.building.xml $NS3_HOME
cd $NS3_HOME
./waf
./waf --run "scratch/obstacle-shadowing-model-basic-example"
```
