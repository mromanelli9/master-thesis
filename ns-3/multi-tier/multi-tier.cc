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

#include "ns3/core-module.h"

#include "Tier.h"

// DEBUG
#include "RoutingHelper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MultiTierExperiment");

/**
 * \ingroup object
 * \brief The MultiTier class implements the main simulation
 *
 */
class MultiTier
{
public:
	/**
	 * \brief Constructor
	 * \return none
	 */
	MultiTier ();

	/**
	 * \brief Destructor
	 * \return none
	 */
	virtual ~MultiTier ();

	/**
	 * \brief Enacts simulation of an ns-3  application
	 * \param argc program arguments count
	 * \param argv program arguments
	 * \return none
	 */
	void Simulate (int argc, char **argv);

private:
	/**
	 * \brief Process command line arguments
	 * \param argc program arguments count
	 * \param argv program arguments
	 * \return none
	 */
	void ParseCommandLineArguments (int argc, char **argv);

	/**
	 * \brief Check values parsed through command line
	 * \return none
	 */
	void CheckCommandLineValues ();

	/**
	 * \brief Set up a prescribed scenario
	 * \return none
	 */
	void SetupScenario ();

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

	uint32_t 													m_nNodes;	// total number of nodes
	uint32_t 													m_nDrones;	// total number of drones
	uint32_t 													m_nVeichles;	// total number of vehicles
	uint32_t													m_dataStartTime;	// Time at which nodes start to transmit data
	uint32_t													m_totalSimTime;	// simulation time
	Ptr<Tier>													m_vehiclesTier;	// lower tier
	Ptr<Tier>													m_droneTier;	// upper tier
};

/* -----------------------------------------------------------------------------
*			METHOD DEFINITIONS
* ------------------------------------------------------------------------------
*/

MultiTier::MultiTier ()
	:	m_nNodes (0),
		m_nDrones (0),
		m_nVeichles (0),
		m_dataStartTime (1),
		m_totalSimTime (10)
{
	// Init ns3 pseudo-random number generator seed
	RngSeedManager::SetSeed (time (0));

	// Set the index of the current run (default = 0)
	RngSeedManager::SetRun (0);
}


MultiTier::~MultiTier ()
{
	// m_vehiclesTier = 0;
	// m_droneTier = 0;
}

void
MultiTier::Simulate (int argc, char **argv)
{
	// Initial configuration and parameters parsing
	ParseCommandLineArguments (argc, argv);
	SetupScenario ();

	// Configure the network and all the elements in it


	// Run simulation and print some results
	RunSimulation ();
	ProcessOutputs ();
}

void
MultiTier::ParseCommandLineArguments (int argc, char **argv)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Parsing command line arguments.");

	CommandLine cmd;

	// allow command line overrides
	cmd.AddValue ("nodes", "Total number of nodes", m_nNodes);
	cmd.AddValue ("drones", "Number of drones", m_nDrones);
	cmd.AddValue ("vehicles", "Number of vehicles", m_nVeichles);
	cmd.AddValue ("dataStart", "Time at which nodes start to transmit data [seconds]", m_dataStartTime);
	cmd.AddValue ("totalTime", "Simulation end time [seconds]", m_totalSimTime);

	cmd.Parse (argc, argv);

	CheckCommandLineValues ();
}

void
MultiTier::CheckCommandLineValues ()
{
	NS_LOG_FUNCTION (this);

	NS_ASSERT_MSG ((m_nDrones + m_nVeichles) <= m_nNodes, "Invalid number of drones or vehicles. Value must be less or egual to the total number of nodes.");
}

void
MultiTier::SetupScenario ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Configure current scenario.");

	m_nNodes = 10;

	// // Create the bottom tier with vehicles
	// m_vehiclesTier = CreateObject<Tier> ();

	// Create the top tier with drones
	// m_droneTier = CreateObject<Tier> ();
}

void
MultiTier::RunSimulation ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Run simulation...");

	Simulator::Stop (Seconds (m_totalSimTime));
	Simulator::Run ();

	Simulator::Destroy ();
}

void
MultiTier::ProcessOutputs ()
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Process outputs.");
}

// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
	NS_LOG_UNCOND ("Multi-tier experiment.");

	RoutingHelper stats;

	// MultiTier experiment;
	// experiment.Simulate (argc, argv);


}
