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

#ifndef TIER_H
#define TIER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"

#include "RoutingHelper.h"

namespace ns3 {
/**
 * \ingroup wave
 * \brief The Tier class enforces program flow for a single tier
 */
class Tier : public Object
{
public:
	static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   * \return none
   */
  Tier ();

  /**
   * \brief Destructor
   * \return none
   */
   virtual ~Tier ();

  /**
   * \brief Enacts configuration of a tier
   * \return none
   */
  void Install ();

protected:
  /**
   * \brief Sets default attribute values
   * \return none
   */
   void ConfigureDefaults ();

  /**
   * \brief Configure nodes
   * \return none
   */
   void ConfigureNodes ();

  /**
   * \brief Configure channels
   * \return none
   */
   void ConfigureChannels ();

  /**
   * \brief Configure mobility
   * \return none
   */
   void ConfigureMobility ();

  /**
   * \brief Configure applications
   * \return none
   */
   void ConfigureApplications ();

  /**
   * \brief Configure tracing
   * \return none
   */
   void ConfigureTracing ();

  /**
   * \brief Process outputs
   * \return none
   */
   void ProcessOutputs ();

	 /**
		* \brief Set up generation of packets to be routed
		* through the vehicular network
		* \return none
		*/
	 void SetupRoutingMessages ();

 private:
	 uint32_t												m_nNodes;	// number of nodes
	 NodeContainer									m_nodes;	// nodes in this tier
	 NetDeviceContainer							m_devices;	// net devices
	 Ipv4InterfaceContainer					m_interfaces;	// ipv4 interfaces
	 std::string										m_packetSize; // size of the packets
	 std::string										m_rate;	// data rate
	 uint32_t												m_protocol; // routing protocol
	 std::string										m_protocolName; // routing protocol name
	 std::string										m_phyMode;	// Wifi Phy mode
	 uint32_t												m_80211mode;	// 1=802.11p; 2=802.11b; 3=WAVE-PHY
	 std::string										m_phyModeB;	// Phy mode 802.11b
	 uint32_t												m_lossModel; // propagation loss model
	 std::string										m_lossModelName; // propagation loss model name
	 uint32_t												m_loadBuildings;	// enable ObstacleShadowingPropagationLossModel
	 uint32_t												m_txp; // transmit power [db]
	 uint32_t												m_mobility; // mobility mode
	 std::string										m_traceFile; // trace file path
	 Ptr<RoutingHelper> 						m_routingHelper;	// routingHelper instance
	 double													m_dataStartTime;	// Time at which nodes start to transmit data
	 double													m_totalSimTime; // simulation end time
};
} // namespace ns3

#endif /* TIER_H */
