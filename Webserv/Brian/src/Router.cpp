#include "Router.hpp"
#include <algorithm>

Router::Router(const Config& config) : _config(config) {}
Router::~Router() {}

const Location* Router::_findLocation(const ServerConfig& server, const std::string& path) const {
    const Location* best_match = NULL;
    size_t best_length = 0;
    
    for (std::map<std::string, Location>::const_iterator it = server.locations.begin();
         it != server.locations.end(); ++it) {
        
        const std::string& loc_path = it->first;
        
        // Check if request path starts with location path on a segment boundary
        // so /upload does not match /upload.html.
        bool is_prefix = (path.find(loc_path) == 0);
        bool is_boundary = false;
        if (is_prefix) {
            if (path.length() == loc_path.length())
                is_boundary = true;
            else if (!loc_path.empty() && loc_path[loc_path.length() - 1] == '/')
                is_boundary = true;
            else if (path[loc_path.length()] == '/')
                is_boundary = true;
        }

        if (is_prefix && is_boundary) {
            if (loc_path.length() > best_length) {
                best_match = &it->second;
                best_length = loc_path.length();
            }
        }
    }
    
    return best_match;
}

bool Router::_isMethodAllowed(const Location& loc, const std::string& method) const {
    if (loc.allowed_methods.empty())
        return true;
    
    for (size_t i = 0; i < loc.allowed_methods.size(); i++) {
        if (loc.allowed_methods[i] == method)
            return true;
    }
    
    return false;
}

bool Router::_isCgiScript(const std::string& path) const {
    // Check for common CGI extensions
    return (path.find(".php") != std::string::npos ||
            path.find(".py") != std::string::npos ||
            path.find(".pl") != std::string::npos);
}

Route Router::route(HttpRequest& request, int server_port) const {
    Route result;
    const std::string request_path = request.getPath();
    
    // Find server
    const ServerConfig* server = _config.findServer(server_port);
    if (!server) {
        result.error_code = 500;
        return result;
    }
    
    // Find location
    const Location* location = _findLocation(*server, request_path);
    if (!location) {
        result.error_code = 404;
        return result;
    }
    
    // Check if method is allowed
    if (!_isMethodAllowed(*location, request.getMethodStr())) {
        result.error_code = 405;
        return result;
    }
    
    // Build full file path
    std::string full_path = location->root + request_path;
    
    // Determine route type
    if (request.getMethodStr() == "GET") {
        if (!location->upload_store.empty()) {
            std::string relative = request_path;
            if (relative.find(location->path) == 0)
                relative.erase(0, location->path.length());

            if (relative.empty() || relative == "/") {
                full_path = location->upload_store;
            } else {
                if (relative[0] != '/')
                    relative = "/" + relative;
                full_path = location->upload_store + relative;
            }
        }
        else if (location->autoindex && request_path == location->path) {
            full_path = location->root;
        }

        if (_isCgiScript(full_path)) {
            result.type = ROUTE_CGI;
            result.cgi_path = full_path;
        } else {
            result.type = ROUTE_STATIC_FILE;
            result.file_path = full_path;
        }
    }
    else if (request.getMethodStr() == "POST") {
        if (!location->upload_store.empty()) {
            result.type = ROUTE_UPLOAD;
            result.upload_dir = location->upload_store;
        } else {
            result.error_code = 403;
        }
    }
    else if (request.getMethodStr() == "DELETE") {
        result.type = ROUTE_DELETE;

        // DELETE for upload locations should target upload_store files.
        if (!location->upload_store.empty()) {
            std::string relative = request_path;
            if (relative.find(location->path) == 0)
                relative.erase(0, location->path.length());
            if (!relative.empty() && relative[0] != '/')
                relative = "/" + relative;
            result.file_path = location->upload_store + relative;
        } else {
            result.file_path = full_path;
        }
    }
    
    return result;
}