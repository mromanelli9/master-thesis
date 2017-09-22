#! /usr/bin/env python
## -*- Mode: python; py-indent-offset: 2; indent-tabs-mode: nil; coding: utf-8; -*-
#
# Copyright (c) 2017 Universita' di Padova
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation;
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Author: Marco Romanelli <marco.romanelli.1@studenti.math.it>
#
#

"""
A script to generate ns3::Nodes positions, given a bounding box and "no-fly zones"

@file    nodesPositionsGenerator.py
@author  Marco Romanelli
@date    22-09-2017
@version 1
"""

import getopt, sys
import xml.etree.ElementTree as ET
import random
from decimal import Decimal

def parsePolyFile(filename):
	xmlTree = ET.parse(filename)
	saved = []

	for el in xmlTree.iter():
		if el.tag == "poly" and ("building" in " ".join( [v for v in el.attrib.values()])):
			saved.append(el)

	return saved

def extractNoflyZones(polyData):
	zones = []

	for i, osmEl in enumerate(polyData):
		polyEl = polyData[i]

		current = shapeToBounds(polyEl.attrib["shape"])
		zones.append(current)

	return zones

def shapeToBounds(shape):
	bounds = {
		"xMin": -1,
		"xMax": -1,
		"yMin": -1,
		"yMax": -1,
		"zMin": -1,
		"zMax": -1,
	}

	for point in shape.split(' '):
		x, y = point.split(',')
		xNum = float (x)
		yNum = float (y)

		# Min
		if bounds["xMin"] == -1 or xNum < bounds["xMin"]:
			bounds["xMin"] = xNum
		if bounds["yMin"] == -1 or yNum < bounds["yMin"]:
			bounds["yMin"] = yNum

		# Max
		if bounds["xMax"] == -1 or xNum > bounds["xMax"]:
			bounds["xMax"] = xNum
		if bounds["xMax"] == -1 or yNum > bounds["yMax"]:
			bounds["yMax"] = yNum

	return bounds

def generateRandomPoints(n, boundingBox, noflyZones):
	allinone = []

	for i in range(0, n):
		point = (-1,-1,-1)

		loop = True
		while (loop):
			# Generate a point within the bounding box
			x = float(format(Decimal(random.uniform(boundingBox["xMin"], boundingBox["xMax"])), '.2f'))
			y = float(format(Decimal(random.uniform(boundingBox["yMin"], boundingBox["yMax"])), '.2f'))
			z = float(format(Decimal(random.uniform(boundingBox["zMin"], boundingBox["zMax"])), '.2f'))
			point = (x,y,z)
			loop = False

			# Check if the point violate a no-fly zone
			for zone in noflyZones:
				test = 0
				if point[0] > zone["xMin"] and point[0] < zone["xMax"]:
					test += 1
				if point[1] > zone["yMin"] and point[1] < zone["yMax"]:
					test += 1
				# TODO
				# if point[2] < zone["zMin"] or point[2] > zone["yMax"]:
				# 	test += 1

				# If the point failed all the tests, then..
				if (test == 2):
					loop = True

		allinone.append((x,y,z))

	return allinone

def storeInFile(filename, data):
	with open(filename, 'w') as f:
		for point in data:
			val = str(point[0]) + '\t' + str(point[1]) + '\t' + str(point[2]) + '\n'
			f.write(val)

def main(argv, boundingBox, outputFile):
	# Parse args
	try:
		opts, args = getopt.getopt(argv, "", ["poly-file=", "nodes="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print(str(err))  # will print something like "option -a not recognized"
		sys.exit(2)

	polyFilepath = None
	N = 1
	for o, a in opts:
		if o in ("--nodes"):
			N = int(a)
		elif o in ("--poly-file"):
			polyFilepath = str(a)
		else:
			assert False, "[!] Unhandled option."

	assert polyFilepath != None, "[!] Missing inputs."

	# Parse poly file
	polydata = parsePolyFile(polyFilepath)

	# Extract building data
	zones = extractNoflyZones(polydata)

	# Generate random point
	points = generateRandomPoints(N, boundingBox, zones)

	# Save
	storeInFile(outputFile, points)

if __name__ == "__main__":
	# Bounding box aka word bounds
	boundingBox = {
		"xMin": 0,
		"xMax": 300,
		"yMin": 0,
		"yMax": 300,
		"zMin": 70,
		"zMax": 100,
	}

	outputFile = "nodes_coordinates.txt"

	main(sys.argv[1:], boundingBox, outputFile)
