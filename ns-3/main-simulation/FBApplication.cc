/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Università di Padova
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
 * Authors: Marco Romanelli <marco.romanelli.1@studenti.unipd.it>
 */

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/object-ptr-container.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

#include "FBApplication.h"
#include "FBHeader.h"
#include "FBNode.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FBApplication");

NS_OBJECT_ENSURE_REGISTERED (FBApplication);

TypeId
FBApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FBApplication")
    .SetParent<Application> ()
    .SetGroupName("Network");
		// TODO
		// .AddAttribute ("EstimationPhaseRunning", "True if the estimation phase is running.",
		// 							BooleanValue (false),
		// 							MakeBooleanAccessor (&FBApplication::m_estimationPhaseRunning),
		// 							MakeBooleanChecker<bool> ());
		// .AddAttribute ("BroadcastPhaseRunning", "True if the broadcast phase is running.",
		// 							BooleanValue (false),
		// 							MakeBooleanAccessor (&FBApplication::m_broadcastPhaseRunning),
		// 							MakeBooleanChecker<bool> ());
		// .AddAttribute ("EstimationPhaseEvent", "Event associated to the estimation phase.",
		// 							ObjectPtrContainerValue (),
		// 							ObjectPtrContainerAccessor (&FBApplication::m_estimationPhaseEvent),
		// 							MakeObjectPtrContainerChecker<EventId> ());
		// .AddAttribute ("BroadcastPhaseEvent", "Event associated to the broadcast phase.",
		// 							ObjectPtrContainerValue (),
		// 							ObjectPtrContainerAccessor (&FBApplication::m_broadcastPhaseEvent),
		// 							MakeObjectPtrContainerChecker<EventId> ());

  return tid;
}

FBApplication::FBApplication ()
	:	m_nNodes (0),
		m_estimationPhaseRunning (false),
		m_broadcastPhaseRunning (false),
		m_estimationPhaseEvent (),
		m_broadcastPhaseEvent (),
		m_cwMin (32),
		m_cwMax (1024),
		m_flooding (true),
		m_turn (1),
		m_estimatedRange (0),
		m_packetPayload (100)
{
	NS_LOG_FUNCTION (this);
}

FBApplication::~FBApplication ()
{
  NS_LOG_FUNCTION (this);
}

// void
// FBApplication::Setup (NodeContainer nodes)
// {
// 	NS_LOG_FUNCTION (this << &nodes);
// 	NS_LOG_INFO ("Setup FB Application (" << this << ").");
//
// 	// m_nodes will contain all nodes in <nodes>
// 	m_nodes.Add (nodes);
// 	m_nNodes = m_nodes.GetN ();
//
// 	// Setup FBnode parameters for all the nodes in m_nodes
// 	for (uint32_t i = 0; i < m_nNodes; i++)
// 		Ptr<FBNode> current = m_nodes.Get (i);
// 		SetupFBNode (current);
// }

void
FBApplication::AddNode (Ptr<Node> node, Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<FBNode> fbNode = CreateObject<FBNode> ();
	fbNode->SetNode (node);
	fbNode->SetSocket (socket);
	fbNode->SetCMFR (m_estimatedRange);
	fbNode->SetLMFR (m_estimatedRange);
	fbNode->SetCMBR (m_estimatedRange);
	fbNode->SetLMBR (m_estimatedRange);
	fbNode->UpdatePosition ();

	m_nodes.push_back (fbNode);
}

void
FBApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
}

void
FBApplication::StopApplication (void)
{
	NS_LOG_FUNCTION (this);

	// Stop both phases
	StopEstimationPhase ();
  StopBroadcastPhase ();
}

void
FBApplication::StartEstimationPhase (void)
{
	NS_LOG_FUNCTION (this);
}

void
FBApplication::StartBroadcastPhase (void)
{
	NS_LOG_FUNCTION (this);
}

void
FBApplication::StopEstimationPhase (void)
{
	NS_LOG_FUNCTION (this);

	m_estimationPhaseRunning = false;

	if (m_estimationPhaseEvent.IsRunning ())
	{
		Simulator::Cancel (m_estimationPhaseEvent);
	}
}

void
FBApplication::StopBroadcastPhase (void)
{
	NS_LOG_FUNCTION (this);

	m_broadcastPhaseRunning = false;

	if (m_broadcastPhaseEvent.IsRunning ())
	{
		Simulator::Cancel (m_broadcastPhaseEvent);
	}
}

// void
// FBApplication::HandleHelloMessage (Ptr<FBNode> node, FBHeader fbHeader)
// {
// 	NS_LOG_FUNCTION (this << node << fbHeader);
// 	NS_LOG_INFO ("Handle a Hello Message (" << node->GetId () << ").");
//
// 	// Retrieve CMFR from the packet received and CMBR from the current node
// 	uint32_t otherCMFR = fbHeader.GetMaxRange ();	// TODO: controllare che max_range sia il cmfr
// 	uint32_t myCMBR = node->GetCMBR ();
//
// 	// Retrieve the position of the current node
// 	Vector currentPosition = node->UpdatePosition ();
//
// 	// Retrieve the position of the starter node
// 	Vector starterPosition = fbHeader.GetStarterPosition ();
//
// 	// Compute distance
// 	uint32_t distance = CalculateDistance (starterPosition, currentPosition);
//
// 	// Update new values
// 	uint32_t m0 = std::max (myCMBR, otherCMFR);
// 	uint32_t maxi = std::max (m0, distance);
//
// 	node->SetCMBR (maxi);
// 	node->SetLMBR (myCMBR);
// }

// void
// FBApplication::HandleAlertMessage (Ptr<FBNode> node, FBHeader fbHeader, uint32_t distance)
// {
// 	// We assume that the message is coming from the front
//
// 	NS_LOG_FUNCTION (this << node << fbHeader << distance);
// 	NS_LOG_INFO ("Handle an Alert Message (" << node->GetId () << ").");

	// // Compute the size of the contention window
	// uint32_t cmbr = node->GetCMBR ();
	// uint32_t cwnd = ComputeContetionWindow (cmbr, distance);
	//
	// // Compute a random waiting time (1 <= waitingTime <= cwnd)
	// uint32_t waitingTime = (rand () % cwnd) + 1;

	//
	// if (!m_flooding)
	// {
	// 	Simulator::ScheduleWithContext (node->GetId (), MilliSeconds (rs), &FBApplication::KeepWaiting, this, ...); // TODO: add arguments
	// }
	// else
	// {
	// 	Simulator::ScheduleWithContext (node->GetId (), MilliSeconds (0), &FBApplication::ForwardAlertMessage, this, ...); // TODO: add arguments
	// }
// }

void
FBApplication::GenerateHelloMessage (Ptr<FBNode> fbNode)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_DEBUG ("Generate Hello Message (node <" << fbNode->GetNode ()->GetId () << ">).");

	// Create a packet with the correct parameters taken from the node
	Vector position = fbNode->UpdatePosition ();
	FBHeader fbHeader;
	fbHeader.SetMaxRange (fbNode->GetCMBR ());
	fbHeader.SetStarterPosition (position);
	fbHeader.SetPosition (position);

	Ptr<Packet> packet = Create<Packet> (m_packetPayload);
	packet->AddHeader (fbHeader);

	fbNode->Send (packet);
}

uint32_t
FBApplication::ComputeContetionWindow (uint32_t maxRange, uint32_t distance)
{
	// NB: distance is always >= 0 (its type is uint32_t)
	NS_LOG_FUNCTION (this << maxRange << distance);

	double cwnd = 0;

	double rapp = (maxRange - distance) / (double) maxRange;
	cwnd = (rapp * (m_cwMax - m_cwMin)) + m_cwMin;

	return std::floor (cwnd);
}

double
FBApplication::ComputeDistance (Vector a, Vector b)
{
	NS_LOG_FUNCTION ("ComputeDistance" << a << b);
	double distance = 0;

	uint32_t diffx = (b.x - a.x) * (b.x - a.x);
	uint32_t diffy = (b.y - a.y) * (b.y - a.y);

	distance = sqrt (diffx + diffy);

	return distance;
}

} // namespace ns3
