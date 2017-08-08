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

#include "FBApplication.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/object-ptr-container.h"
#include "ns3/core-module.h"

namespace ns3 {
	NS_LOG_COMPONENT_DEFINE ("FBApplication");

	NS_OBJECT_ENSURE_REGISTERED (FBApplication);

	TypeId
	FBApplication::GetTypeId (void)
	{
	  static TypeId tid = TypeId ("ns3::FBApplication")
	    .SetParent<Application> ()
	    .SetGroupName("Network");
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
		: m_estimationPhaseRunning (false),
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
	FBApplication::StartApplication (void)
	{
	  NS_LOG_FUNCTION (this);
	}

	void
	FBApplication::StopApplication (void)
	{
		NS_LOG_FUNCTION (this);

	  m_estimationPhaseRunning = false;
		m_broadcastPhaseRunning = false;

	  if (m_estimationPhaseEvent.IsRunning ())
		{
			Simulator::Cancel (m_estimationPhaseEvent);
		}

		if (m_broadcastPhaseEvent.IsRunning ())
		{
			Simulator::Cancel (m_broadcastPhaseEvent);
		}
	}
} // namespace ns3
