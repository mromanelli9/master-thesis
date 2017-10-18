#!/bin/bash

SIMULATION_END_TIME=50

OSM_FILE=$1
BASENAME=$(basename "$OSM_FILE" .osm.xml)

NETCONVERT_CC="$(which netconvert)"
POLYCONVERT_CC="$(which polyconvert)"
RANDOM_TRIPS_CC="$(which sumo-randomTrips)"
SUMO_CC="$(which sumo)"
TRACE_EXPORTER="$(which sumo-traceExporter)"

# Check if software exitsts
I="0"
if [ "$NETCONVERT_CC" == "" ]; then
	echo "[!] Error: netconvert not found."
	I=$((I + 1))
fi
if [ "$POLYCONVERT_CC" == "" ]; then
	echo "[!] Error: polyconvert not found."
	I=$((I + 1))
fi
if [ "$RANDOM_TRIPS_CC" == "" ]; then
	echo "[!] Error: randomTrips not found."
	I=$((I + 1))
fi
if [ "$SUMO_CC" == "" ]; then
	echo "[!] Error: sumo not found."
	I=$((I + 1))
fi
if [ "$TRACE_EXPORTER" == "" ]; then
	echo "[!] Error: traceExporter not found."
	I=$((I + 1))
fi

if [ "$I" != "0" ]; then
	echo
	exit 2
fi


# Generate data
NET_FILE="$BASENAME.net.xml"
BUILDINGS_FILE="$BASENAME.poly.xml"
ROUTES_FILE="$BASENAME.trips.xml"
SUMO_CFG_FILE="$BASENAME.sumo.cfg"
TRACE_FILE="$BASENAME.trace.xml"
MOBILITY_FILE="$BASENAME.ns2mobility.xml"


# Generate network
$NETCONVERT_CC --osm-files="$OSM_FILE" -o "$NET_FILE" &&

# Generate buildings
$POLYCONVERT_CC --osm-files="$OSM_FILE" --net-file="$NET_FILE" --shapefile.add-param=true --prune.in-net=true --prune.explicit="fountain" -o "$BUILDINGS_FILE" &&

# Generate routes
$RANDOM_TRIPS_CC -n "$NET_FILE" -e "$SIMULATION_END_TIME" -o "$ROUTES_FILE" &&


# Generate config file
cat >> "$SUMO_CFG_FILE" <<EOF
<configuration>
    <input>
        <net-file value="$NET_FILE"/>
        <route-files value="$ROUTES_FILE"/>
        <additional-files value="$BUILDINGS_FILE"/>
    </input>
		<output>
			<fcd-output value="$TRACE_FILE" />
		</output>
    <time>
        <begin value="0"/>
        <end value="$SIMULATION_END_TIME"/>
    </time>
</configuration>
EOF

sleep 2

$SUMO_CC -c "$SUMO_CFG_FILE"  &&
$TRACE_EXPORTER -i "$TRACE_FILE" --ns2mobility-out "$MOBILITY_FILE"

# Exit
echo
exit 0
