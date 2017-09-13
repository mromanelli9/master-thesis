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
//using namespace dsr;

NS_LOG_COMPONENT_DEFINE ("Wireless");

//Avoid useless messages on the screen
int state=-1;
bool flooding=true;
int received=0, sent=0;

class RoutingExperiment
{
	public:
	RoutingExperiment ();
	void Run (double txp, std::string CSVfileName);
	std::string CommandSetup (int argc, char **argv);

	private:
	Ptr<Socket> SetupPacketReceive (Ipv4Address addr, Ptr<Node> node);
	Ptr<Socket> SetupPacketSend (Ipv4Address addr, Ptr<Node> node);
	static void ReceivePacket (Ptr<Socket> socket);
	static void GenerateHelloTraffic (Ptr<Socket> socket);
	static void GenerateAlertTraffic (Ptr<Node> node, NodeContainer adhocNodes);
	static void HandleHello (Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress, MyHeader head);
	static void HandleAlert (Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress, MyHeader head, int distance, int phase);
	static void Broad (Ptr<Node> node, int phase, uint32_t rs, int sx, int sy);
	static void WaitAgain (Ptr<Node> node, int phase, int range, uint32_t rs, std::vector<int> par);
	static void PrintPositions(Ptr<Node> node);
	static void StopNode(Ptr<Node> node);
	static int GetNodeXPosition(Ptr<Node> node);
	static int GetNodeYPosition(Ptr<Node> node);
	static double CalculateDistance (int x1, int y1, int x2, int y2);
	static void Hello(NodeContainer adhocNodes, int count);
	static Ipv4Address GetAddress (Ptr<Node> node);

	uint32_t port;
	std::string m_CSVfileName;
	double m_txp;
	bool m_traceMobility;
	static const int m_range=300;
};

RoutingExperiment::RoutingExperiment ()
: port (9),
m_CSVfileName ("manet-routing.output.csv"),
m_traceMobility (false)
{
}

Ipv4Address
RoutingExperiment::GetAddress (Ptr<Node> node)
{
	Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
	Ipv4InterfaceAddress iaddr2 = ipv4->GetAddress (1,0);
	Ipv4Address addri = iaddr2.GetLocal ();
	return addri;
}

int
RoutingExperiment::GetNodeXPosition (Ptr<Node> node)
{
	Ptr<MobilityModel> positionmodel = node->GetObject<MobilityModel> ();
	Vector pos = positionmodel->GetPosition ();
	return pos.x;
}

int
RoutingExperiment::GetNodeYPosition (Ptr<Node> node)
{
	Ptr<MobilityModel> positionmodel = node->GetObject<MobilityModel> ();
	Vector pos = positionmodel->GetPosition ();
	return pos.y;
}

double
RoutingExperiment::CalculateDistance (int x1, int y1, int x2, int y2)
{
	int distancex = (x2 - x1) * (x2 - x1);
	int distancey = (y2 - y1) * (y2 - y1);
	double distance = sqrt(distancex + distancey);
	return distance;
}

void
RoutingExperiment::HandleHello (Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress, MyHeader head)
{
	Ptr<Node> node= socket->GetNode();
	//CMFR received
	int CMFR = head.getCMFR();
	//My position
	int myXPosition=GetNodeXPosition(socket->GetNode());
	int myYPosition=GetNodeYPosition(socket->GetNode());
	//Sender Position
	int senderXPosition=head.getSenderXPosition();
	int senderYPosition=head.getSenderYPosition();
	uint32_t distance=CalculateDistance(senderXPosition, senderYPosition, myXPosition, myYPosition);
	//My CMBR
	int myCMBR=node->GetCMBR();

	int maxi;
	uint32_t m0=std::max(myCMBR, CMFR);
	maxi=std::max(m0, distance);
	node->SetCMBR(maxi);
	//Saving CMBR in LMBR
	node->SetLMBR(myCMBR);
}

//1: Hello message
//2: Alert message
void
RoutingExperiment::ReceivePacket (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address senderAddress;
	Ptr<Node> node= socket->GetNode();
	while ((packet = socket->RecvFrom (senderAddress)))
	{
		MyHeader head;
		packet->RemoveHeader(head);
		//Type
		uint32_t type=head.getType();
		//Hello message
		if(type==1)
		{
			//My position
			int myXPosition=GetNodeXPosition(socket->GetNode());
			int myYPosition=GetNodeYPosition(socket->GetNode());
			//Sender Position
			int senderXPosition=head.getSenderXPosition();
			int senderYPosition=head.getSenderYPosition();
			double myDistance2=CalculateDistance(senderXPosition, senderYPosition, myXPosition, myYPosition);
			//If the node is in range, I can read the packet
			if(myDistance2<=RoutingExperiment::m_range)
				HandleHello (socket, packet, senderAddress, head);
		}
		//Alert message
		else if (type==2)
		{
			//My position
			int myXPosition=GetNodeXPosition(socket->GetNode());
			int myYPosition=GetNodeYPosition(socket->GetNode());
			//Sender Position
			int senderXPosition=head.getSenderXPosition();
			int senderYPosition=head.getSenderYPosition();
			//Start position
			int startXPosition=head.getStartXPosition();
			int startYPosition=head.getStartYPosition();
			//Distances
			double myDistance=CalculateDistance(myXPosition, myYPosition, startXPosition, startYPosition);
			double senderDistance=CalculateDistance(senderXPosition, senderYPosition, startXPosition, startYPosition);
			double myDistance2=CalculateDistance(senderXPosition, senderYPosition, myXPosition, myYPosition);
			int phase=head.getPhase();
			//If the node is in range, I can read the packet
			if( (std::abs(myDistance2)<=RoutingExperiment::m_range))
			{
				received++;
				if (myDistance>senderDistance && !socket->GetNode()->GetReceived())
				{
					uint32_t sl=head.getSlot();
					socket->GetNode()->SetSlot(socket->GetNode()->GetSlot()+sl);
					StopNode(socket->GetNode());
					socket->GetNode()->SetReceived(true);
					if(socket->GetNode()->GetNum()==0)
						socket->GetNode()->SetNum(phase);
					HandleAlert(socket, packet, senderAddress, head, myDistance2, phase);
				}
				else
				{
					if (node->GetPhase()<phase)
						node->SetPhase(phase);
				}
			}
		}
	}
}

void
RoutingExperiment::Broad (Ptr<Node> node, int phase, uint32_t rs, int sx, int sy)
{
	//If I'm the first to wake up, I must forward the message
	if ( (!flooding && phase>=node->GetPhase()) || (flooding && !node->GetSent()) )
	//if(phase>=node->GetPhase())
	{
		//Create a packet with the correct parameters taken from the node
		Ptr<Packet> p= Create<Packet> (100);
		MyHeader header;
		header.setType(2);
		uint32_t LMBR=node->GetLMBR();
		uint32_t CMBR=node->GetCMBR();
		uint32_t maxi=std::max(LMBR, CMBR);
		header.setSenderXPosition(GetNodeXPosition(node));
		header.setSenderYPosition(GetNodeYPosition(node));
		header.setStartXPosition(sx);
		header.setStartYPosition(sy);
		header.setMax(maxi);
		header.setPhase(phase+1);
		header.setSlot(node->GetSlot()+rs);
		p->AddHeader(header);
		Ptr<Socket>sock=node->getBroadcast();
		//My address
		Ipv4Address addri = GetAddress(node);
		state=phase;
		std::cout<<"\n"<<addri<<" ("<<phase<< "): forwarding alert";
		sock->Send(p);
		node->SetSent(true);
		sent++;
	}
}

void
RoutingExperiment::HandleAlert (Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress, MyHeader head, int distance, int phase)
{
	Ptr<Node> node= socket->GetNode();
	int myPhase=node->GetPhase();
	//If it's from the front
	if(phase>myPhase)
	{
		node->SetPhase(phase);
		//Calculate the window
		int BMR=node->GetCMBR();
		int cwMin=32, cwMax=1024;
		int dist=std::abs(distance);
		double rapp=(double)(BMR-dist)/BMR;
		if (rapp<0)
			rapp=0;
		int range=(rapp*(cwMax-cwMin))+cwMin;
		//Random value
		uint32_t rs = (rand() % range)+1;
		int startXPosition=head.getStartXPosition();
		int startYPosition=head.getStartYPosition();
		std::vector<int> par;
		par.push_back(startXPosition);
		par.push_back(startYPosition);
		if(flooding==false)
			Simulator::ScheduleWithContext (socket->GetNode ()->GetId (), MilliSeconds(rs*200*3), &RoutingExperiment::WaitAgain, socket->GetNode(), phase, range, rs, par);
		else
			Simulator::ScheduleWithContext (socket->GetNode ()->GetId (), MilliSeconds(0), &RoutingExperiment::Broad, socket->GetNode(), phase, rs, par.at(0), par.at(1));
	}
}

void
RoutingExperiment::WaitAgain (Ptr<Node> node, int phase, int range, uint32_t rs, std::vector<int> par)
{
	if(phase>=node->GetPhase())
	{
		uint32_t rnd = (rand() % 20)+1;
		uint32_t rnd1 = (rand() % 20)+1;
		uint32_t rnd2 = (rand() % 20)+1;
		uint32_t rnd3 = (rand() % 20)+1;
		Simulator::ScheduleWithContext (node->GetId (), MilliSeconds(10* (rs+rnd+rnd1+rnd2+rnd3)*200*3), &RoutingExperiment::Broad, node, phase, rs, par.at(0), par.at(1));
	}
}

//A node sends a Hello Message to all the nodes in its range
void
RoutingExperiment::GenerateHelloTraffic (Ptr<Socket> socket)
{
	//Create a packet with the correct parameters taken from the node
	Ptr<Node> node= socket->GetNode();
	Ptr<Packet> p= Create<Packet> (100);
	MyHeader header;
	header.setCMFR(node->GetCMBR());
	header.setStartXPosition(GetNodeXPosition(node));
	header.setStartYPosition(GetNodeYPosition(node));
	header.setSenderXPosition(GetNodeXPosition(node));
	header.setSenderYPosition(GetNodeYPosition(node));
	header.setType(1);
	p->AddHeader(header);
	Ptr<Socket> sock=node->getBroadcast();
	sock->Send(p);
}

//The first node send an Alert Message to all the nodes that are in its range
void
RoutingExperiment::GenerateAlertTraffic (Ptr<Node> node, NodeContainer adhocNodes)
{
	//Create a packet with the correct parameters taken from the node
	Ptr<Packet> p= Create<Packet> (100);
	MyHeader header;
	header.setType(2);
	uint32_t LMBR=node->GetLMBR();
	uint32_t CMBR=node->GetCMBR();
	uint32_t maxi=std::max(LMBR, CMBR);
	header.setStartXPosition(GetNodeXPosition(node));
	header.setStartYPosition(GetNodeYPosition(node));
	header.setSenderXPosition(GetNodeXPosition(node));
	header.setSenderYPosition(GetNodeYPosition(node));
	header.setMax(maxi);
	header.setPhase(0);
	header.setSlot(0);
	p->AddHeader(header);
	Ptr<Socket> sock=node->getBroadcast();
	//My address
	Ipv4Address addri = GetAddress(node);
	std::cout<<"\n"<<addri<<": sending Alert";
	sock->Send(p);
	node->SetSent(true);
	StopNode(node);
	sent++;
}

//Setup receivers
Ptr<Socket>
RoutingExperiment::SetupPacketReceive (Ipv4Address addr, Ptr<Node> node)
{
	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Socket> sink = Socket::CreateSocket (node, tid);
	InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), port);
	sink->Bind (local);
	sink->SetRecvCallback (MakeCallback (&RoutingExperiment::ReceivePacket));

	return sink;
}

//Setup senders
Ptr<Socket>
RoutingExperiment::SetupPacketSend (Ipv4Address addr, Ptr<Node> node)
{
	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Socket> sender = Socket::CreateSocket (node, tid);
	InetSocketAddress remote = InetSocketAddress (addr, port);
	sender->Connect (remote);

	return sender;
}

std::string
RoutingExperiment::CommandSetup (int argc, char **argv)
{
	CommandLine cmd;
	cmd.AddValue ("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
	cmd.AddValue ("traceMobility", "Enable mobility tracing", m_traceMobility);
	cmd.Parse (argc, argv);
	return m_CSVfileName;
}

//Schedule who must send a hello message (several "intervals" with a random node)
void
RoutingExperiment::Hello (NodeContainer adhocNodes, int count)
{
	std::vector<int> he;
	uint hel=20;
	if(count>0)
	{
		for (uint i=0; i<hel; i++)
		{
			int pos=rand()%adhocNodes.GetN();
			he.push_back(pos);
			Ptr<Socket> sk=adhocNodes.Get(pos)->getBroadcast();
			Simulator::ScheduleWithContext (adhocNodes.Get(pos)->GetId (), Seconds (i*40), &RoutingExperiment::GenerateHelloTraffic, sk);
		}
		//Other nodes must send Hello messages
		Simulator::Schedule (Seconds (250), &RoutingExperiment::Hello, adhocNodes, count -1);
	}
}

//Print a node current position
void
RoutingExperiment::PrintPositions (Ptr<Node> node)
{
	Ptr<MobilityModel> positionmodel = node->GetObject<MobilityModel> ();
	Vector pos = positionmodel->GetPosition ();
	std::cout << "\nx= "<< pos.x << "; y=" << pos.y << "; z=" << pos.z;
}

//Stop a node
void
RoutingExperiment::StopNode (Ptr<Node> node)
{
	Ptr<ConstantVelocityMobilityModel> mob = node->GetObject<ConstantVelocityMobilityModel>();
	mob->SetVelocity(Vector(0, 0, 0));
}

int
main (int argc, char *argv[])
{
	std::cout<<"Main\n";

	RoutingExperiment experiment;
	std::string CSVfileName = experiment.CommandSetup (argc,argv);
	double txp = 7.5;

	std::cout<<"Starting\n";
	experiment.Run (txp, CSVfileName);
}

void
RoutingExperiment::Run (double txp, std::string CSVfileName)
{
	Packet::EnablePrinting ();
	m_txp = txp;

	std::cout<<"Setting parameters\n";
	srand(time(0));
	//Starting CMFR, LMFR, CMBR and LMBR
	int param = 300; //0 for FB, 300 for C300 and 1000 for C1000
	//Number of cars
	int nWifis = 0;
	int dist=12;
	uint start=2242;  //12m=2242; 24m=1137;
	//Simulation time
	double TotalTime = 990000.0;
	//Road
	uint32_t road = 4000;
	uint32_t bord= (road/300)-1;
	int rCirc=1000;

	//Other parameters
	std::string rate ("2048bps");
	std::string phyMode ("DsssRate11Mbps");
	Config::SetDefault  ("ns3::OnOffApplication::PacketSize",StringValue ("64"));
	Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (rate));
	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",StringValue (phyMode));

	NodeContainer adhocNodes;
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

	std::cout<<"Creating nodes\n";

	//Left side
	uint32_t borders=0, cont=0;
	while (cont<bord)
	{
		adhocNodes.Add(CreateObject<Node> ());
		adhocNodes.Get(borders)->SetCMFR(param);
		adhocNodes.Get(borders)->SetLMFR(param);
		adhocNodes.Get(borders)->SetCMBR(param);
		adhocNodes.Get(borders)->SetLMBR(param);
		adhocNodes.Get(borders)->SetNum(0);
		adhocNodes.Get(borders)->SetPhase(-1);
		adhocNodes.Get(borders)->SetSent(false);
		adhocNodes.Get(borders)->SetReceived(false);
		adhocNodes.Get(borders)->SetSlot(0);
		positionAlloc->Add (Vector (0.0, (cont+1)*300, 0.0));

		cont++;
		borders++;
		nWifis++;
	}
	cont=0;
	//Bottom side
	while (cont<bord)
	{
		adhocNodes.Add(CreateObject<Node> ());
		adhocNodes.Get(borders)->SetCMFR(param);
		adhocNodes.Get(borders)->SetLMFR(param);
		adhocNodes.Get(borders)->SetCMBR(param);
		adhocNodes.Get(borders)->SetLMBR(param);
		adhocNodes.Get(borders)->SetNum(0);
		adhocNodes.Get(borders)->SetPhase(-1);
		adhocNodes.Get(borders)->SetSent(false);
		adhocNodes.Get(borders)->SetReceived(false);
		adhocNodes.Get(borders)->SetSlot(0);
		positionAlloc->Add (Vector ((cont+1)*300, road, 0.0));

		cont++;
		borders++;
		nWifis++;
	}
	cont=0;
	//Right side
	while (cont<bord)
	{
		adhocNodes.Add(CreateObject<Node> ());
		adhocNodes.Get(borders)->SetCMFR(param);
		adhocNodes.Get(borders)->SetLMFR(param);
		adhocNodes.Get(borders)->SetCMBR(param);
		adhocNodes.Get(borders)->SetLMBR(param);
		adhocNodes.Get(borders)->SetNum(0);
		adhocNodes.Get(borders)->SetPhase(-1);
		adhocNodes.Get(borders)->SetSent(false);
		adhocNodes.Get(borders)->SetReceived(false);
		adhocNodes.Get(borders)->SetSlot(0);
		positionAlloc->Add (Vector (road, (cont+1)*300, 0.0));


		cont++;
		borders++;
		nWifis++;
	}
	cont=0;
	//Top side
	while (cont<bord)
	{
		adhocNodes.Add(CreateObject<Node> ());
		adhocNodes.Get(borders)->SetCMFR(param);
		adhocNodes.Get(borders)->SetLMFR(param);
		adhocNodes.Get(borders)->SetCMBR(param);
		adhocNodes.Get(borders)->SetLMBR(param);
		adhocNodes.Get(borders)->SetNum(0);
		adhocNodes.Get(borders)->SetPhase(-1);
		adhocNodes.Get(borders)->SetSent(false);
		adhocNodes.Get(borders)->SetReceived(false);
		adhocNodes.Get(borders)->SetSlot(0);
		positionAlloc->Add (Vector ((cont+1)*300, 0.0, 0.0));

		cont++;
		borders++;
		nWifis++;
	}
	//Other nodes
	for (uint i=0; i<bord; i++)
	{
		int r=road;
		int d=dist;
		while(r>0)
		{
			adhocNodes.Add(CreateObject<Node> ());
			adhocNodes.Get(nWifis)->SetCMFR(param);
			adhocNodes.Get(nWifis)->SetLMFR(param);
			adhocNodes.Get(nWifis)->SetCMBR(param);
			adhocNodes.Get(nWifis)->SetLMBR(param);
			adhocNodes.Get(nWifis)->SetNum(0);
			adhocNodes.Get(nWifis)->SetPhase(-1);
			adhocNodes.Get(nWifis)->SetSent(false);
			adhocNodes.Get(nWifis)->SetReceived(false);
			adhocNodes.Get(nWifis)->SetSlot(0);
			positionAlloc->Add (Vector ((i+1)*300, d, 0.0));

			borders++;
			d=d+dist;
			r=r-dist;
			nWifis++;
		}
	}
	for (uint i=0; i<bord; i++)
	{
		int r=road;
		int d=dist;
		while(r>0)
		{
			adhocNodes.Add(CreateObject<Node> ());
			adhocNodes.Get(nWifis)->SetCMFR(param);
			adhocNodes.Get(nWifis)->SetLMFR(param);
			adhocNodes.Get(nWifis)->SetCMBR(param);
			adhocNodes.Get(nWifis)->SetLMBR(param);
			adhocNodes.Get(nWifis)->SetNum(0);
			adhocNodes.Get(nWifis)->SetPhase(-1);
			adhocNodes.Get(nWifis)->SetSent(false);
			adhocNodes.Get(nWifis)->SetReceived(false);
			adhocNodes.Get(nWifis)->SetSlot(0);
			positionAlloc->Add (Vector (d, (i+1)*300, 0.0));

			borders++;
			d=d+dist;
			r=r-dist;
			nWifis++;
		}
	}

	std::cout<<"Nodes created\n";
	//Nodes with constant speed
	std::cout<<"Setting mobility model\n";
	mobility.SetPositionAllocator (positionAlloc);
	mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
	mobility.Install (adhocNodes);
	for (uint n=0 ; n < adhocNodes.GetN() ; n++)
	{
		Ptr<ConstantVelocityMobilityModel> mob = adhocNodes.Get(n)->GetObject<ConstantVelocityMobilityModel>();
		//mob->SetVelocity(Vector(-1, 0, 0));
		mob->SetVelocity(Vector(0, 0, 0));
	}
	std::cout<<"Mobility model set\n";

	//Setting up wifi phy and channel using helpers
	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

	//Setting max range
	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (RoutingExperiment::m_range+100));
	wifiPhy.SetChannel (wifiChannel.Create ());

	//Add a mac and disable rate control
	WifiMacHelper wifiMac;
	//NqosWifiMacHelper wifiMac=NqosWifiMacHelper::Default();
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
	"DataMode",StringValue (phyMode),
	"ControlMode",StringValue (phyMode));
	wifiPhy.Set ("TxPowerStart",DoubleValue (txp));
	wifiPhy.Set ("TxPowerEnd", DoubleValue (txp));
	wifiMac.SetType ("ns3::AdhocWifiMac");
	NetDeviceContainer adhocDevices = wifi.Install (wifiPhy, wifiMac, adhocNodes);

	InternetStackHelper internet;
	internet.Install (adhocNodes);
	NS_LOG_INFO ("Sssigning ip address");

	Ipv4AddressHelper addressAdhoc;
	addressAdhoc.SetBase ("10.1.0.0", "255.255.0.0");
	Ipv4InterfaceContainer adhocInterfaces;
	adhocInterfaces = addressAdhoc.Assign (adhocDevices);

	OnOffHelper onoff1 ("ns3::UdpSocketFactory",Address ());
	onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
	onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

	std::cout<<"Setting connections\n";
	//Receiver j (da sistemare un attimo)
	for (int j = 0; j < nWifis; j++)
	{
		Ptr<Socket> sink = SetupPacketReceive (adhocInterfaces.GetAddress (j), adhocNodes.Get (j));
		AddressValue remoteAddress (InetSocketAddress (ns3::Ipv4Address::GetAny (), port));
		onoff1.SetAttribute ("Remote", remoteAddress);
		Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
	}
	//Sender i
	for (int i = 0; i < nWifis; i++)
	{
		//Sender i>broadcast
		Ptr<Socket> sender = SetupPacketSend (ns3::Ipv4Address("10.1.255.255"),  adhocNodes.Get (i));
		sender->SetAllowBroadcast (true);
		//Add the socket to the node
		adhocNodes.Get(i)->setBroadcast(sender);
	}
	std::cout<<"Connections set\n";

	//Hello messages
	//Simulator::Schedule (Seconds (500), &RoutingExperiment::Hello, adhocNodes, 60);
	std::cout<<"Hello scheduled\n";

	//Generate alert message
	Simulator::ScheduleWithContext (adhocNodes.Get (start)->GetId (), Seconds (45000), &RoutingExperiment::GenerateAlertTraffic, adhocNodes.Get (start), adhocNodes);
	std::cout<<"Alert scheduled\n";
	std::cout<<"Run Simulation\n\n----------------------------------------\n";
	NS_LOG_INFO ("Run Simulation.");

	Simulator::Stop (Seconds (TotalTime));

	//AnimationInterface anim ("/home/marco/repos/ns-3-allinone/ns-3-dev/scratch/Animation3D.xml");

	Simulator::Run ();

	int cover=1;
	int circ=0, circCont=0;
	for (uint i=0; i<adhocNodes.GetN(); i++)
	{

		//std::cout<<i<<": ("<<GetNodeXPosition(adhocNodes.Get (i))<<";"<<GetNodeYPosition(adhocNodes.Get (i))<<")\n";

		double distStart=CalculateDistance(GetNodeXPosition(adhocNodes.Get (i)),GetNodeYPosition(adhocNodes.Get (i)),GetNodeXPosition(adhocNodes.Get (start)),GetNodeYPosition(adhocNodes.Get (start)));
		if(i!=start && distStart>0 &&  ( (distStart-rCirc<=(dist/2) && distStart-rCirc>=0) || (rCirc-distStart<=(dist/2) && distStart-rCirc<=0)  ) )
		{
			circCont++;
			if(adhocNodes.Get(i)->GetReceived())
				circ++;
		}
		if(adhocNodes.Get(i)->GetReceived())
			cover++;
		else
		{
			std::cout<<i<<" didn't receive: ("<<GetNodeXPosition(adhocNodes.Get (i))<<";"<<GetNodeYPosition(adhocNodes.Get (i))<<")\n";
		}
		if (!adhocNodes.Get(i)->GetSent())
		{
			std::cout<<i<<" didn't send: ("<<GetNodeXPosition(adhocNodes.Get (i))<<";"<<GetNodeYPosition(adhocNodes.Get (i))<<")\n";
		}
	}

	std::cout << "\n"<<m_range<<" m/"<<param<<" max/"<<nWifis<<" cars/";

	std::cout<<"\n"<<cover<<"/"<<nWifis<<": "<<((double)cover/(double)nWifis)*100<<"%";

	std::cout<<"\n"<<circ<<"/"<<circCont<<": "<<((double)circ/(double)circCont)*100<<"%\n";

	for (uint i=0; i<(bord*4); i++)
	{
		std::cout<<adhocNodes.Get(i)->GetNum()<<" ";
	}
	std::cout<<"\n";

	std::cout<<"Messages sent: "<<sent<<"\n";

	std::cout<<"Messages received: "<<received<<"\n";

	Simulator::Destroy ();
}
