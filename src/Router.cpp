#include "../inc/Router.hpp"

Router::Router()
{
}

Router::~Router()
{
}

bool Router::_hasPathTraversal(const std::string &path) const
{
    return (path == ".." ||
        path.find("../") == 0 ||
        path.find("/../") != std::string::npos ||
        (path.length() >= 3 && path.compare(path.length() - 3, 3, "/..") == 0));
}

bool Router::_isCgiRequest(const std::string &path) const
{
    return ((path == "/cgi-bin" || path.find("/cgi-bin/") == 0) ||
        (path.size() >= 3 && path.rfind(".py") == path.size() - 3) ||
        (path.size() >= 3 && path.rfind(".sh") == path.size() - 3));
}

bool Router::_isUploadPath(const std::string &path) const
{
    return (path == "/upload" || path.find("/upload/") == 0);
}

std::string Router::_buildPublicPath(const std::string &path) const
{
    if (path == "/")
        return "public/index.html";
    return "public" + path;
}

Route Router::route(HttpRequest &request) const
{
    Route result;
    std::string path = request.getPath();

    if (request.errorCode() != 0)
    {
        result.error_code = request.errorCode();
        return result;
    }
    if (!request.parsingCompleted())
    {
        result.error_code = 400;
        return result;
    }
    if (path.empty() || path[0] != '/')
    {
        result.error_code = 400;
        return result;
    }
    if (_hasPathTraversal(path))
    {
        result.error_code = 400;
        return result;
    }
    if (request.getMethod() != GET &&
        request.getMethod() != HEAD &&
        request.getMethod() != POST &&
        request.getMethod() != PUT &&
        request.getMethod() != DELETE)
    {
        result.error_code = 405;
        return result;
    }
    if (request.getMethod() == DELETE)
    {
        result.type = ROUTE_DELETE;
        result.file_path = _buildPublicPath(path);
        return result;
    }
    if (_isCgiRequest(path))
    {
        result.type = ROUTE_CGI;
        result.cgi_path = path[0] == '/' ? path.substr(1) : path; // rm the / if ther eis
        return result;
    }
    if (request.getMethod() == POST || request.getMethod() == PUT)
    {
        result.type = ROUTE_WRITE_FILE;
        result.file_path = _buildPublicPath(path);
        if (_isUploadPath(path))
            result.upload_dir = "public/uploads";
        return result;
    }
    result.type = ROUTE_STATIC_FILE;
    result.file_path = _buildPublicPath(path);
    return result;
}
