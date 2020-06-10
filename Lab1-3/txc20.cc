/*
 * txc20.cc
 *
 *  Created on: Apr 6, 2020
 *      Author: hongt
 */



#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "tictoc20_m.h"

using namespace omnetpp;

class Txc20 : public cSimpleModule
{
    private:
        bool isChecked;
        int start, finish;
    protected:
        virtual TicTocMsg20 *generateMessage();
        virtual void forwardMessage(TicTocMsg20 *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc20);

void Txc20::initialize()
{
    isChecked = false;
    start = par("start");
    finish = par("finish");
    if (getIndex() == start) {
        // Boot the process scheduling the initial message as a self-message.
        char msgname[20];
        sprintf(msgname, "node-%d", getIndex());
        TicTocMsg20 *msg = generateMessage();
        isChecked = true;
        msg->setHopCount(msg->getHopCount()+1);
        scheduleAt(0.0, msg);
    }
}

void Txc20::handleMessage(cMessage *msg)
{
    TicTocMsg20 *ttmsg = check_and_cast<TicTocMsg20 *>(msg);
    if (getIndex() == finish && ttmsg->getHopCount() == 13) {

        EV << "Message " << msg << " arrived.\n Hop count = " << ttmsg->getHopCount();
        delete ttmsg;
    }
    else {
        if(isChecked == false){
            isChecked = true;
            ttmsg->setHopCount(ttmsg->getHopCount()+1);
        }
        forwardMessage(ttmsg);
    }
}

TicTocMsg20 *Txc20::generateMessage()
{
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;
    char msgname[20];
    sprintf(msgname, "node-%d-to-%d", src, dest);

    TicTocMsg20 *msg = new TicTocMsg20(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void Txc20::forwardMessage(TicTocMsg20 *msg)
{
    //msg->setHopCount(msg->getHopCount()+1);
    // In this example, we just pick a random gate to send it on.
    // We draw a random number between 0 and the size of gate `out[]'.
    int n = gateSize("out");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on port out[" << k << "] "<< "count = " << msg->getHopCount() <<"\n";
    send(msg, "out", k);
}
