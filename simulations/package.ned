package bunker_net_sim.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;
import ned.DatarateChannel;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.mec.MECHost;
import simu5g.nodes.mec.MECOrchestrator.MecOrchestrator;
import simu5g.nodes.mec.UALCMP.UALCMP;
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
        double closeCableLength @unit(m);
        double closeCableBandwidth @unit(bps);
        @display("bgb=3000,3000");

    types:
        channel cable extends DatarateChannel
        {
            delay = replaceUnit(parent.cableLength / 2e8, "s");
            datarate = parent.cableBandwidth;
        }
        channel closeCable extends DatarateChannel
        {
            delay = replaceUnit(parent.closeCableLength / 2e8, "s");
            datarate = parent.closeCableBandwidth;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=43,29");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=276,29");
        }
        channelControl: LteChannelControl {
            @display("p=387,32");
        }
        binder: Binder {
            @display("p=186,29");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=119,29");
        }
        // =====[ Main ]======
        mainrouter: Router {
            @display("p=1113.8789,175.76");
        }
        server: StandardHost {
            @display("p=964.483,98.865");
        }

        upf: Upf {
            @display("p=1113.8789,311.97397");
        }
        // =====[ Bunker 1 ]======
        bunker1host[bunker1_host_number]: NRUe {
            parameters:
                @display("p=81,227");
        }

        iUpf: Upf {
            @display("p=1113.8789,474.55197");
        }

        bunker1gnb1: gNodeB {
            @display("p=318,211");
        }

        // =====[ Bunker 2 ]======
        bunker2host[bunker2_host_number]: NRUe {
            parameters:
                @display("p=735,1509");
        }
        bunker2gnb1: gNodeB {
            @display("p=735,1228");
        }

        // =====[ Bunker 3 ]======
        bunker3host[bunker3_host_number]: NRUe {
            parameters:
                @display("p=2280,100");
        }
        bunker3gnb1: gNodeB {
            @display("p=1890,124");
        }

        ualcmp: UALCMP {
            @display("p=1307.215,98.865");
        }
        mecOrchestrator: MecOrchestrator {
            @display("p=1504.945,81.288994");
        }
        mecHost: MECHost {
            @display("p=1113.8789,727.207");
        }
    connections:
        server.pppg++ <--> closeCable <--> mainrouter.pppg++;
        upf.pppg++ <--> closeCable <--> iUpf.pppg++;
        upf.filterGate <--> closeCable <--> mainrouter.pppg++;

        ualcmp.ppp++ <--> closeCable <--> mainrouter.pppg++;
        ualcmp.toMecOrchestrator --> mecOrchestrator.fromUALCMP;
        ualcmp.fromMecOrchestrator <-- mecOrchestrator.toUALCMP;
        mecHost.ppp++ <--> closeCable <--> iUpf.pppg++;

        bunker1gnb1.ppp <--> cable <--> iUpf.pppg++;
        bunker2gnb1.ppp <--> cable <--> iUpf.pppg++;
        bunker3gnb1.ppp <--> cable <--> iUpf.pppg++;
        


}
