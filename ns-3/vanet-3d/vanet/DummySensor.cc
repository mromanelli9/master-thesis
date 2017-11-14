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

 #include "DummySensor.h"

 #include "ns3/core-module.h"
 #include "ns3/log.h"
 #include "ns3/uinteger.h"
 #include "ns3/object-vector.h"
 #include "ns3/socket.h"
 #include "ns3/packet.h"


 namespace ns3 {

 uint32_t DummySensor::g_idCounter = 0;

 NS_LOG_COMPONENT_DEFINE ("DummySensor");

 NS_OBJECT_ENSURE_REGISTERED (DummySensor);

 TypeId DummySensor::GetTypeId (void)
 {
	 static TypeId tid = TypeId ("ns3::DummySensor")
	 	.SetParent<Object> ()
		.SetGroupName ("Network")
		.AddConstructor<DummySensor> ();

	  return tid;
	}

	DummySensor::DummySensor()
	  : m_id (g_idCounter++),
			m_received (false)
	{
	  NS_LOG_FUNCTION (this);
	}

	DummySensor::~DummySensor ()
	{
	  NS_LOG_FUNCTION (this);
	}

	uint32_t
	DummySensor::GetId (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_id;
	}

	Ptr<Node>
	DummySensor::GetNode (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_node;
	}

	bool
	DummySensor::GetReceived (void) const
	{
		NS_LOG_FUNCTION (this);
		return m_received;
	}

	void
	DummySensor::SetNode (Ptr<Node> node)
	{
		NS_LOG_FUNCTION (this << node);
	  m_node = node;
	}

	void
	DummySensor::SetSocket (Ptr<Socket> socket)
	{
		NS_LOG_FUNCTION (this << socket);
		m_socket = socket;
	}

	void
	DummySensor::SetReceived (bool value)
	{
	  NS_LOG_FUNCTION (this << value);
	  m_received = value;
	}

	void
	DummySensor::Send (Ptr<Packet> packet)
	{
		NS_LOG_FUNCTION (this << packet);

		m_socket->Send (packet);
	}

	void
	DummySensor::ReceivePacket (Ptr<Socket> socket)
	{
		NS_LOG_FUNCTION (this << socket);

		// Get the node who received this message and the corresponding FBNode
		Ptr<Node> node = socket->GetNode ();

		// Double-check
		if (node->GetId () != m_node->GetId ())
			NS_LOG_ERROR ("Nodes ids not matching!");


	  Ptr<Packet> packet;
		Address senderAddress;

	  while ((packet = socket->RecvFrom (senderAddress)))
	  {
			NS_LOG_DEBUG ("Packet received by dummy sensor " << node->GetId () << ".");

			if (!this->GetReceived ())
			{
				this->Send(packet);
				this->SetReceived (true);

				NS_LOG_DEBUG ("Packet forwarded by dummy sensor " << node->GetId () << ".");
			}
		}
	}
} // namespace ns3
