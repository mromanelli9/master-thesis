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

import sys
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

def main( argv ):
	# Parse args
	osmFilepath = str(argv[0])
	polyFilepath = str(argv[1])

	# Parse osm file
	osmdata = []
	osmdata.extend( parseOsmFile( osmFilepath ) )
	print( osmdata )

	# Parse poly file
	polydata = []
	polydata.extend( parsePolyFile( polyFilepath ) )
	print( polydata )

	# Merge data

	# Build output

if __name__ == "__main__":
	main( sys.argv[1:] )
