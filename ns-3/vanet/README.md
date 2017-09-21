# VANET
My version of barichello's code.

### Prerequisites
ns-3.23+, Obstacle Shadowing Model

### Installation
```
cd vanet/
cp -r vanet $NS3_HOME/scratch
cp Griglia.poly.xml $NS3_HOME
```
where $NS3_HOME is your ns-3 main folder.
Then compile the code (e.g. using waf utility).

### Usage
Inside the ns-3 main folder, run:
```
./waf --run vanet
```

#### Logging
This code use ns-3 logging system to display information about what's going on during the simulation.
For basic information:
```
export NS_LOG="fb-vanet=info|prefix_level:FBApplication=info|prefix_level"
```
If you want to display more data (e.g. who/when a packet is sent/received):
```
export NS_LOG="fb-vanet=level_info|prefix_level:FBApplication=level_info|prefix_level"
```
