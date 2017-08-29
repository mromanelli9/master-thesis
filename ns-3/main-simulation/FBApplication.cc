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
		m_broadcastPhaseStart (0),
		m_cwMin (32),
		m_cwMax (1024),
		m_flooding (true),
		m_turn (1000),
		m_actualRange (300),
		m_estimatedRange (300),	// DEBUG --> TODO change it!!!
		m_packetPayload (100),
		m_slot (20)
{
	NS_LOG_FUNCTION (this);

	srand (time (0));
}

FBApplication::~FBApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
FBApplication::Setup (uint32_t broadcastPhaseStart)
{
	m_broadcastPhaseStart = broadcastPhaseStart;
}

void
FBApplication::AddNode (Ptr<Node> node, Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<FBNode> fbNode = CreateObject<FBNode> ();
	fbNode->SetNode (node);
	fbNode->SetSocket (socket);
	socket->SetRecvCallback (MakeCallback (&FBApplication::ReceivePacket, this));
	fbNode->SetCMFR (m_estimatedRange);
	fbNode->SetLMFR (m_estimatedRange);
	fbNode->SetCMBR (m_estimatedRange);
	fbNode->SetLMBR (m_estimatedRange);
	fbNode->UpdatePosition ();

	m_nodes.push_back (fbNode);
	m_nodesMap.insert (std::pair<uint32_t, uint32_t> (node->GetId (), m_nNodes));

	m_nNodes++;
}

void
FBApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

	// Start Estimation Phase
	m_estimationPhaseRunning = true;
	GenerateHelloTraffic ();

	// Schedule Broadcast Phase
	Simulator::Schedule (Seconds (m_broadcastPhaseStart), &FBApplication::StartBroadcastPhase, this);
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
FBApplication::GenerateHelloTraffic (void)
{
	NS_LOG_FUNCTION (this);

	// Stop the generation of hello messages
	if (!m_estimationPhaseRunning)
		return;

	// For each node ...
	for (uint32_t j = 0; j < m_nNodes; j++)
	{
		Ptr<FBNode> fbNode = m_nodes.at (j);

		// Compute a random time
		// problem: how much time?
		uint32_t waitingTime = rand () % m_slot;

		std::cout << "waitingTime " << waitingTime << std::endl;

		// Schedule the generation of a Hello Message for the current node
		Simulator::ScheduleWithContext (fbNode->GetNode ()->GetId (),
																		MilliSeconds (waitingTime * m_slot),
																		&FBApplication::GenerateHelloMessage, this,
																		fbNode);
	}

	// Schedule another turn
	Simulator::Schedule (MilliSeconds (m_turn), &FBApplication::GenerateHelloTraffic, this);
}

void
FBApplication::StartBroadcastPhase (void)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Start Broadcast Phase.");

	m_estimationPhaseRunning = false;
	m_broadcastPhaseRunning = true;

	// // Select the starting node that will generate the first alert
	// Ptr<FBNode> fbNode = m_nodes.at (m_startingNode);
	// // To be sure that there will be no hello message, wait two turns
	// Simulator::ScheduleWithContext (fbNode->GetNode ()->GetId (), MilliSeconds (m_turn * 2), &FBApplication::GenerateAlertMessage, this, fbNode);
}

void
FBApplication::StopEstimationPhase (void)
{
	NS_LOG_FUNCTION (this);

	m_estimationPhaseRunning = false;
}

void
FBApplication::StopBroadcastPhase (void)
{
	NS_LOG_FUNCTION (this);

	m_broadcastPhaseRunning = false;
}

void
FBApplication::GenerateHelloMessage (Ptr<FBNode> fbNode)
{
	NS_LOG_FUNCTION (this << fbNode);
	NS_LOG_DEBUG ("Generate Hello Message (node <" << fbNode->GetNode ()->GetId () << ">).");

	// Create a packet with the correct parameters taken from the node
	Vector position = fbNode->UpdatePosition ();
	FBHeader fbHeader;
	fbHeader.SetType (HELLO_MESSAGE);
	fbHeader.SetMaxRange (fbNode->GetCMBR ());
	fbHeader.SetStarterPosition (position);
	fbHeader.SetPosition (position);

	Ptr<Packet> packet = Create<Packet> (m_packetPayload);
	packet->AddHeader (fbHeader);

	fbNode->Send (packet);
}

void
FBApplication::GenerateAlertMessage (Ptr<FBNode> fbNode)
{
	NS_LOG_FUNCTION (this << fbNode);
	NS_LOG_DEBUG ("Generate Alert Message (node <" << fbNode->GetNode ()->GetId () << ">).");

	// Create a packet with the correct parameters taken from the node
	uint32_t LMBR, CMBR, maxi;
	LMBR = fbNode->GetLMBR ();
	CMBR = fbNode->GetCMBR ();
	maxi = std::max (LMBR, CMBR);

	Vector position = fbNode->UpdatePosition ();
	FBHeader fbHeader;
	fbHeader.SetType (ALERT_MESSAGE);
	fbHeader.SetMaxRange (maxi);
	fbHeader.SetStarterPosition (position);
	fbHeader.SetPosition (position);

	Ptr<Packet> packet = Create<Packet> (m_packetPayload);
	packet->AddHeader (fbHeader);

	fbNode->Send (packet);
}

void
FBApplication::ReceivePacket (Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << socket);

	// Get the node who received this message and the corresponding FBNode
	Ptr<Node> node = socket->GetNode ();
	Ptr<FBNode> fbNode = GetFBNode (node);

  Ptr<Packet> packet;
	Address senderAddress;

  while ((packet = socket->RecvFrom (senderAddress)))
  {
		NS_LOG_DEBUG ("Packet received: " << Simulator::Now ().GetSeconds () << " (node <" << node->GetId () << ">).");

		FBHeader fbHeader;
		packet->RemoveHeader (fbHeader);

		// Get the type of the message (Hello or Alert)
		uint32_t messageType = fbHeader.GetType ();

		// Get the current position of the node
		Vector currentPosition = fbNode->UpdatePosition ();

		// Get the position of the sender node
		Vector senderPosition = fbHeader.GetPosition ();

		// Compute the distance between the sender and the node who received the message
	 	double distanceSenderToCurrent = ComputeDistance(senderPosition, currentPosition);
		uint32_t distanceSenderToCurrent_uint = std::abs (std::floor (distanceSenderToCurrent));

		// If the node is in range
		if (distanceSenderToCurrent_uint < m_actualRange)
		{
			if (messageType == HELLO_MESSAGE)
				HandleHelloMessage (fbNode, fbHeader);
			else if (messageType == ALERT_MESSAGE)
			{
				// Compute the two different distances
				Vector starterPosition = fbHeader.GetStarterPosition ();
				double distanceStarterToSender = ComputeDistance(starterPosition, senderPosition);
				double distanceStarterToCurrent = ComputeDistance(starterPosition, currentPosition);

				if (distanceStarterToSender < distanceStarterToCurrent)
					HandleAlertMessage (fbNode, fbHeader, distanceSenderToCurrent_uint);
			}
		}
  }
}

void
FBApplication::HandleHelloMessage (Ptr<FBNode> node, FBHeader fbHeader)
{
	NS_LOG_FUNCTION (this << node << fbHeader);
	NS_LOG_INFO ("Handle a Hello Message (" << node->GetNode ()->GetId () << ").");

	// Retrieve CMFR from the packet received and CMBR from the current node
	uint32_t otherCMFR = fbHeader.GetMaxRange ();	// TODO: controllare che max_range sia il cmfr
	uint32_t myCMBR = node->GetCMBR ();

	// Retrieve the position of the current node
	Vector currentPosition = node->UpdatePosition ();

	// Retrieve the position of the starter node
	Vector starterPosition = fbHeader.GetStarterPosition ();

	// Compute distance
	double distance_double = CalculateDistance (starterPosition, currentPosition);
	uint32_t distance = std::abs (std::floor (distance_double));

	// Update new values
	uint32_t m0 = std::max (myCMBR, otherCMFR);
	uint32_t maxi = std::max (m0, distance);

	node->SetCMBR (maxi);
	node->SetLMBR (myCMBR);
}

void
FBApplication::HandleAlertMessage (Ptr<FBNode> fbNode, FBHeader fbHeader, uint32_t distance)
{
	// We assume that the message is coming from the front
	NS_LOG_FUNCTION (this << fbNode << fbHeader << distance);
	NS_LOG_INFO ("Handle an Alert Message (" << fbNode->GetNode ()->GetId () << ").");

	// Compute the size of the contention window
	uint32_t cmbr = fbNode->GetCMBR ();
	uint32_t cwnd = ComputeContetionWindow (cmbr, distance);

	// Compute a random waiting time (1 <= waitingTime <= cwnd)
	uint32_t waitingTime = (rand () % cwnd) + 1;

	// TODO: e se qualcosa arriva nel frattempo?
	// Wait <waitingTime> milliseconds and then forward the message
	Simulator::ScheduleWithContext (fbNode->GetNode ()->GetId (), MilliSeconds (waitingTime * m_slot), &FBApplication::ForwardAlertMessage, this, fbNode, fbHeader);


	// if (!m_flooding)
	// {
	// 	Simulator::ScheduleWithContext (node->GetId (), MilliSeconds (rs), &FBApplication::KeepWaiting, this, ...); // TODO: add arguments
	// }
	// else
	// {
	// 	Simulator::ScheduleWithContext (node->GetId (), MilliSeconds (0), &FBApplication::ForwardAlertMessage, this, ...); // TODO: add arguments
	// }
}

void
FBApplication::ForwardAlertMessage (Ptr<FBNode> fbNode, FBHeader oldFBHeader)
{
	NS_LOG_FUNCTION (this << fbNode << oldFBHeader);
	NS_LOG_DEBUG ("Forwarding Alert Message (node <" << fbNode->GetNode ()->GetId () << ">).");

	// Create a packet with the correct parameters taken from the node
	uint32_t LMBR, CMBR, maxi;
	LMBR = fbNode->GetLMBR ();
	CMBR = fbNode->GetCMBR ();
	maxi = std::max (LMBR, CMBR);

	Vector position = fbNode->UpdatePosition ();
	Vector starterPosition = oldFBHeader.GetStarterPosition ();

	FBHeader fbHeader;
	fbHeader.SetType (ALERT_MESSAGE);
	fbHeader.SetMaxRange (maxi);
	fbHeader.SetStarterPosition (starterPosition);
	fbHeader.SetPosition (position);

	Ptr<Packet> packet = Create<Packet> (m_packetPayload);
	packet->AddHeader (fbHeader);

	// Forward
	fbNode->Send (packet);
}

Ptr<FBNode>
FBApplication::GetFBNode (Ptr<Node> node)
{
	uint32_t id = node->GetId ();
	uint32_t index = m_nodesMap.at (id);

	return m_nodes.at (index);
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
