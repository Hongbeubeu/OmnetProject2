#include <string.h>
#include <omnetpp.h>
#include "simple_message_m.h"

using namespace omnetpp;

class RingHost : public cSimpleModule
{
    private:
        int numHosts;
    protected:
        virtual SimpleMessage *generateMessage();
        virtual void forwardMessage(SimpleMessage *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(RingHost);

void RingHost::initialize()
{
    numHosts = par("numHosts");
    if (getIndex() == 0) {
        char msgname[20];
        sprintf(msgname, "Host-%d-to-Host-%d", getIndex(), numHosts - 1);
        SimpleMessage *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void RingHost::handleMessage(cMessage *msg)
{
    if (getIndex() == 0) {
        delete msg;
        SimpleMessage *newMsg = generateMessage();
        forwardMessage(newMsg);
    }
    else {
        SimpleMessage *ttmsg = check_and_cast<SimpleMessage *>(msg);
        if(getIndex() == numHosts-1){
            ttmsg->setHopCount(ttmsg->getHopCount() + 1);
            EV << "Message " << msg << " arrived.\n Hop count = " << ttmsg->getHopCount();
            delete msg;
            cMessage *newmsg = new cMessage("message arrived to last host");
            send(newmsg, "out");
        }else
            forwardMessage(ttmsg);
    }
}

SimpleMessage *RingHost::generateMessage()
{
    int src = getIndex();
    SimpleMessage *msg = new SimpleMessage("TestSimpleMessage");
    msg->setSource(src);
    msg->setDestination(numHosts-1);
    msg->setHopCount(0);
    return msg;
}

void RingHost::forwardMessage(SimpleMessage *msg)
{
    msg->setHopCount(msg->getHopCount() + 1);
    EV << "Forwarding message " << msg <<"\n";
    send(msg, "out");
}


