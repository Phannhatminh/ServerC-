#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "session.cpp"

using namespace boost::asio;

class App {
private:
    Session* session;
    std::string user_name;
    std::string password;
public:
    App(const std::string& server_address, unsigned short port) {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(server_address, std::to_string(port));
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        session -> setSocket(&socket);
        std::cout << "App created\n";
    }
    ~App() {
        std::cout << "App destroyed\n";
    }
    void input_handler() {
        std::cout << "Enter your username: ";
        std::cin >> user_name;
        std::cout << "Enter your password: ";
        std::cin >> password;
    }
    void start() {
        session->startSession();
    }
};

