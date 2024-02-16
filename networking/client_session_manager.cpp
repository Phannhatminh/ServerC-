#include <iostream>
#include <boost/asio.hpp>
include "session.cpp"

class ClientSessionManager {
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
    ClientSessionManager(IPVersion version, int port) {
        std::cout << "SessionManager created\n";    
    }

    //private destructor
    ~ClientSessionManager() {
        std::cout << "SessionManager destroyed\n";
    }

    // Make sure that there is at most one instance of SessionManager
    ClientSessionManager(const ClientSessionManager&) = delete; // delete copy constructor
    ClientSessionManager& operator=(const ClientSessionManager&) = delete;
public:
    //function to get the instance of the SessionManager
    static ClientSessionManager& getInstance(IPVersion version, int port) {
        //create a new instance of the SessionManager
        static ClientSessionManager instance(version, port);

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

    void start() {
        Session* session = new Session();
        session->setMessageCallback(message_callback);
        session->setLoginCallback(login_callback);
        session->setSignupCallback(signup_callback);
        sessionIDToSession[session->getSessionID()] = session;
        sessionToSessionID[session] = session->getSessionID();
        try {
            //Resolve the server endpoint
            boost::asio::io_context io_context;
            boost::asio::ip::tcp::resolver resolver(io_context);
            auto endpoints = resolver.resolve("195.252.210.98", 8080);

            //Connect to the server
            boost::asio::ip::tcp::socket socket(io_context);
            boost::asio::connect(socket, endpoints);

            //set the socket to the session
            session -> setSocket(&socket);
        } catch (const std::exception& e) {
            std::cerr << "Error communicating with the server: " << e.what() << std::endl;
        }
        session -> startSession();
    }
};
