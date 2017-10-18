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

@file    prunePoly.py
@author  Marco Romanelli
@date    17-10-2017
@version 1
"""

import getopt, sys
import xml.etree.ElementTree as ET


def prunePoly(polyFilename, outputFilename):
	xmlTree = ET.parse(polyFilename)

	root = xmlTree.getroot()

	end = False

	while (not end):
		end = True
		for el in root:
			if ("id" in el.attrib and  "#" in el.attrib["id"]):
				root.remove(el)
				end = False
				continue

			if ("layer" in el.attrib and el.attrib["layer"] != "-1.00"):
				root.remove(el)
				end = False
				continue

			if ("type" in el.attrib and  el.attrib["type"] == "water"):
				root.remove(el)
				end = False

	for el in root:
		if ("shape" in el.attrib):
			el.attrib["shape"] += ' '

	writeXml(outputFilename, xmlTree)

def writeXml(filename, data):
	data.write(open(filename, "wb"), encoding="UTF-8")

def main(argv):
	# Parse args
	try:
		opts, args = getopt.getopt(argv, "", ["poly-file=", "output-file="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print(str(err))  # will print something like "option -a not recognized"
		sys.exit(2)

	polyFilename = outputFilename = None
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

	assert polyFilename != None and outputFilename != None, "[!] Missing inputs."

	# Parse poly file
	prunePoly(polyFilename, outputFilename)

if __name__ == "__main__":
	main(sys.argv[1:])
