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
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/topology.h"
#include "ns3/netanim-module.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("fb-vanet");

/* -----------------------------------------------------------------------------
*			CLASS AND METHODS PROTOTIPES
* ------------------------------------------------------------------------------
*/

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval )
{
	NS_LOG_FUNCTION ("GenerateTraffic");

  if (pktCount > 0)
    {
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize,pktCount - 1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}

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
   * \brief Prints actual position and velocity when a course change event occurs
   * \return none
   */
	static void
	CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility);

	/**
	 * \brief Process a received routing packet
	 * \param socket the receiving socket
	 * \return none
	 */
	void ReceivePacket (Ptr<Socket> socket);

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
		m_TotalSimTime (10)
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
		mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		mobility.SetPositionAllocator (positionAlloc);
		mobility.Install (m_adhocNodes);
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


	// The below set of helpers will help us to put together the wifi NICs we want
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();

	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (m_actualRange + 100));

	// Propagation loss models are additive.  If Obstacle modeling is included,
	// then we add obstacle-shadowing
	if (m_loadBuildings != 0)
	{
		wifiChannel.AddPropagationLoss ("ns3::ObstacleShadowingPropagationLossModel");
	}

  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wifiPhy.SetChannel (channel);

	// ns-3 supports generate a pcap trace
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();

	wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode", StringValue (m_phyMode),
                                      "ControlMode", StringValue (m_phyMode));
  NetDeviceContainer m_adhocDevices = wifi80211p.Install (wifiPhy, wifi80211pMac, m_adhocNodes);

	// Tracing
  wifiPhy.EnablePcap ("wave-simple-80211p", m_adhocDevices);
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

	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (m_adhocNodes.Get (0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&FBVanetExperiment::ReceivePacket, this));

  Ptr<Socket> source = Socket::CreateSocket (m_adhocNodes.Get (1), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);

	Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                  Seconds (1.0), &GenerateTraffic,
                                  source, 1000, 2, Seconds (1));
	//
	// OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address ());
	// onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
	// onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

	// Set receiver (for each node)
	// for (uint32_t j = 0; j < m_nNodes; j++)
	// {
	// 	Ptr<Socket> sink = SetupPacketReceive (m_adhocNodes.Get (j));
	// 	AddressValue remoteAddress (InetSocketAddress (ns3::Ipv4Address::GetAny (), m_port));
	// 	onoff1.SetAttribute ("Remote", remoteAddress);
	// }

	// // Set unicast sender
	// for (uint32_t i = 0; i < m_nNodes; i++)
	// {
	// 	Ptr<Socket> sender = SetupPacketSend (ns3::Ipv4Address(m_address.c_str ()),  m_adhocNodes.Get (i));
	// 	sender->SetAllowBroadcast (true);
	//
	// 	// Add socket to the node
	// 	m_adhocNodes.Get (i)->setBroadcast (sender);
	// }
}

void
FBVanetExperiment::ConfigureTracingAndLogging ()
{
	NS_LOG_FUNCTION (this);

	// Enable logging from the ns2 helper
	LogComponentEnable ("Ns2MobilityHelper", LOG_LEVEL_DEBUG);

	// wifi80211p.EnableLogComponents ();	// TODO

	Packet::EnablePrinting ();
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
		m_nNodes = 10;
		m_startingNode = (m_nNodes / 2) - 1;	// Start in the middle
		uint32_t roadLength = 1000;
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
FBVanetExperiment::ReceivePacket (Ptr<Socket> socket)
{
	NS_LOG_FUNCTION (this << socket);

	std::ostringstream oss;
  Ptr<Packet> packet;
  Address srcAddress;
  while (socket->Recv ())
  {
		oss << Simulator::Now ().GetSeconds () << " " << socket->GetNode ()->GetId ();

		oss << " received one packet!";
  }

	NS_LOG_DEBUG (oss);
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
