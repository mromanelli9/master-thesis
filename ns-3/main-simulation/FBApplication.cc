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
	:	m_nNodes (1),
		m_estimationPhaseRunning (false),
		m_broadcastPhaseRunning (false),
		m_estimationPhaseEvent (),
		m_broadcastPhaseEvent ()
{
	NS_LOG_FUNCTION (this);
}

FBApplication::~FBApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
FBApplication::Setup (void)
{
	NS_LOG_FUNCTION (this);
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

void
FBApplication::HandleHelloMessage (Ptr<FBNode> node, Ptr<Packet> packet)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Handle an Hello Message (" << node->GetId () << ").");

	// Extract FB header from the packet
	FBHeader fbHeader;
	packet->RemoveHeader (fbHeader);

	// Retrieve CMFR from the packet received and CMBR from the current node
	uint32_t otherCMFR = fbHeader.GetMaxRange ();	// TODO: controllare che max_range sia il cmfr
	uint32_t myCMBR = node->GetCMBR ();

	// Retrieve the position of the current node
	Vector currentPosition = node->UpdatePosition ();

	// Retrieve the position of the starter node
	Vector starterPosition = fbHeader.GetStarterPosition ();

	// Compute distance
	// double distance = CalculateDistance (starterPosition, currentPosition);

	// Update new values
	uint32_t m0 = std::max (myCMBR, otherCMFR);
	uint32_t maxi = std::max (m0, distance);
	node->SetCMBR (maxi);
	node->SetLMBR (myCMBR);
}

} // namespace ns3
