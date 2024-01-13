#include <iostream>
#include <boost/asio.hpp>
#include "session.cpp"


using namespace boost::asio;

class Server {
private:
    SessionManager* session_manager = &SessionManager::getInstance();
    Server() {
        std::cout << "Server created\n";
    }
    ~Server() {
        std::cout << "Server destroyed\n";
    }
    int handleMessage(std::string message, boost::asio::ip::tcp::socket& socket) {
        return session_manager->sendTo(message, socket);
    }
    int handleLoginRequest() {
        //Login the user...
        session_manager->addSession(1, 2);
        return 0;
    }
    // Make sure that there is at most one instance of Server
    Server(const Server&) = delete; // delete copy constructor
    Server& operator=(const Server&) = delete;
public:
    static Server& getInstance() {
        static Server instance;
        return instance;
    }
    static int onLoginRequest() {
        return Server::getInstance().handleLoginRequest();
    }
    static int onChatMessage(std::string message, boost::asio::ip::tcp::socket& socket) {
        return Server::getInstance().handleMessage(message, socket);
    }
    void start() {
        std::cout << "Server started, listening on port 8080\n";
        session_manager->setLoginCallback(Server::onLoginRequest);
        session_manager->setMessageCallback(Server::onChatMessage);
        session_manager->listenForConnection();
    }
};

int main() {
    Server& server = Server::getInstance();
    server.start();
    return 0;
}

//Write the code for mapping sessionID to socket index
//Done! 13 January 2024, 3:46 AM.
//Write the code for echoing the message back to the client
//Tested, works
//Done! 13 January 2024, 3:46 AM.
//Then write the code for login request and session checking
//Test
//Then write the code for logout request
//Test
//Then write the code for automatic logout
//Test
//Then write the code for handling general messages
//Test
//Then write the code for defining messages structure
//Test
//Then define the message structure and rewrite the code
//Test
//Then adjust the code for handling real clients at other hosts
//Test
//Then write the client code
//Test
//Then test the whole system
//Done, then push it to github