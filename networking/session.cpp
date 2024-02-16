#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <queue>
#include <functional>
#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <thread>

using namespace boost::asio;

//Callback function for handling messages
using CallbackMessageFunction = int (*)(std::string, int);
using CallbackLoginFunction = int (*)(std::string, std::string, int);
using CallbackLogoutFunction = int (*)(int);
using CallbackSignupFunction = int (*)(std::string, std::string);
using CallbackCheckSignupFunction = bool (*)(std::string, std::string);

//IPVersion enum class
enum class IPVersion {
    IPv4,
    IPv6
};

//Session class to handle the session
class Session : public std::enable_shared_from_this<Session> {
private:
    CallbackMessageFunction message_callback;
    ip::tcp::socket* socket;
    int sessionID;
    std::queue<std::string> outGoingMessages;

    //streambuf to handle the message
    boost::asio::streambuf streamBuf;


    //async_read: Wait for the client to send a message
    void async_read() {
        async_read_until(*socket, streamBuf, "\n", [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            on_read(ec, bytes_transferred);
        });
    }
    //on_read
    void on_read(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
            socket->close();

            //add an error handler
            return;
        }

        std::stringstream message;
        message << std::istream(&streamBuf).rdbuf();

        message_callback(message.str(), sessionID); // Convert std::stringstream to std::string

        //add a message handler
        async_read();
    }

    //Post method
    void post(std::string message) {
        bool queueIdle = outGoingMessages.empty();
        outGoingMessages.push(message);

        if (queueIdle) {
            async_write(message);
        } else {

        }
    }

    //async_write: Send a message to the client
    //write the message to the queue, and the queue will be processed by the async_write
    void async_write(const std::string& message) {
        boost::asio::async_write(*socket, boost::asio::buffer(message), [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            on_write(ec, bytes_transferred);
        });
    }

    //on_write
    void on_write(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        // Handle write completion
        if (ec) {
            socket->close();

            //add an error handler
            return;
        }

        outGoingMessages.pop();

        if (!outGoingMessages.empty()) {
            async_write(outGoingMessages.front());
        }
    }

public:
    //constructor
    Session() {
        std::cout << "Session created\n";
        streamBuf.prepare(1024);
    }

    //destructor
    ~Session() {
        std::cout << "Session destroyed\n";
    }

    // Make sure that there is at most one instance of Session
    void setMessageCallback(CallbackMessageFunction cb) {
        message_callback = cb;
    }

    //function to set the socket
    void setSocket(ip::tcp::socket* socket_ptr) {
        socket = socket_ptr;
    }

    //function to set the sessionID
    void setSessionID(int sessionID) {
        sessionID = sessionID;
    }

    //function to get the sessionID
    int getSessionID() {
        return sessionID;
    }

    //function to get the socket
    ip::tcp::socket* getSocket() {
        return socket;
    }

    //function to authenticate the user
    bool authenticate(int ID) {
        return ID == sessionID;
    }

    //function to send message to the user
    void sendMessage(std::string message) {
        boost::system::error_code error;
        write(*socket, buffer(message), error);
    }

    //function to start the session
    void startSession(/* ARGUMENTS -- CALLBACKS*/) {
        //start the session
        //launch the read and write threads
        std::thread readThread([this]() {
            async_read();
        });
        std::thread writeThread([this]() {
            async_write(outGoingMessages.front());
        });
        // Wait for both threads to finish
        readThread.detach();
        writeThread.detach();
    }
};