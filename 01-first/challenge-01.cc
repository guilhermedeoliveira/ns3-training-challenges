/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
    #include "ns3/network-module.h"
    #include "ns3/internet-module.h"
    #include "ns3/point-to-point-module.h"
    #include "ns3/applications-module.h"
    
    using namespace ns3;
    
    NS_LOG_COMPONENT_DEFINE ("FirstChallenge01");
    
    int
    main (int argc, char *argv[])
    {
      Time::SetResolution (Time::NS);
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    
      NodeContainer nodes;
      nodes.Create (2);
    
      PointToPointHelper pointToPoint;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    
      NetDeviceContainer devices;
      devices = pointToPoint.Install (nodes);
    
      InternetStackHelper stack;
      stack.Install (nodes);
    
      Ipv4AddressHelper address;
      address.SetBase ("10.1.1.0", "255.255.255.0");
    
      Ipv4InterfaceContainer interfaces = address.Assign (devices);
      
      // application

	// first packet

      UdpEchoServerHelper echoServer (9);
    
      ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
      serverApps.Start (Seconds (1.0));
      serverApps.Stop (Seconds (20.0)); // finish
    
      UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
      echoClient.SetAttribute ("MaxPackets", UintegerValue (1)); 
      echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); 
      echoClient.SetAttribute ("PacketSize", UintegerValue (1024)); // first packet size
    
      ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
      clientApps.Start (Seconds (2.0)); // send
      clientApps.Stop (Seconds (20.0)); // finish
    
	// second packet

      UdpEchoClientHelper echoClient2 (interfaces.GetAddress (1), 9);
      echoClient2.SetAttribute ("MaxPackets", UintegerValue (1)); 
      echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0))); 
      echoClient2.SetAttribute ("PacketSize", UintegerValue (2048)); // second packet size
    
      ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get (0));
      clientApps2.Start (Seconds (6.0)); // send
      clientApps2.Stop (Seconds (20.0)); // finish
    

      Simulator::Run ();
      Simulator::Destroy ();
      return 0;
    }
