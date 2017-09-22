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

def main(argv):
	# Parse args
	try:
		opts, args = getopt.getopt(sys.argv[1:], "", ["poly-file="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print( str( err ) )  # will print something like "option -a not recognized"
		sys.exit(2)

	osmFilepath = polyFilepath = None
	for o, a in opts:
		if o in ("--poly-file"):
			polyFilepath = str(a)
		else:
			assert False, "[!] Unhandled option."

	assert polyFilepath != None, "[!] Missing inputs."

	# Parse osm file
	print(polyFilepath)

if __name__ == "__main__":
	main(sys.argv[1:])
