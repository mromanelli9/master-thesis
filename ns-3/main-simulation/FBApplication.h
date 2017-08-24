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

#ifndef FBAPPLICATION_H
#define FBAPPLICATION_H

#include "ns3/application.h"
#include "ns3/network-module.h"

#include "FBNode.h"
#include "FBHeader.h"

namespace ns3 {

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
	 * \brief Configure the application
	 */
	void Setup (NodeContainer nodes);

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

	/**
	 * \brief Set up Fast Broadcast protocol parameters for a single node
	 * \param node node to configure
	 * \return none
	 */
	void SetupFBNode (Ptr<FBNode> node);

	// TODO: headers
	void StartEstimationPhase (void);
	void StartBroadcastPhase (void);
	void StopEstimationPhase (void);
	void StopBroadcastPhase (void);

	void HandleHelloMessage (Ptr<FBNode> node, FBHeader fbHeader);
	void HandleAlertMessage (Ptr<FBNode> node, FBHeader fbHeader, uint32_t distance);
	// void KeepWaiting (Ptr<FBNode> node)

	/**
	 * \brief Send a Hello message to all nodes in its range
	 * \return none
	 */
	void GenerateHelloMessage (void);
	
	uint32_t ComputeContetionWindow (uint32_t maxRange, uint32_t distance);

	static double ComputeDistance (Vector a, Vector b);

private:
	uint32_t				m_nNodes;	// number of nodes
	NodeContainer		m_nodes;	// nodes that run this application
	bool            m_estimationPhaseRunning;	// true if the estimation phase is running
	bool            m_broadcastPhaseRunning;	// true if the broadcast phase is running
	EventId         m_estimationPhaseEvent;	// event associated to the estimation phase
	EventId         m_broadcastPhaseEvent;	// event associated to the broadcast phase
	uint32_t				m_cwMin;	// min size of the contention window (in slot)
	uint32_t				m_cwMax;	// max size of the contention window (in slot)
	bool						m_flooding;	// used for control the flooding of the Alert messages
	uint32_t				m_turn;	// duration of a single turn
	uint32_t				m_estimatedRange;	// range of transmission to be estimated
};

} // namespace ns3

#endif /* FBAPPLICATION_H */
