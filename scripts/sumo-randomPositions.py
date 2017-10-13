#!/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/python2.7
"""
@file    randomPositions.py
@author  Marco Romanelli, marco.romanelli.1@studenti.unipd.it
@date    2017-10-12
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
import bisect
import datetime
import subprocess
from collections import defaultdict
import math
import optparse

  # SUMO_HOME = os.environ.get('SUMO_HOME',
						   # os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
# sys.path.append(os.path.join(SUMO_HOME, 'tools'))

import sumolib
from sumolib import route2trips
from sumolib.miscutils import euclidean
from sumolib.net.lane import SUMO_VEHICLE_CLASSES

def get_options(args=None):
	optParser = optparse.OptionParser()
	optParser.add_option("-n", "--net-file", dest="netfile",
						 help="define the net file (mandatory)")
	optParser.add_option("-o", "--output-trip-file", dest="tripfile",
						 default="trips.trips.xml", help="define the output trip filename")
	(options, args) = optParser.parse_args(args=args)
	if not options.netfile:
		optParser.print_help()
		sys.exit(1)

	return options


def generate_one(idx, depart, departPos, laneFrom, laneTo):
	return '<trip id="%s" depart="%.2f" departPos="%d" from="%s" to="%s" type="car"/>' % (idx, depart, departPos, laneFrom, laneTo)

def isFeasibleRoad(edge):
	# Road with no connection (fringe)
	if edge.is_fringe():
		return False

	# Roads must be >= 4 meters long (lenth of an average car)
	if (int(edge.getLength()) <=4):
		return False

	# Min allowed speed must be greaterh then 13 m/s (~50 km/h)
	if (float(edge.getSpeed()) < 13.0):
		return False

	# Allowed edge type (http://sumo.dlr.de/wiki/Definition_of_Vehicles,_Vehicle_Types,_and_Routes)
	return (edge.allows("passenger") or edge.allows("private") or edge.allows("custom1") or edge.allows("custom2"))

def getOppositeDirection(idx):
	# If the id is in the form "-111288429"
	if (idx[0] == '-'):
		return idx[1:]
	else:
	# the id is in the form "111288429"
		return '-' + idx

def main(options):
	print("[+] Reading net file...")
	net = sumolib.net.readNet(options.netfile)

	edges = net.getEdges()
	print("[+] Found %d edges." % len(edges))

	print("[+] Running...")
	trips = []
	visited = {}
	tripId = 0

	# For each edge/lane
	# (i assume that each edge has only one lane)
	for road in edges:
		# Check if the lane is a road
		if (not isFeasibleRoad(road)):
			continue

		idx = str(road.getID())
		length = int(road.getLength())

		# Check if i already visited the opposite direction
		opposite = getOppositeDirection(idx)
		if opposite in visited:
			# I already done the opposite direction, so skip
			continue

		# Otherwise go on
		visited[idx] = 1

		pos = 1
		while (pos < (length - __minDistance)):
			current = generate_one(tripId, 0, pos, idx, idx)
			trips.append(current)

			pos += random.randint(__minDistance, __maxDistance)
			tripId += 1

	print("[+] I created %d vehicles." % (tripId + 1))
	print("[+] Writing trips file.")

	with open(options.tripfile, 'w') as fouttrips:
		sumolib.writeXMLHeader(fouttrips, "$Id: randomTrips.py 23999 2017-04-21 09:04:47Z behrisch $", "routes")

		fouttrips.write('\t<vType id="car" accel="0.8" decel="4.5" sigma="0.5" length="4" maxSpeed="50"/>\n\n')

		for trip in trips:
			fouttrips.write("\t%s\n" % trip)

		fouttrips.write("</routes>\n")


# info: print([method_name for method_name in dir(net._edges[0])])

if __name__ == "__main__":
	__minDistance = 10	# meters
	__maxDistance = 14	# meters

	random.seed(datetime.datetime.now().microsecond)

	print("[+] %s" % sys.argv[0])

	if not main(get_options()):
		sys.exit(1)

	print("[+] Done. Bye :)\n")
