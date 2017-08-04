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
*/

/*
* Timeline (seconds): TODO
*  + 5						start simulation
*	 + 5-?					Hello messages (1200 messages)
*	 + ?-						Alert messages
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
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/topology.h"
#include "ns3/netanim-module.h"

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
	 * \param node node to configure
	 * \return none
	 */
	void SetupFBParameters (Ptr<Node> node);

	/* -----------------------------------------------------------------------------
	*			SPECIFIC TO FB PROTOCOL
	* ------------------------------------------------------------------------------
	*/

	/**
	 * \brief Set up receivers socket
	 * \param node noe to configure
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
	 * \brief Procure triggered when a packet is received
	 * \param socket socket of the receiver
	 * \return none
	 */
	void ReceivePacket (Ptr<Socket> socket);

	/**
	 * \brief Send a Hello message to all nodes in its range
	 * \return none
	 */
	void GenerateHelloMessage ();

	/**
	 * \brief Send an Alert message to the nodes (in its range)
	 * \return none
	 */
	void GenerateAlertMessage ();

	/**
	 * \brief Procedure triggered when a Hello packet is received
	 * \param socket socket of the node
	 * \param head FB packet header
	 * \return none
	 */
	void HandleHello (Ptr<Socket> socket, FBHeader head);

	/**
	 * \brief Procedure triggered when an Alert packet is received
	 * \param socket socket of the node
	 * \param head FB packet header
	 * \param distance distance between the nodes
	 * \param FB protocol phase
	 * \return none
	 */
	void HandleAlert (Ptr<Socket> socket, FBHeader head, int distance, int phase);

	// TODO: check function definition
	/**
	 * \brief Procedure Broad
	 * \param node
	 * \param phase FB protocol phase
	 * \param rs slot size
	 * \param sx x coordinate of the sender
	 * \param sy y coordinate of the sender
	 * \return none
	 */
	void Broad (Ptr<Node> node, int phase, uint32_t rs, int sx, int sy);

	/**
	 * \brief Node waiting procedure
	 * \param node node
	 * \param phase FB protocol phase
	 * \param rs slot size
	 * \param par sender x-y coordinates
	 * \return none
	 */
	void WaitAgain (Ptr<Node> node, int phase, uint32_t rs, std::vector<int> par);

	/**
	 * \brief Stop a node
	 * \param node node
	 * \return none
	 */
	void StopNode (Ptr<Node> node);

	/**
	 * \brief Get the position of a node
	 * \param node node
	 * \return node x coordinate
	 */
	int GetNodeXPosition (Ptr<Node> node);

	/**
	 * \brief Get the position of a node
	 * \param node node
	 * \return node y coordinate
	 */
	int GetNodeYPosition (Ptr<Node> node);

	/**
	 * \brief Compute the distance between two nodes
	 * \param x1 x coordinate of the first node
	 * \param y1 y coordinate of the first node
	 * \param x2 x coordinate of the second node
	 * \param y2 y coordinate of the second node
	 * \return distance
	 */
	double CalculateDistance (int x1, int y1, int x2, int y2);

	/**
	 * \brief Schedule who must send a Hello message
	 * \param count number of nodes that haven't send the message yet
	 * \return none
	 */
	void StartHelloPhase ();

	/**
	 * \brief Get the address of a node
	 * \param node node
	 * \return node address
	 */
	Ipv4Address GetAddress (Ptr<Node> node);

	/**
   * \brief Prints actual position and velocity when a course change event occurs
   * \return none
   */
	static void
	CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility);

	double									m_txp;
	uint32_t 								m_nNodes;
	NodeContainer						m_adhocNodes;
	NetDeviceContainer			m_adhocDevices;
	Ipv4InterfaceContainer	m_adhocInterfaces;
	uint32_t								m_actualRange;
	uint32_t								m_estimatedRange;
	uint32_t								m_estimationProtocol;
	bool										m_flooding;
	uint32_t								m_rCirc;
	std::string 						m_rate;
	std::string 						m_phyMode;
	uint32_t 								m_mobility;
	uint32_t 								m_scenario;
	std::vector <Vector>		m_fixNodePosition;
	std::string							m_address;
	uint32_t								m_port;
	uint32_t								m_totalPacketReceived;
	uint32_t								m_totalPacketSent;
	uint32_t								m_cwMin;
	uint32_t								m_cwMax;
	uint32_t 								m_packetPayloadSize;
	uint32_t								m_startingNode;
	uint32_t								m_totalHelloMessages;
	uint32_t								m_helloPhaseStartTime;
	uint32_t								m_alertPhaseStartTime;
	std::string							m_traceFile;
	uint32_t								m_loadBuildings;
	std::string							m_bldgFile;
	std::string 						m_CSVfileName;
	std::string							m_animationFileName;
	double									m_TotalSimTime;
};

/* -----------------------------------------------------------------------------
*			METHOD DEFINITIONS
* ------------------------------------------------------------------------------
*/

FBVanetExperiment::FBVanetExperiment ()
	: m_txp (7.5),
		m_nNodes (0),	// random value, it will be set later
		m_actualRange (300),
		m_estimatedRange (0),
		m_estimationProtocol (1),
		m_flooding (true),
		m_rCirc (1000),
		m_rate ("2048bps"),
		m_phyMode ("DsssRate11Mbps"),
		m_mobility (1),
		m_scenario (1),
		m_address ("10.1.255.255"),
		m_port (9),
		m_totalPacketReceived (0),
		m_totalPacketSent (0),
		m_cwMin (32),
		m_cwMax (1024),
		m_packetPayloadSize (100),
		m_startingNode (0),
		m_totalHelloMessages (1200),
		m_helloPhaseStartTime (5),	// [seconds]
		m_alertPhaseStartTime (40000),	// [seconds]
		m_traceFile (""),
		m_loadBuildings (0),
		m_bldgFile (""),
		m_CSVfileName ("outputs/manet-routing.output.csv"),
		m_animationFileName ("outputs/fb-vanet-animation.xml"),
		m_TotalSimTime (990000.01)	// TODO: this need to change
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
	NS_LOG_FUNCTION (this);

	Config::SetDefault ("ns3::OnOffApplication::PacketSize",StringValue ("64"));
	Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (m_rate));
	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (m_phyMode));

	if (m_estimationProtocol == 1) {
		m_estimatedRange = 0;
	}
	else if (m_estimationProtocol == 2)
	{
		m_estimatedRange = 300;
	}
	else if (m_estimationProtocol == 3)
	{
		m_estimatedRange = 1000;
	}
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
	NS_LOG_INFO ("Setup nodes.");

	m_adhocNodes.Create (m_nNodes);
}

void
FBVanetExperiment::ConfigureMobility ()
{
	NS_LOG_FUNCTION (this << m_mobility);
	NS_LOG_INFO ("Configure current mobility mode (" << m_mobility << ").");

	if (m_mobility == 1)
	{
		MobilityHelper mobility;
		Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

		// I don't like put that in here, but dunno where.
		// Set nodes position
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
	else
	{
		// Create Ns2MobilityHelper with the specified trace log file as parameter
		Ns2MobilityHelper ns2 = Ns2MobilityHelper (m_traceFile);
		NS_LOG_UNCOND ("Loading ns2 mobility file \"" << m_traceFile << "\".");

		ns2.Install (); // configure movements for each node, while reading trace file
		// initially assume all nodes are not moving

		// Configure callback for logging
		std::ofstream m_os;
		Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
										 MakeBoundCallback (&FBVanetExperiment::CourseChange, &m_os));
	}
}

void
FBVanetExperiment::SetupAdhocDevices ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure channels.");

	// Setting up wifi phy and channel using helpers
	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

	//Setting max range
	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (m_actualRange + 100));

	// Propagation loss models are additive.  If Obstacle modeling is included,
	// then we add obstacle-shadowing
	if (m_loadBuildings != 0)
	{
		wifiChannel.AddPropagationLoss ("ns3::ObstacleShadowingPropagationLossModel");
	}

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
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure connections.");

	InternetStackHelper internet;
	internet.Install (m_adhocNodes);

	Ipv4AddressHelper addressAdhoc;
	addressAdhoc.SetBase ("10.1.0.0", "255.255.0.0");	// TODO: make them variable

	m_adhocInterfaces = addressAdhoc.Assign (m_adhocDevices);

	OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address ());
	onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
	onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

	// Set receiver (for each node)
	for (uint32_t j = 0; j < m_nNodes; j++)
	{
		Ptr<Socket> sink = SetupPacketReceive (m_adhocNodes.Get (j));
		AddressValue remoteAddress (InetSocketAddress (ns3::Ipv4Address::GetAny (), m_port));
		onoff1.SetAttribute ("Remote", remoteAddress);
	}

	// Set unicast sender
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		Ptr<Socket> sender = SetupPacketSend (ns3::Ipv4Address(m_address.c_str ()),  m_adhocNodes.Get (i));
		sender->SetAllowBroadcast (true);

		// Add socket to the node
		m_adhocNodes.Get (i)->setBroadcast (sender);
	}
}

void
FBVanetExperiment::SetupFBProtocol ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure FB protocol parameters.");

	// Configure FB protocol parameters for all nodes
	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		SetupFBParameters (m_adhocNodes.Get (i));
	}
}

void
FBVanetExperiment::ConfigureTracingAndLogging ()
{
	NS_LOG_FUNCTION (this);

	// Enable logging from the ns2 helper
	LogComponentEnable ("Ns2MobilityHelper", LOG_LEVEL_DEBUG);

	Packet::EnablePrinting ();
}

void
FBVanetExperiment::ScheduleFBProtocol ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Schedule FB protocol phases.");

	uint32_t alertPhaseStartTimeUpdated = m_helloPhaseStartTime;
	// // Hello messages
	if (m_estimationProtocol == 1)
	{
		Simulator::Schedule (Seconds (m_helloPhaseStartTime), &FBVanetExperiment::StartHelloPhase, this);
		alertPhaseStartTimeUpdated = m_alertPhaseStartTime;
	}

	// Generate alert message
	Simulator::ScheduleWithContext (m_adhocNodes.Get (m_startingNode)->GetId (),
																	Seconds (alertPhaseStartTimeUpdated),
																	&FBVanetExperiment::GenerateAlertMessage, this);
}

void
FBVanetExperiment::CommandSetup (int argc, char **argv)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Parsing command line arguments.");

	CommandLine cmd;

	// allow command line overrides
	cmd.AddValue ("txp", "Transmit power (dB), e.g. txp=7.5", m_txp);
	cmd.AddValue ("nodes", "Number of nodes (i.e. vehicles)", m_nNodes);
	cmd.AddValue ("actualRange", "Actual transimision range (meters)", m_actualRange);
	cmd.AddValue ("estimationProtocol", "Estimantion protoco: 1=FB, 2=C300, 3=C1000", m_estimationProtocol);
	cmd.AddValue ("flooding", "Enable flooding", m_flooding);
	cmd.AddValue ("rCirc", "Vehicles transimision area" , m_rCirc);
	cmd.AddValue ("rate", "Rate", m_rate);
	cmd.AddValue ("phyModeB", "Phy mode 802.11b", m_phyMode);
	cmd.AddValue ("mobility", "Node mobility: 1=stationary, 2=moving", m_mobility);
	// cmd.AddValue ("speed", "Node speed (m/s)", m_nodeSpeed);
	cmd.AddValue ("scenario", "1=straight street, 2=grid layout, 3=real world", m_scenario);
	cmd.AddValue ("address", "IP address", m_address);
	cmd.AddValue ("port", "IP port", m_port);
	cmd.AddValue ("traceFile", "Ns2 movement trace file", m_traceFile);
	cmd.AddValue ("buildings", "Load building (obstacles)", m_loadBuildings);
	cmd.AddValue ("bldgFile", "Polyconvert obstacles file", m_bldgFile);
	cmd.AddValue ("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
	cmd.AddValue ("totaltime", "Simulation end time", m_TotalSimTime);

	cmd.Parse (argc, argv);
}

void
FBVanetExperiment::SetupScenario ()
{
	NS_LOG_FUNCTION (this << m_scenario);
	NS_LOG_INFO ("Configure current scenario (" << m_scenario << ").");

	if (m_scenario == 1)
	{
		// straight line, nodes in a row
		m_mobility = 1;
		m_nNodes = 700;
		m_startingNode = (m_nNodes / 2) - 1;	// Start in the middle
		uint32_t roadLength = 8000;
		uint32_t distance = (roadLength / m_nNodes);

		// Node positions (in meters) along the straight street (or line)
		for (uint32_t i = 0; i < m_nNodes; i++) {
			m_fixNodePosition.push_back( Vector (i * distance, 0.0, 0.0));
		}
	}
	else if (m_scenario == 2)
	{
		// Grid layout
		m_mobility = 1;

		// Arrange nodes along the crossroads of the grid
		uint32_t roadLength = 3600;	// (actually 4000x3900)
		uint32_t nodeDistance = 12;	// distance between nodes (in meters)
		uint32_t roadDistance = 300;	// distance between parallel roads (in meters)

		m_nNodes = 0;
		for (uint32_t i = 0; i <= (roadLength / nodeDistance); i++) {
			if ((i * nodeDistance) % roadDistance == 0)
			{
				for (uint32_t j = 0; j <= (roadLength / nodeDistance); j++)
				{
					m_fixNodePosition.push_back (Vector (j*nodeDistance, i*nodeDistance, 0.0));
				}
			}
			else
			{
				for (uint32_t t = 0; t <= roadLength ; t += roadDistance)
				{
					m_fixNodePosition.push_back (Vector (t, i*nodeDistance, 0.0));
				}
			}
		}

		m_nNodes = m_fixNodePosition.size();
		m_startingNode = (m_nNodes / 2) - 1;	// Start in the middle
	}
	else if (m_scenario == 3)
	{
		m_mobility = 2;
		m_nNodes = 22;	// TODO: check value
		m_traceFile = "inputs/Blocco-IME.ns2mobility.xml";
		m_bldgFile = "inputs/Blocco-IME.poly.xml";

		if (m_loadBuildings != 0)
		{
			NS_LOG_UNCOND ("Loading buildings file \"" << m_bldgFile << "\".");
			Topology::LoadBuildings (m_bldgFile);
		}
	}
}

void
FBVanetExperiment::SetupFBParameters (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

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
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Run simulation...");

	Run ();
}

void
FBVanetExperiment::ProcessOutputs ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Print some statistics.");

	// Print some statistics
	uint32_t received = 0;
	uint32_t sent = 0;

	for (uint32_t i = 0; i < m_nNodes; i++)
	{
		Ptr<Node> node = m_adhocNodes.Get (i);

		if (node->GetReceived ())
			received++;

		if (node->GetSent ())
			sent++;
	}

	NS_LOG_INFO ("Number of nodes that received a message: " << ((received/(double)m_nNodes) * 100) << "%.");
	NS_LOG_INFO ("Number of nodes that send a message: " << ((sent/(double)m_nNodes) * 100) << "%.");

	NS_LOG_INFO ("Total messages sent: " << m_totalPacketSent << ".");
	NS_LOG_INFO ("Total messages received: " << m_totalPacketReceived << ".");
}

void
FBVanetExperiment::Run ()
{
	NS_LOG_FUNCTION (this);

	// Create the animation object and configure for specified output
	// AnimationInterface anim (m_animationFileName);
	// anim.SetMobilityPollInterval (Seconds (0.250));
	// anim.EnablePacketMetadata (true);
	// anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (m_TotalSimTime));
	// anim.EnableWifiMacCounters (Seconds (0), Seconds (m_TotalSimTime));
	// anim.EnableWifiPhyCounters (Seconds (0), Seconds (m_TotalSimTime));

	Simulator::Stop (Seconds (m_TotalSimTime));
	Simulator::Run ();

	Simulator::Destroy ();
}

/* -----------------------------------------------------------------------------
*			SPECIFIC TO FB PROTOCOL
* ------------------------------------------------------------------------------
*/
Ptr<Socket>
FBVanetExperiment::SetupPacketReceive (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Socket> sink = Socket::CreateSocket (node, tid);
	InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
	sink->Bind (local);
	sink->SetRecvCallback (MakeCallback (&FBVanetExperiment::ReceivePacket, this));	// [DEBUG] without "this" it won't work

	return sink;
}

Ptr<Socket>
FBVanetExperiment::SetupPacketSend (Ipv4Address addr, Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << addr << node);

	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Socket> sender = Socket::CreateSocket (node, tid);
	InetSocketAddress remote = InetSocketAddress (addr, m_port);	// [DEBUG] why this way here and the other way above?

	sender->Connect (remote);

	return sender;
}

void
FBVanetExperiment::ReceivePacket (Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << socket);

	Ptr<Packet> packet;
	Address senderAddress;
	Ptr<Node> node= socket->GetNode();

	while ((packet = socket->RecvFrom (senderAddress)))
	{
		FBHeader head;
		packet->RemoveHeader (head);

		//Type
		uint32_t type = head.getType();

		// Current node position
		int myXPosition = GetNodeXPosition (socket->GetNode ());
		int myYPosition = GetNodeYPosition (socket->GetNode ());

		// Sender position
		int senderXPosition = head.getSenderXPosition ();
		int senderYPosition = head.getSenderYPosition ();

		// If it's a Hello message
		if (type == 1)
		{
			double myDistance2 = CalculateDistance (senderXPosition, senderYPosition, myXPosition, myYPosition);

			// If the node is in range, I can read the packet
			if ( myDistance2 <= m_actualRange )
			{
				HandleHello (socket, head);
			}
		}
		// Alert message otherwise
		else if ( type == 2)
		{
			// Starting node position
			int startXPosition = head.getStartXPosition ();
			int startYPosition = head.getStartYPosition ();

			// Compute various distances
			double myDistance = CalculateDistance (myXPosition, myYPosition, startXPosition, startYPosition);
			double senderDistance = CalculateDistance (senderXPosition, senderYPosition, startXPosition, startYPosition);
			double myDistance2 = CalculateDistance (senderXPosition, senderYPosition, myXPosition, myYPosition);

			// TODO: change int to uint32_t
			int phase = head.getPhase ();

			// If the node is in range, I can read the packet
			if ((std::abs (myDistance2) <= m_actualRange))
			{
				m_totalPacketReceived++;

				if (myDistance > senderDistance && !socket->GetNode ()->GetReceived ())
				{
					uint32_t sl = head.getSlot ();
					socket->GetNode ()->SetSlot (socket->GetNode ()->GetSlot () + sl);
					StopNode (socket->GetNode ());
					socket->GetNode ()->SetReceived (true);

					if (socket->GetNode ()->GetNum () == 0)
					{
						socket->GetNode ()->SetNum (phase);
					}

					HandleAlert (socket, head, myDistance2, phase);
				}
				else
				{
					if (node->GetPhase () < phase)
						node->SetPhase (phase);
				}
			}
		}
	}
}

void
FBVanetExperiment::GenerateHelloMessage ()
{
	NS_LOG_FUNCTION (this);

	Ptr<Node> node = NodeList::GetNode (Simulator::GetContext());

	NS_LOG_DEBUG ("Generate Hello Message (node <" << node->GetId() << ">).");

	// Create a packet with the correct parameters taken from the node
	FBHeader header;
	header.setCMFR (node->GetCMBR());
	header.setStartXPosition (GetNodeXPosition (node));
	header.setStartYPosition (GetNodeYPosition (node));
	header.setSenderXPosition (GetNodeXPosition (node));
	header.setSenderYPosition (GetNodeYPosition (node));
	header.setType(1);

	Ptr<Packet> p = Create<Packet> (m_packetPayloadSize);
	p->AddHeader (header);

	Ptr<Socket> sock = node->getBroadcast ();
	sock->Send (p);
}

void
FBVanetExperiment::GenerateAlertMessage ()
{
	NS_LOG_FUNCTION (this);

	Ptr<Node> node = NodeList::GetNode (Simulator::GetContext());

	Ptr<Packet> p= Create<Packet> (m_packetPayloadSize);
	uint32_t LMBR, CMBR, maxi;
	// Create a packet with the correct parameters taken from the node
	FBHeader header;
	header.setType (2);

	LMBR = node->GetLMBR ();
	CMBR = node->GetCMBR ();
	maxi = std::max (LMBR, CMBR);

	header.setStartXPosition (GetNodeXPosition (node));
	header.setStartYPosition (GetNodeYPosition (node));
	header.setSenderXPosition (GetNodeXPosition (node));
	header.setSenderYPosition (GetNodeYPosition (node));
	header.setMax (maxi);
	header.setPhase (0);
	header.setSlot (0);
	p->AddHeader (header);

	Ptr<Socket> sock = node->getBroadcast ();

	// Current node address
	Ipv4Address addr = GetAddress(node);

	NS_LOG_DEBUG (addr << ": sending Alert Message.");

	sock->Send (p);
	node->SetSent (true);
	StopNode (node);

	m_totalPacketSent++;
}

void
FBVanetExperiment::HandleHello (Ptr<Socket> socket, FBHeader head)
{
	NS_LOG_FUNCTION (this << socket << head);

	Ptr<Node> node = socket->GetNode ();

	// CMFR received
	int CMFR = head.getCMFR ();

	// Current node position
	int myXPosition = GetNodeXPosition (socket->GetNode ());
	int myYPosition = GetNodeYPosition (socket->GetNode ());

	// Sender position
	int senderXPosition = head.getSenderXPosition ();
	int senderYPosition = head.getSenderYPosition ();
	uint32_t distance = CalculateDistance (senderXPosition, senderYPosition, myXPosition, myYPosition);

	// Current node CMBR
	int myCMBR = node->GetCMBR ();


	uint32_t m0 = std::max (myCMBR, CMFR);
	int maxi = std::max (m0, distance);
	node->SetCMBR (maxi);

	// Storing CMBR in LMBR
	node->SetLMBR (myCMBR);
}

void
FBVanetExperiment::HandleAlert (Ptr<Socket> socket, FBHeader head, int distance, int phase)
{
	NS_LOG_FUNCTION (this << socket << head << distance << phase);

	Ptr<Node> node= socket->GetNode ();
	int myPhase = node->GetPhase ();

	// If it's from the front
	if (phase > myPhase)
	{
		node-> SetPhase (phase);

		// Compute the window
		int BMR = node->GetCMBR ();
		int dist = std::abs (distance);
		double rapp = (double) (BMR - dist) / BMR;
		rapp = (rapp < 0) ? 0 : rapp;

		int range = (rapp * (m_cwMax - m_cwMin)) + m_cwMin;

		//Random value
		uint32_t rs = (rand () % range) + 1;
		int startXPosition = head.getStartXPosition ();
		int startYPosition = head.getStartYPosition ();

		std::vector<int> par;
		par.push_back (startXPosition);
		par.push_back (startYPosition);

		if (m_flooding == false)
			Simulator::ScheduleWithContext (socket->GetNode ()->GetId (),
																			MilliSeconds (rs * 200 * 3),
																			&FBVanetExperiment::WaitAgain, this,
																			socket->GetNode (),
																			phase, rs, par);
		else
			Simulator::ScheduleWithContext (socket->GetNode ()->GetId (),
																			MilliSeconds (0),
																			&FBVanetExperiment::Broad, this,
																			socket->GetNode (),
																			phase, rs, par.at (0), par.at (1));
	}
}

void
FBVanetExperiment::Broad (Ptr<Node> node, int phase, uint32_t rs, int sx, int sy)
{
	NS_LOG_FUNCTION (this << node << phase << rs << sx << sy);

	// If I'm the first to wake up, I must forward the message
	if ((!m_flooding && phase >= node->GetPhase ()) || (m_flooding && !node->GetSent ()) )
	{
		// Create a packet with the correct parameters taken from the node
		Ptr<Packet> p = Create<Packet> (m_packetPayloadSize);
		FBHeader header;

		header.setType (2);
		uint32_t LMBR = node -> GetLMBR ();
		uint32_t CMBR = node -> GetCMBR ();
		uint32_t maxi = std::max (LMBR, CMBR);

		header.setSenderXPosition (GetNodeXPosition (node));
		header.setSenderYPosition (GetNodeYPosition (node));
		header.setStartXPosition (sx);
		header.setStartYPosition (sy);
		header.setMax (maxi);
		header.setPhase (phase + 1);
		header.setSlot (node->GetSlot () + rs);
		p->AddHeader (header);

		Ptr<Socket> sock = node->getBroadcast ();

		// My address
		Ipv4Address addr = GetAddress (node);

		NS_LOG_DEBUG (addr<<" (" << phase << "): forwarding Alert Message.");

		sock->Send (p);
		node->SetSent (true);
		m_totalPacketSent++;
	}
}

void
FBVanetExperiment::WaitAgain (Ptr<Node> node, int phase, uint32_t rs, std::vector<int> par)
{
	NS_LOG_FUNCTION (this << node << phase << rs << &par);

	if (phase >= node->GetPhase ())
	{
		uint32_t rnd = (rand() % 20) + 1;
		uint32_t rnd1 = (rand() % 20) + 1;
		uint32_t rnd2 = (rand() % 20) + 1;
		uint32_t rnd3 = (rand() % 20) + 1;
		Simulator::ScheduleWithContext (node->GetId (),
																		MilliSeconds (10 * (rs + rnd + rnd1 + rnd2 + rnd3) * 200 * 3),
																		&FBVanetExperiment::Broad, this,
																		node, phase, rs, par.at(0), par.at(1));
	}
}

// Prints actual position and velocity when a course change event occurs
void
FBVanetExperiment::CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility)
{
	NS_LOG_FUNCTION ( &os << foo << mobility);	// problem with the argument *os

  Vector pos = mobility->GetPosition (); // Get position
  Vector vel = mobility->GetVelocity (); // Get velocity

  pos.z = 1.5;

  int nodeId = mobility->GetObject<Node> ()->GetId ();

  NS_LOG_DEBUG ("Changing pos for node <" << nodeId << "> at " << Simulator::Now ().GetSeconds ()
	 							<< "\n\tPOS: x=" << pos.x <<
								", y=" << pos.y <<
								", z=" << pos.z <<
								";\n\tVEL:" << vel.x <<
								", y=" << vel.y <<
								", z=" << vel.z );
}

void
FBVanetExperiment::StopNode (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<ConstantVelocityMobilityModel> mob = node->GetObject<ConstantVelocityMobilityModel> ();
	mob->SetVelocity (Vector (0, 0, 0));
}

int
FBVanetExperiment::GetNodeXPosition (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<MobilityModel> positionmodel = node->GetObject<MobilityModel> ();
	Vector pos = positionmodel->GetPosition ();
	return pos.x;
}

int
FBVanetExperiment::GetNodeYPosition (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<MobilityModel> positionmodel = node->GetObject<MobilityModel> ();
	Vector pos = positionmodel->GetPosition ();
	return pos.y;
}

double
FBVanetExperiment::CalculateDistance (int x1, int y1, int x2, int y2)
{
	NS_LOG_FUNCTION (this << x1 << y1 << x2 << y2);

	int distancex = (x2 - x1) * (x2 - x1);
	int distancey = (y2 - y1) * (y2 - y1);
	double distance = sqrt (distancex + distancey);

	return distance;
}

void
FBVanetExperiment::StartHelloPhase ()
{
	NS_LOG_FUNCTION (this);

	uint32_t timeMin = 10;
	uint32_t timeMax = 30;

	Ptr<Node> node;
	uint32_t pos;
	uint32_t wait = 0;

	for (uint32_t i = 0; i < m_totalHelloMessages; i++)
	{
		// select a node randomly
		pos = rand() % m_nNodes;
		node = m_adhocNodes.Get (pos);

		// select a random time between <timeMin> and <timeMax> seconds
		wait += (rand() % (timeMax-timeMin)) + timeMin;

		// Schedule the generation of the current Hello message
		Simulator::ScheduleWithContext (node->GetId (),
																		Seconds (wait),
																		&FBVanetExperiment::GenerateHelloMessage, this);
	}
}

Ipv4Address
FBVanetExperiment::GetAddress (Ptr<Node> node)
{
	NS_LOG_FUNCTION (this << node);

	Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
	Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1,0);
	Ipv4Address addr = iaddr.GetLocal ();

	return addr;
}

/* -----------------------------------------------------------------------------
*			MAIN
* ------------------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
	// NS_LOG_FUNCTION_NOARGS;
	NS_LOG_UNCOND ("FB Vanet Experiment.");

	FBVanetExperiment experiment;
	experiment.Simulate (argc, argv);
}