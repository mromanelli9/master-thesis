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
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wave-helper.h"
#include "ns3/itu-r-1411-los-propagation-loss-model.h"

#include "Tier.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Tier");

NS_OBJECT_ENSURE_REGISTERED (Tier);

TypeId
Tier::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::Tier")
	 .SetParent<Object> ()
	 .AddConstructor<Tier> ()
	 .AddAttribute ("Nodes", "Number of nodes.",
									 UintegerValue (0),
									 MakeUintegerAccessor (&Tier::m_nNodes),
									 MakeUintegerChecker<uint32_t> ())
	 .AddAttribute ("PacketSize", "The size of the packets in the OnOffApplication.",
									StringValue ("64"),
									MakeStringAccessor (&Tier::m_packetSize),
									MakeStringChecker ())
	 .AddAttribute ("DataRate", "The data rate in the OnOffApplication.",
									StringValue ("2048bps"),
									MakeStringAccessor (&Tier::m_rate),
									MakeStringChecker ())
	 .AddAttribute ("RoutingProtocol", "The routing protocol (NONE, OLSR, AODV, DSDV, DSR).",
									 StringValue ("DSDV"),
									 MakeStringAccessor (&Tier::m_protocolName),
									 MakeStringChecker ())
	 .AddAttribute ("PhyMode", "Wifi Phy mode.",
									 StringValue ("OfdmRate6MbpsBW10MHz"),
									 MakeStringAccessor (&Tier::m_phyMode),
									 MakeStringChecker ())
	 .AddAttribute ("80211mode", "802.11* mode [1=802.11p; 2=802.11b; 3=WAVE-PHY].",
									 UintegerValue (1),
									 MakeUintegerAccessor (&Tier::m_80211mode),
									 MakeUintegerChecker<uint32_t> ())
	 .AddAttribute ("PhyModeB", "Phy mode for 802.11b.",
 									StringValue ("DsssRate11Mbps"),
 									MakeStringAccessor (&Tier::m_phyModeB),
 									MakeStringChecker ())
	 .AddAttribute ("PropagationLossModel", "Propagation loss model name [Friis;ItuR1411Los;TwoRayGround;LogDistance].",
									 StringValue ("ns3::FriisPropagationLossModel"),
									 MakeStringAccessor (&Tier::m_lossModelName),
									 MakeStringChecker ())
	 .AddAttribute ("Buildings", "Enable ObstacleShadowingPropagationLossModel.",
									 UintegerValue (0),
									 MakeUintegerAccessor (&Tier::m_loadBuildings),
									 MakeUintegerChecker<uint32_t> ())
	 .AddAttribute ("Txp", "Transmit power [dB].",
	 								UintegerValue (20),
	 								MakeUintegerAccessor (&Tier::m_txp),
	 								MakeUintegerChecker<uint32_t> ())
	 .AddAttribute ("Mobility", "Mobility mode.",
									 UintegerValue (1),
									 MakeUintegerAccessor (&Tier::m_mobility),
									 MakeUintegerChecker<uint32_t> ())
	 .AddAttribute ("TraceFile", "ns2 trace file path.",
									 StringValue (""),
									 MakeStringAccessor (&Tier::m_traceFile),
									 MakeStringChecker ())
	 .AddAttribute ("PositionAllocator", "Position allocator.",
										PointerValue (0),
										MakePointerAccessor (&Tier::m_positionAllocator),
										MakePointerChecker<PositionAllocator> ())
	 .AddAttribute ("NetworkAddress", "The initial network number to use during allocation.",
	 									Ipv4AddressValue ("10.1.0.0"),
	 									MakeIpv4AddressAccessor (&Tier::m_networkAddress),
	 									MakeIpv4AddressChecker ())
	 .AddAttribute ("NetworkMask", "The network mask.",
									Ipv4MaskValue ("255.255.0.0"),
									MakeIpv4MaskAccessor (&Tier::m_networkMask),
									MakeIpv4MaskChecker ())
	 .AddAttribute ("NetworkBase", "The initial address used for IP address allocation.",
								 Ipv4AddressValue ("0.0.0.1"),
								 MakeIpv4AddressAccessor (&Tier::m_networkBase),
								 MakeIpv4AddressChecker ())
	 .AddAttribute ("DataStartTime", "Time at which nodes start to transmit data [seconds],",
										 DoubleValue (1),
										 MakeDoubleAccessor (&Tier::m_dataStartTime),
										 MakeDoubleChecker<double> ())
	 .AddAttribute ("TotalSimTime", "Simulation end time [seconds].",
										 DoubleValue (1),
										 MakeDoubleAccessor (&Tier::m_totalSimTime),
										 MakeDoubleChecker<double> ());

	 return tid;
 }

Tier::Tier ()
:	m_nNodes (0),
	m_packetSize ("64"),
	m_rate ("2048bps"),
	m_protocol (3),
	m_protocolName ("DSDV"),
	m_phyMode ("OfdmRate6MbpsBW10MHz"),
	m_80211mode (1),
	m_phyModeB ("DsssRate11Mbps"),
	m_lossModel (1),
	m_lossModelName ("ns3::FriisPropagationLossModel"),
	m_loadBuildings (0),
	m_txp (20),
	m_mobility (1),
	m_traceFile (""),
	m_networkAddress ("10.1.0.0"),
	m_networkMask ("255.255.0.0"),
	m_networkBase ("0.0.0.1"),
	m_dataStartTime (1.0),
	m_totalSimTime (0.0)
{
	NS_LOG_FUNCTION (this);

	m_routingHelper = CreateObject<RoutingHelper> ();
}

Tier::~Tier ()
{
	NS_LOG_FUNCTION (this);
}

void
Tier::Install ()
{
	NS_LOG_FUNCTION (this);

	ConfigureDefaults ();
  ConfigureNodes ();
  ConfigureChannels ();
  ConfigureMobility ();
  ConfigureApplications ();
  ConfigureTracing ();

	// Schedule ProcessOutputs after simulation
	Simulator::ScheduleDestroy (&Tier::ProcessOutputs, this);
}

void
Tier::ConfigureDefaults ()
{
	NS_LOG_FUNCTION (this);

	Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue (m_packetSize));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (m_rate));

	//Set Non-unicastMode rate to unicast mode (2=802.11b)
	if (m_80211mode == 2)
		Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (m_phyModeB));
	else
		Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (m_phyMode));
}

void
Tier::ConfigureNodes ()
{
	NS_LOG_FUNCTION (this);

	m_nodes.Create (m_nNodes);
}

void
Tier::ConfigureChannels ()
{
	NS_LOG_FUNCTION (this);

	if (m_lossModelName == "ns3::FriisPropagationLossModel")
		m_lossModel = 1;
	else if (m_lossModelName == "ns3::ItuR1411LosPropagationLossModel")
		m_lossModel = 2;
	else if (m_lossModelName == "ns3::TwoRayGroundPropagationLossModel")
		m_lossModel = 3;
	else if (m_lossModelName == "ns3::LogDistancePropagationLossModel")
		m_lossModel = 4;
	else
		NS_FATAL_ERROR ("Invalid propagation loss model specified. Values must be [1-4], where 1=Friis;2=ItuR1411Los;3=TwoRayGround;4=LogDistance");

	// frequency
	double freq = 0.0;
	if ((m_80211mode == 1) || (m_80211mode == 3))
	{
		// 802.11p 5.9 GHz
		freq = 5.9e9;
	}
	else
	{
		// 802.11b 2.4 GHz
		freq = 2.4e9;
	}

	// Setup propagation models
	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	if (m_lossModel == 3)
	{
		// two-ray requires antenna height (else defaults to Friss)
		wifiChannel.AddPropagationLoss (m_lossModelName, "Frequency", DoubleValue (freq), "HeightAboveZ", DoubleValue (1.5));
	}
	else
		wifiChannel.AddPropagationLoss (m_lossModelName, "Frequency", DoubleValue (freq));

	// If Obstacle modeling is included, then we add obstacle-shadowing
	if (m_loadBuildings != 0)
		wifiChannel.AddPropagationLoss ("ns3::ObstacleShadowingPropagationLossModel");

	// the channel
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();

	// The below set of helpers will help us to put together the wifi NICs we want
	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	wifiPhy.SetChannel (channel);
	// ns-3 supports generate a pcap trace
	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

	YansWavePhyHelper wavePhy =  YansWavePhyHelper::Default ();
	wavePhy.SetChannel (channel);
	wavePhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

	// Setup WAVE PHY and MAC
	NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
	WaveHelper waveHelper = WaveHelper::Default ();
	Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();

	WifiHelper wifi;

	// Setup 802.11b stuff
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																"DataMode",StringValue (m_phyModeB),
																"ControlMode",StringValue (m_phyModeB));

	// Setup 802.11p stuff
	wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																			"DataMode",StringValue (m_phyMode),
																			"ControlMode",StringValue (m_phyMode));

	// Setup WAVE-PHY stuff
	waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																			"DataMode",StringValue (m_phyMode),
																			"ControlMode",StringValue (m_phyMode));

	// Set Tx Power
	wifiPhy.Set ("TxPowerStart",DoubleValue (m_txp));
	wifiPhy.Set ("TxPowerEnd", DoubleValue (m_txp));
	wavePhy.Set ("TxPowerStart",DoubleValue (m_txp));
	wavePhy.Set ("TxPowerEnd", DoubleValue (m_txp));

	// Add an upper mac and disable rate control
	WifiMacHelper wifiMac;
	wifiMac.SetType ("ns3::AdhocWifiMac");
	QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();

	// Setup net devices
	if (m_80211mode == 1)
		m_devices = wifi80211p.Install (wifiPhy, wifi80211pMac, m_nodes);
	else if (m_80211mode == 3)
		m_devices = waveHelper.Install (wavePhy, waveMac, m_nodes);
	else
		m_devices = wifi.Install (wifiPhy, wifiMac, m_nodes);
}

void
Tier::ConfigureMobility ()
{
	NS_LOG_FUNCTION (this);

	// DEBUG
	if (m_mobility == 0)
	{
		// if not specified, choose randomly
		if (m_positionAllocator == 0)
		{
			ObjectFactory pos;
			pos.SetTypeId ("ns3::RandomBoxPositionAllocator");
			pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"));
			pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"));
			pos.Set ("Z", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=0.0]"));

			m_positionAllocator = pos.Create ()->GetObject<PositionAllocator> ();
		}

		MobilityHelper mobility;
		mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		mobility.SetPositionAllocator (m_positionAllocator);
		mobility.Install (m_nodes);
	}
	else if (m_mobility == 1)
	{
		// Check if m_traceFile exists
		NS_ASSERT_MSG (m_traceFile != "", "Invalid ns2 trace file specified.");

		// Create Ns2MobilityHelper with the specified trace log file as parameter
		Ns2MobilityHelper ns2 = Ns2MobilityHelper (m_traceFile);

		// configure movements for each node, while reading trace file
		ns2.Install ();
	}
	else
		NS_LOG_ERROR ("Invalid mobility model specified. Values must be [1].");
}

void
Tier::ConfigureApplications ()
{
	NS_LOG_FUNCTION (this);

	SetupRoutingMessages ();
}

void
Tier::ConfigureTracing ()
{
	NS_LOG_FUNCTION (this);

	Packet::EnablePrinting ();

	// Setup logging
	LogComponentEnable ("RoutingHelper", LOG_LEVEL_DEBUG);
	LogComponentEnable ("RoutingStats", LOG_LEVEL_DEBUG);
}

void
Tier::ProcessOutputs ()
{
	NS_LOG_FUNCTION (this);

	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		Ptr<Node> node = m_nodes.Get (i);
		Ptr<MobilityModel> positionmodel = node->GetObject<MobilityModel> ();
		std::cout << "nodo " << node->GetId () << ", pos " << positionmodel->GetPosition () << std::endl;
	}
}

void
Tier::SetupRoutingMessages ()
{
	NS_LOG_FUNCTION (this);

	if (m_protocolName == "NONE")
		m_protocol = 0;
	else if (m_protocolName == "OLSR")
			m_protocol = 1;
	else if (m_protocolName == "AODV")
			m_protocol = 2;
	else if (m_protocolName == "DSDV")
			m_protocol = 3;
	else if (m_protocolName == "DSR")
			m_protocol = 4;
	else
		NS_FATAL_ERROR ("No such protocol:" << m_protocolName);

	m_routingHelper->Install (m_nodes,
														m_devices,
														m_interfaces,
														m_dataStartTime,
														m_totalSimTime,
														m_protocol,
														m_networkAddress,
														m_networkMask,
														m_networkBase,
														m_nNodes);
	Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::OnOffApplication/Tx",
									MakeCallback (&RoutingHelper::OnOffTrace, m_routingHelper));
}
} // namespace ns3
