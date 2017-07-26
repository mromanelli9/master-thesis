/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
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
 * Author: Marco Romanelli <marco.romanelli.1@studenti.unipd.it>
 *
 */

/*
* Class Diagram:
*   main()
*     +--uses-- FBVanetExperiment
*
*/

#include <fstream>
#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("fb-vanet");

/**
 * \brief The VanetRoutingExperiment class implements an application that
 * allows this VANET experiment to be simulated
 */
class FBVanetExperiment
{
public:
	/**
	 * \brief Constructor
	 * \return none
	 */
	FBVanetExperiment ();

	/**
	 * \brief Destructor
	 * \return none
	 */
	virtual ~FBVanetExperiment ();

	/**
	 * \brief Enacts simulation of an ns-3  application
	 * \param argc program arguments count
	 * \param argv program arguments
	 * \return none
	 */
	void Simulate (int argc, char **argv);

protected:
	/**
	 * \brief Process command line arguments
	 * \param argc program arguments count
	 * \param argv program arguments
	 * \return none
	 */
	void ParseCommandLineArguments (int argc, char **argv);

	/**
	 * \brief Configure default attributes
	 * \return none
	 */
	void ConfigureDefaults ();

	/**
	 * \brief Configure nodes
	 * \return none
	 */
	void ConfigureNodes ();

	/**
	 * \brief Configure devices
	 * \return none
	 */
	void ConfigureDevices ();

	/**
	 * \brief Configure mobility
	 * \return none
	 */
	void ConfigureMobility ();

	/**
	 * \brief Set up the adhoc devices
	 * \return none
	 */
	void SetupAdhocDevices ();

	/**
	 * \brief Configure connections
	 * \return none
	 */
	void ConfigureConnections ();

	/**
	 * \brief Set up Fast Broadcast protocol
	 * \return none
	 */
	void SetupFBProtocol ();

	/**
	 * \brief Configure tracing and logging
	 * \return none
	 */
	void ConfigureTracingAndLogging ();

	/**
	 * \brief Schedule FB protocol phases
	 * \return none
	 */
	void ScheduleFBProtocol ();

	/**
	 * \brief Run the simulation
	 * \return none
	 */
	void RunSimulation ();

	/**
	 * \brief Process outputs
	 * \return none
	 */
	void ProcessOutputs ();

private:
	/**
	 * \brief Run the simulation
	 * \return none
	 */
	void Run ();

	/**
	 * \brief Run the simulation
	 * \return none
	 */
	void CommandSetup (int argc, char **argv);

	/**
	 * \brief Set up a prescribed scenario
	 * \return none
	 */
	void SetupScenario ();

	/**
	 * \brief Set up Fast Broadcast protocol parameters for a node
	 * \parm node node to configure
	 * \return none
	 */
	void SetupFBParameters (Ptr<Node> node);

	double									m_txp;
	uint32_t 								m_nNodes;
	NodeContainer						m_adhocNodes;
	NetDeviceContainer			m_adhocDevices;
	Ipv4InterfaceContainer	m_adhocInterfaces;
	uint32_t								m_actualRange;
	uint32_t								m_estimatedRange;
	bool										m_flooding;
	uint32_t								m_rCirc;
	std::string 						m_rate;
	std::string 						m_phyMode;
	uint32_t 								m_mobility;
	uint32_t 								m_scenario;
	std::vector <uint32_t>	m_lineNodePositions;
	std::string							m_address;
	uint32_t								m_port;
	std::string 						m_CSVfileName;
	double									m_TotalSimTime;
};

FBVanetExperiment::FBVanetExperiment ()
	: m_txp (7.5),
		m_nNodes (10),
		m_actualRange (300),
		m_estimatedRange (0),
		m_flooding (true),
		m_rCirc (1000),
		m_rate ("2048bps"),
		m_phyMode ("DsssRate11Mbps"),
		m_mobility (1),
		m_scenario (1),
		m_address ("10.1.255.255"),
		m_port (9),
		m_CSVfileName ("manet-routing.output.csv"),
		m_TotalSimTime (300.01)
{
	srand (time (0));
}

FBVanetExperiment::~FBVanetExperiment ()
{
}


void
FBVanetExperiment::Simulate (int argc, char **argv)
{
	NS_LOG_INFO ("Enter 'FB Vanet Experiment' enviroment.");

	// Initial configuration and parameters parsing
	ParseCommandLineArguments (argc, argv);
	ConfigureDefaults ();

	// Configure the network and all the elements in it
	ConfigureNodes ();
	ConfigureMobility ();
	SetupAdhocDevices ();
	ConfigureConnections ();
	SetupFBProtocol ();
	ConfigureTracingAndLogging ();

	// Schedule FB protocol phases
	ScheduleFBProtocol ();

	// Run simulation and print some results
	RunSimulation ();
	ProcessOutputs ();
}

void
FBVanetExperiment::ConfigureDefaults ()
{

	Config::SetDefault ("ns3::OnOffApplication::PacketSize",StringValue ("64"));
	Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (m_rate));
	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (m_phyMode));
}

void
FBVanetExperiment::ParseCommandLineArguments (int argc, char **argv)
{
	CommandSetup (argc, argv);
	SetupScenario ();
}

void
FBVanetExperiment::ConfigureNodes ()
{
	m_adhocNodes.Create (m_nNodes);
}

void
FBVanetExperiment::ConfigureMobility ()
{
	NS_LOG_INFO ("Configure mobility.");

	if (m_mobility == 1)
	{
		MobilityHelper mobility;
		Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

		// I don't like put that in here, but dunno where.
		// Set nodes position
		for (uint32_t i = 0 ; i < m_nNodes; i++)
		{
			positionAlloc->Add (Vector (m_lineNodePositions[i], 0.0, 0.0));
		}

		// Install nodes in a constant velocity mobility model
		mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
		mobility.SetPositionAllocator (positionAlloc);
		mobility.Install (m_adhocNodes);

		// Set the velocity value (constant) to zero
		for (uint32_t i = 0 ; i < m_nNodes; i++)
		{
			Ptr<ConstantVelocityMobilityModel> mob = m_adhocNodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
			mob->SetVelocity (Vector(0, 0, 0));
		}
	}
	else
	{
		NS_LOG_ERROR ("Mobility 2 is not implemented yet.");
	}
}

void
FBVanetExperiment::SetupAdhocDevices ()
{
	NS_LOG_INFO ("Configure channels.");

	// Setting up wifi phy and channel using helpers
	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

	//Setting max range
	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (m_actualRange + 100));
	wifiPhy.SetChannel (wifiChannel.Create ());

	//Add a mac and disable rate control
	WifiMacHelper wifiMac;
	//NqosWifiMacHelper wifiMac=NqosWifiMacHelper::Default();
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																"DataMode",StringValue (m_phyMode),
																"ControlMode",StringValue (m_phyMode));
	wifiPhy.Set ("TxPowerStart",DoubleValue (m_txp));
	wifiPhy.Set ("TxPowerEnd", DoubleValue (m_txp));
	wifiMac.SetType ("ns3::AdhocWifiMac");
	m_adhocDevices = wifi.Install (wifiPhy, wifiMac, m_adhocNodes);
}

void
FBVanetExperiment::ConfigureConnections ()
{
	NS_LOG_INFO ("Configure Internet stack.");

	InternetStackHelper internet;
	internet.Install (m_adhocNodes);

	Ipv4AddressHelper addressAdhoc;
	addressAdhoc.SetBase ("10.1.0.0", "255.255.0.0");	// TODO: make them variable

	m_adhocInterfaces = addressAdhoc.Assign (m_adhocDevices);

	OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address ());
	onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
	onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

	NS_LOG_INFO ("Configure connections.");

	// Set receiver (for each node)
	for (uint32_t j = 0; j < m_nNodes; j++)
	{
		//Ptr<Socket> sink = SetupPacketReceive (m_adhocInterfaces.GetAddress (j), m_adhocNodes.Get (j));	// TODO: add SetupPacketReceive function
		AddressValue remoteAddress (InetSocketAddress (ns3::Ipv4Address::GetAny (), m_port));
		onoff1.SetAttribute ("Remote", remoteAddress);
	}

	// Set unicast sender
	// TODO: add SetupPacketSend function
	// for (uint32_t i = 0; i < m_nNodes; i++)
	// {
	// 	Ptr<Socket> sender = SetupPacketSend (ns3::Ipv4Address(m_address.c_str ()),  m_adhocNodes.Get (i));
	// 	sender->SetAllowBroadcast (true);
	//
	// 	// Add socket to the node
	// 	m_adhocNodes.Get(i)->setBroadcast(sender);
	// }
}

void
FBVanetExperiment::SetupFBProtocol ()
{
	// Configure FB protocol parameters for all nodes
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		SetupFBParameters (m_adhocNodes.Get (i));
	}
}

void
FBVanetExperiment::ConfigureTracingAndLogging ()
{
	// Enable logging from the ns2 helper
	LogComponentEnable ("Ns2MobilityHelper", LOG_LEVEL_DEBUG);

	Packet::EnablePrinting ();
}

void
FBVanetExperiment::ScheduleFBProtocol ()
{
	// TODO
	// // Hello messages
	// Simulator::Schedule (Seconds (500), &RoutingExperiment::Hello, adhocNodes, 60);
	//
	// // Generate alert message
	// Simulator::ScheduleWithContext (adhocNodes.Get (start)->GetId (), Seconds (45000), &RoutingExperiment::GenerateAlertTraffic, adhocNodes.Get (start), adhocNodes);
}

void
FBVanetExperiment::CommandSetup (int argc, char **argv)
{
	CommandLine cmd;

	// allow command line overrides
	cmd.AddValue ("txp", "Transmit power (dB), e.g. txp=7.5", m_txp);
	cmd.AddValue ("nodes", "Number of nodes (i.e. vehicles)", m_nNodes);
	cmd.AddValue ("actualRange", "Actual transimision range (meters)", m_actualRange);
	// cmd.AddValue ("estimatedRange", "Estimated transimision range (meters)", m_estimatedRange);	// disabled for now
	cmd.AddValue ("flooding", "Enable flooding", m_flooding);
	cmd.AddValue ("rCirc", "Vehicles transimision area" , m_rCirc);
	cmd.AddValue ("rate", "Rate", m_rate);
	cmd.AddValue ("phyModeB", "Phy mode 802.11b", m_phyMode);
	cmd.AddValue ("mobility", "Node mobility: 1=stationary, 2=moving", m_mobility);
	// cmd.AddValue ("speed", "Node speed (m/s)", m_nodeSpeed);
	cmd.AddValue ("scenario", "1=straight street, 2=grid layout, 3=real world", m_scenario);
	cmd.AddValue ("address", "IP address", m_address);
	cmd.AddValue ("port", "IP port", m_port);
	cmd.AddValue ("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
	cmd.AddValue ("totaltime", "Simulation end time", m_TotalSimTime);

	cmd.Parse (argc, argv);
}

void
FBVanetExperiment::SetupScenario ()
{
	NS_LOG_INFO ("Setup current scenario.");

	if (m_scenario == 1)
	{
		m_mobility = 1;
		m_nNodes = 10;

		// Node positions (in meters) along the straight street (or line)
		m_lineNodePositions.resize (10, 0);
		m_lineNodePositions[0] = 100;
		m_lineNodePositions[1] = 300;
		m_lineNodePositions[2] = 500;
		m_lineNodePositions[3] = 700;
		m_lineNodePositions[4] = 900;
		m_lineNodePositions[5] = 1000;
		m_lineNodePositions[6] = 1100;
		m_lineNodePositions[7] = 1500;
		m_lineNodePositions[8] = 1700;
		m_lineNodePositions[9] = 2000;
	}
	else if (m_scenario == 2)
	{
		m_mobility = 1;
		NS_LOG_ERROR ("Scenario 3 is not implemented yet.");
	}
	else if (m_scenario == 3)
	{
		m_mobility = 2;
		NS_LOG_ERROR ("Scenario 3 is not implemented yet.");
	}
}

void
FBVanetExperiment::SetupFBParameters (Ptr<Node> node)
{
	node->SetCMFR (m_estimatedRange);
	node->SetLMFR (m_estimatedRange);
	node->SetCMBR (m_estimatedRange);
	node->SetLMBR (m_estimatedRange);
	node->SetNum (0);
	node->SetPhase (-1);
	node->SetSent (false);
	node->SetReceived (false);
	node->SetSlot (0);
}

void
FBVanetExperiment::RunSimulation ()
{
	Run ();
}

void
FBVanetExperiment::ProcessOutputs ()
{
	NS_LOG_INFO ("Process outputs.");

	// TODO
}

void
FBVanetExperiment::Run ()
{
	NS_LOG_INFO ("Run simulation.");

	// Setup netanim config ?
	Simulator::Stop (Seconds (m_TotalSimTime));
	Simulator::Run ();
	Simulator::Destroy ();
}


int main (int argc, char *argv[])
{
	FBVanetExperiment experiment;
	experiment.Simulate (argc, argv);
}
