/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
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
 *
 */

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/object-ptr-container.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/mobility-module.h"

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

  return tid;
}

FBApplication::FBApplication ()
	:	m_nNodes (0),
		m_startingNode (0),
		m_staticProtocol (false),
		m_estimationPhaseRunning (false),
		m_broadcastPhaseRunning (false),
		m_broadcastPhaseStart (0),
		m_cwMin (32),
		m_cwMax (1024),
		m_flooding (true),
		m_turn (1000),
		m_actualRange (300),
		m_estimatedRange (0),
		m_packetPayload (100),
		m_slot (20),
		m_totalHelloMessages (0),
		m_totalHops (0)
{
	NS_LOG_FUNCTION (this);

	srand (time (0));
}

FBApplication::~FBApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
FBApplication::Setup (uint32_t protocol, uint32_t startingNode, uint32_t broadcastPhaseStart, uint32_t actualRange, uint32_t cwMin, uint32_t cwMax, uint32_t turn, uint32_t slot)
{
	if (protocol == PROTOCOL_FB)
	{
		m_estimatedRange = 0;
		m_staticProtocol = false;
	}
	else if (protocol == PROTOCOL_STATIC_300)
	{
		m_estimatedRange = 300;
		m_staticProtocol = true;
	}
	else if (protocol == PROTOCOL_STATIC_1000)
	{
		m_estimatedRange = 1000;
		m_staticProtocol = true;
	}
	else
		NS_LOG_ERROR ("Protocol not found.");

	m_startingNode = startingNode;
	m_broadcastPhaseStart = broadcastPhaseStart;
	m_actualRange = actualRange;
	m_cwMin = cwMin;
	m_cwMax	= cwMax;
	m_turn = turn;
	m_slot = slot;
}

void
FBApplication::AddNode (Ptr<Node> node, Ptr<Socket> source, Ptr<Socket> sink)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<FBNode> fbNode = CreateObject<FBNode> ();
	fbNode->SetNode (node);
	fbNode->SetSocket (source);
	sink->SetRecvCallback (MakeCallback (&FBApplication::ReceivePacket, this));
	fbNode->SetCMFR (m_estimatedRange);
	fbNode->SetLMFR (m_estimatedRange);
	fbNode->SetCMBR (m_estimatedRange);
	fbNode->SetLMBR (m_estimatedRange);
	fbNode->UpdatePosition ();

	// misc stuff
	m_nodes.push_back (fbNode);
	m_nodesMap.insert (std::pair<uint32_t, uint32_t> (node->GetId (), m_nNodes));

	m_broadcastForwardCheck.insert (std::pair<uint32_t, bool> (node->GetId (), false));

	m_helloMessageDisabled.push_back (false);

	m_nNodes++;
}

void
FBApplication::DisableEstimationPhase (void)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Estimation Phase disabled.");

	m_staticProtocol = true;
	m_estimatedRange = m_actualRange;
}

void
FBApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

	if (!m_staticProtocol)
	{
		// Start Estimation Phase
		NS_LOG_INFO ("Start Estimation Phase.");
		m_estimationPhaseRunning = true;
		GenerateHelloTraffic ();

		// Schedule Broadcast Phase
		Simulator::Schedule (Seconds (m_broadcastPhaseStart), &FBApplication::StartBroadcastPhase, this);
	}
	else
		StartBroadcastPhase ();
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

	// Clear all node
	std::fill(m_helloMessageDisabled.begin(), m_helloMessageDisabled.end(), false);

	// For each node ...
	for (uint32_t j = 0; j < m_nNodes; j++)
	{
		Ptr<FBNode> fbNode = m_nodes.at (j);

		// Compute a random time
		// problem: how much time?
		uint32_t waitingTime = rand () % (m_turn / 4);

		// Schedule the generation of a Hello Message for the current node
		uint32_t nodeId = fbNode->GetNode ()->GetId ();
		Simulator::ScheduleWithContext (nodeId, MilliSeconds (waitingTime * m_slot), &FBApplication::GenerateHelloMessage, this, fbNode);
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

	Ptr<FBNode> fbNode = m_nodes.at (m_startingNode);
	// Simulate the event (that will cause the generation of an alert)
	// by stopping the node
	StopNode (fbNode);

	// Generate the first alert message
	GenerateAlertMessage (fbNode);
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

	// Check if this node can send a hello message (in this turn)
	uint32_t nodeId = fbNode->GetNode ()->GetId ();
	uint32_t fbNodeId = m_nodesMap.at (nodeId);
	if (m_helloMessageDisabled.at (fbNodeId))
	{
		// if so, do not generate a hello message
		return;
	}

	NS_LOG_DEBUG ("Generate Hello Message (node " << nodeId << ").");

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

	m_totalHelloMessages++;
}

void
FBApplication::GenerateAlertMessage (Ptr<FBNode> fbNode)
{
	NS_LOG_FUNCTION (this << fbNode);
	NS_LOG_DEBUG ("Generate Alert Message (node " << fbNode->GetNode ()->GetId () << ").");

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
		NS_LOG_DEBUG ("Packet received by node " << node->GetId () << ".");

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
		// TODO check if actual range or the estimated range
		if (distanceSenderToCurrent_uint <= m_actualRange)
		{
			if (messageType == HELLO_MESSAGE)
				HandleHelloMessage (fbNode, fbHeader);
			else if (messageType == ALERT_MESSAGE)
			{
				// Compute the two different distances
				Vector starterPosition = fbHeader.GetStarterPosition ();
				double distanceStarterToSender = ComputeDistance(starterPosition, senderPosition);
				double distanceStarterToCurrent = ComputeDistance(starterPosition, currentPosition);

				// If starter-to-sender distance is less than starter-to-current distance,
				// then the message is coming from the front and it needs to be menaged,
				// otherwise do nothing
				if (distanceStarterToSender < distanceStarterToCurrent)
					HandleAlertMessage (fbNode, fbHeader, distanceSenderToCurrent_uint);
				else
				{
					NS_LOG_DEBUG ("Node " << node->GetId () << " has dropped an Alert Message.");

					// Check if this node has already a forwarding procedure pending
					// If true, delete the pending event
					// TODO: check if it's true
					if (m_broadcastForwardCheck.at (node->GetId ()) == true)
					{
						NS_LOG_DEBUG ("Another forwarding procedure is pending so the previous one will be deleted (" << node->GetId () << ").");
						Simulator::Cancel (m_broadcastForwardEvent.at (node->GetId ()));
					}
				}
			}
		}
  }
}

void
FBApplication::HandleHelloMessage (Ptr<FBNode> fbNode, FBHeader fbHeader)
{
	NS_LOG_FUNCTION (this << fbNode << fbHeader);
	uint32_t nodeId = fbNode->GetNode ()->GetId ();
	NS_LOG_DEBUG ("Handle a Hello Message (" << nodeId << ").");

	// I have received a hello message, so in this turn i won't send another
	uint32_t fbNodeId = m_nodesMap.at (nodeId);
	m_helloMessageDisabled.at (fbNodeId) = true;

	// Override the old values
	fbNode->SetLMFR (fbNode->GetCMFR ());
	fbNode->SetLMBR (fbNode->GetCMBR ());

	// Retrieve CMFR from the packet received and CMBR from the current node
	uint32_t otherCMFR = fbHeader.GetMaxRange ();
	uint32_t myCMBR = fbNode->GetCMBR ();

	// Retrieve the position of the current node
	Vector currentPosition = fbNode->UpdatePosition ();

	// Retrieve the position of the sender node
	Vector senderPosition = fbHeader.GetPosition ();

	// Compute distance
	double distance_double = CalculateDistance (senderPosition, currentPosition);
	uint32_t distance = std::abs (std::floor (distance_double));

	// Update new values
	uint32_t maxi = std::max (std::max (myCMBR, otherCMFR), distance);

	fbNode->SetCMBR (maxi);
}

void
FBApplication::HandleAlertMessage (Ptr<FBNode> fbNode, FBHeader fbHeader, uint32_t distance)
{
	// We assume that the message is coming from the front
	NS_LOG_FUNCTION (this << fbNode << fbHeader << distance);
	uint32_t nodeId = fbNode->GetNode ()->GetId ();
	NS_LOG_DEBUG ("Handle an Alert Message (" << nodeId << ").");

	// If I'm the last car in the platoon then the broadcast phase needs to end, goal reached
	if (nodeId == m_nodes.at (m_nNodes-1)->GetNode ()->GetId ())	// DEBUG: maybe this can be optimized
	{
		NS_LOG_DEBUG ("Broadcast Phase has reached the last node.");
		StopBroadcastPhase ();
		return;
	}

	// Check if this node has already a forwarding procedure pending
	// If true, delete the pending event
	if (m_broadcastForwardCheck.at (nodeId) == true)
	{
		NS_LOG_DEBUG ("Another forwarding procedure is pending so the previous one will be deleted (" << nodeId << ").");
		Simulator::Cancel (m_broadcastForwardEvent.at (nodeId));
	}

	// Compute the size of the contention window
	uint32_t cmbr = fbNode->GetCMBR ();
	uint32_t cwnd = ComputeContetionWindow (cmbr, distance);

	// Compute a random waiting time (1 <= waitingTime <= cwnd)
	uint32_t waitingTime = (rand () % cwnd) + 1;

	// Wait <waitingTime> milliseconds and then forward the message
	EventId event = Simulator::Schedule (MilliSeconds (waitingTime * m_slot), &FBApplication::ForwardAlertMessage, this, fbNode, fbHeader);

	// Store the event for further use
	m_broadcastForwardEvent.insert (std::pair<uint32_t, EventId> (nodeId, event));
	m_broadcastForwardCheck.at (nodeId) = true;
}

void
FBApplication::ForwardAlertMessage (Ptr<FBNode> fbNode, FBHeader oldFBHeader)
{
	NS_LOG_FUNCTION (this << fbNode << oldFBHeader);
	NS_LOG_DEBUG ("Forwarding Alert Message (node " << fbNode->GetNode ()->GetId () << ").");

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

	// Increase the hop counter
	m_totalHops++;
}

void
FBApplication::StopNode (Ptr<FBNode> fbNode)
{
	NS_LOG_FUNCTION (this);

	Ptr<Node> node = fbNode->GetNode ();

	Ptr<ConstantVelocityMobilityModel> mob = node->GetObject<ConstantVelocityMobilityModel>();
	mob->SetVelocity (Vector (0, 0, 0));
}

Ptr<FBNode>
FBApplication::GetFBNode (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this);

	uint32_t id = node->GetId ();
	uint32_t index = m_nodesMap.at (id);

	return m_nodes.at (index);
}

void
FBApplication::PrintStats (void)
{
	NS_LOG_FUNCTION (this);

	if (!m_staticProtocol) {
		NS_LOG_INFO ("Total Hello Messages sent: " << m_totalHelloMessages << ".");
		NS_LOG_INFO ("Estimated transimision range: " << m_nodes.at (m_startingNode)->GetCMBR () << " meters (actual range: " << m_actualRange << " m).");
	}
	else
	{
		NS_LOG_INFO ("Estimation Phase disabled (static protocol); estimated range: " << m_actualRange << " meters.");
	}
	NS_LOG_INFO ("Total number of hops (Broadcast Phase): " << m_totalHops << ".");
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
