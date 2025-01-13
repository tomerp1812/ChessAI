#include "WebSocket.h"
#include <iostream>

using namespace boost::asio;
using namespace boost::beast;


WebSocket::WebSocket(int port)
    : acceptor(ioc, ip::tcp::endpoint(ip::tcp::v4(), port)) {
}
WebSocket::~WebSocket() {
    ioc.stop();
}

void WebSocket::session(ip::tcp::socket socket) {
    std::unique_ptr<ChessAi> sessionAi = nullptr;
    try {

        websocket::stream<ip::tcp::socket> ws(std::move(socket));
        ws.accept();

        bool running = false;

        while (true) {
            boost::beast::flat_buffer buffer;
            ws.read(buffer);
            std::string message = buffers_to_string(buffer.data());

            if(message == "START_GAME"){
                sessionAi = std::make_unique<ChessAi>();
                running = true;
                ws.write(boost::asio::buffer("AI_CREATED"));
            }else if(message == "END_GAME"){
                sessionAi.reset();
                running = false;
                ws.write(boost::asio::buffer("AI_DELETED"));
            }else if(running){
                std::string aiMove = sessionAi->run(message);
                ws.write(boost::asio::buffer(aiMove));
            }
        }
    } catch (std::exception &e) {
        std::cerr << "Session error: " << e.what() << std::endl;
    }
    sessionAi.reset();
}

void WebSocket::run() {
    while (true) {
        auto socket = acceptor.accept();
        std::thread(&WebSocket::session, this, std::move(socket)).detach();
    }
}
