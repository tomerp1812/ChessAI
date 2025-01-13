#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

class ThreadPool;

class WebSocket {
private:
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::acceptor acceptor;
    void session(boost::asio::ip::tcp::socket socket);
    ThreadPool* tp;
public:
    WebSocket(int port);
    ~WebSocket();
    void run();
};

#endif
