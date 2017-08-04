/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Universita' di Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Marco Romanelli <marco.romanelli.1@studenti.unipd.it>
 */
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/topology.h"
#include "ns3/obstacle-shadowing-propagation-loss-model.h"
#include <vector>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("obstacle-shadowing-propagation-loss-model-basic-example");

// ===========================================================================
//
//				 					    (3,2)									 (7,2)
//										 			 +----------------+
//				       						 |    				    |
//				firstNode  ========|    building  	|======== secondNode
//					(0,2)		   			 |							  |					  (10,2)
//				  	   			  		 +----------------+
//									    (3,0)                  (7,0)
//
// ===========================================================================

int main (int argc, char *argv[])
{
	/* ---------------------- Init ---------------------- */
	uint32_t buildings = 1;
	std::string bldgFile = "basic-example.building.xml";

  CommandLine cmd;
	cmd.AddValue ("buildings", "Enable a building between the nodes.", buildings);
	cmd.AddValue ("buildingsFile", "Load building file.", bldgFile);
  cmd.Parse (argc, argv);

	/* ---------------------- Main ---------------------- */
	if (buildings != 0)
	{
		NS_LOG_UNCOND ("Loading buildings file " << bldgFile);
		Topology::LoadBuildings(bldgFile);
	}

  Ptr<PropagationLossModel> model = CreateObject<ObstacleShadowingPropagationLossModel> ();
	Ptr<MobilityModel> firstNode = CreateObject<ConstantPositionMobilityModel> ();
  Ptr<MobilityModel> secondNode = CreateObject<ConstantPositionMobilityModel> ();

	firstNode->SetPosition (Vector (0, 2, 0));
  secondNode->SetPosition (Vector (10, 2, 0));

	std::cout << "Propagation loss: " << model->CalcRxPower (0, firstNode, secondNode) << std::endl;

  return 0;
}
