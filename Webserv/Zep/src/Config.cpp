#include "../includes/Config.hpp"
#include <iostream>
#include <sstream>

Config::Config() {}
Config::~Config() {}

std::string Config::_trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    
    size_t end = str.find_last_not_of(" \t\r\n;");
    return str.substr(start, end - start + 1);
}

bool Config::_parseServer(std::ifstream& file, ServerConfig& server) {
    std::string line;
    
    while (std::getline(file, line)) {
        line = _trim(line);
        
        if (line.empty() || line[0] == '#')
            continue;
        
        if (line == "}")
            return true;
        
        // Parse directives
        std::istringstream iss(line);
        std::string directive;
        iss >> directive;
        
        if (directive == "listen") {
            iss >> server.port;
        }
        else if (directive == "server_name") {
            iss >> server.server_name;
        }
        else if (directive == "client_max_body_size") {
            iss >> server.client_max_body_size;
        }
        else if (directive == "location") {
            Location loc;
            iss >> loc.path;
            
            if (_parseLocation(file, loc))
                server.locations[loc.path] = loc;
        }
    }
    
    return false;
}

bool Config::_parseLocation(std::ifstream& file, Location& location) {
    std::string line;
    
    while (std::getline(file, line)) {
        line = _trim(line);
        
        if (line.empty() || line[0] == '#')
            continue;
        
        if (line == "}")
            return true;
        
        std::istringstream iss(line);
        std::string directive;
        iss >> directive;
        
        if (directive == "root") {
            iss >> location.root;
        }
        else if (directive == "index") {
            std::string index_file;
            while (iss >> index_file)
                location.index.push_back(index_file);
        }
        else if (directive == "allowed_methods") {
            std::string method;
            while (iss >> method)
                location.allowed_methods.push_back(method);
        }
        else if (directive == "upload_store") {
            iss >> location.upload_store;
        }
        else if (directive == "autoindex") {
            std::string value;
            iss >> value;
            location.autoindex = (value == "on");
        }
    }
    
    return false;
}

bool Config::parse(const std::string& filename) {
    std::ifstream file(filename.c_str());
    
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << "\n";
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = _trim(line);
        
        if (line.empty() || line[0] == '#')
            continue;
        
        if (line == "server {") {
            ServerConfig server;
            if (_parseServer(file, server))
                _servers.push_back(server);
        }
    }
    
    file.close();
    return !_servers.empty();
}

const ServerConfig* Config::findServer(int port) const {
    for (size_t i = 0; i < _servers.size(); i++) {
        if (_servers[i].port == port)
            return &_servers[i];
    }
    return NULL;
}