#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "ChessAi.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <thread>


class WebSocket {
private:
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::acceptor acceptor;
    void session(boost::asio::ip::tcp::socket socket);

public:
    WebSocket(int port);
    ~WebSocket();
    void run();
};

#endif
