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

#ifndef DUMMYSENSOR_H
#define DUMMYSENSOR_H

#include "ns3/core-module.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/vector.h"
#include "ns3/socket.h"
#include "ns3/packet.h"


namespace ns3 {

/**
 * \ingroup network
 * \brief A special node used in Fast Broadcast protocol.
 */
class DummySensor : public Object
{
public:
	static TypeId GetTypeId (void);

	DummySensor();
	virtual ~DummySensor();

	/**
	 * \returns the sensor identifier
	 */
	uint32_t GetId (void) const;

	/**
	 * \returns the node included in the sensor
	 */
	Ptr<Node> GetNode (void) const;

	/**
	 * \returns true if the node has received an alert
	 */
	bool GetReceived (void) const;

	/**
	 * \brief set the node
	 * \param node ns-3 node
	 */
	void SetNode (Ptr<Node> node);

	/**
	 * \brief set the socket of the node
	 * \param socket internet socket
	 */
	void SetSocket (Ptr<Socket> socket);

	/**
	 * \brief set the received field
	 * \param value boolean value
	 */
	void SetReceived (bool value);

	/**
	 * \brief send a packet
	 * \param packet packet to send
	 */
	void Send (Ptr<Packet> packet);

	/**
	 * \brief call when sensor receive a packet
	 * \param socket socket that received the packet
	 */
	void ReceivePacket (Ptr<Socket> socket);

	uint32_t							m_id;	// node id
	static uint32_t				g_idCounter;	// global id counter
	Ptr<Node>							m_node;	// ns-3 node
	Ptr<Socket> 					m_socket; // ns-3 socket
	bool									m_received;	// true if the sensor has already received a message
};

} // namespace ns3

#endif /* DUMMYSENSOR_H */
