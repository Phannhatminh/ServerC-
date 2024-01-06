#include <iostream>
#include "server.cpp"

int main() {
    Server server;
    server.start();
    std::cout << "Hello, World!" << std::endl;
    return 0;  
}