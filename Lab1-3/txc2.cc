/*
 * txc2.cc
 *
 *  Created on: Mar 29, 2020
 *      Author: hongt
 */

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * Derive the Txc2 class from cSimpleModule. In the Tictoc1 network,
 * both the `tic' and `toc' modules are Txc2 objects, created by OMNeT++
 * at the beginning of the simulation.
 */
class Txc2 : public cSimpleModule
{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(Txc2);

void Txc2::initialize()
{
    // Initialize is called at the beginning of the simulation.
    // To bootstrap the tic-toc-tic-toc process, one of the modules needs
    // to send the first message. Let this be `tic'.

    // Am I Tic or Toc?
    if (strcmp("tic", getName()) == 0) {
        // create and send first message on gate "out". "tictocMsg" is an
        // arbitrary string which will be the name of the message object.
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc2::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, we just send it to the other module, through
    // gate `out'. Because both `tic' and `toc' does the same, the message
    // will bounce between the two.
    EV << "Received message `" << msg->getName() << "', sending it out again\n";
    send(msg, "out");
}



