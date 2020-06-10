/*
 * receiver.cc
 *
 *  Created on: May 16, 2020
 *      Author: hongt
 */
#include <omnetpp.h>

using namespace omnetpp;

class Receiver: public cSimpleModule {
private:
    double TIMEOUT;
    double TIME_INTERVAL;
    int *receivedMsgCount;
    int arrayLength;
    int intervalCount = 0;
    int sumMsg;

    void sendSignalToSwitch();
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Receiver);

/*
 * Khởi tạo giá trị biến từ omnetpp.ini
 */
void Receiver::initialize() {
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    TIMEOUT = par("TIMEOUT").doubleValue();
    sumMsg = 0;
    arrayLength = TIMEOUT / TIME_INTERVAL;
    receivedMsgCount = new int[arrayLength];
    memset(receivedMsgCount, 0, arrayLength * sizeof(int));

    scheduleAt(0, new cMessage("nextInterval"));
}

/*
 * xử lý message nhận được từ sender
 * đếm số message nhận được trong từng interval
 */
void Receiver::handleMessage(cMessage *msg) {
    if (simTime() >= TIMEOUT){
        return;
    }

    if (strcmp(msg->getName(), "sender to receiver msg") == 0) {
        sendSignalToSwitch();
        EV << "Received msg" << endl;
        sumMsg++;
        receivedMsgCount[intervalCount]++;
        delete msg;

    }

    if (strcmp(msg->getName(), "nextInterval") == 0) {
        intervalCount++;
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }
}

/**
 * Send signal to switch when receiver received message
 * @input no
 * @output no
 */
void Receiver::sendSignalToSwitch(){
    send(new cMessage("signal"), "out");
}

/*
 * in ra thống kê số gói tin nhận được theo từng interval
 */
void Receiver::finish(){
    for (int i = 0; i < arrayLength; i++) {
        EV << "interval " << i << ", received " << receivedMsgCount[i] << " messages" << endl;
    }
    EV << "INTERVAL: " << TIME_INTERVAL << endl;
    EV << "SUM INTERVAL: " << intervalCount << endl;
    EV << "SUM MESSAGE: " << sumMsg << endl;
    EV << "AVERAGE: " << sumMsg/TIMEOUT << endl;
}


