#pragma once

#include "../../Raja/includes/FileHandler.hpp"
#include "HttpRequest.hpp"
#include "../../Raja/includes/HttpResponse.hpp"
#include "Router.hpp"
#include "../../Zep/includes/Config.hpp"
#include "../../Raja/includes/UploadHandler.hpp"
#include "../../Raja/includes/CgiHandler.hpp"

class RequestHandler
{
    public:
        RequestHandler();
        ~RequestHandler();

        // Called by Server — takes raw bytes, returns raw HTTP response string
        std::string buildResponseForRawRequest(const std::string& raw_request) const;

        // Allow Server to inject config after construction
        void setConfig(const Config* config, int port);

    private:
        FileHandler    _file_handler;
        UploadHandler  _upload_handler;
        CgiHandler     _cgi_handler;
        const Config*  _config;
        int            _port;

        HttpResponse _handleGet(HttpRequest& request, const Route& route) const;
        HttpResponse _handlePost(HttpRequest& request, const Route& route) const;
        HttpResponse _handleDelete(const Route& route) const;
        HttpResponse _handleCgi(HttpRequest& request, const Route& route) const;
        HttpResponse _buildErrorResponse(int status_code) const;
        std::string  _buildDirectoryListing(const std::string& dir_path,
                                             const std::string& url_path) const;
};