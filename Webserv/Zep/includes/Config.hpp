#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>

struct Location {
    std::string path;
    std::string root;
    std::vector<std::string> index;
    std::vector<std::string> allowed_methods;
    std::string upload_store;
    bool autoindex;
    
    Location() : autoindex(false) {}
};

struct ServerConfig {
    int port;
    std::string server_name;
    std::map<std::string, Location> locations;
    size_t client_max_body_size;
    
    ServerConfig() : port(80), client_max_body_size(1048576) {}
};

class Config {
private:
    std::vector<ServerConfig> _servers;
    
    bool _parseServer(std::ifstream& file, ServerConfig& server);
    bool _parseLocation(std::ifstream& file, Location& location);
    std::string _trim(const std::string& str);
    
public:
    Config();
    ~Config();
    
    bool parse(const std::string& filename);
    const std::vector<ServerConfig>& getServers() const { return _servers; }
    const ServerConfig* findServer(int port) const;
};

#endif