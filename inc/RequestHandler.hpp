# pragma once

#include "FileHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Router.hpp"

class RequestHandler
{
    public:
        RequestHandler();
        ~RequestHandler();

        std::string buildResponseForRawRequest(const std::string &raw_request) const;

    private:
        FileHandler _file_handler;
        Router _router;

        HttpResponse _buildErrorResponse(int status_code) const;
};
