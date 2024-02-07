#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "session.cpp"

using namespace boost::asio;
using CallbackLoginFunction = int (*)(std::string, std::string, int);
using CallbackLogoutFunction = int (*)(int);
using CallbackSignupFunction = int (*)(std::string, std::string);
using CallbackCheckSignupFunction = bool (*)(std::string, std::string);

class SessionManager {
private:
    CallbackMessageFunction message_callback;
    CallbackLoginFunction login_callback;
    CallbackSignupFunction signup_callback;
    std::map<int, Session*> sessionIDToSession;
    std::map<Session*, int> sessionToSessionID;

    SessionManager() {
        std::cout << "SessionManager created\n";
    }
    ~SessionManager() {
        std::cout << "SessionManager destroyed\n";
    }
    // Make sure that there is at most one instance of SessionManager
    SessionManager(const SessionManager&) = delete; // delete copy constructor
    SessionManager& operator=(const SessionManager&) = delete;
public:
    static SessionManager& getInstance() {
        static SessionManager instance;
        return instance;
    }
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
    void sendTo(int sessionID, std::string message) {
        sessionIDToSession[sessionID]->sendMessage(message);
    }
    void start() {
        io_service service;
        ip::tcp::endpoint ep(ip::tcp::v4(), 8080);
        ip::tcp::acceptor acc(service, ep);
        while(true) {
            ip::tcp::socket sock(service);
            acc.accept(sock);
            if (checkSignUpRequest()) {
                signup_callback("username", "password");
            }
            if (checkLoginRequest()) {
                Session* session = new Session();
                sessionIDToSession[session->getSessionID()] = session;
                sessionToSessionID[session] = session->getSessionID();
                session->setMessageCallback(message_callback);
                session->setSocket(&sock);
                //Call the login_callback to check if the user is authenticated
                login_callback("username", "password", session->getSessionID());
                //send the SessionID to the client
                session->startSession();
            }
        }
    }   
};

//Write the code for echoing the message back to the client
//Test
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


//create a map between sessionID and socket.
//when a client send a message to a socket in a session, the session_manager will handle that message for that particular client
