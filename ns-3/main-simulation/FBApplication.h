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
	void Setup ();

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
	void StartEstimationPhase (void);
	void StartBroadcastPhase (void);
	void StopEstimationPhase (void);
	void StopBroadcastPhase (void);

	void HandleHelloMessage (Ptr<FBNode> node, Ptr<Packet> packet);
	void HandleAlertMessage (Ptr<FBNode> node, Ptr<Packet> packet);


private:
	uint32_t				m_nNodes;	// number of nodes
	bool            m_estimationPhaseRunning;	// true if the estimation phase is running
	bool            m_broadcastPhaseRunning;	// true if the broadcast phase is running
	EventId         m_estimationPhaseEvent;	// event associated to the estimation phase
	EventId         m_broadcastPhaseEvent;	// event associated to the broadcast phase
};

} // namespace ns3

#endif /* FBAPPLICATION_H */
