package bunker_net_sim.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;
import ned.DatarateChannel;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.world.radio.LteChannelControl;
import simu5g.nodes.NR.NRUe;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.Upf;

@license(LGPL);
network Network
{
    parameters:
        int bunker1_host_number;
        int bunker2_host_number;
        int bunker3_host_number;
        double cableLength @unit(m);
        double cableBandwidth @unit(bps);
        @display("bgb=1581.255,800.28");

    types:
        channel cable extends DatarateChannel
        {
            delay = replaceUnit(parent.cableLength / 2e8, "s");
            datarate = parent.cableBandwidth;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=61,31");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=347,35");
        }
        channelControl: LteChannelControl {
            @display("p=257,28");
        }
        binder: Binder {
            @display("p=196,29");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=128,29");
        }
        // =====[ Main ]======
        mainrouter: Router {
            @display("p=642,229");
        }
        server: StandardHost {
            @display("p=642,87");
        }

        upf: Upf {
            @display("p=640.575,386.1");
        }
        // =====[ Bunker 1 ]======
        bunker1host[bunker1_host_number]: NRUe {
            parameters:
                @display("p=59,412");
        }

        bunker1iUpf: Upf {
            @display("p=177,126");
        }

        bunker1gnb1: gNodeB {
            @display("p=59,677;is=vl");
        }

        bunker1gnb2: gNodeB {
            @display("p=59,229;is=vl");
        }
        // =====[ Bunker 2 ]======
        bunker2host[bunker2_host_number]: NRUe {
            parameters:
                @display("p=575,712");
        }
        bunker2iUpf: Upf {
            @display("p=923,454");
        }

        bunker2gnb1: gNodeB {
            @display("p=440,645;is=vl");
        }

        bunker2gnb2: gNodeB {
            @display("p=996,645;is=vl");
        }
        // =====[ Bunker 3 ]======
        bunker3host[bunker3_host_number]: NRUe {
            parameters:
                @display("p=1307,108");
        }
        bunker3iUpf: Upf {
            @display("p=991,66");
        }

        bunker3gnb1: gNodeB {
            @display("p=1163,387;is=vl");
        }

        bunker3gnb2: gNodeB {
            @display("p=1163,82;is=vl");
        }
    connections:
        server.pppg++ <--> cable <--> mainrouter.pppg++;


        upf.filterGate <--> cable <--> mainrouter.pppg++;
        upf.pppg++ <--> cable <--> bunker1iUpf.pppg++;
        bunker1gnb1.ppp <--> cable <--> bunker1iUpf.pppg++;
        bunker1gnb2.ppp <--> cable <--> bunker1iUpf.pppg++;
        bunker1gnb1.x2++ <--> cable <--> bunker1gnb2.x2++;


        upf.pppg++ <--> cable <--> bunker2iUpf.pppg++;
        bunker2gnb1.ppp <--> cable <--> bunker2iUpf.pppg++;
        bunker2gnb2.ppp <--> cable <--> bunker2iUpf.pppg++;
        bunker2gnb1.x2++ <--> cable <--> bunker2gnb2.x2++;


        upf.pppg++ <--> cable <--> bunker3iUpf.pppg++;
        bunker3gnb1.ppp <--> cable <--> bunker3iUpf.pppg++;
        bunker3gnb2.ppp <--> cable <--> bunker3iUpf.pppg++;
        bunker3gnb1.x2++ <--> cable <--> bunker3gnb2.x2++;

}
