#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"

enum RouteType {
    ROUTE_STATIC_FILE, // Serve a regular static file from disk
    ROUTE_CGI,         // Execute a CGI script and return its output
    ROUTE_UPLOAD,      // Handle file upload target/destination
    ROUTE_DELETE,      // Handle resource/file deletion request
    ROUTE_REDIRECT,    // Return HTTP redirect to another URL
    ROUTE_ERROR        // Route failed; return an error response
};

struct Route {
    RouteType type;         // Final routing decision (static/cgi/upload/delete/redirect/error)
    std::string file_path;  // Target file path for static serving or deletion
    std::string cgi_path;   // CGI script/executable path when type is ROUTE_CGI
    std::string upload_dir; // Upload destination directory when type is ROUTE_UPLOAD
    int redirect_code;      // HTTP redirect status code (e.g. 301, 302)
    std::string redirect_url; // Redirect destination URL when type is ROUTE_REDIRECT
    int error_code;         // HTTP error status code when type is ROUTE_ERROR
    
    // Safe defaults until router sets a more specific route.
    Route() : type(ROUTE_ERROR), redirect_code(302), error_code(404) {}
};

class Router {
private:
    const Config& _config; // Global parsed config used for all routing decisions
    
    // Find the best-matching location block for a given request path.
    const LocationConfig* _findLocation(const ServerConfig& server, const std::string& path) const;
    bool _isMethodAllowed(const LocationConfig& loc, const std::string& method) const;
    bool _isCgiScript(const std::string& path) const;
    
public:
    Router(const Config& config);
    ~Router();
    
    // Main routing entry point: map request + port to a concrete Route action.
    Route route(HttpRequest& request, int server_port) const;
};

#endif
