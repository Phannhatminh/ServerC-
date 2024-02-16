#include <iostream>
#include "../networking/client.cpp"


int main() {
    try {
        std::string server_address = "127.0.0.1";  // Loopback address for local testing
        unsigned short port = PORT;

        Client client(server_address, port);

        // Send a predefined message "Hello from client"
        std::string message = "Hello from client";
        client.sendMessage(message);

        client.closeConnection();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}