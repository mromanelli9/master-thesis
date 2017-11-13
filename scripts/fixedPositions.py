#!/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/python2.7
"""
@file    fixedPositions.py
@author  Marco Romanelli [marco.romanelli.1@studenti.unipd.it]
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
from math import sqrt
import Queue

  # SUMO_HOME = os.environ.get('SUMO_HOME',
						   # os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
# sys.path.append(os.path.join(SUMO_HOME, 'tools'))

import sumolib
from sumolib import route2trips
from sumolib.miscutils import euclidean
from sumolib.net.lane import SUMO_VEHICLE_CLASSES

global __vDistance

class Point:
	def __init__(self,x_init,y_init):
		self.x = x_init
		self.y = y_init

	def shift(self, x, y):
		self.x += x
		self.y += y

	def __repr__(self):
		return "".join(["Point(", str(self.x), ",", str(self.y), ")"])

def distance(a, b):
	return sqrt((a.x - b.x)**2 + (a.y - b.y)**2)

def get_options(args=None):
	optParser = optparse.OptionParser()
	optParser.add_option("-n", "--net-file", dest="netfile",
						 help="define the net file (mandatory)")
	optParser.add_option("-o", "--output-trip-file", dest="tripfile",
						 default="trips.trips.xml", help="define the output trip filename")
 	optParser.add_option("-d", "--distance", dest="vDistance", type="float",
 						 default="50.0", help="define the distance between vehicles")
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
	if (int(edge.getLength()) <= 4):
		return False

	# Allowed edge type (http://sumo.dlr.de/wiki/Definition_of_Vehicles,_Vehicle_Types,_and_Routes)
	return (edge.allows("custom1") or edge.allows("custom2") or edge.allows("passenger") or edge.allows("private"))

def getOppositeDirection(idx):
	# If the id is in the form "-111288429"
	if (idx[0] == '-'):
		return idx[1:]
	else:
	# the id is in the form "111288429"
		return '-' + idx

def dfs_search(nodes, edges, trips, vdistance = 50):
	tripId = 0
	visited = {}

	# DFS colors: dict with edge id as key and color as value
	colors = {}
	for e in edges:
		colors[str(e.getID())] = 'W'

	for e in edges:
		if colors[str(e.getID())] == 'W':
			visit_node(e, colors, visited, trips, vdistance)

def visit_node(edge, colors, visited, trips, vdistance, missingPos = None):
	# Set color to grey
	colors[str(edge.getID())] = 'G'

	miss = None
	if isFeasibleRoad(edge):
		idx = str(edge.getID())
		length = int(edge.getLength())

		# Check if i already visited the opposite direction
		opposite = getOppositeDirection(idx)
		if not (opposite in visited):
			visited[idx] = 1

			# Check where i put the last vehicle
			pos = missingPos if (missingPos != None) else 1

			while (pos < length):
				current = generate_one(len(trips), 0, pos, idx, idx)
				trips.append(current)

				pos += vdistance

			miss = pos - length

	for out, itr in edge.getOutgoing().iteritems():
		if colors[str(out.getID())] == 'W':
			visit_node(out, colors, visited, trips, vdistance, miss)

	# Set color to black
	colors[str(edge.getID())] = 'B'

def getLastId(trip):
	pos = str.index(trip, "\"") + 1
	pos2 = str.index(trip[pos:], "\"")

	return int(trip[pos:(pos2 + pos)]) + 1

def main(options):
	print("[+] Reading net file...")
	net = sumolib.net.readNet(options.netfile)

	nodes = net.getNodes()
	edges = net.getEdges()
	print("[+] %d nodes and %d edges." % (len(nodes), len(edges)))

	print("[+] Running...")
	trips = []
	dfs_search(nodes, edges, trips, options.vDistance)

	lastId = getLastId(trips[len(trips)-1])

	print("[+] I created %d vehicles." % lastId)
	print("[+] Writing trips file.")

	with open(options.tripfile, 'w') as fouttrips:
		sumolib.writeXMLHeader(fouttrips, "$Id: randomTrips.py 23999 2017-04-21 09:04:47Z behrisch $", "routes")

		fouttrips.write('\t<vType id="car" accel="0.8" decel="4.5" sigma="0.5" length="4" maxSpeed="50"/>\n\n')

		for trip in trips:
			fouttrips.write("\t%s\n" % trip)

		fouttrips.write("</routes>\n")


# info: print([method_name for method_name in dir(net._edges[0])])

if __name__ == "__main__":
	# Increase the recursion limit
	sys.setrecursionlimit(10000)

	print("[+] %s" % sys.argv[0])

	if not main(get_options()):
		sys.exit(1)

	print("[+] Done. Bye :)\n")
