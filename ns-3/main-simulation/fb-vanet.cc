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

/* -----------------------------------------------------------------------------
*			HEADERS
* ------------------------------------------------------------------------------
*/

#include <fstream>
#include <iostream>

#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/topology.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/netanim-module.h"

#include "FBApplication.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("fb-vanet");

/* -----------------------------------------------------------------------------
*			CLASS AND METHODS PROTOTIPES
* ------------------------------------------------------------------------------
*/

/**
 * \ingroup obstacle
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
	 * \brief Configure tracing and logging
	 * \return none
	 */
	void ConfigureTracingAndLogging ();

	/**
	 * \brief Configure the FB application
	 * \return none
	 */
	void ConfigureFBApplication ();

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
	 * \brief Set up receivers socket
	 * \param node node to configure
	 * \return socket created
	 */
	Ptr<Socket> SetupPacketReceive (Ptr<Node> node);

	/**
	 * \brief Set up senders socket
	 * \param addr address of the node
	 * \param node node to configure
	 * \return socket created
	 */
	Ptr<Socket> SetupPacketSend (Ipv4Address addr, Ptr<Node> node);

	/**
   * \brief Prints actual position and velocity when a course change event occurs
   * \return none
   */
	static void
	CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility);


	Ptr<FBApplication>			m_fbApplication;
	uint32_t 								m_nNodes;
	NodeContainer						m_adhocNodes;
	NetDeviceContainer			m_adhocDevices;
	Ipv4InterfaceContainer	m_adhocInterfaces;
	std::vector <Ptr<Socket>>		m_adhocSources;
	std::vector <Ptr<Socket>>		m_adhocSinks;
	std::string							m_packetSize;
	std::string							m_rate;
	std::string							m_phyMode;
	uint32_t								m_txp;
	uint32_t								m_actualRange;
	uint32_t								m_startingNode;
	uint32_t								m_staticProtocol;
	uint32_t								m_alertGeneration;
	uint32_t								m_mobility;
	uint32_t								m_scenario;
	std::vector <Vector>		m_fixNodePosition;
	uint32_t								m_loadBuildings;
	uint32_t								m_animation;
	std::string							m_traceFile;
	std::string							m_bldgFile;
	std::string							m_animationFileName;
	double									m_TotalSimTime;
};

/* -----------------------------------------------------------------------------
*			METHOD DEFINITIONS
* ------------------------------------------------------------------------------
*/

FBVanetExperiment::FBVanetExperiment ()
	:	m_nNodes (0),	// random value, it will be set later
		m_packetSize ("64"),
		m_rate ("2048bps"),
		m_phyMode ("OfdmRate6MbpsBW10MHz"),
		m_txp (20),
		m_actualRange (300),
		m_startingNode (0),
		m_staticProtocol (1),
		m_alertGeneration (20),
		m_mobility (1),
		m_scenario (1),
		m_loadBuildings (0),
		m_animation (0),
		m_traceFile (""),
		m_bldgFile (""),
		m_animationFileName ("outputs/fb-vanet-animation.xml"),
		m_TotalSimTime (30)
{
	srand (time (0));
}

FBVanetExperiment::~FBVanetExperiment ()
{
}

void
FBVanetExperiment::Simulate (int argc, char **argv)
{
	// Initial configuration and parameters parsing
	ParseCommandLineArguments (argc, argv);
	ConfigureDefaults ();

	// Configure the network and all the elements in it
	ConfigureNodes ();
	ConfigureMobility ();
	SetupAdhocDevices ();
	ConfigureConnections ();
	ConfigureTracingAndLogging ();

	// Configure FB Application
	ConfigureFBApplication ();

	// Run simulation and print some results
	RunSimulation ();
	ProcessOutputs ();
}

void
FBVanetExperiment::ConfigureDefaults ()
{
	NS_LOG_FUNCTION (this);

	Config::SetDefault ("ns3::OnOffApplication::PacketSize",StringValue (m_packetSize));
	Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (m_rate));
	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (m_phyMode));

	if (m_staticProtocol == 1)
		m_staticProtocol = PROTOCOL_FB;
	else if (m_staticProtocol == 2)
		m_staticProtocol = PROTOCOL_STATIC_300;
	else if (m_staticProtocol == 3)
		m_staticProtocol = PROTOCOL_STATIC_1000;
}

void
FBVanetExperiment::ParseCommandLineArguments (int argc, char **argv)
{
	NS_LOG_FUNCTION (this);

	CommandSetup (argc, argv);
	SetupScenario ();
}

void
FBVanetExperiment::ConfigureNodes ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Setup nodes (" << m_nNodes << ").");

	m_adhocNodes.Create (m_nNodes);
}

void
FBVanetExperiment::ConfigureMobility ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure current mobility mode (" << m_mobility << ").");

	if (m_mobility == 1)
	{
		MobilityHelper mobility;
		Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

		for (uint32_t i = 0 ; i < m_nNodes; i++)
		{
			positionAlloc->Add (m_fixNodePosition[i]);
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
	else if (m_mobility == 2)
	{
		// Create Ns2MobilityHelper with the specified trace log file as parameter
		Ns2MobilityHelper ns2 = Ns2MobilityHelper (m_traceFile);
		NS_LOG_INFO ("Loading ns2 mobility file \"" << m_traceFile << "\".");

		ns2.Install (); // configure movements for each node, while reading trace file

		// Configure callback for logging
		// std::ofstream m_os;
		// Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
		// 								 MakeBoundCallback (&FBVanetExperiment::CourseChange, &m_os));
	}
	else
		NS_LOG_ERROR ("Invalid mobility mode specified. Values must be [1-2].");
}

void
FBVanetExperiment::SetupAdhocDevices ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure channels.");

	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	//YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
	YansWifiChannelHelper wifiChannel;

	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (m_actualRange));
	// wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel", "Frequency", DoubleValue (5.9e9), "HeightAboveZ", DoubleValue (1.5));

	if (m_loadBuildings != 0)
	{
		wifiChannel.AddPropagationLoss ("ns3::ObstacleShadowingPropagationLossModel");
	}

	Ptr<YansWifiChannel> channel = wifiChannel.Create ();
	wifiPhy.SetChannel (channel);
	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

	// Set Tx Power
  wifiPhy.Set ("TxPowerStart",DoubleValue (m_txp));
  wifiPhy.Set ("TxPowerEnd", DoubleValue (m_txp));

	NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
	Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();

	wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																			"DataMode",StringValue (m_phyMode),
																			"ControlMode",StringValue (m_phyMode));
	m_adhocDevices = wifi80211p.Install (wifiPhy, wifi80211pMac, m_adhocNodes);
}

void
FBVanetExperiment::ConfigureConnections ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure connections.");

	InternetStackHelper internet;
	internet.Install (m_adhocNodes);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("10.1.0.0", "255.255.0.0");
	m_adhocInterfaces = ipv4.Assign (m_adhocDevices);

	// TODO
	OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address ());
	onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
	onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

	// Set receiver (for each node in the application)
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		SetupPacketReceive (m_adhocNodes.Get (i));
		AddressValue remoteAddress (InetSocketAddress (ns3::Ipv4Address::GetAny (), 9));
		onoff1.SetAttribute ("Remote", remoteAddress);
	}

	// Set unicast sender (for each node in the application)
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		SetupPacketSend (ns3::Ipv4Address("255.255.255.255"),  m_adhocNodes.Get (i));
	}
}

void
FBVanetExperiment::ConfigureTracingAndLogging ()
{
	NS_LOG_FUNCTION (this);

	Packet::EnablePrinting ();
}

void
FBVanetExperiment::ConfigureFBApplication ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure main application.");

	// Create the application and schedule start and end time
	m_fbApplication = CreateObject<FBApplication> ();
	m_fbApplication->Setup (m_staticProtocol, 0, m_alertGeneration, m_actualRange, 32, 1024, 1000, 20);
	m_fbApplication->SetStartTime (Seconds (5));
	m_fbApplication->SetStopTime (Seconds (m_TotalSimTime));

	// Add the desired nodes to the application
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		m_fbApplication->AddNode (m_adhocNodes.Get (i), m_adhocSources.at (i), m_adhocSinks.at (i));
	}

	// Add the application to a node
	m_adhocNodes.Get (m_startingNode)->AddApplication (m_fbApplication);
}

void
FBVanetExperiment::CommandSetup (int argc, char **argv)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Parsing command line arguments.");

	CommandLine cmd;

	// allow command line overrides
	cmd.AddValue ("nodes", "Number of nodes (i.e. vehicles)", m_nNodes);
	cmd.AddValue ("actualRange", "Actual transimision range (meters)", m_actualRange);
	cmd.AddValue ("protocol", "Estimantion protocol: 1=FB, 2=C300, 3=C1000", m_staticProtocol);
	cmd.AddValue ("alertGeneration", "Time at which the first Alert Message should be generated.", m_alertGeneration);
	cmd.AddValue ("mobility", "Node mobility: 1=stationary, 2=moving", m_mobility);
	cmd.AddValue ("scenario", "1=straight street, 2=grid layout, 3=real world", m_scenario);
	cmd.AddValue ("buildings", "Load building (obstacles)", m_loadBuildings);
	cmd.AddValue ("animation", "Enable netanim animation.", m_animation);
	cmd.AddValue ("totalTime", "Simulation end time", m_TotalSimTime);

	cmd.Parse (argc, argv);
}

void
FBVanetExperiment::SetupScenario ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure current scenario (" << m_scenario << ").");

	if (m_scenario == 0) {
		// DEBUG Scenario
		m_mobility = 1;
		m_nNodes = 5;

		m_fixNodePosition.push_back( Vector (100, 0.0, 0.0));
		m_fixNodePosition.push_back( Vector (300, 0.0, 0.0));
		m_fixNodePosition.push_back( Vector (500, 0.0, 0.0));
		m_fixNodePosition.push_back( Vector (700, 0.0, 0.0));
		m_fixNodePosition.push_back( Vector (900, 0.0, 0.0));

	}
	else if (m_scenario == 1)
	{
		m_mobility = 1;

		// Default value for this scenario
		if (m_nNodes == 0)
			m_nNodes = 500;

		uint32_t roadLength = 8000;	// in meters
		uint32_t distance = roadLength / m_nNodes;

		for (uint32_t i = 0; i < m_nNodes; i++)
			m_fixNodePosition.push_back( Vector (i*distance, 0.0, 0.0));

	}
	else if (m_scenario == 2)
	{
		m_mobility = 1;

		// DEBUG
		m_nNodes = 3;

		m_fixNodePosition.push_back( Vector (350,350, 0.0));
		m_fixNodePosition.push_back( Vector (0,350, 0.0));
		m_fixNodePosition.push_back( Vector (360,46,0.0));
	}
	else if (m_scenario == 3)
	{
		// Real word scenario
		m_mobility = 2;
		m_nNodes = 5;	// TODO: check this value
		m_traceFile = "inputs/Blocco-IME.ns2mobility.xml";
		m_bldgFile = "inputs/Blocco-IME.poly.xml";

		if (m_loadBuildings != 0)
		{
			NS_LOG_INFO ("Loading buildings file \"" << m_bldgFile << "\".");
			Topology::LoadBuildings (m_bldgFile);
		}

	}
	else
		NS_LOG_ERROR ("Invalid scenario specified. Values must be [1-3].");
}

void
FBVanetExperiment::RunSimulation ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Run simulation...");

	Run ();
}

void
FBVanetExperiment::ProcessOutputs ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("----------------------------------------\nPrint some statistics.");

	m_fbApplication->PrintStats ();
}

void
FBVanetExperiment::Run ()
{
	NS_LOG_FUNCTION (this);

	if (m_animation != 0)
	{
		// Create the animation object and configure for specified output
		AnimationInterface anim (m_animationFileName);
		anim.SetMobilityPollInterval (Seconds (0.250));
		anim.EnablePacketMetadata (true);
		// TODO: this will cause a SIGSEGV
		// anim.EnableIpv4L3ProtocolCounters (Seconds (1), Seconds (m_TotalSimTime));
		// anim.EnableWifiMacCounters (Seconds (1), Seconds (m_TotalSimTime));
		// anim.EnableWifiPhyCounters (Seconds (1), Seconds (m_TotalSimTime));

		Simulator::Stop (Seconds (m_TotalSimTime));
	}

	Simulator::Run ();

	Simulator::Destroy ();
}

void
FBVanetExperiment::CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility)
{
	NS_LOG_FUNCTION ( &os << foo << mobility);	// problem with the argument *os

  Vector pos = mobility->GetPosition (); // Get position
  Vector vel = mobility->GetVelocity (); // Get velocity

  int nodeId = mobility->GetObject<Node> ()->GetId ();

  NS_LOG_DEBUG ("Changing pos for node " << nodeId << " at " << Simulator::Now ().GetSeconds ()
	 							<< "; POS: (" << pos.x << ", " << pos.y << ", " << pos.z << ")"
								<< "; VEL: (" << vel.x << ", " << vel.y << ", " << vel.z << ").");
}

Ptr<Socket>
FBVanetExperiment::SetupPacketReceive (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Socket> sink = Socket::CreateSocket (node, tid);
	InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 9);
	sink->Bind (local);

	// Store socket
	m_adhocSinks.push_back (sink);

	return sink;
}

Ptr<Socket>
FBVanetExperiment::SetupPacketSend (Ipv4Address addr, Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << addr << node);

	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Socket> sender = Socket::CreateSocket (node, tid);
	InetSocketAddress remote = InetSocketAddress (addr, 9);
	sender->SetAllowBroadcast (true);
	sender->Connect (remote);

	// Store socket
	m_adhocSources.push_back (sender);

	return sender;
}


/* -----------------------------------------------------------------------------
*			MAIN
* ------------------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
	NS_LOG_UNCOND ("FB Vanet Experiment.");

	FBVanetExperiment experiment;
	experiment.Simulate (argc, argv);
}
