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
	 .AddConstructor<Tier> ();

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
	m_dataStartTime (0),
	m_totalSimTime (0)
{
	NS_LOG_FUNCTION (this);
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
  ProcessOutputs ();
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

	switch (m_lossModel) {
		case 1:	m_lossModelName = "ns3::FriisPropagationLossModel";
						break;
		case 2:	m_lossModelName = "ns3::ItuR1411LosPropagationLossModel";
						break;
		case 3:	m_lossModelName = "ns3::TwoRayGroundPropagationLossModel";
						break;
		case 4:	m_lossModelName = "ns3::LogDistancePropagationLossModel";
						break;
		default:	 NS_LOG_ERROR ("Invalid propagation loss model specified. Values must be [1-4], where 1=Friis;2=ItuR1411Los;3=TwoRayGround;4=LogDistance");
	}

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

	if (m_mobility == 1)
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
}

void
Tier::SetupRoutingMessages ()
{
	NS_LOG_FUNCTION (this);

	m_routingHelper->Install (m_nodes,
														m_devices,
														m_interfaces,
														m_totalSimTime,
														m_protocol,
														m_nNodes);
}
} // namespace ns3
