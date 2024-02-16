#include <iostream>
#include <boost/asio.hpp>
#include "client_session_manager.cpp"
#include "user.cpp"

#define PORT 8080

using boost::asio::ip::tcp;

class Client {
private:
    //SessionManager to handle the session
    ClientSessionManager* session_manager = &SessionManager::getInstance(IPVersion::IPv4, 8080);

    User user;

    //private constructor
    Client() {
        std::cout << "Client created\n";
    }
    //private destructor
    ~Client() {
        std::cout << "Client destroyed\n";
    }
    // Make sure that there is at most one instance of Client
    Client(const Client&) = delete; // delete copy constructor
    Client& operator=(const Client&) = delete;

public:
    //function to validate user input
    void input_handler() {
        std::cout << "Enter your username: ";
        std::cin >> user.username;
        std::cout << "Enter your password: ";
        std::cin >> user.password;
    }
    
    //function to get the instance of the client
    static Client& getInstance() {
        static Client instance;
        return instance;
    }

    //Callbacks
    //function to handle login request
    static void onLoginRequest(std::string username, std::string password) {
        //send the login request to the server
        Client::getInstance().session_manager->sendLoginRequest(username, password);
    }

    //function to handle signup request
    static void onSignupRequest(std::string username, std::string password) {
        //send the signup request to the server
        Client::getInstance().session_manager->sendSignupRequest(username, password);
    }

    //function to handle message request
    static void onMessageRequest(std::string message) {
        //send the message to the server
        Client::getInstance().session_manager->sendMessageRequest(message);
    }

    //function to handle the message from the server
    static void onMessageReceived(std::string message) {
        //print the message
        std::cout << message << std::endl;
    }
};

