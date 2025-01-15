#include "WebSocket.h"
#include <iostream>
#include "ThreadPool.h"
#include "ChessAi.h"

using namespace boost::asio;
using namespace boost::beast;
using namespace std;

WebSocket::WebSocket(int port)
    : acceptor(ioc, ip::tcp::endpoint(ip::tcp::v4(), port)) {
        this->tp = new ThreadPool(5); // creating 5 threads
}
WebSocket::~WebSocket() {
    delete tp;
    ioc.stop();
}

void WebSocket::session(ip::tcp::socket socket) {
    ChessAi* sessionAi = nullptr;
    try {

        websocket::stream<ip::tcp::socket> ws(move(socket));
        ws.accept();

        bool running = false;

        while (true) {
            boost::beast::flat_buffer buffer;
            ws.read(buffer);
            string message = buffers_to_string(buffer.data());

            if(message == "START_GAME"){
                if(sessionAi){
                    cout << "chessAi already exists" << endl;
                    delete sessionAi; 
                }
                sessionAi = new ChessAi();
                running = true;
                ws.write(boost::asio::buffer("AI_CREATED"));
            }else if(message == "END_GAME"){
                ws.write(boost::asio::buffer("AI_DELETED"));
                break;
            }else if(running){
                string aiMove = sessionAi->run(message);
                ws.write(boost::asio::buffer(aiMove));
            }
        }
    } catch (exception &e) {
        cerr << "Session error: " << e.what() << endl;
    } 
    delete sessionAi;   
}

void WebSocket::run() {
    while (true) {
        auto socket = make_shared<ip::tcp::socket>(acceptor.accept());
        tp->enqueue([this, socket]() mutable {
            this->session(move(*socket));
        });
    }
}
