#!/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/python2.7
"""
@file    randomTrips.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2010-03-06
@version $Id: randomTrips.py 23999 2017-04-21 09:04:47Z behrisch $

Generates random trips for the given network.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
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

def isFeasibleRoad(lane):
	# Roads must be >= 4 meters long (lenth of an average car)
	if (int(lane.getLength()) <=4):
		return False

	# Min allowed speed must be greaterh then 13 m/s (~50 km/h)
	if (float(lane.getSpeed()) < 13.0):
		return False

	return True

def main(options):
	net = sumolib.net.readNet(options.netfile)

	nEdges = len(net._edges)
	print("[+] There are %d roads." % nEdges)

	minStep = __minDistance
	maxStep = __maxDistance
	trips = []
	tripId = 0

	print("[+] Running...")

	for i in range(0, nEdges):
		lane = net._edges[i]

		# Check if the lane is a road
		if (not isFeasibleRoad(lane)):
			continue

		idx = lane.getID()
		length = int(lane.getLength())

		pos = 1
		while (pos < (length - minStep)):
			current = generate_one(tripId, 0, pos, idx, idx)
			trips.append(current)

			pos += random.randint(minStep, maxStep)
			tripId += 1

	print("[+] I create %d vehicles." % (tripId + 1))
	print("[+] Creating trips file.")

	with open(options.tripfile, 'w') as fouttrips:
		sumolib.writeXMLHeader(fouttrips, "$Id: randomTrips.py 23999 2017-04-21 09:04:47Z behrisch $", "routes")

		fouttrips.write('\t<vType id="car" accel="0.8" decel="4.5" sigma="0.5" length="1" maxSpeed="50"/>\n\n')

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
