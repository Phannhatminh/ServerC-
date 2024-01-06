#include <iostream>
#include <stdio.h>
#include <netinet/in.h> // for sockaddr_in and inet_ntoa()
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h> // different address structures are declared here
#include <stdlib.h> // atoi() which convert string to integer
#include <string.h> // memset() which fills the structure with a constant byte
#include <unistd.h> // close() function
#define PORT 8080

class Server {
private:
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
public:
    Server() {
        std::cout << "Server created" << std::endl;
    }
    ~Server() {
        std::cout << "Server destroyed" << std::endl;
    }
    void start() {
        // Creating socket file descriptor
        if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Forcefully attaching socket to the port 8080
        if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if(listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        while(true) {
            if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            valread = read(new_socket, buffer, 1024-1);
            printf("%s\n", buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
        }
    }
};

int main() {
    Server server;
    server.start();
    return 0;
}