ns3 is a discrete event network simulators.

This repository contains some challenges based on the oficial tutorial and some codes presented in the package. If you are a beginner, it is a good way of 


				FIRST

this directory has three challenges about the first.cc example of the tutorial. Open first.cc and try the challenge

challenge-01
Send two packets with the following configuration:

	- First packet must be sent at 2 seconds;
	- Second packet must be twice the size of the first;
	- Second packet must be sent at 6 seconds;
	- Applications must be finished at 20 seconds.		

challenge-02
Keep the previous changes and transmit packets at a rate of 1 Kbps and 1 Gbps. Explain the difference in the log.

				SECOND

challenge-01
Send two equal size packets (without creating new client objects) with the following configuration:

	- Payloads = 2048 bytes;
	- Packets must be sent to port 7 of the server;
	- First packet must be sent at 2 seconds and the second at 8 seconds;
	- Applications must be finished at 25 seconds.

challenge-02
Keep the previous changes and try this new environment:
	- CSMA network must have 5 nodes (change via terminal, do not hardcode);
	- Server must be node 0;
	- Client must be the last CSMA node;

				THIRD

challenge-01
Open third.cc and set the environment. Send 3 packets with the following configuration:

	- Change Wi-Fi network address to 192.168.1.0;
	- Change server port number to 7;
	- Packets must have a difference of 5 seconds between them;
	- Application should end at 25 seconds;
	- First packet must be sent at 5 seconds;
	- First packet should be 512 bytes and each packet must double the previous in size;
	- Network must have 5 STAs and 5 CSMA nodes (it must be changed via terminal);
	- Server must be the last CSMA node;
	- First packet must be sent by STA 1;
	- Second packet must be sent by STA 2;
	- Third packet must be sent by STA 3.

challenge-02
NetAnim is an animation tool that works with files in XML format data generated from a simulation. Search how to configure NetAnim and generate an animation from the previous challenge. You may run a script in terminal by typing: bash script-example.sh

				RATE ADAPTATION

Go to examples/wireless directory and open rate-adaptation-distance.cc
Make a graph throughput vs. distance for the following rate adaptation algorithms:
	- Minstrel, Aarf, Onoe, CARA and RRAA.
Compare performances;

Remember that the classes representing each algorithms belong to the namespace ns3.
You should create a script to automate the task.


				FINAL

- To analise some important informations, add a tracking target and connect it to the trace source (before LOG_COMPONENT, add the following code):

void ​
CourseChange (std::string context, Ptr<const MobilityModel> model) ​
{ ​
Vector position = model->GetPosition (); ​
NS_LOG_UNCOND (context << ​
" x = " << position.x << ", y = " << position.y); ​
}

- To get information about mobility model position, we will add some code. This peace of code will track each time when echo client switch position (do this using Config::Connect function). Add the following code before run simulator:
​
std::ostringstream oss; ​
oss << ​
"/NodeList/" << wifiStaNodes.Get (nWifi - 1)->GetId () << ​
"/$ns3::MobilityModel/CourseChange"; ​
Config::Connect (oss.str (), MakeCallback (&CourseChange));

- So here final challenge goes:
	- Change the mobility model position allocator to "RandomDiscPositionAllocator" with the following attributes:
		- Disk center coordinate (20,20);
		- Disk radius must vary between 0 to 20;
	- Change the attributes of the mobility model as follows:
		- Time is the conditioner "mode" of speed and direction changes and it must be = 1s;
		- Model speed must be "UniformRandomVariable" and vary between 1 to 1.8 m/s;
		- Add a direction attribute ranging from 0 to 20 radians;
		- The bounds must vary between 0 to 50 (x and y).


- To change method allocator position and mobility model, you may use as reference the documentation "https://www.nsnam.org/doxygen/main-random-walk_8cc_source.html";
- Plot the positions of STAs;
- Visualize the movement of STAS via NetAnim;
- Compare the modified scenario with default.