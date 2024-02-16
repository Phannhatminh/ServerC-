#include <iostream>
#include <boost/asio.hpp>
#include "server_session_manager.cpp"
#include "user.cpp"

using namespace boost::asio;

class Server {
private:
    //SessionManager to handle the session
    ServerSessionManager* session_manager = &ServerSessionManager::getInstance(IPVersion::IPv4, 8080);

    //This map handles users name
    std::map<std::string, User> users;

    //This map handles the sessionID of the user
    std::map<int, User> sessionIDToUser;
    
    //private constructor
    Server() {
        std::cout << "Server created\n";
    }
    
    //private destructor
    ~Server() {
        std::cout << "Server destroyed\n";
    }

    // Make sure that there is at most one instance of Server
    Server(const Server&) = delete; // delete copy constructor
    Server& operator=(const Server&) = delete;

    //function to send message to a specific user
    void sendTo(int sessionID, std::string message) {
        session_manager->sendTo(sessionID, message);
    }

    //function to broadcast message to all users
    void broadcast(std::string message, int sessionID) {
        for (auto& user : Server::getInstance().users) {
            //check if the user is not the sender
            if (user.second.sessionID != sessionID) {
                //send the message to the user
                Server::getInstance().sendTo(user.second.sessionID, sessionIDToUser[sessionID].username + " has sent: " + message);
            }
        }
        Server::getInstance().sendTo(sessionID, "You have sent: " + message);
    }
public:
    //function to get the instance of the server
    static Server& getInstance() {
        static Server instance;
        return instance;
    }

    //Callbacks
    //function to handle login request
    static int onLoginRequest(std::string username, std::string password, int sessionID) {
        //check if the username and password are correct
        if (Server::getInstance().users.find(username) != Server::getInstance().users.end()) {
            //check if the password is correct
            if (Server::getInstance().users[username].password == password) {
                //setting up the sessionID for the user
                Server::getInstance().users[username].sessionID = sessionID;

                //add the sessionID to the sessionIDToUser map
                Server::getInstance().sessionIDToUser[sessionID] = Server::getInstance().users[username];

                //send the sessionID to the user
                Server::getInstance().sendTo(sessionID, "Login successful. Your sessionID is " + std::to_string(sessionID));

                return 0;
            }
            return -1;
        } 
    }

    //function to handle chat message
    static int onChatMessage(std::string message, int sessionID) {
        //send the message to all users except the sender
        Server::getInstance().broadcast(message, sessionID);
    } 

    //function to handle signup request
    static int onSignupRequest(std::string username, std::string password) {
        //check if the username is already taken
        if (Server::getInstance().users.find(username) != Server::getInstance().users.end()) {
            return -1;
        }
        //create a new user
        User user;
        user.username = username;
        user.password = password;
        Server::getInstance().users[username] = user;
        return 0;
    }

    //function to start the server
    void start() {
        session_manager->setMessageCallback(onChatMessage);
        session_manager->setLoginCallback(onLoginRequest);
        session_manager->setSignupCallback(onSignupRequest);
        session_manager->start();
    }
};

