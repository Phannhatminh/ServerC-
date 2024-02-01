#include <iostream>
#include <boost/asio.hpp>
#include "mock_session_manager.cpp"


using namespace boost::asio;

struct User {
    std::string username;
    std::string password;
    int sessionID;
};

class Server {
private:
    SessionManager* session_manager = &SessionManager::getInstance();
    std::map<int, std::string> sessionIDToUsername;
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
    int handleMessage(std::string message, std::string username) {
        return session_manager->sendTo(message, users[username].sessionID);
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
    std::string getUsernameBySessionID(int sessionID) {
        for(auto& user : users) {
            if(user.second.sessionID == sessionID) {
                return user.first;
            }
        }
        return "";
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
    static int onChatMessage(std::string message, int sessionID) {
        return Server::getInstance().handleMessage(message, Server::getInstance().getUsernameBySessionID(sessionID)); //Invalid using sessionID in a static member function
    }  
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

//Write the code for mapping sessionID to socket index
//Done! 13 January 2024, 3:46 AM.
//Write the code for echoing the message back to the client
//Tested, works
//Done! 13 January 2024, 3:46 AM.


//Jan 23, plan: 4 p.m. - 7 p.m.:
//Understand boost::asio socket and then use it to write class Session
//Write the code for the asynchronous server by boost and test if it can run (for boost socket to work in a class) 
//(copy from the guideline)
//If success, try to implement the way to write the class Session
//SessionManager is able to create and manage sessions
//SessionManager is able to send messages to clients

//Define the message structure
//Study asn1
//Write the code for decoding and encoding messages


//change the parameter in the MessageCallbackFunction (SessionManager.cpp): boost::asio::ip::tcp::socket& to int
//change the parameter of the onMessage function (Server.cpp)
//change the parameter of the handleMessgae() function (Server.cpp)
//change the parameter of the sendTo() function (SessionManager.cpp)
//std::map<int, int> sessionIDToSocketIndex; (SessionManager.cpp)


//Write the class Session that can manager an active socket
//when a client log in to the system, a session is created for that client
//Message sending to any client is done through the session
//the session manager is responsible for creating and managing sessions