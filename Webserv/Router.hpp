#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"

enum RouteType {
    ROUTE_STATIC_FILE,
    ROUTE_CGI,
    ROUTE_UPLOAD,
    ROUTE_DELETE,
    ROUTE_ERROR
};

struct Route {
    RouteType type;
    std::string file_path;
    std::string cgi_path;
    std::string upload_dir;
    int error_code;
    
    Route() : type(ROUTE_ERROR), error_code(404) {}
};

class Router {
private:
    const Config& _config;
    
    const LocationConfig* _findLocation(const ServerConfig& server, const std::string& path) const;
    bool _isMethodAllowed(const LocationConfig& loc, const std::string& method) const;
    bool _isCgiScript(const std::string& path) const;
    
public:
    Router(const Config& config);
    ~Router();
    
    Route route(HttpRequest& request, int server_port) const;
};

#endif