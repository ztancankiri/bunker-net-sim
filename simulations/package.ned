package bunker_net_sim.simulations;


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.wireless.AccessPoint;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.inet.WirelessHost;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=1025,631");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=61,31");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=138,31");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=218,34");
        }

        // =====[ Main ]======
        mainrouter: Router {
            @display("p=474,231");
        }
        server: StandardHost {
            @display("p=536,276");
        }

        // =====[ Bunker 1 ]======
        bunker1router: Router {
            @display("p=280,231");
        }
        bunker1host1: StandardHost {
            @display("p=68,118");
        }
        bunker1host2: StandardHost {
            @display("p=68,231");
        }
        bunker1host3: StandardHost {
            @display("p=68,349");
        }
        // =====[ Bunker 2 ]======
        bunker2router: Router {
            @display("p=474,378");
        }
        bunker2host1: StandardHost {
            @display("p=372,505");
        }
        bunker2host2: StandardHost {
            @display("p=474,505");
        }
        bunker2host3: StandardHost {
            @display("p=599,505");
        }
        // =====[ Bunker 3 ]======
        bunker3router: Router {
            @display("p=683,231");
        }
        bunker3host1: StandardHost {
            @display("p=844,98");
        }
        bunker3host2: StandardHost {
            @display("p=844,231");
        }
        bunker3host3: StandardHost {
            @display("p=836,382");
        }

    connections:
        bunker1router.ethg++ <--> Eth10G <--> mainrouter.ethg++;
        bunker2router.ethg++ <--> Eth10G <--> mainrouter.ethg++;
        bunker3router.ethg++ <--> Eth10G <--> mainrouter.ethg++;
        server.ethg++ <--> Eth10G <--> mainrouter.ethg++;


        bunker1host1.ethg++ <--> Eth10G <--> bunker1router.ethg++;
        bunker1host2.ethg++ <--> Eth10G <--> bunker1router.ethg++;
        bunker1host3.ethg++ <--> Eth10G <--> bunker1router.ethg++;


        bunker2host1.ethg++ <--> Eth10G <--> bunker2router.ethg++;
        bunker2host2.ethg++ <--> Eth10G <--> bunker2router.ethg++;
        bunker2host3.ethg++ <--> Eth10G <--> bunker2router.ethg++;

        bunker3host1.ethg++ <--> Eth10G <--> bunker3router.ethg++;
        bunker3host2.ethg++ <--> Eth10G <--> bunker3router.ethg++;
        bunker3host3.ethg++ <--> Eth10G <--> bunker3router.ethg++;
}

