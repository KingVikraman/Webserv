#include "RequestHandler.hpp"
#include <sstream>
#include <dirent.h>
#include <cstring>

RequestHandler::RequestHandler()
    : _config(NULL), _port(8000) {}

RequestHandler::~RequestHandler() {}

void RequestHandler::setConfig(const Config* config, int port) {
    _config  = config;
    _port    = port;
}

// ─── Error helper ────────────────────────────────────────────────────────────

HttpResponse RequestHandler::_buildErrorResponse(int status_code) const {
    if (_config) {
        const std::vector<ServerConfig>& servers = _config->getServers();
        const ServerConfig* server = NULL;

        for (size_t i = 0; i < servers.size(); ++i) {
            if (servers[i].port == _port) {
                server = &servers[i];
                break;
            }
        }

        if (server) {
            std::map<int, std::string>::const_iterator it = server->error_pages.find(status_code);
            if (it != server->error_pages.end()) {
                std::string target = it->second;
                std::string full_path = target;

                if (!target.empty() && target[0] == '/')
                    full_path = server->root + target;

                if (_file_handler.fileExists(full_path)) {
                    HttpResponse resp;
                    resp.setStatus(status_code);
                    resp.setHeader("Content-Type", _file_handler.getMimeType(full_path));
                    resp.setBody(_file_handler.getFileContents(full_path));
                    return resp;
                }
            }
        }
    }

    if (status_code == 404)
        return HttpResponse::notFound();
    if (status_code == 405)
        return HttpResponse::methodNotAllowed();
    if (status_code == 500)
        return HttpResponse::internalError();
    if (status_code == 403) {
        HttpResponse resp;
        resp.setStatus(403);
        resp.setHeader("Content-Type", "text/html");
        resp.setBody("<html><body><h1>403 Forbidden</h1></body></html>");
        return resp;
    }
    if (status_code == 501) {
        HttpResponse resp;
        resp.setStatus(501);
        resp.setHeader("Content-Type", "text/html");
        resp.setBody("<html><body><h1>501 Not Implemented</h1></body></html>");
        return resp;
    }
    return HttpResponse::badRequest();
}

// ─── Directory listing ───────────────────────────────────────────────────────

std::string RequestHandler::_buildDirectoryListing(
    const std::string& dir_path,
    const std::string& url_path) const
{
    DIR* dir = opendir(dir_path.c_str());
    if (!dir)
        return "";

    std::ostringstream html;
    html << "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
         << "<title>Index of " << url_path << "</title></head><body>"
         << "<h1>Index of " << url_path << "</h1><hr><pre>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;

        std::string href_base = url_path;
        if (url_path == "/list")
            href_base = "/uploads";

        html << "<a href=\"" << href_base;
        if (href_base[href_base.size() - 1] != '/')
            html << "/";
        html << name << "\">" << name << "</a>\n";
    }
    closedir(dir);

    html << "</pre><hr></body></html>";
    return html.str();
}

// ─── GET handler ─────────────────────────────────────────────────────────────

HttpResponse RequestHandler::_handleGet(HttpRequest& request,
                                         const Route& route) const
{
    std::string path = route.file_path;

    // If path ends with '/' or is a directory, try index file first
    if (_file_handler.isDirectory(path)) {
        std::string index_path = path;
        if (index_path[index_path.size() - 1] != '/')
            index_path += "/";
        index_path += "index.html";

        if (_file_handler.fileExists(index_path)) {
            std::string body = _file_handler.getFileContents(index_path);
            std::string mime = _file_handler.getMimeType(index_path);
            return HttpResponse::ok(body, mime);
        }

        // No index — try directory listing
        std::string listing = _buildDirectoryListing(path, request.getPath());
        if (!listing.empty())
            return HttpResponse::ok(listing, "text/html");

        return _buildErrorResponse(404);
    }

    if (!_file_handler.fileExists(path))
        return _buildErrorResponse(404);

    std::string body = _file_handler.getFileContents(path);
    std::string mime = _file_handler.getMimeType(path);
    return HttpResponse::ok(body, mime);
}

// ─── CGI handler ─────────────────────────────────────────────────────────────

HttpResponse RequestHandler::_handleCgi(HttpRequest& request,
                                          const Route& route) const
{
    if (!_file_handler.fileExists(route.cgi_path))
        return _buildErrorResponse(404);

    std::string cgi_output = _cgi_handler.execute(route.cgi_path, request);
    if (cgi_output.empty())
        return _buildErrorResponse(500);

    // CGI output starts with its own headers — find the blank line separator
    size_t header_end = cgi_output.find("\r\n\r\n");
    if (header_end == std::string::npos)
        header_end = cgi_output.find("\n\n");

    if (header_end == std::string::npos)
        return HttpResponse::ok(cgi_output, "text/html");

    // Split headers from body
    std::string cgi_headers = cgi_output.substr(0, header_end);
    std::string cgi_body;
    if (cgi_output.find("\r\n\r\n") != std::string::npos)
        cgi_body = cgi_output.substr(header_end + 4);
    else
        cgi_body = cgi_output.substr(header_end + 2);

    // Extract Content-Type from CGI headers if present
    std::string content_type = "text/html";
    size_t ct_pos = cgi_headers.find("Content-Type:");
    if (ct_pos == std::string::npos)
        ct_pos = cgi_headers.find("content-type:");
    if (ct_pos != std::string::npos) {
        size_t ct_end = cgi_headers.find("\n", ct_pos);
        std::string ct_line = cgi_headers.substr(ct_pos + 13,
                                                   ct_end - ct_pos - 13);
        // trim
        size_t s = ct_line.find_first_not_of(" \t\r");
        size_t e = ct_line.find_last_not_of(" \t\r");
        if (s != std::string::npos)
            content_type = ct_line.substr(s, e - s + 1);
    }

    return HttpResponse::ok(cgi_body, content_type);
}

// ─── POST / upload handler ───────────────────────────────────────────────────

HttpResponse RequestHandler::_handlePost(HttpRequest& request,
                                           const Route& route) const
{
    if (route.upload_dir.empty())
        return _buildErrorResponse(403);

    std::string saved = _upload_handler.saveFile(route.upload_dir, request);
    if (saved.empty())
        return HttpResponse::internalError();

    return HttpResponse::created("File uploaded: " + saved);
}

// ─── DELETE handler ──────────────────────────────────────────────────────────

HttpResponse RequestHandler::_handleDelete(const Route& route) const {
    if (!_file_handler.fileExists(route.file_path))
        return _buildErrorResponse(404);

    if (_upload_handler.deleteFile(route.file_path))
        return HttpResponse::noContent();

    return _buildErrorResponse(500);
}

// ─── Main entry point ────────────────────────────────────────────────────────

std::string RequestHandler::buildResponseForRawRequest(
    const std::string& raw_request) const
{
    HttpRequest request;
    request.feed(raw_request.c_str(), raw_request.size());

    if (request.errorCode() != 0)
        return _buildErrorResponse(request.errorCode()).build();
    if (!request.parsingCompleted())
        return HttpResponse::badRequest().build();

    // If no config is set, fall back to basic static file serving
    if (!_config) {
        if (request.getMethod() != GET)
            return HttpResponse::methodNotAllowed().build();
        std::string path = (request.getPath() == "/")
                           ? "public/index.html"
                           : "public" + request.getPath();
        if (!_file_handler.fileExists(path))
            return HttpResponse::notFound().build();
        std::string body = _file_handler.getFileContents(path);
        std::string mime = _file_handler.getMimeType(path);
        return HttpResponse::ok(body, mime).build();
    }

    // Route the request through Router
    Router router(*_config);
    Route  route = router.route(request, _port);

    if (route.type == ROUTE_ERROR)
        return _buildErrorResponse(route.error_code).build();

    HttpResponse response;

    switch (route.type) {
        case ROUTE_STATIC_FILE:
            response = _handleGet(request, route);
            break;
        case ROUTE_CGI:
            response = _handleCgi(request, route);
            break;
        case ROUTE_UPLOAD:
            response = _handlePost(request, route);
            break;
        case ROUTE_DELETE:
            response = _handleDelete(route);
            break;
        default:
            response = _buildErrorResponse(404);
            break;
    }

    return response.build();
}
