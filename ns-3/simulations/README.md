# Simulations

Various experiments and examples.

### Prerequisites
ns-3.23

### Usage
Copy cc files into scratch folder and input data into main ns-3 folder, than lunch waf.
E.g. if you want to test the basic Obstacle Shadowing Model example:
```
cd basic-example
cp obstacle-shadowing-model-basic-example.cc  $NS3_HOME/scratch
cp basic-example.building.xml $NS3_HOME
cd $NS3_HOME
./waf
./waf --run "scratch/obstacle-shadowing-model-basic-example"
```
