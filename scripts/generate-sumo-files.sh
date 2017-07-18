#!/bin/bash

OSM_FILE=$1
BASENAME=$(basename "$OSM_FILE" .osm.xml)

NET_FILE="$BASENAME.net.xml"
BUILDINGS_FILE="$BASENAME.buildings.xml"
ROUTES_FILE="$BASENAME.rou.xml"
SUMO_CFG_FILE="$BASENAME.sumo.cfg"

SIMULATION_END_TIME=20

# Generate network
netconvert --osm-files="$OSM_FILE" -o "$NET_FILE"

# Generate buildings
polyconvert --osm-files="$OSM_FILE" -o "$BUILDINGS_FILE"

# Generate routes
/opt/local/bin/sumo-randomTrips -n "$NET_FILE" -e "$SIMULATION_END_TIME" -o "$ROUTES_FILE"


# Generate config file
cat >> "$SUMO_CFG_FILE" <<EOF
<configuration>
    <OSM_FILE>
        <net-file value="$NET_FILE"/>
        <route-files value="$ROUTES_FILE"/>
        <additional-files value="$BUILDINGS_FILE"/>
    </OSM_FILE>
    <time>
        <begin value="0"/>
        <end value="$SIMULATION_END_TIME"/>
    </time>
</configuration>
EOF

# Exit
exit 0
