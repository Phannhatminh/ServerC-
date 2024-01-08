#include <iostream>
#include <boost/asio.hpp>
#define PORT 8080

using namespace boost::asio;

class Server {
public:
    Server() {
        std::cout << "Server created" << std::endl;
    }
    ~Server() {
        std::cout << "Server destroyed" << std::endl;
    }
    void start() {
        // Create an io_context object
        io_context io_context;

        // Create an endpoint to accept connections on port 8080
        ip::tcp::endpoint endpoint(ip::tcp::v4(), 8080);

        // Create an acceptor to listen for incoming connections
        ip::tcp::acceptor acceptor(io_context, endpoint);

        std::cout << "Server started. Listening on port 8080...\n";

        while (true) {
            // Create a socket and accept a new connection
            ip::tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;

            // Create a buffer to store data received from the client
            char data[1024];
            std::size_t length = socket.read_some(buffer(data));

            // Echo back the received message to the client
            socket.write_some(buffer(data, length));

            std::cout << "Echoed back: " << data << std::endl;

            // Close the socket
            socket.close();
        }
    }
};

int main() {
    Server server;
    server.start();
    return 0;
}