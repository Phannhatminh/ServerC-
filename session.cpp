#include <iostream>
#include <map>
#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;
using CallbackMessageFunction = int (*)(std::string, int);

class Session {
private:
    CallbackMessageFunction message_callback;
    ip::tcp::socket* socket;
    int sessionID;
public:
    Session() {
        std::cout << "Session created\n";
    }
    ~Session() {
        std::cout << "Session destroyed\n";
    }
    void setMessageCallback(CallbackMessageFunction cb) {
        message_callback = cb;
    }
    void setSocket(ip::tcp::socket* socket_ptr) {
        socket = socket_ptr;
    }
    void setSessionID(int sessionID) {
        sessionID = sessionID;
    }
    int getSessionID() {
        return sessionID;
    }
    ip::tcp::socket* getSocket() {
        return socket;
    }
    bool authenticate(int ID) {
        return ID == sessionID;
    }
    void sendMessage(std::string message) {
        boost::system::error_code error;
        write(*socket, buffer(message), error);
    }
    void startSession() {
        while(true) {
            char data[1024];
            boost::system::error_code error;
            size_t length = socket->read_some(buffer(data), error);
            if (error == error::eof) {
                std::cout << "Connection closed by client\n";
                break;
            } else if (error) {
                throw boost::system::system_error(error);
            }
            std::string message = data;
            message_callback(message, sessionID); //OnMessageReceivedl
        }
    }
};