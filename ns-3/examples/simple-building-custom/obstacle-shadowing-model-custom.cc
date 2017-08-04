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

NS_LOG_COMPONENT_DEFINE ("obstacle-shadowing-propagation-loss-model-custom-example");

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

class MyTopology : public Topology
{
public:
	MyTopology ();
	~MyTopology ();

	void CreateBuilding (std::string id, std::vector<Point> shape, double beta, double gamma);
};

MyTopology::MyTopology () : Topology ()
{
}

void
MyTopology::CreateBuilding (std::string id, std::vector<Point> shape, double beta, double gamma)
{
	NS_LOG_UNCOND ("Create building.");

	Obstacle obstacle;
	obstacle.SetId(id);

	for ( uint32_t i = 0; i < shape.size(); i++ ) {
		obstacle.AddVertex (shape[i]);
	}

	obstacle.SetBeta (beta);
	obstacle.SetGamma (gamma);

	obstacle.Locate();

	Point center = obstacle.GetCenter();
  Key k = Key(center, obstacle);

	this->m_obstacles.push_back(k);
	this->MakeRangeTree();
}


int main (int argc, char *argv[])
{
	/* ---------------------- Main ---------------------- */
	double m_beta = 9.0;
	double m_gamma = 0.4;

	CommandLine cmd;
  cmd.AddValue ("beta", "Set beta value", m_beta);
  cmd.AddValue ("gamma", "Set gamma value", m_gamma);
  cmd.Parse (argc,argv);

  MyTopology *topology = new MyTopology ();
	Ptr<MobilityModel> firstNode = CreateObject<ConstantPositionMobilityModel> ();
  Ptr<MobilityModel> secondNode = CreateObject<ConstantPositionMobilityModel> ();

	firstNode->SetPosition (Vector (0, 2, 0));
  secondNode->SetPosition (Vector (10, 2, 0));

	std::vector<Point> shape;

	shape.push_back (Point (3.0, 2.0));
	shape.push_back (Point (7.0, 2.0));
	shape.push_back (Point (7.0, 0.0));
	shape.push_back (Point (3.0, 0.0));

	topology->CreateBuilding("b1", shape, m_beta, m_gamma);

	Ptr<PropagationLossModel> model = CreateObject<ObstacleShadowingPropagationLossModel> (topology);
	std::cout << "Propagation loss: " << model->CalcRxPower (0, firstNode, secondNode) << std::endl;

  Simulator::Destroy ();

  return 0;
}
