#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "session.cpp"
#include "session_manager.cpp"

using namespace boost::asio;

//SessionManager class to handle the session
class ServerSessionManager {
private:
    //ip version
    IPVersion version;

    //port
    int port;

    //Callback functions
    CallbackMessageFunction message_callback;
    CallbackLoginFunction login_callback;
    CallbackSignupFunction signup_callback;
    std::map<int, Session*> sessionIDToSession;
    std::map<Session*, int> sessionToSessionID;

    //private constructor
    ServerSessionManager(IPVersion version, int port) {
        std::cout << "SessionManager created\n";    
    }

    //private destructor
    ~ServerSessionManager() {
        std::cout << "SessionManager destroyed\n";
    }

    // Make sure that there is at most one instance of SessionManager
    ServerSessionManager(const ServerSessionManager&) = delete; // delete copy constructor
    ServerSessionManager& operator=(const ServerSessionManager&) = delete;
public:
    //function to get the instance of the SessionManager
    static ServerSessionManager& getInstance(IPVersion version, int port) {
        //create a new instance of the SessionManager
        static ServerSessionManager instance(version, port);

        //return the instance
        return instance;
    }

    //Callbacks
    void setMessageCallback(CallbackMessageFunction cb) {
        message_callback = cb;
    }

    void setLoginCallback(CallbackLoginFunction cb) {
        login_callback = cb;
    }

    void setSignupCallback(CallbackSignupFunction cb) {
        signup_callback = cb;
    }

    bool checkSignUpRequest() { //method for checking if the message is a signup request
        return false;
    }

    bool checkLoginRequest() { //method for checking if the message is a login request
        return true;
    }

    //function to send message to a specific user
    void sendTo(int sessionID, std::string message) {
        sessionIDToSession[sessionID]->sendMessage(message);
    }

    //function to start the session manager
    void start() {
        //create a new io_service
        io_service service;

        //create a new endpoint
        ip::tcp::endpoint ep(version == IPVersion::IPv4 ? ip::tcp::v4() : ip::tcp::v6(), port);

        //create a new acceptor
        ip::tcp::acceptor acc(service, ep);

        //start the server
        while(true) {
            //create a new socket for client communication
            ip::tcp::socket sock(service);

            //accept the client
            acc.accept(sock);

            //check if the message is a login request
            if (checkSignUpRequest()) {
                signup_callback("username", "password");
            } 

            //check if the message is a signup request

            else if (checkLoginRequest()) {
                //create a new session
                Session* session = new Session();

                //set the message callback
                sessionIDToSession[session->getSessionID()] = session;

                //set the sessionID
                sessionToSessionID[session] = session->getSessionID();

                //set the message callback
                session->setMessageCallback(message_callback);

                //set the socket
                session->setSocket(&sock);

                //Call the login_callback to check if the user is authenticated
                login_callback("username", "password", session->getSessionID());

                //send the SessionID to the client
                session->startSession();
            }
        }
    }   
};

