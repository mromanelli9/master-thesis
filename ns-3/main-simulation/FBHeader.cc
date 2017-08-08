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

#include "FBHeader.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/object-vector.h"

namespace ns3 {
	NS_LOG_COMPONENT_DEFINE ("FBHeader");

	NS_OBJECT_ENSURE_REGISTERED (FBHeader);

	TypeId
	FBHeader::GetTypeId (void)
	{
	  static TypeId tid = TypeId ("ns3::FBHeader")
	    .SetParent<Header> ()
	    .SetGroupName("Network")
			.AddAttribute ("Position", "The spatial location (gps) of the sender.",
										TypeId::ATTR_GET || TypeId::ATTR_SET,
										Vector3DValue (Vector (0,0,0)),
										MakeVector3DAccessor (&FBHeader::m_position),
										MakeVector3DChecker ())
			.AddAttribute ("StarterPosition", "The spatial location (gps) of the starter node.",
										TypeId::ATTR_GET || TypeId::ATTR_SET,
										Vector3DValue (Vector (0,0,0)),
										MakeVector3DAccessor (&FBHeader::m_starterPosition),
										MakeVector3DChecker ())
			.AddAttribute ("MaxRange", "The maximum range.",
										TypeId::ATTR_GET || TypeId::ATTR_SET,
										UintegerValue (0),
										MakeUintegerAccessor (&FBHeader::m_maxRange),
										MakeUintegerChecker<uint32_t> ());

	  return tid;
	}

	FBHeader::~FBHeader ()
	{
	  NS_LOG_FUNCTION (this);
	}

	void
	FBHeader::SetPosition (Vector pos)
	{
		NS_LOG_FUNCTION (this);
		m_position = pos;
	}

	void
	FBHeader::SetStarterPosition (Vector pos)
	{
		NS_LOG_FUNCTION (this);
		m_starterPosition = pos;
	}

	void
	FBHeader::SetMaxRange (uint32_t value)
	{
		NS_LOG_FUNCTION (this);
		m_maxRange = value;
	}

	Vector
	FBHeader::GetPosition (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_position;
	}

	Vector
	FBHeader::GetStarterPosition (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_starterPosition;
	}

	uint32_t
	FBHeader::GetMaxRange (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_maxRange;
	}

	TypeId
	FBHeader::GetInstanceTypeId (void) const
	{
		return GetTypeId ();
	}

	uint32_t
	FBHeader::GetSerializedSize (void) const
	{
		// Vector3D = 24
		// uint32_t = 4
		NS_LOG_FUNCTION (this);

		uint32_t  length = 52;
		return  length;
	}

	void
	FBHeader::Serialize (Buffer::Iterator start) const
	{
		NS_LOG_FUNCTION (this);
		Buffer::Iterator i = start;

		i.WriteU64(m_position.x);
		i.WriteU64(m_position.y);
		i.WriteU64(m_position.z);
		i.WriteU64(m_starterPosition.x);
		i.WriteU64(m_starterPosition.y);
		i.WriteU64(m_starterPosition.z);
		i.WriteU32(m_maxRange);
	}

	uint32_t
	FBHeader::Deserialize (Buffer::Iterator start)
	{
		NS_LOG_FUNCTION (this);
		Buffer::Iterator i = start;

		m_position = Vector (i.ReadU64 (), i.ReadU64 (), i.ReadU64 ());
		m_starterPosition = Vector (i.ReadU64 (), i.ReadU64 (), i.ReadU64 ());
		m_maxRange = i.ReadU32 ();

		return  GetSerializedSize  ();
	}

	void
	FBHeader::Print (std::ostream &os) const
	{
		NS_LOG_FUNCTION (this);
		os << "m_position (" << m_position.x << "," << m_position.y << "," << m_position.z << ") "
			<< "m_starterPosition (" << m_starterPosition.x << "," << m_starterPosition.y << "," << m_starterPosition.z << ") "
			<< "m_maxRange " << m_maxRange << std::endl;
	}

} // namespace ns3
