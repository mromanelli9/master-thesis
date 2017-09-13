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

#ifndef FBAPPLICATION_H
#define FBAPPLICATION_H

#include "ns3/application.h"
#include "ns3/network-module.h"

#include "FBNode.h"
#include "FBHeader.h"

namespace ns3 {

static const uint32_t PROTOCOL_FB = 0;
static const uint32_t PROTOCOL_STATIC_300 = 1;
static const uint32_t PROTOCOL_STATIC_1000 = 2;

/**
 * \ingroup network
 * \brief A special application that implements Fast Broadcast protocol.
 */
class FBApplication : public Application
{
public:
	/**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

	FBApplication ();
	virtual ~FBApplication();

	/**
	 * \brief Set up some application parameters
	 * \param TODO
	 * \return none
	 */
	void Setup (uint32_t protocol, uint32_t startingNode, uint32_t broadcastPhaseStart, uint32_t actualRange, uint32_t cwMin, uint32_t cwMax);

	/**
	 * \brief Add a new node to the applicatin and set up protocol parameters
	 * \param node node to add
	 * \param source source socket of the node
	 * \param sink sink socket of the node
	 * \return none
	 */
	void AddNode (Ptr<Node> node, Ptr<Socket> source, Ptr<Socket> sink);

	/**
	 * \brief Print value of some useful field
	 * \return none
	 */
	void PrintStats (void);

private:
	/**
	 * \brief Application specific startup code
	 *
	 * The StartApplication method is called at the start time specified by Start
	 * This method should be overridden by all or most application
	 * subclasses.
	 */
	virtual void StartApplication (void);

	/**
	 * \brief Application specific shutdown code
	 *
	 * The StopApplication method is called at the stop time specified by Stop
	 * This method should be overridden by all or most application
	 * subclasses.
	 */
  virtual void StopApplication (void);

	// TODO: headers
	void GenerateHelloTraffic (uint32_t count);
	void StartBroadcastPhase (void);
	void StopEstimationPhase (void);
	void StopBroadcastPhase (void);

	/**
	 * \brief Send a Hello message to all nodes in its range
	 * \return none
	 */
	void GenerateHelloMessage (Ptr<FBNode> fbNode);

	/**
	 * \brief Send a Alert message
	 * \return none
	 */
	void GenerateAlertMessage (Ptr<FBNode> fbNode);

	/**
   * \brief Process a received packet
   * \param socket the receiving socket
   * \return none
   */
	void ReceivePacket (Ptr<Socket> socket);
	void HandleHelloMessage (Ptr<FBNode> fbNode, FBHeader fbHeader);
	void HandleAlertMessage (Ptr<FBNode> fbNode, FBHeader fbHeader, uint32_t distance);
	void ForwardAlertMessage (Ptr<FBNode> fbNode, FBHeader oldFBHeader);

	/**
	 * \brief Stop a node
	 * \param fbNode node to be stopped
	 * \return none
	 */
	void StopNode (Ptr<FBNode> fbNode);

	Ptr<FBNode> GetFBNode (Ptr<Node> node);

	uint32_t ComputeContetionWindow (uint32_t maxRange, uint32_t distance);

	static double ComputeDistance (Vector a, Vector b);


private:
	uint32_t																m_nNodes;	// number of nodes
	std::vector<Ptr<FBNode>>								m_nodes;	// nodes that run this application
	uint32_t																m_startingNode; // index of the node that will generate the Alert Message
	std::map<uint32_t, uint32_t> 						m_nodesMap;	// map nodes and FBNodes
	bool																		m_staticProtocol;	// true if static protocol is used
	bool           										 			m_estimationPhaseRunning;	// true if the estimation phase is running
	bool            									 			m_broadcastPhaseRunning;	// true if the broadcast phase is running
	uint32_t													 			m_broadcastPhaseStart;	// broadcast phase start time (seconds)
	uint32_t													 			m_cwMin;	// min size of the contention window (in slot)
	uint32_t													 			m_cwMax;	// max size of the contention window (in slot)
	bool															 			m_flooding;	// used for control the flooding of the Alert messages
	uint32_t													 			m_actualRange;	// real transmission range
	uint32_t													 			m_estimatedRange;	// range of transmission to be estimated
	uint32_t													 			m_packetPayload; // size of the packet payload
	uint32_t													 			m_totalHelloMessages;	// number of hello messages sent
	uint32_t																m_totalAlertMessages; // // number of alert messages sent
	uint32_t													 			m_totalHops;	// total number of hop in the broadcast phase
};

} // namespace ns3

#endif /* FBAPPLICATION_H */
