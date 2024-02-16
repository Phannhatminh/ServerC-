#include <iostream>
#include "../networking/server.cpp"

int main() {
    Server& server = Server::getInstance();
    server.start();
    std::cout << "Server is interupted\n";
    std::cout << "Server is stopped\n";
    return 0;
}

//Compile command: /opt/homebrew/bin/g++-13 main.cpp -o main -I/opt/homebrew/Cellar/boost/1.83.0/include -L/opt/homebrew/Cellar/boost/1.83.0/lib -lboost_system