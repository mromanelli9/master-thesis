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

 namespace ns3 {

 NS_LOG_COMPONENT_DEFINE ("FBNode");

 NS_OBJECT_ENSURE_REGISTERED (FBNode);

 /**
  * \brief A global switch to enable all checksums for all protocols.
  */
 static GlobalValue g_checksumEnabled  = GlobalValue ("ChecksumEnabled",
                                                      "A global switch to enable all checksums for all protocols",
                                                      BooleanValue (false),
                                                      MakeBooleanChecker ());

	FBNode::FBNode()
	  : Node(),
			CMFR (0),
			LMFR (0),
			CMBR (0),
			LMBR (0)
	{
	  NS_LOG_FUNCTION (this);
	}

	FBNode::~FBNode ()
	{
	  NS_LOG_FUNCTION (this);
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

	void
	FBNode::SetCMFR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this);
	  m_CMFR = value;
	}

	void
	FBNode::SetLMFR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this);
	  m_LMFR = value;
	}

	void
	FBNode::SetCMBR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this);
	  m_CMBR = value;
	}

	void
	FBNode::SetLMBR (uint32_t value)
	{
	  NS_LOG_FUNCTION (this);
	  m_LMBR = value;
	}

} // namespace ns3
