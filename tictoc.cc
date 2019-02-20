#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

SimTime duration, RTT;

int seqNo = 0;

class Tic: public cSimpleModule {
private:
    int noOfPackets;  // Counter to limit no of packets
    cMessage *event; // self message
    cOutVector RTTVector, delayVector; // Variables to store values to graph
public:
    Tic();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Tic);

Tic::Tic() {
    event = nullptr;
}
void Tic::initialize() {

    RTTVector.setName("RTTVector");
    delayVector.setName("TicDelayVector");
    WATCH(seqNo); //  to see the sequence number  in the graphical runtime environment
    WATCH(noOfPackets); //  to see the counter  value  in the graphical runtime environment
    noOfPackets = par("counter");      //getting counter value from the ini file
    event = new cMessage("event");                // self message

    EV << "Sending initial message\n"; // Generate and send the initial message.
    char msgname[20];
    sprintf(msgname, "tic-%d", ++seqNo);           // Initial message
    cMessage *msg = new cMessage(msgname);
    duration = msg->getCreationTime(); // getting the packet creation time for calculating RTT
    send(msg, "out");
}

void Tic::handleMessage(cMessage *msg) {

    if (noOfPackets == 0) {
        EV << getName() << " counter reached zero, Hence deleting message\n";
        delete msg;
    } else {
        if (msg == event) {
            duration = simTime() - duration; // RTT can be found by subtracting the simulation time from previous packet creation  time
            RTT = duration;
            RTTVector.record(RTT); // for  ploting  graph of round trip time(RTT)
            EV << " RTT for sequence number " << seqNo << " is:" << RTT
                      << " ms\n";
            char msgname[20];
            char seqname[10];
            sprintf(msgname, "from tic-%d", ++seqNo);
            sprintf(seqname, "SeqNo- %d", seqNo);
            getDisplayString().setTagArg("t", 0, seqname); //for showing the seq no of packet in runtime above the tic module
            msg = new cMessage(msgname); // generating packet embedded with sequence number from tic

            duration = msg->getCreationTime(); // getting the packet creation time for calculating RTT
            send(msg, "out");

        } else {
            noOfPackets--;
            simtime_t processingDelayTic = par("delayTimeTic"); // generating random processing delay for tic
            delayVector.record(processingDelayTic); // Used to plot graph of random processing delay for tic
            EV << "the processing delay of tic  with sequence no" << seqNo
                      << " is : " << processingDelayTic << "ms\n ";
            scheduleAt(simTime() + processingDelayTic, event);
        }
    }
}

class Toc: public cSimpleModule {

private:
    cMessage *event;                    // selfmessage
    cOutVector delayVector;             // Variables to store values to graph

public:
    Toc();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Toc);

Toc::Toc() {
    event = nullptr;
}

void Toc::initialize() {
    event = new cMessage("event");       // self message
    delayVector.setName("TocDelayVector");
    WATCH(seqNo); //  to see the sequence number  in the graphical runtime environment
}

void Toc::handleMessage(cMessage *msg) {
    if (msg == event) {
        char msgname[20];
        char seqname[10];
        sprintf(msgname, "from toc-%d", seqNo);
        sprintf(seqname, "SeqNo- %d", seqNo);
        getDisplayString().setTagArg("t", 0, seqname); //for showing the seq no of packet in runtime above the toc module
        msg = new cMessage(msgname); // generating packet embedded with sequence number from toc
        send(msg, "out");
    } else {
        simtime_t processingDelayToc = par("delayTimeToc"); // generating random processing delay for toc
        delayVector.record(processingDelayToc); //Used to plot graph of random processing  delay for toc
        EV << "the processing delay of toc  with sequence no " << seqNo
                  << "is : " << processingDelayToc << " ms\n ";
        scheduleAt(simTime() + processingDelayToc, event);
    }
}
