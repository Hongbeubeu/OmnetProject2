#include <omnetpp.h>
#include <queue>
#include <limits>

using namespace omnetpp;
using namespace std;
class Nodes: public cSimpleModule{
private:
    int EXB_SIZE;
    int type;
    double TIMEOUT;
    double TIME_INTERVAL;
    double CHANNEL_DELAY;
    double TIME_OPERATION_OF_SWITCH;
    bool isChannelBussy;

    queue<int> ENB[4], EXB_SW[4]; // enchance buffer và exit buffer của switch
    queue<int> SQ, EXB_SD; //Source queue và exit buffer của sender

    //wrap function sử dụng cho từng loại thiết bị sender, switch, receiver
    void senders(cMessage *msg);
    void switches(cMessage *msg);
    void receivers(cMessage *msg);


    //các hàm sử dụng cho sender
    void generateMessage();
    void sendToExitBuffer_SD();
    void sendToSwitch();

    //các hàm sử dụng cho switch
    void sendToExitBuffer_SW();
    bool checkENB();
    void sendToNextNode();
    void sendSignalToNeighbor(int);

    //các hàm sử dụng cho receiver
    void sendSignalToSwitch();
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Nodes);

void Nodes::initialize(){
    EXB_SIZE = par("EXB_SIZE").intValue();
    TIMEOUT = par("TIMEOUT").doubleValue();
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    CREDIT_DELAY = par("CREDIT_DELAY").doubleValue();
    CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();
    isChannelBussy = false;
    TIME_OPERATION_OF_SWITCH = par("TIME_OPERATION_OF_SWITCH").doubleValue();
    type = par("type").intValue();

    if (type == 2)
        scheduleAt(0 + TIME_OPERATION_OF_SWITCH, new cMessage("send"));
    if (type == 1){
        scheduleAt(0, new cMessage("generate"));
        scheduleAt(0, new cMessage("send"));
    }
    if (type > 1)
        scheduleAt(0 + TIME_INTERVAL, new cMessage("nextInterval"));

}

void Nodes::handleMessage(cMessage *msg){
    if(type == 2){
        switches(msg);
    }else if(type == 1){
        senders(msg);
    }else{
        receivers(msg);
    }
}

void Nodes::switches(cMessage *msg){
    if(simTime() >= TIMEOUT){
        EV << "Time out" << endl;
        return;
    }

    const char * eventName = msg->getName();

    /**
     * lấy id của gói tin mà các sender gửi lên
     * lưu các id vào ENB tương ứng
     * sinh sự kiện gửi gói tin từ ENB sang EXB sau 1 chu kỳ hoạt động của switch = chu kỳ sinh gói tin
     */
    if(strcmp(eventName, "sender to receiver") == 0){
        int index = msg->getSenderModule()->getIndex();
        int payload = msg->par("msgId").longValue();
        ENB[index].push(payload);
        return;
    }

    //Kiểm tra gói tin muốn sang cổng EXB theo chu kỳ hoạt động của switch
    if(strcmp(eventName, "nextPeriod")){
        if(EXB.size() < EXB_SIZE){
            sendToExitBuffer();
        }
    }

    //Set channel status if send success message
    if(strcmp(eventName, "signal") == 0){
        isChannelBussy = false;
        delete msg;
    }

    //Send message to receiver

    if(strcmp(eventName, "send") == 0){
        if(!EXB.empty()){
            if(!isChannelBussy){
                sendToReceiver();
                isChannelBussy = true;
            }
        }
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }
}

void Nodes::Senders(cMessage msg){
    if(simTime() >= TIMEOUT)
        return;

    if(strcmp(msg->getName(), "generate") == 0){
        generateMessage();
        if(EXB.size() < EXB_SIZE)
            sendToExitBuffer();
        scheduleAt(simTime() + TIME_GEN_MSG, msg);
    }

    if(strcmp(msg->getName(), "send") == 0){
        if(BUFFER_COUNTER > 0){
            sendToSwitch();
            sendToExitBuffer();
            --BUFFER_COUNTER;
        }
        scheduleAt(simTime() + CHANNEL_DELAY, msg);
    }

    if(strcmp(msg->getName(), "signal") == 0){
        scheduleAt(simTime() + CREDIT_DELAY, new cMessage("incBuff"));
        delete msg;
    }

    if(strcmp(msg->getName(), "incBuff") == 0){
        ++BUFFER_COUNTER;
        delete msg;
    }
}


void Nodes::Receivers(cMessage *msg){
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
 * gửi thông báo ENB tương ứng có chỗ trống
 * @input số hiệu cổng gửi signal
 * @return không
 */

bool Nodes::checkENB(){
    for (int i = 0; i < 3; i++){
        if(!ENB[i].empty())
            return true;
    }
    return false;
}

void Nodes::sendSignalToSender(int port){
    send(new cMessage("signal"), "out", port);
}

/**
 * gửi gói tin từ ENB sang EXB
 * @input không
 * @return không
 */

void Nodes::sendToExitBuffer(){
    int id = numeric_limits<int>::max();
    int port = 0;
    for ( int i = 0; i < 3; i++){
        if (!ENB[i].empty()){
            if(ENB[i].front() < id){
                id = ENB[i].front();
                ENB[i].pop();
                port = i;
            }
        }
    }
    EXB.push(id);
    sendSignalToSender(port);
}

/**
 * gửi gói tin đến receiver
 * @return không
 */

void Nodes::sendToReceiver(){
    int sendMsgId = EXB.front();
    EXB.pop();

    cMessage *sendMsg = new cMessage("sender to receiver msg");

    cMsgPar *msgParam = new cMsgPar("msgId");
    msgParam->setLongValue(sendMsgId);
    sendMsg->addPar(msgParam);

    send(sendMsg, "out", 3);
}
