#include <iostream>
#include <boost/asio.hpp>

#define PORT 8080

using boost::asio::ip::tcp;

class Client {
private:
    boost::asio::io_context io_context;
    tcp::socket socket;

public:
    Client(const std::string& server_address, unsigned short port)
        : socket(io_context) {
        // Resolve the server address and port
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(server_address, std::to_string(port));

        // Connect to the server
        boost::asio::connect(socket, endpoints);
    }

    void sendMessage(const std::string& message) {
        boost::asio::write(socket, boost::asio::buffer(message + "\n"));
    }

    std::string receiveMessage() {
        char response[1024];
        size_t len = socket.read_some(boost::asio::buffer(response, sizeof(response)));
        return std::string(response, len);
    }

    void closeConnection() {
        socket.close();
    }
};

int main() {
    try {
        std::string server_address = "127.0.0.1";  // Loopback address for local testing
        unsigned short port = PORT;

        Client client(server_address, port);

        // Send a predefined message "Hello from client"
        std::string message = "Hello from client";
        client.sendMessage(message);

        // Wait for and print any response from the server
        std::string receivedMessage = client.receiveMessage();
        std::cout << "Response from server: " << receivedMessage << std::endl;

        client.closeConnection();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
