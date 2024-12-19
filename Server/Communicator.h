#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include "ChessAi.h"

class Communicator{
    private:
        SOCKET serverSocket;
        bool running;
        ChessAi *ai;
    public:
        Communicator(int port, ChessAi ai);
        ~Communicator();
        void communicate();
};


#endif