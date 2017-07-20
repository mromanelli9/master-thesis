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
A script for converting OSM (Open Street Map) buildings to data readable by ns-3 Building Module.

@file    buuldingExporter.py
@author  Marco Romanelli
@date    07-19-2017
@version 1
"""

import getopt, sys
import xml.etree.ElementTree as ET
import re


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

def parsePolyFile( filename ):
	xmlTree = ET.parse( filename )
	saved = []

	for el in xmlTree.iter():
		if el.tag == "poly" and ( "building" in " ".join( [v for v in el.attrib.values()] ) ):
			saved.append( el )

	return saved

def extractData( osmData, polyData ):
	osmData_n = len( osmData )
	polyData_n = len( polyData )

	assert osmData_n == polyData_n, "[!] Different number of buildings: %d and %d" % (osmData_n, polyData_n)

	output = []

	for i, osmEl in enumerate( osmData ):
		polyEl = polyData[i]
		building = {}


		building["Name"] = getName( osmEl )
		building["Height"], building["Min_Height"] = getHeight( osmEl )
		building["NFloors"] = getNFloor( osmEl )
		building["Box"] = getBoundaries( polyEl, building["Height"], building["Min_Height"], building["NFloors"] )

		output.append( building )

	return output

def getNFloor( el ):
	n = 0

	for tags in el.findall("tag"):
		attributes = tags.attrib

		if ( attributes.get( 'k' ) == "building:levels" ):
			n = int( attributes.get( 'v' ) )

	return n

def getHeight( el ):
	h = 0.0
	m_h = 0.0

	for tags in el.findall("tag"):
		attributes = tags.attrib

		if ( attributes.get( 'k' ) == "height" ):
			h = float( attributes.get( 'v' ) )

		if ( attributes.get( 'k' ) == "min_height" ):
			m_h = float( attributes.get( 'v' ) )

	return h, m_h

def getName( el ):
	name = ""

	for tags in el.findall("tag"):
		attributes = tags.attrib

		if ( attributes.get( 'k' ) == "name" ):
			name = str( attributes.get( 'v' ) )

	name = genVarName( name )

	return name

def getBoundaries( el, height, min_height, nfloors, floor_height=2.7 ):
	x_min = x_max = y_min = y_max = z_min = z_max = 0

	shape = el.attrib["shape"]
	assert shape != None, "[!] No shape found in building %s" % el

	values = [ vertex for vertex in shape.split( ' ' )  ]

	vertices = []
	for pair in values:
		z = map( float, pair.split( ',' ) )
		vertices.append( (z[0], z[1]) )

	x_es = [v[0] for v in vertices]
	z_es = [v[1] for v in vertices]

	x_min = min( x_es )
	x_max = max( x_es )

	y_min = min_height
	if ( height != 0 ):
		y_max = height
	else:
		y_max = nfloors * floor_height
	y_max += y_min

	z_min = min( z_es )
	z_max = max( z_es )

	return ( x_min, x_max, y_min, y_max, z_min, z_max )

def createBuildings( data ):
	s = ""

	for building in data:
		var = building["Name"]
		s += "double x_min = %s;\n" % building["Box"][0]
		s += "double x_max = %s;\n" % building["Box"][1]
		s += "double y_min = %s;\n" % building["Box"][2]
		s += "double y_max = %s;\n" % building["Box"][3]
		s += "double z_min = %s;\n" % building["Box"][4]
		s += "double z_max = %s;\n" % building["Box"][5]

		s += "Ptr<Building> %s = CreateObject <Building> ();\n" % var

		s += "%s->SetBoundaries (Box (x_min, x_max, y_min, y_max, z_min, z_max));\n" % var

		nfloors = building["NFloors"]
		if ( nfloors != None ):
			s += "%s->SetNFloors (%s);\n" % (var, nfloors)
		else:
			s += "%s->SetNFloors (0);\n" % var

		s += "\n"

	return s

def genVarName( sp ):
	# 1: Remove special chars
	sp = re.sub( "\W+", '', sp )

	# 2: Remove numbers
	sp = filter( lambda x: x.isalpha(), sp )

	return sp

def main( argv ):
	# Parse args
	try:
		opts, args = getopt.getopt(sys.argv[1:], "", ["osm-file=", "poly-file="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print( str( err ) )  # will print something like "option -a not recognized"
		sys.exit(2)

	osmFilepath = polyFilepath = None
	for o, a in opts:
		if o in ("--osm-file"):
			osmFilepath = str(a)
		elif o in ("--poly-file"):
			polyFilepath = str(a)
		else:
			assert False, "[!] Unhandled option."

	assert osmFilepath != None and polyFilepath != None, "[!] Missing inputs."

	# Parse osm file
	osmdata = parseOsmFile( osmFilepath )

	# Parse poly file
	polydata = parsePolyFile( polyFilepath )

	# Merge data
	data = extractData( osmdata, polydata )
	print(data)

	# Build output
	out = createBuildings( data )
	print( out )

if __name__ == "__main__":
	main( sys.argv[1:] )
