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


	std::string 						m_CSVfileName;
	double									m_TotalSimTime;
};

FBVanetExperiment::FBVanetExperiment ()
  : m_CSVfileName ("manet-routing.output.csv"),
		m_TotalSimTime (300.01)
{
}

FBVanetExperiment::~FBVanetExperiment ()
{
}


void
FBVanetExperiment::Simulate (int argc, char **argv)
{
	NS_LOG_INFO ("Enter 'FB Vanet Experiment' enviroment.");

  ParseCommandLineArguments (argc, argv);
	// varie configurazionei
	RunSimulation ();
	ProcessOutputs ();
}

void
FBVanetExperiment::ParseCommandLineArguments (int argc, char **argv)
{
	CommandSetup (argc, argv);
	SetupScenario ();
}

void
FBVanetExperiment::CommandSetup (int argc, char **argv)
{
	CommandLine cmd;

	// allow command line overrides
	cmd.AddValue ("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
	cmd.AddValue ("totaltime", "Simulation end time", m_TotalSimTime);

	cmd.Parse (argc, argv);
}

void
FBVanetExperiment::SetupScenario ()
{
	NS_LOG_INFO ("Setup current scenario.");

	// vari scenari.
	// alemmno 2 o 3
}

void
FBVanetExperiment::RunSimulation ()
{
	NS_LOG_INFO ("Run simulation.");

  Run ();
}

void
FBVanetExperiment::ProcessOutputs ()
{
	NS_LOG_INFO ("Process outputs.");

}

void
FBVanetExperiment::Run ()
{
  NS_LOG_INFO ("Run Simulation.");

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
