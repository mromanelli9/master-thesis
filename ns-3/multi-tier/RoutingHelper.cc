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
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"

#include "RoutingHelper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RoutingHelper");

NS_OBJECT_ENSURE_REGISTERED (RoutingHelper);

TypeId
RoutingHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RoutingHelper")
    .SetParent<Object> ()
    .AddConstructor<RoutingHelper> ();
  return tid;
}

RoutingHelper::RoutingHelper ()
  : m_dataStartTime (1.0),
		m_TotalSimTime (300.01),
    m_protocol (0),
    m_port (9),
    m_nSinks (0)
{
	NS_LOG_FUNCTION (this);
}

RoutingHelper::~RoutingHelper ()
{
	NS_LOG_FUNCTION (this);
}

void
RoutingHelper::Install (NodeContainer & c,
                        NetDeviceContainer & d,
                        Ipv4InterfaceContainer & i,
												double startTime,
                        double totalTime,
                        int protocol,
                        uint32_t nSinks)
{
	NS_LOG_FUNCTION (this << &c << &d << &i << totalTime << protocol << nSinks);

	m_dataStartTime = startTime;
  m_TotalSimTime = totalTime;
  m_protocol = protocol;
  m_nSinks = nSinks;

  SetupRoutingProtocol (c);
  AssignIpAddresses (d, i);
  SetupRoutingMessages (c, i);
}

Ptr<Socket>
RoutingHelper::SetupRoutingPacketReceive (Ipv4Address addr, Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << addr << node);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, m_port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback (&RoutingHelper::ReceiveRoutingPacket, this));

  return sink;
}

void
RoutingHelper::SetupRoutingProtocol (NodeContainer & c)
{
	NS_LOG_FUNCTION (this << &c);

  AodvHelper aodv;
  OlsrHelper olsr;
  DsdvHelper dsdv;
  DsrHelper dsr;
  DsrMainHelper dsrMain;
  Ipv4ListRoutingHelper list;
  InternetStackHelper internet;

  Time rtt = Time (5.0);
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> rtw = ascii.CreateFileStream ("routing_table");

  switch (m_protocol)
    {
    case 0:
      m_protocolName = "NONE";
      break;
    case 1:
      list.Add (olsr, 100);
      m_protocolName = "OLSR";
      break;
    case 2:
      list.Add (aodv, 100);
      m_protocolName = "AODV";
      break;
    case 3:
      list.Add (dsdv, 100);
      m_protocolName = "DSDV";
      break;
    case 4:
      // setup is later
      m_protocolName = "DSR";
      break;
    default:
      NS_FATAL_ERROR ("No such protocol:" << m_protocol);
      break;
    }

  if (m_protocol < 4)
    {
      internet.SetRoutingHelper (list);
      internet.Install (c);
    }
  else if (m_protocol == 4)
    {
      internet.Install (c);
      dsrMain.Install (dsr, c);
    }

	NS_LOG_DEBUG ("Routing Setup for " << m_protocolName << ".");
}

void
RoutingHelper::AssignIpAddresses (NetDeviceContainer & d,
                                  Ipv4InterfaceContainer & adhocTxInterfaces)
{
	NS_LOG_FUNCTION (this << &d << &adhocTxInterfaces);

  NS_LOG_DEBUG ("Assigning IP addresses.");
  Ipv4AddressHelper addressAdhoc;
  // we may have a lot of nodes, and want them all
  // in same subnet, to support broadcast
  addressAdhoc.SetBase ("10.1.0.0", "255.255.0.0");
  adhocTxInterfaces = addressAdhoc.Assign (d);
}

void
RoutingHelper::SetupRoutingMessages (NodeContainer & c,
                                     Ipv4InterfaceContainer & adhocTxInterfaces)
{
	NS_LOG_FUNCTION (this << &c << &adhocTxInterfaces);

	// protocol == 0 means no routing data, so do not set up sink
	if (m_protocol != 0)
		for (uint32_t i = 0; i < m_nSinks; i++)
		{
			Ptr<Socket> sink = SetupRoutingPacketReceive (adhocTxInterfaces.GetAddress (i), c.Get (i));
		}

	for (uint32_t i = 0; i < c.GetN (); i++)
	{
		for (uint32_t j = 0; j < m_nSinks; j++)
		{
			// Setup routing transmissions
			OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (adhocTxInterfaces.GetAddress (j), m_port)));
			onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
			onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
			AddressValue remoteAddress (InetSocketAddress (adhocTxInterfaces.GetAddress (i), m_port));
      onoff1.SetAttribute ("Remote", remoteAddress);

			// Skip the same node
			if (i == j)
				continue;

				ApplicationContainer apps1 = onoff1.Install (c.Get (i));
				Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
				apps1.Start (Seconds (var->GetValue (m_dataStartTime, m_dataStartTime + 1)));
				apps1.Stop (Seconds (m_TotalSimTime));

		}
	}
}

void
RoutingHelper::ReceiveRoutingPacket (Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address srcAddress;
  while ((packet = socket->RecvFrom (srcAddress)))
  {
		// application data, for goodput
		uint32_t RxRoutingBytes = packet->GetSize ();
		GetRoutingStats ().IncRxBytes (RxRoutingBytes);
		GetRoutingStats ().IncRxPkts ();

		uint32_t nodeId = socket->GetNode()->GetId ();
    NS_LOG_DEBUG ("Node " << nodeId << " received packet "<< packet->GetUid () << " from " << srcAddress << ".");
  }
}

void
RoutingHelper::OnOffTrace (std::string context, Ptr<const Packet> packet)
{
	NS_LOG_FUNCTION (this << context << packet);

  uint32_t pktBytes = packet->GetSize ();
  routingStats.IncTxBytes (pktBytes);

	uint32_t nodeId = Simulator::GetContext ();
	NS_LOG_DEBUG ("Node " << nodeId << " sent packet " << packet->GetUid () << ".");
}

RoutingStats &
RoutingHelper::GetRoutingStats ()
{
	NS_LOG_FUNCTION (this);

  return routingStats;
}
}
