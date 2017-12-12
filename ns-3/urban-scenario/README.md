# VANET
My version of barichello's code.

### Prerequisites
ns-3.23+, Obstacle Shadowing Model

### Installation
* Copy vanet folder into $NS3_HOME/scratch
* Copy ns2 mobility file and polygon data file into $NS3_HOME
* Compile code (e.g. using waf)

### Usage
Run vanet program:
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
