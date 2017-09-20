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
#include "ns3/dsdv-module.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DroneExperiment");

/* -----------------------------------------------------------------------------
*			CLASS AND METHODS PROTOTIPES
* ------------------------------------------------------------------------------
*/

/**
 * \ingroup object
 * \brief The DroneExperiment class implements the main simulation
 *
 */
class DroneExperiment
{
public:
	/**
	 * \brief Constructor
	 * \return none
	 */
	DroneExperiment ();

	/**
	 * \brief Destructor
	 * \return none
	 */
	virtual ~DroneExperiment ();

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
	void ConfigureApplications ();

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

	uint32_t 													m_nNodes;	// number of nodes
	NodeContainer											m_adhocNodes; // all nodes
	NetDeviceContainer								m_adhocDevices;	// net devices
	Ipv4InterfaceContainer						m_adhocInterfaces;	// ipv4 interfaces
	std::string												m_packetSize; // size of the packets
	std::string												m_rate;	// data rate
	std::string												m_phyMode;	// the PHY mode to use for the PHY layer
	double														m_txp;	// transmit power
	uint32_t													m_actualRange; // transmission range
	uint32_t													m_mobility;	// type of mobility
	uint32_t													m_scenario; // scenario
	uint32_t													m_animation;	// animation enabler
	std::string												m_traceFile; // nodes trace files
	std::string												m_animFile;	// output filename for animation
	uint32_t													m_totalSimTime;	// simulation time
};

/* -----------------------------------------------------------------------------
*			METHOD DEFINITIONS
* ------------------------------------------------------------------------------
*/

DroneExperiment::DroneExperiment ()
	:	m_nNodes (0),	// random value, it will be set later
		m_packetSize ("64"),
		m_rate ("2048bps"),
		m_phyMode ("OfdmRate6Mbps"),
		m_txp (7.5),
		m_actualRange (300),
		m_mobility (1),
		m_scenario (1),
		m_animation (0),
		m_traceFile (""),
		m_animFile ("DroneExperiment.animation.xml"),
		m_totalSimTime (10)
{
	srand (time (0));
}


DroneExperiment::~DroneExperiment ()
{
}

void
DroneExperiment::Simulate (int argc, char **argv)
{
	// Initial configuration and parameters parsing
	ParseCommandLineArguments (argc, argv);
	ConfigureDefaults ();

	// Configure the network and all the elements in it
	ConfigureNodes ();
	ConfigureMobility ();
	SetupAdhocDevices ();
	ConfigureConnections ();

	// Configure applications
	ConfigureApplications ();

	ConfigureTracingAndLogging ();

	// Run simulation and print some results
	RunSimulation ();
	ProcessOutputs ();
}

void
DroneExperiment::ConfigureDefaults ()
{
	NS_LOG_FUNCTION (this);

	// Config::SetDefault ("ns3::OnOffApplication::PacketSize",StringValue (m_packetSize));
	// Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (m_rate));
	// Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (m_phyMode));
}

void
DroneExperiment::ParseCommandLineArguments (int argc, char **argv)
{
	NS_LOG_FUNCTION (this);

	CommandSetup (argc, argv);
	SetupScenario ();
}

void
DroneExperiment::ConfigureNodes ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Setup nodes.");

	m_adhocNodes.Create (m_nNodes);
}

void
DroneExperiment::ConfigureMobility ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure current mobility mode (" << m_mobility << ").");

	if (m_mobility == 1)
	{
		MobilityHelper mobility;

		ObjectFactory pos;
		pos.SetTypeId ("ns3::RandomBoxPositionAllocator");
		pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=500.0]"));
		pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=500.0]"));
		pos.Set ("Z", StringValue ("ns3::UniformRandomVariable[Min=70.0|Max=100.0]"));

		Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();

		mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
		mobility.SetPositionAllocator (taPositionAlloc);
		mobility.Install (m_adhocNodes);
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
		// 								 MakeBoundCallback (&DroneExperiment::CourseChange, &m_os));
	}
	else
		NS_LOG_ERROR ("Invalid mobility mode specified. Values must be [1-2].");
}

void
DroneExperiment::SetupAdhocDevices ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure channels.");

	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																"DataMode",StringValue (m_phyMode),
																"ControlMode",StringValue (m_phyMode));

	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5.9e9));	// 802.11n 5.9 GHz

	wifiPhy.SetChannel (wifiChannel.Create ());

	WifiMacHelper wifiMac;
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
																"DataMode",StringValue (m_phyMode),
																"ControlMode",StringValue (m_phyMode));
	// Set Tx Power
  wifiPhy.Set ("TxPowerStart",DoubleValue (m_txp));
  wifiPhy.Set ("TxPowerEnd", DoubleValue (m_txp));
	wifiMac.SetType ("ns3::AdhocWifiMac");

	m_adhocDevices = wifi.Install (wifiPhy, wifiMac, m_adhocNodes);
}

void
DroneExperiment::ConfigureConnections ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure connections.");

	DsdvHelper dsdv;
	Ipv4ListRoutingHelper listRouting; // or Ipv4StaticRoutingHelper?
	listRouting.Add (dsdv, 100);	// (&routing, priority)

	InternetStackHelper internet;
	internet.SetRoutingHelper (listRouting);
	internet.Install (m_adhocNodes);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("10.1.1.0", "255.255.255.0");
	m_adhocInterfaces = ipv4.Assign (m_adhocDevices);
}

void
DroneExperiment::ConfigureTracingAndLogging ()
{
	NS_LOG_FUNCTION (this);

	Packet::EnablePrinting ();
}

void
DroneExperiment::ConfigureApplications ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure FB application.");
}

void
DroneExperiment::CommandSetup (int argc, char **argv)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Parsing command line arguments.");

	CommandLine cmd;

	// allow command line overrides
	cmd.AddValue ("nodes", "Number of nodes (i.e. vehicles)", m_nNodes);
	cmd.AddValue ("actualRange", "Actual transimision range (meters)", m_actualRange);
	cmd.AddValue ("mobility", "Node mobility: 1=stationary, 2=moving", m_mobility);
	cmd.AddValue ("scenario", "Scenario: 1=random, 2=real world", m_scenario);
	cmd.AddValue ("animation", "Enable netanim animation.", m_animation);
	cmd.AddValue ("totalTime", "Simulation end time", m_totalSimTime);

	cmd.Parse (argc, argv);
}

void
DroneExperiment::SetupScenario ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure current scenario (" << m_scenario << ").");

	if (m_scenario == 1)
	{
		m_mobility = 1;
		m_nNodes = 10;
	}
	else if (m_scenario == 2)
	{
		// TODO
	}
	else
		NS_LOG_ERROR ("Invalid scenario specified. Values must be [1-2].");
}

void
DroneExperiment::RunSimulation ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Run simulation...");

	Run ();
}

void
DroneExperiment::ProcessOutputs ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Process outputs.");
}

void
DroneExperiment::Run ()
{
	NS_LOG_FUNCTION (this);

	if (m_animation != 0)
	{
		// Create the animation object and configure for specified output
		AnimationInterface anim (m_animFile);
		anim.SetMobilityPollInterval (Seconds (0.250));
		anim.EnablePacketMetadata (true);
		// anim.EnableIpv4L3ProtocolCounters (Seconds (1), Seconds (m_totalSimTime));
		// anim.EnableWifiMacCounters (Seconds (1), Seconds (m_totalSimTime));
		// anim.EnableWifiPhyCounters (Seconds (1), Seconds (m_totalSimTime));
	}

	Simulator::Stop (Seconds (m_totalSimTime));
	Simulator::Run ();
	Simulator::Destroy ();
}

void
DroneExperiment::CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility)
{
	NS_LOG_FUNCTION ( &os << foo << mobility);	// problem with the argument *os

  Vector pos = mobility->GetPosition (); // Get position
  Vector vel = mobility->GetVelocity (); // Get velocity

  int nodeId = mobility->GetObject<Node> ()->GetId ();

  NS_LOG_DEBUG ("Changing pos for node " << nodeId << " at " << Simulator::Now ().GetSeconds ()
	 							<< "; POS: (" << pos.x << ", " << pos.y << ", " << pos.z << ")"
								<< "; VEL: (" << vel.x << ", " << vel.y << ", " << vel.z << ").");
}


/* -----------------------------------------------------------------------------
*			MAIN
* ------------------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
	NS_LOG_UNCOND ("Drone Routing Experiment.");

	DroneExperiment experiment;
	experiment.Simulate (argc, argv);
}
