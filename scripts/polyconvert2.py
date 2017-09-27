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
A script to generate enhanced polygon with information about the height of a building

@file    polyconvert2.py
@author  Marco Romanelli
@date    27-09-2017
@version 1
"""

import getopt, sys
import xml.etree.ElementTree as ET

def parseOsmFile( filename ):
	xmlTree = ET.parse( filename )
	saved = []

	for el in xmlTree.iter():
		# Check if is a potential building
		if el.tag == "way":
			# Check if is an actual building
			isBuilding = False

			for subel in el.iter():
				if subel.tag == "tag" and ( "building" in subel.attrib.values() ):
					saved.append( el )
					break

	return saved

def parsePolyFile(filename):
	xmlTree = ET.parse( filename )
	saved = []

	for el in xmlTree.iter():
		if el.tag == "poly":
			saved.append(el)

	return saved

def getHeights(polyData, osmData):
	info = []

	for i, poly in enumerate(polyData):
		# Retrieve the same building by search the same id
		polyId = getId (poly)
		osm = getElementById(osmData, polyId)

		# Get the info abut the height
		height = levels = roofLevels = None
		if (osm == None):
			continue

		for child in osm:
			if (child.tag != "tag"):
				continue

			if child.attrib["k"] == "height":
				height = float(child.attrib["v"])
			if child.attrib["k"] == "building:levels":
				levels = float(child.attrib["v"])
			if child.attrib["k"] == "roof:levels":
				roofLevels = float(child.attrib["v"])

		if (height != None):
			pass
		elif (levels != None):
			height = levels * _defaultFloorHeight
		else:
			height = _defaultFloorHeight
		if (roofLevels != None):
			height += roofLevels * _defaultFloorHeight

		info.append({
			"id" : polyId,
			"height" : str(height)
		})

	return info

def getElementById(data, polyId):
	i = 0
	while (i < len(data)):
		current = data[i]
		if ("id" in current.attrib and current.attrib["id"] == polyId):
			return current

		i += 1

	return None

def getId(xmlEl):
	if "id" in xmlEl.attrib:
		return str(xmlEl.attrib["id"])

	return None

def enhancedPoly(polyFilename, info, outputFilename):
	xmlTree = ET.parse(polyFilename)

	for el in xmlTree.iter():
		if el.tag == "poly":
			polyId = getId(el)

			generator = list(item for item in info if item["id"] == polyId)
			if (len(generator) >= 1):
				# Element found, so add the additional info
				el.set("height", generator[0]["height"])

	writeXml(outputFilename, xmlTree)

def writeXml(filename, data):
	data.write(open(filename, "wb"), encoding="UTF-8")

def main(argv):
	# Parse args
	try:
		opts, args = getopt.getopt(argv, "", ["osm-file=", "poly-file=", "output-file="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print(str(err))  # will print something like "option -a not recognized"
		sys.exit(2)

	osmFilename = polyFilename = outputFilename = None
	N = 1
	for o, a in opts:
		if o in ("--osm-file"):
			osmFilename = str(a)
		elif o in ("--poly-file"):
			polyFilename = str(a)
		elif o in ("--output-file"):
			outputFilename = str(a)
		else:
			assert False, "[!] Unhandled option."

	assert osmFilename != None and polyFilename != None and outputFilename != None, "[!] Missing inputs."

	# Parse osm file
	osmdata = parseOsmFile(osmFilename)

	# Parse poly file
	polydata = parsePolyFile(polyFilename)

	info = getHeights(polydata, osmdata)

	enhancedPoly(polyFilename, info, outputFilename)

if __name__ == "__main__":
	# Default height for a floor [meters]
	global _defaultFloorHeight
	_defaultFloorHeight = 2.7

	main(sys.argv[1:])
