/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
  /*
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
   */
   
   #include "ns3/core-module.h"
   #include "ns3/point-to-point-module.h"
   #include "ns3/network-module.h"
   #include "ns3/applications-module.h"
   #include "ns3/wifi-module.h"
   #include "ns3/mobility-module.h"
   #include "ns3/csma-module.h"
   #include "ns3/internet-module.h"
   
   // Default Network Topology
   //
   //   Wifi 192.168.1.0
   //                 AP
   //  *    *    *    *
   //  |    |    |    |    10.1.1.0
   // n5   n6   n7   n0 -------------- n1   n2   n3   n4
   //                   point-to-point  |    |    |    |
   //                                   ================
   //                                     LAN 10.1.2.0
   
   // How to run: ./waf --run "ThirdChallenge01 --nCsma=5 --nWifi=5"

   using namespace ns3;
   
   NS_LOG_COMPONENT_DEFINE ("ThirdChallenge01");
   
   int 
   main (int argc, char *argv[])
   {
     bool verbose = true;
     uint32_t nCsma = 3;
     uint32_t nWifi = 3;
   
     CommandLine cmd;
     cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
     cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
     cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
   
     cmd.Parse (argc,argv);
   
     if (nWifi > 18)
       {
         std::cout << "Number of wifi nodes " << nWifi << 
                      " specified exceeds the mobility bounding box" << std::endl;
         exit (1);
       }
   
     if (verbose)
       {
         LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
         LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
       }
    
     NodeContainer p2pNodes;
     p2pNodes.Create (2);
    
     PointToPointHelper pointToPoint;
     pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
     pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    
     NetDeviceContainer p2pDevices;
     p2pDevices = pointToPoint.Install (p2pNodes);
    
     NodeContainer csmaNodes;
     csmaNodes.Add (p2pNodes.Get (1));
     csmaNodes.Create (nCsma);
    
     CsmaHelper csma;
     csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
     csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
    
     NetDeviceContainer csmaDevices;
     csmaDevices = csma.Install (csmaNodes);
    
     NodeContainer wifiStaNodes;
     wifiStaNodes.Create (nWifi);
     NodeContainer wifiApNode = p2pNodes.Get (0);
    
     YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
     YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
     phy.SetChannel (channel.Create ());
    
     WifiHelper wifi = WifiHelper::Default ();
     wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
    
     NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
   
     Ssid ssid = Ssid ("ns-3-ssid");
     mac.SetType ("ns3::StaWifiMac",
                  "Ssid", SsidValue (ssid),
                  "ActiveProbing", BooleanValue (false));
   
     NetDeviceContainer staDevices;
     staDevices = wifi.Install (phy, mac, wifiStaNodes);
   
     mac.SetType ("ns3::ApWifiMac",
                  "Ssid", SsidValue (ssid));
   
     NetDeviceContainer apDevices;
     apDevices = wifi.Install (phy, mac, wifiApNode);
   
     MobilityHelper mobility;
   
     mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (0.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (5.0),
                                    "DeltaY", DoubleValue (10.0),
                                    "GridWidth", UintegerValue (3),
                                    "LayoutType", StringValue ("RowFirst"));
   
     mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
     mobility.Install (wifiStaNodes);
   
     mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
     mobility.Install (wifiApNode);
   
     InternetStackHelper stack;
     stack.Install (csmaNodes);
     stack.Install (wifiApNode);
     stack.Install (wifiStaNodes);
   
     Ipv4AddressHelper address;
   
     address.SetBase ("10.1.1.0", "255.255.255.0"); 
     Ipv4InterfaceContainer p2pInterfaces;
     p2pInterfaces = address.Assign (p2pDevices);
   
     address.SetBase ("10.1.2.0", "255.255.255.0");
     Ipv4InterfaceContainer csmaInterfaces;
     csmaInterfaces = address.Assign (csmaDevices);
   
     address.SetBase ("192.168.1.0", "255.255.255.0"); // address
     address.Assign (staDevices);
     address.Assign (apDevices);
   
     // application

     UdpEchoServerHelper echoServer (7); // server port
   
     ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma - 1)); // server (nCsma - 1)
     serverApps.Start (Seconds (1.0));
     serverApps.Stop (Seconds (25.0)); // set end
   
	// first packet

     UdpEchoClientHelper echoClient1 (csmaInterfaces.GetAddress (nCsma - 1), 7); // server (nCsma - 1) && port = 7
     echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
     echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
     echoClient1.SetAttribute ("PacketSize", UintegerValue (512)); // 512 bytes
   
     ApplicationContainer clientApps1 = echoClient1.Install (wifiStaNodes.Get (0)); // STA 1
     clientApps1.Start (Seconds (5.0)); // send
     clientApps1.Stop (Seconds (25.0)); // finish
   
	// second packet

     UdpEchoClientHelper echoClient2 (csmaInterfaces.GetAddress (nCsma - 1), 7); // server (nCsma - 1) && port = 7
     echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
     echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
     echoClient2.SetAttribute ("PacketSize", UintegerValue (1024)); // double packet
   
     ApplicationContainer clientApps2 = echoClient2.Install (wifiStaNodes.Get (1)); // STA 2
     clientApps2.Start (Seconds (10.0)); // send
     clientApps2.Stop (Seconds (25.0)); // finish

	// third packet

     UdpEchoClientHelper echoClient3 (csmaInterfaces.GetAddress (nCsma - 1), 7); // server (nCsma - 1) && port = 7
     echoClient3.SetAttribute ("MaxPackets", UintegerValue (1));
     echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
     echoClient3.SetAttribute ("PacketSize", UintegerValue (2048)); // double packet
   
     ApplicationContainer clientApps3 = echoClient3.Install (wifiStaNodes.Get (2)); // STA 3
     clientApps3.Start (Seconds (15.0)); // send
     clientApps3.Stop (Seconds (25.0)); // finish

     Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
   
     Simulator::Stop (Seconds (25.0)); // end
   
     pointToPoint.EnablePcapAll ("ThirdChallenge01");
     phy.EnablePcap ("ThirdChallenge01", apDevices.Get (0));
     csma.EnablePcap ("ThirdChallenge01", csmaDevices.Get (0), true);
   
     Simulator::Run ();
     Simulator::Destroy ();
     return 0;
   }
