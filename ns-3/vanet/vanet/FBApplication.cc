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
		m_packetPayload (100),
		m_received (0),
		m_sent (0)
{
	NS_LOG_FUNCTION (this);

	srand (time (0));
}

FBApplication::~FBApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
FBApplication::Install (uint32_t protocol, uint32_t broadcastPhaseStart, uint32_t actualRange, uint32_t aoi, bool flooding, uint32_t cwMin, uint32_t cwMax)
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

	m_broadcastPhaseStart = broadcastPhaseStart;
	m_aoi = aoi;
	m_actualRange = actualRange;
	m_flooding = flooding;
	m_cwMin = cwMin;
	m_cwMax	= cwMax;
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
	fbNode->SetNum (0);
	fbNode->SetPhase (-1);
	fbNode->SetSlot (0);
	fbNode->SetReceived (false);
	fbNode->SetSent (false);

	// misc stuff
	m_nodes.push_back (fbNode);
	m_nNodes++;
}

void
FBApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

	m_startingNode = this->GetNode ()->GetId ();

	if (!m_staticProtocol)
	{
		// Start Estimation Phase
		NS_LOG_INFO ("Start Estimation Phase.");
		GenerateHelloTraffic (60);
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
	uint32_t hel = 20;

	if (count > 0)
	{
		for (uint32_t i = 0; i < hel; i++)
		{
			int pos = rand() % m_nNodes;
			he.push_back (pos);
			Ptr<FBNode> fbNode = m_nodes.at(pos);
			Simulator::ScheduleWithContext (fbNode->GetNode ()->GetId (),
																			Seconds (i * 40),
																			&FBApplication::GenerateHelloMessage, this, fbNode);
		}

		// Other nodes must send Hello messages
		Simulator::Schedule (Seconds (250), &FBApplication::GenerateHelloTraffic, this, count - 1);
	}
}

void
FBApplication::StartBroadcastPhase (void)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Start Broadcast Phase.");

	Ptr<FBNode> fbNode = m_nodes.at (m_startingNode);

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
	StopNode (fbNode);
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
	 	double distanceSenderToCurrent = ComputeDistance(senderPosition, currentPosition);
		uint32_t distanceSenderToCurrent_uint = std::floor (distanceSenderToCurrent);

		// If the node is in range I can read the packet
		if (distanceSenderToCurrent_uint <= m_actualRange)
		{
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
				double distanceSenderToStarter = ComputeDistance(senderPosition, starterPosition);
				double distanceCurrentToStarter = ComputeDistance(currentPosition, starterPosition);

				// If starter-to-sender distance is less than starter-to-current distance,
				// then the message is coming from the front and it needs to be menaged,
				// otherwise do nothing
				if (distanceCurrentToStarter > distanceSenderToStarter && !fbNode->GetReceived ())
				{
					uint32_t sl = fbHeader.GetSlot ();
					fbNode->SetSlot (fbNode->GetSlot() + sl);
					StopNode (fbNode);
					fbNode->SetReceived (true);
					if (fbNode->GetNum( ) == 0)
						fbNode->SetNum (phase);

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
	double distance_double = CalculateDistance (senderPosition, currentPosition);
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

	uint32_t id = node->GetId ();

	return m_nodes.at (id);
}

void
FBApplication::PrintStats (void)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("------------------ STATISTICS ------------------");

	uint32_t cover=1;
	uint32_t circ = 0, circCont = 0;
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		Ptr<FBNode> current = m_nodes.at (i);
		Ptr<FBNode> startingNode = m_nodes.at (m_startingNode);

		// NB: UpdatePosition will cause an assert error in constant-velocity-helper.cc
		Vector currentPosition = current->GetPosition ();
		Vector startingNodePosition = startingNode->GetPosition ();
		// Vector currentPosition = current->UpdatePosition ();
		// Vector startingNodePosition = startingNode->UpdatePosition ();

		double distStart = CalculateDistance (currentPosition, startingNodePosition);

		uint32_t dist = 12; 	// TODO cambiare (distanza fra mezzi)
		uint32_t rCirc = 1000;
		if (i != m_startingNode && distStart > 0 &&
				((distStart - rCirc <= (dist/2) && distStart - rCirc >= 0) || (rCirc - distStart <= (dist/2) && distStart - rCirc <= 0)))
		{
			circCont++;
			if (current->GetReceived ())
				circ++;
		}

		if (current->GetReceived ())
			cover++;
	}

	NS_LOG_INFO ("Actual range: " << m_actualRange << " meter.");
	std::string protocolName = "";
	if (m_estimatedRange == 0)
		protocolName = "FAST_BROADCAST";
	else if (m_estimatedRange == 300)
		protocolName = "STATIC_300";
	else
		protocolName = "STATIC_1000";
	NS_LOG_INFO ("Protocol used: " << protocolName << ".");
	NS_LOG_INFO ("Total number of vehicles: " << m_nNodes << ".");
	NS_LOG_INFO ("Number of vehicles covered (whole area): "
									<< ((double)cover/(double)m_nNodes)*100 << "% ("
									<< cover << "/" << m_nNodes << ").");
	NS_LOG_INFO ("Number of vehicles covered (outer limit): "
									<< ((double)circ/(double)circCont)*100 << "% ("
									<< circ << "/" << circCont << ").");

	uint32_t bord = (4000 / 300) -1;
	std::string nums = "";
	for (uint i=0; i<(bord*4); i++)
	{
		nums += std::to_string (m_nodes.at (i)->GetNum());
		nums += " ";
	}
	NS_LOG_INFO ("Nums: " << nums << ".");

	NS_LOG_INFO ("Total messages sent: " << m_sent << ".");
	NS_LOG_INFO ("Total messages received: " << m_received << ".");
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