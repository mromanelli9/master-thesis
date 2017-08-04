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

 namespace ns3 {

 NS_LOG_COMPONENT_DEFINE ("FBNode");

 NS_OBJECT_ENSURE_REGISTERED (FBNode);

 TypeId FBNode::GetTypeId (void)
 {
	 static TypeId tid = TypeId ("ns3::FBNode")
	 	.SetParent<Node> ()
		.SetGroupName ("Network")
		.AddConstructor<FBNode> ()
		.AddAttribute ("CMFR", "The value for the Current Maximum Front Range of this node.",
									 TypeId::ATTR_GET || TypeId::ATTR_SET,
									 UintegerValue (0),
									 MakeUintegerAccessor (&FBNode::m_CMFR),
									 MakeUintegerChecker<uint32_t> ())
		.AddAttribute ("LMFR", "The value for the Last Maximum Front Range of this node.",
										TypeId::ATTR_GET || TypeId::ATTR_SET,
										 UintegerValue (0),
										 MakeUintegerAccessor (&FBNode::m_LMFR),
										 MakeUintegerChecker<uint32_t> ())
		.AddAttribute ("CMBR", "The value for the Current Maximum Back Range of this node.",
										TypeId::ATTR_GET || TypeId::ATTR_SET,
										UintegerValue (0),
										MakeUintegerAccessor (&FBNode::m_CMBR),
										MakeUintegerChecker<uint32_t> ())
		.AddAttribute ("LMBR", "The value for the Last Maximum Back Range of this node.",
									 TypeId::ATTR_GET || TypeId::ATTR_SET,
									 UintegerValue (0),
									 MakeUintegerAccessor (&FBNode::m_LMBR),
									 MakeUintegerChecker<uint32_t> ());

	  return tid;
	}

	FBNode::FBNode()
	  : m_CMFR (0),
			m_LMFR (0),
			m_CMBR (0),
			m_LMBR (0)
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
