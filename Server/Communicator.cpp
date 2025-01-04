#include "Communicator.h"
#include <iostream>
#include "ChessAi.h"

using namespace std;

Communicator::Communicator(int port)
{
    this->ai = new ChessAi();
    this->running = true;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    // Check for initialization success
    if (wsaerr != 0)
    {
        cout << "The Winsock dll not found!" << endl;
        return;
    }
    // AF_INET - specifiec IPv4 protocol family
    // socketfd - file descriptor for the socket
    // SOCK_STREAM - socket is of type TCP
    this->serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Check for socket creation success
    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }
    // stores the address of the socket
    sockaddr_in service;
    service.sin_family = AF_INET;
    // converts unsigned int from machine byte order to network byte order
    service.sin_port = htons(port);
    // listens to all availalbe IPs
    service.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<SOCKADDR *>(&service), sizeof(service)) == SOCKET_ERROR)
    {
        cout << "bind() failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        cout << "listen(): Error listening on socket: " << WSAGetLastError() << endl;
    }
}

Communicator::~Communicator()
{
    // closing the socket
    closesocket(this->serverSocket);
    WSACleanup();
}

void Communicator::communicate()
{

    while (this->running)
    {
        // Accept incoming connections
        SOCKET acceptSocket = accept(this->serverSocket, nullptr, nullptr);
        if (acceptSocket == INVALID_SOCKET)
        {
            cout << "accept() failed: " << WSAGetLastError() << endl;
            continue;
        }
        while (true)
        {
            // Communication with the client
            int dataSize = 0;
            unsigned char sizeBytes[4];
            int result = recv(acceptSocket, reinterpret_cast<char *>(sizeBytes), 4, 0);
            if (result != 4)
            {
                cout << "recv() failed or client disconnected: " << endl;
                closesocket(acceptSocket);
                this->running = false;
                break;
            }

            // little endiean to big endian
            dataSize = (sizeBytes[0] << 24) | (sizeBytes[1] << 16) | (sizeBytes[2] << 8) | sizeBytes[3];
            char *receiveBuffer = new char[dataSize + 1];
            receiveBuffer[dataSize] = '\0';

            result = recv(acceptSocket, receiveBuffer, dataSize, 0);
            if (result <= 0)
            {
                cout << "Failed to receive data: " << WSAGetLastError() << endl;
                delete[] receiveBuffer;
                closesocket(acceptSocket);
                continue;
            }

            string receivedData(receiveBuffer);
            const char *responseBuffer;
            if (receivedData == "Done!")
            {
                this->running = false;
                responseBuffer = "Ok! communication finished";
            }
            else
            {
                string response = ai->run(receivedData);
                responseBuffer = response.c_str();
            }

            int sbyteCount = send(acceptSocket, responseBuffer, strlen(responseBuffer), 0);
            if (sbyteCount == SOCKET_ERROR)
            {
                cout << "send() failed: " << WSAGetLastError() << endl;
            }
            else
            {
                cout << "Sent: " << responseBuffer << endl;
            }

            // delete the buffer
            delete[] receiveBuffer;

            if (!this->running)
            {
                closesocket(acceptSocket);
                break;
            }
        }
    }
}
