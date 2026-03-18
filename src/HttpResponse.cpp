#include "../inc/HttpResponse.hpp"

#include <sstream>

HttpResponse::HttpResponse() : _status_code(200), _status_message("OK")
{
}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::_getStatusMessage(int code) const
{
    switch (code)
    {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 414: return "URI Too Long";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        default: return "Unknown";
    }
}

void HttpResponse::setStatus(int code)
{
    _status_code = code;
    _status_message = _getStatusMessage(code);
}

void HttpResponse::setHeader(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string &body)
{
    std::ostringstream oss;

    _body = body;
    oss << body.length();
    setHeader("Content-Length", oss.str());
}

std::string HttpResponse::build() const
{
    std::ostringstream response;

    response << "HTTP/1.1 " << _status_code << " " << _status_message << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    response << _body;
    return response.str();
}

HttpResponse HttpResponse::ok(const std::string &body, const std::string &content_type)
{
    HttpResponse resp;

    resp.setStatus(200);
    resp.setHeader("Content-Type", content_type);
    resp.setBody(body);
    return resp;
}

HttpResponse HttpResponse::created(const std::string &body)
{
    HttpResponse resp;

    resp.setStatus(201);
    resp.setHeader("Content-Type", "text/plain");
    resp.setBody(body);
    return resp;
}

HttpResponse HttpResponse::noContent()
{
    HttpResponse resp;

    resp.setStatus(204);
    resp.setHeader("Content-Length", "0");
    return resp;
}

HttpResponse HttpResponse::notFound()
{
    HttpResponse resp;

    resp.setStatus(404);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>404 Not Found</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::methodNotAllowed()
{
    HttpResponse resp;

    resp.setStatus(405);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::uriTooLong()
{
    HttpResponse resp;

    resp.setStatus(414);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>414 URI Too Long</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::notImplemented()
{
    HttpResponse resp;

    resp.setStatus(501);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>501 Not Implemented</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::internalError()
{
    HttpResponse resp;

    resp.setStatus(500);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::badRequest()
{
    HttpResponse resp;

    resp.setStatus(400);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>400 Bad Request</h1></body></html>");
    return resp;
}
