
#include "Communicator.h"
#include "WebSocket.h"
using namespace std;

int main(){
    // Communicator* communicator = new Communicator(8080);
    // communicator->communicate();
    // delete communicator;

    WebSocket* webSocket = new WebSocket(8080);
    webSocket->run();
    delete webSocket;
    return 0;
}