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

#ifndef FBNODE_H
#define FBNODE_H

#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/vector.h"

namespace ns3 {

/**
 * \ingroup network
 * \brief A special node used in Fast Broadcast protocol.
 */
class FBNode : public Node
{
public:
	static TypeId GetTypeId (void);

	FBNode();
	virtual ~FBNode();

	/**
   * \returns the value of the CMFR field
   */
	uint32_t GetCMFR (void) const;

	/**
	 * \returns the value of the LMFR field
	 */
	uint32_t GetLMFR (void) const;

	/**
	 * \returns the value of the CMBR field
	 */
	uint32_t GetCMBR (void) const;

	/**
	 * \returns the value of the LMBR field
	 */
	uint32_t GetLMBR (void) const;

	/**
	 * \returns the position of the node
	 */
	Vector GetPosition (void) const;

	/**
	 * \brief set the value of the CMFR field
	 * \param value new value of CMFR
	 */
	void SetCMFR (uint32_t value);

	/**
	 * \brief set the value of the LMFR field
	 * \param value new value of LMFR
	 */
	void SetLMFR (uint32_t value);

	/**
	 * \brief set the value of the CMBR field
	 * \param value new value of CMBR
	 */
	void SetCMBR (uint32_t value);

	/**
	 * \brief set the value of the LMBR field
	 * \param value new value of LMBR
	 */
	void SetLMBR (uint32_t value);

	/**
	 * \brief update it's (node) current position
	 * \returns the new position of the node
	 */
	Vector UpdatePosition (void);


	uint32_t 	  m_CMFR;	// Current Maximum Front Range
	uint32_t 	  m_LMFR;	// Last Maximum Front Range
	uint32_t 	  m_CMBR;	// Current Maximum Back Range
	uint32_t 	  m_LMBR; // Last Maximum Back Range

	Vector 			m_position;	// node current position
};

} // namespace ns3

#endif /* FBNODE_H */
