#include <iostream>
#include <boost/asio.hpp>
#include "session_manager.cpp"


using namespace boost::asio;

struct User {
    std::string username;
    std::string password;
    int sessionID = -1;
};

class Server {
private:
    SessionManager* session_manager = &SessionManager::getInstance();
    //This map handles users name
    std::map<std::string, User> users;
    //This map handles the sessionID of the user
    std::map<int, User> sessionIDToUser;
    
    Server() {
        std::cout << "Server created\n";
    }
    ~Server() {
        std::cout << "Server destroyed\n";
    }
    // Make sure that there is at most one instance of Server
    Server(const Server&) = delete; // delete copy constructor
    Server& operator=(const Server&) = delete;

    void sendTo(int sessionID, std::string message) {
        session_manager->sendTo(sessionID, message);
    }
public:
    static Server& getInstance() {
        static Server instance;
        return instance;
    }
    //Callbacks
    //function to handle login request
    static int onLoginRequest(std::string username, std::string password, int sessionID) {
        //check if the username and password are correct
        if (Server::getInstance().users.find(username) != Server::getInstance().users.end()) {
            if (Server::getInstance().users[username].password == password) {
                //setting up the sessionID for the user
                Server::getInstance().users[username].sessionID = sessionID;
                Server::getInstance().sessionIDToUser[sessionID] = Server::getInstance().users[username];
                //send the sessionID to the user
                Server::getInstance().sendTo(sessionID, "Login successful. Your sessionID is " + std::to_string(sessionID));
                return 0;
            }
            return -1;
        } 
    }
    static int onChatMessage(std::string message, int sessionID) {
        for (auto& user : Server::getInstance().users) {
            if (user.second.sessionID != sessionID) {
                Server::getInstance().sendTo(user.second.sessionID, message);
            }
        }
    }  
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
    void start() {
        session_manager->setMessageCallback(onChatMessage);
        session_manager->setLoginCallback(onLoginRequest);
        session_manager->setSignupCallback(onSignupRequest);
        session_manager->start();
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