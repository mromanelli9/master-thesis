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

#include <numeric>
#include <math.h>

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
		m_broadcastPhaseStart (0),
		m_cwMin (32),
		m_cwMax (1024),
		m_flooding (true),
		m_actualRange (300),
		m_estimatedRange (0),
		m_aoi (m_actualRange * 2),
		m_aoi_error (0),
		m_packetPayload (100),
		m_received (0),
		m_sent (0)
{
	NS_LOG_FUNCTION (this);

	srand (time (0));
	RngSeedManager::SetSeed (time (0));
}

FBApplication::~FBApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
FBApplication::Install (uint32_t protocol, uint32_t broadcastPhaseStart, uint32_t actualRange, uint32_t aoi, uint32_t aoi_error, bool flooding, uint32_t cwMin, uint32_t cwMax)
{

	if (protocol == PROTOCOL_FB)
	{
		m_estimatedRange = PROTOCOL_FB;
		m_staticProtocol = false;
	}
	else if (protocol == PROTOCOL_STATIC_300)
	{
		m_estimatedRange = PROTOCOL_STATIC_300;
		m_staticProtocol = true;
	}
	else if (protocol == PROTOCOL_STATIC_500)
	{
		m_estimatedRange = PROTOCOL_STATIC_500;
		m_staticProtocol = true;
	}
	else
		NS_LOG_ERROR ("Protocol not found.");

	m_broadcastPhaseStart = broadcastPhaseStart;
	m_aoi = aoi;
	m_aoi_error = aoi_error;
	m_actualRange = actualRange;
	m_flooding = flooding;
	m_cwMin = cwMin;
	m_cwMax	= cwMax;
}

void
FBApplication::AddNode (Ptr<Node> node, Ptr<Socket> source, Ptr<Socket> sink, bool onstats)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<FBNode> fbNode = CreateObject<FBNode> ();
	fbNode->SetNode (node);
	fbNode->SetId (node->GetId ());
	fbNode->SetSocket (source);
	sink->SetRecvCallback (MakeCallback (&FBApplication::ReceivePacket, this));
	fbNode->SetCMFR (m_estimatedRange);
	fbNode->SetLMFR (m_estimatedRange);
	fbNode->SetCMBR (m_estimatedRange);
	fbNode->SetLMBR (m_estimatedRange);
	fbNode->UpdatePosition ();
	fbNode->SetNum (0);
	fbNode->SetPhase (-1);
	fbNode->SetSlot (0);
	fbNode->SetReceived (false);
	fbNode->SetSent (false);
	fbNode->SetMeAsVehicle (onstats);

	// misc stuff
	m_nodes.push_back (fbNode);
	m_id2id[fbNode->GetId ()] = m_nodes.size() - 1;
	m_nNodes++;
}

void
FBApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

	// Get startingNode as node and as fbNode
	m_startingNode = this->GetNode ()->GetId ();

	if (m_id2id.count(m_startingNode) == 0)
	{
		NS_LOG_ERROR ("Starting node is not a fb node!");
	}

	if (!m_staticProtocol)
	{
		// Start Estimation Phase
		NS_LOG_INFO ("Start Estimation Phase.");
		GenerateHelloTraffic (5);
	}

	// Schedule Broadcast Phase
	Simulator::Schedule (Seconds (m_broadcastPhaseStart), &FBApplication::StartBroadcastPhase, this);
}

void
FBApplication::StopApplication (void)
{
	NS_LOG_FUNCTION (this);
}

void
FBApplication::GenerateHelloTraffic (uint32_t count)
{
	NS_LOG_FUNCTION (this << count);

	std::vector<int> he;
	uint32_t hel = (int) m_nNodes / 100 * 50;		// 40% of total nodes
	uint32_t time_factor = 10;

	if (count > 0)
	{
		for (uint32_t i = 0; i < hel; i++)
		{
			int pos = rand() % m_nNodes;
			he.push_back (pos);
			Ptr<FBNode> fbNode = m_nodes.at(pos);
			Simulator::ScheduleWithContext (fbNode->GetNode ()->GetId (),
																			MicroSeconds (i * time_factor),
																			&FBApplication::GenerateHelloMessage, this, fbNode);
		}

		// Other nodes must send Hello messages
		double s = ceil((hel * time_factor) / 1000000.0);
		Simulator::Schedule (Seconds (s), &FBApplication::GenerateHelloTraffic, this, count - 1);
	}
}

void
FBApplication::StartBroadcastPhase (void)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Start Broadcast Phase.");

	Ptr<FBNode> fbNode = this->GetFBNode(m_startingNode);

	// Generate the first alert message
	GenerateAlertMessage (fbNode);
}

void
FBApplication::GenerateHelloMessage (Ptr<FBNode> fbNode)
{
	NS_LOG_FUNCTION (this << fbNode);
	NS_LOG_DEBUG ("Generate Hello Message (" << fbNode->GetNode ()->GetId () << ").");

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
	NS_LOG_DEBUG ("Generate Alert Message (" << fbNode->GetNode ()->GetId () << ").");

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
	fbHeader.SetPhase(0);
	fbHeader.SetSlot(0);

	Ptr<Packet> packet = Create<Packet> (m_packetPayload);
	packet->AddHeader (fbHeader);

	fbNode->Send (packet);
	fbNode->SetSent(true);
	m_sent++;

	// Store current time
	fbNode->SetTimestamp (Simulator::Now ());
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

		// Compute the distance between the sender and me (the node who received the message)
	 	double distanceSenderToCurrent = ns3::CalculateDistance(senderPosition, currentPosition);
		uint32_t distanceSenderToCurrent_uint = std::floor (distanceSenderToCurrent);

		// If the node is in range I can read the packet
		// uint32_t estimatedRange = fbNode->GetCMBR ();
		// if (distanceSenderToCurrent_uint <= estimatedRange)
		// {
		if (messageType == HELLO_MESSAGE)
			HandleHelloMessage (fbNode, fbHeader);
		else if (messageType == ALERT_MESSAGE)
		{
			m_received++;

			// Get the phase
			int32_t phase = fbHeader.GetPhase ();

			// Get the position of the node who start the broadcast
			Vector starterPosition = fbHeader.GetStarterPosition ();

			// Compute the two distances
			double distanceSenderToStarter = ns3::CalculateDistance(senderPosition, starterPosition);
			double distanceCurrentToStarter = ns3::CalculateDistance(currentPosition, starterPosition);

			// If starter-to-sender distance is less than starter-to-current distance,
			// then the message is coming from the front and it needs to be menaged,
			// otherwise do nothing
			if (distanceCurrentToStarter > distanceSenderToStarter && !fbNode->GetReceived ())
			{
				// Store when the current has received the first packet
				fbNode->SetTimestamp (Simulator::Now ());

				uint32_t sl = fbHeader.GetSlot ();
				fbNode->SetSlot (fbNode->GetSlot() + sl);
				fbNode->SetReceived (true);

				if (fbNode->GetNum( ) == 0)
				{
					fbNode->SetNum (phase);
				}

				// check if the message is coming fron the front
				if (phase > fbNode->GetPhase ())
				{
					fbNode->SetPhase (phase);
					HandleAlertMessage (fbNode, fbHeader, distanceSenderToCurrent_uint);
				}
			}
			else
			{
				if (fbNode->GetPhase() < phase)
					fbNode->SetPhase (phase);
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

	// Retrieve CMFR from the packet received and CMBR from the current node
	uint32_t otherCMFR = fbHeader.GetMaxRange ();
	uint32_t myCMBR = fbNode->GetCMBR ();

	// Retrieve the position of the current node
	Vector currentPosition = fbNode->UpdatePosition ();

	// Retrieve the position of the sender node
	Vector senderPosition = fbHeader.GetPosition ();

	// Compute distance
	double distance_double = ns3::CalculateDistance (senderPosition, currentPosition);
	uint32_t distance = std::floor (distance_double);

	// Update new values
	uint32_t maxi = std::max (std::max (myCMBR, otherCMFR), distance);

	fbNode->SetCMBR (maxi);

	// Override the old values
	fbNode->SetLMBR (myCMBR);
}

void
FBApplication::HandleAlertMessage (Ptr<FBNode> fbNode, FBHeader fbHeader, uint32_t distance)
{
	// We assume that the message is coming from the front
	NS_LOG_FUNCTION (this << fbNode << fbHeader << distance);
	uint32_t nodeId = fbNode->GetNode ()->GetId ();

	NS_LOG_DEBUG ("Handle an Alert Message (" << nodeId << ").");

	// Compute the size of the contention window
	uint32_t bmr = fbNode->GetCMBR ();
	uint32_t cwnd = ComputeContetionWindow (bmr, distance);

	// Compute a random waiting time (1 <= waitingTime <= cwnd)
	uint32_t waitingTime = (rand () % cwnd) + 1;

	// Wait and then forward the message
	if (m_flooding == false)
		Simulator::ScheduleWithContext (nodeId, MilliSeconds (waitingTime * 200 * 3),
																	&FBApplication::WaitAgain, this, fbNode, fbHeader, waitingTime);
	else
		Simulator::ScheduleWithContext (nodeId, MilliSeconds(0),
																	&FBApplication::ForwardAlertMessage, this, fbNode, fbHeader, waitingTime);
}

void
FBApplication::WaitAgain (Ptr<FBNode> fbNode, FBHeader fbHeader, uint32_t waitingTime)
{
	 NS_LOG_FUNCTION (this);

	 // Get the phase
	 int32_t phase = fbHeader.GetPhase ();

	 if (phase >= fbNode->GetPhase())
	 {
		 uint32_t rnd = (rand() % 20)+1;
		 uint32_t rnd1 = (rand() % 20)+1;
		 uint32_t rnd2 = (rand() % 20)+1;
		 uint32_t rnd3 = (rand() % 20)+1;
		 Simulator::Schedule (MilliSeconds (10* (waitingTime+rnd+rnd1+rnd2+rnd3) * 200 * 3),
		 											&FBApplication::ForwardAlertMessage, this, fbNode, fbHeader, waitingTime);
	 }
}

void
FBApplication::ForwardAlertMessage (Ptr<FBNode> fbNode, FBHeader oldFBHeader, uint32_t waitingTime)
{
	NS_LOG_FUNCTION (this << fbNode << oldFBHeader);

	// Get the phase
	int32_t phase = oldFBHeader.GetPhase ();

	// If I'm the first to wake up, I must forward the message
	if ((!m_flooding && phase >= fbNode->GetPhase ()) || (m_flooding && !fbNode->GetSent ()))
	{
		NS_LOG_DEBUG ("Forwarding Alert Message (" << fbNode->GetNode ()->GetId () << ").");

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
		fbHeader.SetPhase (phase + 1);
		fbHeader.SetSlot (fbNode->GetSlot() + waitingTime);

		Ptr<Packet> packet = Create<Packet> (m_packetPayload);
		packet->AddHeader (fbHeader);

		// Forward
		fbNode->Send (packet);
		fbNode->SetSent (true);

		m_sent++;
	}
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

	if (m_id2id.count(node->GetId ()) == 0)
	{
		// We got a problem: key not found
		NS_LOG_ERROR ("Error: key for node " << node->GetId () << " not found in fb application.");
	}

	return this->GetFBNode (node->GetId ());
}

Ptr<FBNode>
FBApplication::GetFBNode (uint32_t id)
{
	NS_LOG_FUNCTION (this);

	if (m_id2id.count(id) == 0)
	{
		// We got a problem: key not found
		NS_LOG_ERROR ("Error: key for node " << id << " not found in fb application.");
	}

	uint32_t idin = m_id2id[id];

	return m_nodes.at (idin);
}

void
FBApplication::PrintStats (std::stringstream &dataStream)
{
	NS_LOG_FUNCTION (this);

	uint32_t cover = 1;	// 'cause we count m_startingNode
	uint32_t circ = 0, circCont = 0;

	double radiusMin = m_aoi - m_aoi_error;
	double radiusMax = m_aoi + m_aoi_error;

	long double time_sum = 0;
	long double nums_sum = 0;
	long double slots_sum = 0;

	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		Ptr<FBNode> current = m_nodes.at (i);
		uint32_t nodeId = current->GetId ();

		// Skip the starting node
		if (nodeId == m_startingNode)
			continue;

		// If this isn't a vehciles, skip
		if (!current->AmIaVehicle ())
			continue;

		// Update the total cover value
		if (current->GetReceived ())
			cover++;

		// Compute cover on circumference of radius m_aoi
		Ptr<FBNode> startingNode = this->GetFBNode(m_startingNode);

		Vector currentPosition = current->GetPosition ();
		Vector startingNodePosition = startingNode->GetPosition ();

		double distance = ns3::CalculateDistance (currentPosition, startingNodePosition);

		// Check if the current vehicle is in the circumference and within the range
		if ((distance >= radiusMin) && (distance <= radiusMax))
		{
			// Update the number of vehicles in the circumference
			circCont++;

			// Update the cover value
			if (current->GetReceived ())
			{
				circ++;

				// Update mean time, nums and slots
				nums_sum += current->GetNum();
				slots_sum += current->GetSlot();
				time_sum += current->GetTimestamp().GetMicroSeconds ();
			}
		}
	}

	Time timeref = this->GetFBNode(m_startingNode)->GetTimestamp();

	dataStream << circCont << ","
			<< cover << ","
			<< circ << ","
			<< timeref.GetMicroSeconds () - (time_sum / (double) circ)  << ","
			<< (nums_sum / (double) circ) << ","
			<< (slots_sum / (double) circ) << ","
			<< m_sent << ","
			<< m_received;
}

uint32_t
FBApplication::ComputeContetionWindow (uint32_t maxRange, uint32_t distance)
{
	NS_LOG_FUNCTION (this << maxRange << distance);

	double cwnd = 0.0;
	double rapp = 0.0;

	if (maxRange != 0)
		rapp = (maxRange - distance) / (double) maxRange;
	else
		rapp = 0;

	rapp = (rapp < 0) ? 0 : rapp;

	cwnd = (rapp * (m_cwMax - m_cwMin)) + m_cwMin;

	return std::floor (cwnd);
}
} // namespace ns3
