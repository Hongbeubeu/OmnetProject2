/*
 * txc22.cc
 *
 *  Created on: Apr 7, 2020
 *      Author: hongt
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "tictoc22_m.h"

using namespace omnetpp;

/**
 * Let's make it more interesting by using several (n) `tic' modules,
 * and connecting every module to every other. For now, let's keep it
 * simple what they do: module 0 generates a message, and the others
 * keep tossing it around in random directions until it arrives at
 * module 3.
 */
class Txc22 : public cSimpleModule
{
    private:
        bool isChecked;
        int destinationNode;
    protected:
        virtual TicTocMsg22 *generateMessage();
        virtual void forwardMessage(TicTocMsg22 *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc22);

void Txc22::initialize()
{
    isChecked = false;
    destinationNode = par("destinationNode");
    if (getIndex() == destinationNode) {
        char msgname[20];
        sprintf(msgname, "node-%d", getIndex());
        TicTocMsg22 *msg = generateMessage();
        isChecked = true;
        msg->setHopCount(msg->getHopCount()+1);
        scheduleAt(0.0, msg);
    }
}

void Txc22::handleMessage(cMessage *msg)
{
    TicTocMsg22 *ttmsg = check_and_cast<TicTocMsg22 *>(msg);
    if (getIndex() == destinationNode && ttmsg->getHopCount() == 13) {
        for (int i = 0; i < 169; i++){
            EV <<ttmsg->getMap(i) << " ";
            if((i + 1) % 13 == 0)
                EV << "\n";
        }
            delete ttmsg;
    }
    else {
        if(isChecked == false){
            isChecked = true;
            ttmsg->setHopCount(ttmsg->getHopCount()+1);
        }
        if(ttmsg->getMap(ttmsg->getPreNode()*13 + getIndex()) == 0 && ttmsg->getPreNode() != getIndex()){
            ttmsg->setMap(ttmsg->getPreNode()*13 + getIndex(), 1);
            ttmsg->setMap(getIndex()*13 + ttmsg->getPreNode(), 1);
        }
        ttmsg->setPreNode(getIndex());
        forwardMessage(ttmsg);
    }
}

TicTocMsg22 *Txc22::generateMessage()
{
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;
    char msgname[20];
    sprintf(msgname, "node-%d-to-%d", src, dest);

    TicTocMsg22 *msg = new TicTocMsg22(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    msg->setPreNode(destinationNode);
    return msg;
}

void Txc22::forwardMessage(TicTocMsg22 *msg)
{
    // In this example, we just pick a random gate to send it on.
    // We draw a random number between 0 and the size of gate `out[]'.
    int n = gateSize("out");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on port out[" << k << "] "<< "count = " << msg->getHopCount() <<"\n";
    send(msg, "out", k);
}

