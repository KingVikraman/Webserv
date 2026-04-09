#include "Config.hpp"
#include "Server.hpp"
#include <iostream>
 
int main(int argc, char* argv[]) {
    std::string config_file = "Zep/server.conf";
 
    if (argc == 2)
        config_file = argv[1];
    else if (argc > 2) {
        std::cerr << "Usage: ./webserv [config_file]\n";
        return 1;
    }
 
    Config config;
    if (!config.parse(config_file)) {
        std::cerr << "Failed to load config: " << config_file << "\n";
        return 1;
    }
 
    Server server(config);
    if (!server.start())
        return 1;
 
    server.run();
    return 0;
}