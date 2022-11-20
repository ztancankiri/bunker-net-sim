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
        bunker1host1: WirelessHost {
            @display("i=device/cellphone;p=68,118");
        }
        bunker1host2: WirelessHost {
            @display("i=device/cellphone;p=68,231");
        }
        bunker1host3: WirelessHost {
            @display("i=device/cellphone;p=68,349");
        }
        bunker1AP1: AccessPoint {
            @display("p=164,117");
        }
        bunker1AP2: AccessPoint {
            @display("p=164,230");
        }
        bunker1AP3: AccessPoint {
            @display("p=177,350");
        }
        // =====[ Bunker 2 ]======
        bunker2router: Router {
            @display("p=474,378");
        }
        bunker2host1: WirelessHost {
            @display("i=device/cellphone;p=372,505");
        }
        bunker2host2: WirelessHost {
            @display("i=device/cellphone;p=474,505");
        }
        bunker2host3: WirelessHost {
            @display("i=device/cellphone;p=599,505");
        }
        bunker2AP1: AccessPoint {
            @display("p=372,444");
        }
        bunker2AP2: AccessPoint {
            @display("p=480,444");
        }
        bunker2AP3: AccessPoint {
            @display("p=606,450");
        }
        // =====[ Bunker 3 ]======
        bunker3router: Router {
            @display("p=683,231");
        }
        bunker3host1: WirelessHost {
            @display("i=device/cellphone;p=844,98");
        }
        bunker3host2: WirelessHost {
            @display("i=device/cellphone;p=844,231");
        }
        bunker3host3: WirelessHost {
            @display("i=device/cellphone;p=836,382");
        }
        bunker3AP1: AccessPoint {
            @display("p=758,105");
        }
        bunker3AP2: AccessPoint {
            @display("p=770,230");
        }
        bunker3AP3: AccessPoint {
            @display("p=747,396");
        }

    connections:
        bunker1router.ethg++ <--> Eth10G <--> mainrouter.ethg++;
        bunker2router.ethg++ <--> Eth10G <--> mainrouter.ethg++;
        bunker3router.ethg++ <--> Eth10G <--> mainrouter.ethg++;
        server.ethg++ <--> Eth10G <--> mainrouter.ethg++;


        bunker1AP1.ethg++ <--> Eth10G <--> bunker1router.ethg++;
        bunker1AP2.ethg++ <--> Eth10G <--> bunker1router.ethg++;
        bunker1AP3.ethg++ <--> Eth10G <--> bunker1router.ethg++;


        bunker2AP1.ethg++ <--> Eth10G <--> bunker2router.ethg++;
        bunker2AP2.ethg++ <--> Eth10G <--> bunker2router.ethg++;
        bunker2AP3.ethg++ <--> Eth10G <--> bunker2router.ethg++;

        bunker3AP1.ethg++ <--> Eth10G <--> bunker3router.ethg++;
        bunker3AP2.ethg++ <--> Eth10G <--> bunker3router.ethg++;
        bunker3AP3.ethg++ <--> Eth10G <--> bunker3router.ethg++;
}

