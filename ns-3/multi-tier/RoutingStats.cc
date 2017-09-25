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

#include "ns3/core-module.h"

#include "RoutingStats.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RoutingStats");

NS_OBJECT_ENSURE_REGISTERED (RoutingStats);

TypeId RoutingStats::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::RoutingStats")
	 .SetParent<Object> ()
	 .AddConstructor<RoutingStats> ();

	 return tid;
 }

RoutingStats::RoutingStats ()
  : m_RxBytes (0),
    m_cumulativeRxBytes (0),
    m_RxPkts (0),
    m_cumulativeRxPkts (0),
    m_TxBytes (0),
    m_cumulativeTxBytes (0),
    m_TxPkts (0),
    m_cumulativeTxPkts (0)
{
}

RoutingStats::~RoutingStats ()
{
	NS_LOG_FUNCTION (this);
}

uint32_t
RoutingStats::GetRxBytes ()
{
  return m_RxBytes;
}

uint32_t
RoutingStats::GetCumulativeRxBytes ()
{
  return m_cumulativeRxBytes;
}

uint32_t
RoutingStats::GetRxPkts ()
{
  return m_RxPkts;
}

uint32_t
RoutingStats::GetCumulativeRxPkts ()
{
  return m_cumulativeRxPkts;
}

void
RoutingStats::IncRxBytes (uint32_t rxBytes)
{
  m_RxBytes += rxBytes;
  m_cumulativeRxBytes += rxBytes;
}

void
RoutingStats::IncRxPkts ()
{
  m_RxPkts++;
  m_cumulativeRxPkts++;
}

void
RoutingStats::SetRxBytes (uint32_t rxBytes)
{
  m_RxBytes = rxBytes;
}

void
RoutingStats::SetRxPkts (uint32_t rxPkts)
{
  m_RxPkts = rxPkts;
}

uint32_t
RoutingStats::GetTxBytes ()
{
  return m_TxBytes;
}

uint32_t
RoutingStats::GetCumulativeTxBytes ()
{
  return m_cumulativeTxBytes;
}

uint32_t
RoutingStats::GetTxPkts ()
{
  return m_TxPkts;
}

uint32_t
RoutingStats::GetCumulativeTxPkts ()
{
  return m_cumulativeTxPkts;
}

void
RoutingStats::IncTxBytes (uint32_t txBytes)
{
  m_TxBytes += txBytes;
  m_cumulativeTxBytes += txBytes;
}

void
RoutingStats::IncTxPkts ()
{
  m_TxPkts++;
  m_cumulativeTxPkts++;
}

void
RoutingStats::SetTxBytes (uint32_t txBytes)
{
  m_TxBytes = txBytes;
}

void
RoutingStats::SetTxPkts (uint32_t txPkts)
{
  m_TxPkts = txPkts;
}
}
