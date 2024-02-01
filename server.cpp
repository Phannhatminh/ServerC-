#include <iostream>
#include <boost/asio.hpp>
#include "session_manager.cpp"


using namespace boost::asio;

struct User {
    std::string username;
    std::string password;
    int sessionID;
};

class Server {
private:
    SessionManager* session_manager = &SessionManager::getInstance();
    std::map<std::string, User> users;
    Server() {
        std::cout << "Server created\n";
    }
    int handleSignupRequest(std::string username, std::string password) {
        //Check if the user exists
        if(users.find(username) != users.end()) {
            //User already exists
            return -1;
        } else {
            //User does not exist
            //Create the user
            User user;
            user.username = username;
            user.password = password;
            user.sessionID = -1;
            users[username] = user;
            std::cout << "User " << username << " signed up\n";
            return 0;
        }
    }
    int handleMessage(std::string message, boost::asio::ip::tcp::socket& socket) {
        return session_manager->sendTo(message, socket);
    }
    int handleLoginRequest(std::string username, std::string password, int sessionID) {
        //Login the user...
        //Check if the user exists
        if(users.find(username) != users.end()) {
            //Check if the password is correct
            if(users[username].password == password) {
                //Check if the user is already logged in
                if(users[username].sessionID == -1) {
                    //Login the user
                    users[username].sessionID = sessionID;
                    std::cout << "User " << username << " logged in\n";
                    return 0;
                } else {
                    //User is already logged in
                    std::cout << "User " << username << " is already logged in\n";
                    return -1;
                }
            } else {
                //Password is incorrect
                std::cout << "User " << username << " entered incorrect password\n";
                return -2;
            }
        } else {
            //User does not exist
            std::cout << "User " << username << " does not exist\n";
            return -3;
        }
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
    static int onLoginRequest(std::string username, std::string password, int sessionID) {
        return Server::getInstance().handleLoginRequest(username, password, sessionID);
    }
    static int onChatMessage(std::string message, boost::asio::ip::tcp::socket& socket) {
        return Server::getInstance().handleMessage(message, socket);
    } //change the parameter to user. 
    static int onSignupRequest(std::string username, std::string password) {
        return Server::getInstance().handleSignupRequest(username, password);
    }
    void start() {
        std::cout << "Server started, listening on port 8080\n";
        session_manager->setLoginCallback(Server::onLoginRequest);
        session_manager->setMessageCallback(Server::onChatMessage);
        session_manager->setSignupCallback(Server::onSignupRequest);
        session_manager->listenForConnection();
    }
};

int main() {
    Server& server = Server::getInstance();
    server.start();
    std::cout << "Server is interupted\n";
    return 0;
}

//add message structure for the server:
//including the message type, the message content, and the sender's sessionID
//struct Message {
//    int messageType;
//    std::string messageContent;
//    int senderSessionID;
//};