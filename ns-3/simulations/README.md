# Simulations

Various experiments and examples.

### Prerequisites
ns-3.23

### Usage
Copy cc files into scratch folder and input data into main ns-3 folder, than lunch waf.
E.g. if you want to test the first example of Obstacle Shadowing Model:
```
cp obstacle-shadowing-model-example/obstacle-shadowing-model-example.cc  $NS3_HOME/scratch
cp obstacle-shadowing-model-example/obstacle-shadowing-model-building.xml $NS3_HOME
cd $NS3_HOME
./waf
./waf --run "scratch/obstacle-shadowing-model-example"
```
