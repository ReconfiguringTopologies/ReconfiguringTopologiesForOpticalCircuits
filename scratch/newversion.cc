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


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimpleGlobalRoutingExample");

int width = 4;
int length = 3;
int height = 2;
std::vector< std::vector<bool> > twist = {{true, false}, {false, false}, {false, false}};

int get_index (int x, int y, int z) {
    return z*length*width + y*width + x;
}

int main(int argc, char* argv[]) {
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
    int current_address = 0;

    for (int z=0; z<height; z++) {
        for (int y=0; y<length; y++) {
            for (int x=0; x<width; x++) {
                int i = get_index(x, y, z);
                // fprintf(stderr, "%d, %d, %d\n", x, y, z);
                std::string base1 = "10." + std::to_string(current_address/255 +1) + "." + std::to_string((current_address++)%255) + ".0";
                ns3::Ipv4Address ipv4Base1 = ns3::Ipv4Address(base1.c_str());
                std::string base2 = "10."+ std::to_string(current_address/255 +1) + "."  + std::to_string((current_address++)%255) + ".0";
                ns3::Ipv4Address ipv4Base2 = ns3::Ipv4Address(base2.c_str());
                std::string base3 = "10."+ std::to_string(current_address/255 +1) + "."  + std::to_string((current_address++)%255) + ".0";
                ns3::Ipv4Address ipv4Base3 = ns3::Ipv4Address(base3.c_str());
                // fprintf(stderr, "%s\n", base3.c_str());

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
                // fprintf(stderr, "address: %s", base3.c_str());
                fprintf(stderr, "index: %d, nx_id: %d, ny_id: %d, nz_id: %d\n", i, nx_id, ny_id, nz_id);

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
        apps.Stop(Seconds(1.01));
    }

    OnOffHelper onoff("ns3::UdpSocketFactory",
            Address(InetSocketAddress(interfaceContainers[0]->GetAddress(0), port)));
    onoff.SetConstantRate(DataRate("448kb/s"));
    // Create OnOff applications
    for (int i=0; i<width*length*height; i++) {
        for (int j=0; j<width*length*height; j++) {
            if (i == j) continue;
            onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(interfaceContainers[3*j]->GetAddress(0), port)));
            apps = onoff.Install(c.Get(i));
            apps.Start(Seconds(1.0));
            apps.Stop(Seconds(1.01));
        }
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

    if (enableFlowMonitor)
    {
        flowmonHelper.SerializeToXmlFile("twisted-3d-torus.flowmon", false, false);
    }

    Simulator::Destroy();
    return 0;
}