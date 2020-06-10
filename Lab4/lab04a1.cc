#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class SimpleHost : public cSimpleModule
{
    private:
        int limits;
        int counter;
    protected:
        virtual void forwardMessage(cMessage *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(SimpleHost);

void SimpleHost::initialize(){
    limits = par(limits);
    counter = limits;
    WATCH(counter);
    if(strcmp("sparrow", getName()) == 0){
        EV <<"Send initial message\n";
        cMessage *msg = new cMessage("sparrow call eagle");
        scheduleAt(0.0, msg);
    }
}

void SimpleHost::handleMessage(cMessage *msg){
    if(strcmp("eagle", getName()) == 0){
        counter--;
        if(counter == 0){
            delete msg;
            EV << "Send limmited = " << limits<< "\n";
        }else
            forwardMessage(msg);
    }else{
        counter--;
        if(counter == 0){
            delete msg;
            EV << "Send limmited = " << limits<< "\n";
        }else
            forwardMessage(msg);
    }
}

void SimpleHost::forwardMessage(cMessage *msg){
    if(strcmp("eagle", getName()) == 0){
        msg->setName("Eagle reply Sparrow");
        EV <<msg<<"\n";
        send(msg, "out");
    }else{
        msg->setName("Sparrow call eagle");
        EV <<msg<<"\n";
        send(msg, "out");
    }
}


