#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimpleGlobalRoutingExample");

int width;
int length;
int height;
int current_address = 0;
std::vector< std::vector<bool> > twist = {{false, false}, {false, false}, {false, false}};

int get_index (int x, int y, int z) {
    return z*length*width + y*width + x;
}

ns3::Ipv4Address constructing_addresses() {
    std::string base1 = "10." + std::to_string(current_address/255 +1) + "." + std::to_string((current_address++)%255) + ".0";
    ns3::Ipv4Address ipv4Base1 = ns3::Ipv4Address(base1.c_str());
    return ipv4Base1;
}

void runSimulation(int argc, char* argv[], int twistIndex) {
    fprintf(stderr, "%d, %d, %d\n", width, length, height);
    // Extract twists from the index
    for (int i = 0; i < 3; ++i) {
        twist[i][0] = twistIndex % 2;
        twistIndex /= 2;
        twist[i][1] = twistIndex % 2;
        twistIndex /= 2;
    }
    current_address = 0;
    // Users may find it convenient to turn on explicit debugging
    // for selected modules; the below lines suggest how to do this
    #if 0
        LogComponentEnable ("SimpleGlobalRoutingExample", LOG_LEVEL_INFO);
        LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
    #endif
        // Set up some default values for the simulation
        Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(210));
        Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("448kb/s"));
        
        // Allow the user to override any of the defaults and the above
        // DefaultValue::Bind ()s at run-time, via command-line arguments
        CommandLine cmd(__FILE__);
        bool enableFlowMonitor = true;
        cmd.AddValue("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
        cmd.Parse(argc, argv);

    // Create nodes
    NS_LOG_INFO("Create nodes.");
    NodeContainer c;
    c.Create(width*length*height);

    InternetStackHelper internet;
    internet.Install(c);

    // Create the channels without any IP addressing information
    NS_LOG_INFO("Create channels.");
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    Ipv4AddressHelper ipv4;
    std::vector<Ipv4InterfaceContainer*> interfaceContainers;
    NS_LOG_INFO(interfaceContainers.size());

    for (int z=0; z<height; z++) {
        for (int y=0; y<length; y++) {
            for (int x=0; x<width; x++) {
                int i = get_index(x, y, z);
                ns3::Ipv4Address ipv4Base1 = constructing_addresses();
                ns3::Ipv4Address ipv4Base2 = constructing_addresses();
                ns3::Ipv4Address ipv4Base3 = constructing_addresses();

                int nx, ny, nz;

                nx = (x+1) % width;
                ny = y;
                if (twist[0][0] && nx == 0) {
                    ny = (ny + length/2) % length;
                }
                nz = z;
                if (twist[0][1] && nx == 0) {
                    nz = (nz + height/2) % height;
                }
                int nx_id = get_index(nx, ny, nz);

                ny = (y+1) % length;
                nx = x;
                if (twist[1][0] && ny == 0) {
                    nx = (nx + width/2) % width;
                }
                nz = z;
                if (twist[1][1] && ny == 0) {
                    nz = (nz + height/2) % height;
                }
                int ny_id = get_index(nx, ny, nz);

                nz = (z+1) % height;
                nx = x;
                if (twist[2][0] && nz == 0) {
                    nx = (nx + width/2) % width;
                }
                ny = y;
                if (twist[2][1] && nz == 0) {
                    ny = (ny + length/2) % length;
                }
                int nz_id = get_index(nx, ny, nz);

                NetDeviceContainer device_x, device_y, device_z;
                // To display the connections
                // fprintf(stderr, "index: %d, nx_id: %d, ny_id: %d, nz_id: %d\n", i, nx_id, ny_id, nz_id);
                device_x = p2p.Install(NodeContainer(c.Get(i), c.Get(nx_id)));
                ipv4.SetBase(ipv4Base1, "255.255.255.0");
                Ipv4InterfaceContainer *interface_x = new Ipv4InterfaceContainer(ipv4.Assign(device_x));

                device_y = p2p.Install(NodeContainer(c.Get(i), c.Get(ny_id)));
                ipv4.SetBase(ipv4Base2, "255.255.255.0");
                Ipv4InterfaceContainer *interface_y = new Ipv4InterfaceContainer(ipv4.Assign(device_y));

                device_z = p2p.Install(NodeContainer(c.Get(i), c.Get(nz_id)));
                ipv4.SetBase(ipv4Base3, "255.255.255.0");
                Ipv4InterfaceContainer *interface_z = new Ipv4InterfaceContainer(ipv4.Assign(device_z));

                interfaceContainers.push_back(interface_x);
                interfaceContainers.push_back(interface_y);
                interfaceContainers.push_back(interface_z);
            }
        }
    }

    NS_LOG_INFO(interfaceContainers.size());

    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    NS_LOG_INFO("Create Applications.");
    uint16_t port = 9; // Discard port (RFC 863)
    ApplicationContainer apps;

    // Create packet sinks
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                        Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    for (int i=0; i<width*length*height; i++) {
        apps = sink.Install(c.Get(i));
        apps.Start(Seconds(1.0));
        apps.Stop(Seconds(1.1));
    }

    OnOffHelper onoff("ns3::UdpSocketFactory",
            Address(InetSocketAddress(interfaceContainers[0]->GetAddress(0), port)));
    onoff.SetConstantRate(DataRate("448kb/s"));
    std::vector<float> time_array = {1.0, 1.1, 1.2, 1.3, 1.4};
    int index = 0;
    while (true) {
        fprintf(stderr, "here");
        if (index > 4) {
            break;
        }
        for (int i=0; i<width*length*height; i++) {
            for (int j=0; j<width*length*height; j++) {
                if (i == j) continue;
                if (i / (length*width) != j / (length*width)) continue; 
                onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(interfaceContainers[3*j]->GetAddress(0), port)));
                apps = onoff.Install(c.Get(i));
                apps.Start(Seconds(time_array[index]));
                apps.Stop(Seconds(time_array[index] + 0.1));
            }
        }
        index++;
        for (int i=0; i<width*length*height; i++) {
            for (int j=0; j<width*length*height; j++) {
                if (i == j) continue;
                if (i + width*length % width*length*height == j) {
                    onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(interfaceContainers[3*j]->GetAddress(0), port)));
                    apps = onoff.Install(c.Get(i));
                    apps.Start(Seconds(time_array[index]));
                    apps.Stop(Seconds(time_array[index] + 0.1));
                }
            }
        }
        index++;
    }
    AsciiTraceHelper ascii;
# if 0
    p2p.EnableAsciiAll(ascii.CreateFileStream("simple-global-routing.tr"));
    p2p.EnablePcapAll("simple-global-routing");
# endif

    // Flow Monitor
    FlowMonitorHelper flowmonHelper;
    if (enableFlowMonitor)
    {
        flowmonHelper.InstallAll();
    }

    NS_LOG_INFO("Run Simulation.");
    Simulator::Stop(Seconds(11));
    Simulator::Run();
    NS_LOG_INFO("Done.");

    std::string flowmonFileName = "torus-" + std::to_string(width) + "x" + std::to_string(length) + "x" + std::to_string(height) + "-";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            flowmonFileName += twist[i][j] ? "1" : "0";
        }
    }
    flowmonFileName += ".flowmon";

    if (enableFlowMonitor) {
        flowmonHelper.SerializeToXmlFile(flowmonFileName, false, false);
    }

    // Create a string to represent the twist combination and dimension
    std::string header = flowmonFileName;

    // Run the Python script and capture its output
    FILE* pipe = popen(("python3 flowmon-parse-results.py " + flowmonFileName).c_str(), "r");
    if (!pipe) {
        NS_LOG_ERROR("Couldn't run the Python script.");
        return;
    }

    // Append the header to the CSV file
    std::ofstream csvFile("simulation_results_file_data.csv", std::ios::app);  // Open the file in append mode
    csvFile << header << "\n";

    // Append the Python script output to the CSV file
    char buffer[128];
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr) {
            csvFile << buffer;
        }
    }
    pclose(pipe);

    // Close the CSV file
    csvFile << "\n";
    csvFile.close();

    Simulator::Destroy();
}

int main (int argc, char* argv[]) {
    // Specify the dimensions to simulate
    std::vector<std::vector<int>> dimensions = {
        {2, 1, 1},
        {2, 2, 1},
        {2, 2, 2},
        {4, 2, 2},
        {4, 4, 2},
        {4, 4, 4},
        {8, 4, 4}, 
        {4, 4, 12},
        {12, 4, 4}
    };

   // Iterate through dimensions
    for (const auto &dim : dimensions) {
        width = dim[0];
        length = dim[1];
        height = dim[2];

        std::vector<int> twistIndex_list = {0, 1, 4, 5};

        // Iterate through twists
        for (int i = 0; i < 4; i++) {
            runSimulation(argc, argv, twistIndex_list[i]);
        }
    }
    
    return 0;
}
