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
network Network {
    parameters:
        int bunker1_host_number;
        int bunker2_host_number;
        int bunker3_host_number;
        double cableLength @unit(m);
        double cableBandwidth @unit(bps);
        double closeCableLength @unit(m);
        double closeCableBandwidth @unit(bps);
        @display("bgb=4541.1987,3444.8958");

    types:
        channel cable extends DatarateChannel {
            delay = replaceUnit(parent.cableLength / 2e8, "s");
            datarate = parent.cableBandwidth;
        }
        
        channel closeCable extends DatarateChannel {
            delay = replaceUnit(parent.closeCableLength / 2e8, "s");
            datarate = parent.closeCableBandwidth;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=2862.691,1502.7479");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=2862.691,1175.3949");
        }
        channelControl: LteChannelControl {
            @display("p=2862.691,1021.6049");
        }
        binder: Binder {
            @display("p=2862.691,1337.9729");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=2862.691,880.99695");
        }
        // =====[ Main ]======
        mainrouter: Router {
            @display("p=1113.8789,175.76");
        }
        server: StandardHost {
            @display("p=562.43195,175.76");
        }
        upf: Upf {
            @display("p=1113.8789,331.74698");
        }
        iUpf: Upf {
            @display("p=1113.8789,474.55197");
        }
        ualcmp: UALCMP {
            @display("p=1513.7329,175.76");
        }
        mecOrchestrator: MecOrchestrator {
            @display("p=1827.9039,175.76");
        }
        mecHost: MECHost {
            @display("p=2599.0508,261.443");
        }
        // =====[ Bunker 1 ]======
        bunker1gnb1: gNodeB {
            @display("p=450,475");
        }
        bunker1host[bunker1_host_number]: NRUe {
            parameters:
                @display("p=240,475");
        }
        // =====[ Bunker 2 ]======
        bunker2gnb1: gNodeB {
            @display("p=1110,3050");
        }
        bunker2host[bunker2_host_number]: NRUe {
            parameters:
                @display("p=1110,3250");
        }
        // =====[ Bunker 3 ]======
        bunker3gnb1: gNodeB {
            @display("p=4100,475");
        }
        bunker3host[bunker3_host_number]: NRUe {
            parameters:
                @display("p=4350,475");
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
