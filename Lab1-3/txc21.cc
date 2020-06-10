/*
 * txc21.cc
 *
 *  Created on: Apr 6, 2020
 *      Author: hongt
 */



#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "tictoc21_m.h"

using namespace omnetpp;

class Txc21 : public cSimpleModule
{
    private:
        int isInit;
        int start;
        int size = 0;
        int count = 0;
        int neighbour[6];
    protected:
        virtual TicTocMsg21 *generateMessage();
        virtual void forwardMessage(TicTocMsg21 *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc21);

void Txc21::initialize()
{
    isInit = 0;
    start = par("start");
    if (getIndex() == start) {
        size = gateSize("out");
        count = size;
        isInit = 1;
        for(int i = 0; i < 2 * size; i++)
            neighbour[i] = 0;
        char msgname[20];
        sprintf(msgname, "node-%d", getIndex());
        TicTocMsg21 *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void Txc21::handleMessage(cMessage *msg)
{
    if(isInit == 0){
        size = gateSize("out");
        count = size;
        for(int i = 0; i < 2 * size; i++)
            neighbour[i] = 0;
        isInit = 1;
    }
    TicTocMsg21 *ttmsg = check_and_cast<TicTocMsg21 *>(msg);

    if (count == 0) {
        for(int i = 0; i < size; i++)
            EV << "Neighbour = " << neighbour[i] << ", level = " << neighbour[i + size] << "\n";

        delete(ttmsg);
        TicTocMsg21 *newmsg = generateMessage();
        EV << newmsg << endl;
        forwardMessage(newmsg);
    }
    else {
        if(neighbour[count - 1] == 0){
            neighbour[count - 1] = ttmsg->getSource();
            neighbour[count + size - 1] = ttmsg->getLevel();
            count--;
        }
        delete ttmsg;
        TicTocMsg21 *newmsg = generateMessage();
        EV << newmsg << endl;
        forwardMessage(newmsg);
    }
}

TicTocMsg21 *Txc21::generateMessage()
{
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;
    char msgname[20];
    sprintf(msgname, "node-%d-to-%d", src, dest);

    TicTocMsg21 *msg = new TicTocMsg21(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    msg->setLevel(gateSize("out"));
    return msg;
}

void Txc21::forwardMessage(TicTocMsg21 *msg)
{
    //msg->setHopCount(msg->getHopCount()+1);
    // In this example, we just pick a random gate to send it on.
    // We draw a random number between 0 and the size of gate `out[]'.
    int n = gateSize("out");
    int k = intuniform(0, n-1);
    EV << "Sending message " << ", Node = "<< getIndex()<< ", Size = "<< msg->getLevel() << ", Count = "<< count <<", isInit = "<<isInit<<"\n";
    send(msg, "out", k);
}
