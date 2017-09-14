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

 #include "FBNode.h"

 #include "ns3/log.h"
 #include "ns3/uinteger.h"
 #include "ns3/object-vector.h"
 #include "ns3/node-list.h"
 #include "ns3/mobility-model.h"
 #include "ns3/socket.h"
 #include "ns3/packet.h"


 namespace ns3 {

 uint32_t FBNode::g_idCounter = 0;

 NS_LOG_COMPONENT_DEFINE ("FBNode");

 NS_OBJECT_ENSURE_REGISTERED (FBNode);

 TypeId FBNode::GetTypeId (void)
 {
	 static TypeId tid = TypeId ("ns3::FBNode")
	 	.SetParent<Object> ()
		.SetGroupName ("Network")
		.AddConstructor<FBNode> ();

	  return tid;
	}

	FBNode::FBNode()
	  : m_id (g_idCounter++),
			m_CMFR (0),
			m_LMFR (0),
			m_CMBR (0),
			m_LMBR (0),
			m_position (Vector (0, 0, 0))
	{
	  NS_LOG_FUNCTION (this);
	}

	FBNode::~FBNode ()
	{
	  NS_LOG_FUNCTION (this);
	}

	uint32_t
	FBNode::GetId (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_id;
	}

	Ptr<Node>
	FBNode::GetNode (void) const
	{
		NS_LOG_FUNCTION (this);
	 return m_node;
	}

	uint32_t
	FBNode::GetCMFR (void) const
	{
	  NS_LOG_FUNCTION (this);
	  return m_CMFR;
	}

	uint32_t
	FBNode::GetLMFR (void) const
	{
	  NS_LOG_FUNCTION (this);
	  return m_LMFR;
	}

	uint32_t
	FBNode::GetCMBR (void) const
	{
	  NS_LOG_FUNCTION (this);
	  return m_CMBR;
	}

	uint32_t
	FBNode::GetLMBR (void) const
	{
	  NS_LOG_FUNCTION (this);
	  return m_LMBR;
	}

	Vector
	FBNode::GetPosition (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_position;
	}

	uint32_t
	FBNode::GetNum (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_num;
	}

	int32_t
	FBNode::GetPhase (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_phase;
	}

	uint32_t
	FBNode::GetSlot (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_slot;
	}

	bool
	FBNode::GetReceived (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_received;
	}

	bool
	FBNode::GetSent (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_sent;
	}

	void
	FBNode::SetNode (Ptr<Node> node)
	{
		NS_LOG_FUNCTION (this << node);
	  m_node = node;
	}

	void
	FBNode::SetSocket (Ptr<Socket> socket)
	{
		NS_LOG_FUNCTION (this << socket);
		m_socket = socket;
	}

	void
	FBNode::SetCMFR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_CMFR = value;
	}

	void
	FBNode::SetLMFR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_LMFR = value;
	}

	void
	FBNode::SetCMBR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_CMBR = value;
	}

	void
	FBNode::SetLMBR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_LMBR = value;
	}

	Vector
	FBNode::UpdatePosition (void)
	{
		NS_LOG_FUNCTION (this);
		Ptr<MobilityModel> positionmodel = m_node->GetObject<MobilityModel> ();

		// Check if a mobility model exists
		if (positionmodel != 0)
		{
			m_position = positionmodel->GetPosition ();
		}

		return m_position;
	}

	void
	FBNode::SetNum (uint32_t value)
	{
		NS_LOG_FUNCTION (this << value);
		m_num = value;
	}

	void
	FBNode::SetPhase (int32_t value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_phase = value;
	}

	void
	FBNode::SetSlot (uint32_t value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_slot = value;
	}

	void
	FBNode::SetReceived (bool value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_received = value;
	}

	void
	FBNode::SetSent (bool value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_sent = value;
	}

	void
	FBNode::Send (Ptr<Packet> packet)
	{
		NS_LOG_FUNCTION (this << packet);

		m_socket->Send (packet);
	}
} // namespace ns3
