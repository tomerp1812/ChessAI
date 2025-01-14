#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <winsock2.h>
#include <ws2tcpip.h>
class ChessAi;

class Communicator{
    private:
        SOCKET serverSocket;
        bool running;
        ChessAi *ai;
    public:
        Communicator(int port);
        ~Communicator();
        void communicate();
};


#endif