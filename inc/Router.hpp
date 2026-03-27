#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "HttpRequest.hpp"

enum RouteType
{
    ROUTE_STATIC_FILE,
    ROUTE_CGI,         // run script and return output
    ROUTE_WRITE_FILE,
    ROUTE_DELETE,
    ROUTE_ERROR
};

struct Route
{
        RouteType type;
        std::string file_path;
        std::string cgi_path;
        std::string upload_dir;
        int error_code;

    Route() : type(ROUTE_ERROR), error_code(0)   // Default constructor initializes to error route (fallback)
    {
    }
};

class Router
{
    public:
        Router();
        ~Router();

        Route route(HttpRequest &request) const; // const because it doesn't modify the Router data

    private:
        bool _hasPathTraversal(const std::string &path) const;
        bool _isCgiRequest(const std::string &path) const;
        bool _isUploadPath(const std::string &path) const;
        std::string _buildPublicPath(const std::string &path) const;
};

#endif
