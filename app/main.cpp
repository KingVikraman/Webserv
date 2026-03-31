#include "../inc/ServerManager.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
    std::string host = "0.0.0.0";
    int port = 8000;
    ServerManager server;

    if (argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " [host] [port]" << std::endl;
        return 1;
    }
    if (argc >= 2)
        host = argv[1];
    if (argc == 3)
        port = std::atoi(argv[2]);
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Invalid port: " << port << std::endl;
        return 1;
    }
    if (!server.setup(host, port))
        return 1;
    server.run();
    return 0;
}
