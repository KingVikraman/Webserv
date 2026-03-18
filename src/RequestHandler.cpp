#include "../inc/RequestHandler.hpp"

RequestHandler::RequestHandler()
{
}

RequestHandler::~RequestHandler()
{
}

std::string RequestHandler::_resolvePath(HttpRequest &request) const
{
    if (request.getPath() == "/")
        return "public/index.html";
    return "public" + request.getPath();
}

HttpResponse RequestHandler::_buildErrorResponse(int status_code) const
{
    if (status_code == 404)
        return HttpResponse::notFound();
    if (status_code == 405)
        return HttpResponse::methodNotAllowed();
    if (status_code == 414)
        return HttpResponse::uriTooLong();
    if (status_code == 501)
        return HttpResponse::notImplemented();
    return HttpResponse::badRequest();
}

std::string RequestHandler::buildResponseForRawRequest(const std::string &raw_request) const
{
    HttpRequest request;
    HttpResponse response;
    std::string file_path;
    std::string body;
    std::string mime;

    request.feed(raw_request.c_str(), raw_request.size()); // passing pointer to raw_request data and size of raw_request

    if (request.errorCode() != 0)
        return _buildErrorResponse(request.errorCode()).build(); // using build to convert HttpResponse to string
    if (!request.parsingCompleted())
        return HttpResponse::badRequest().build();
    if (request.getMethod() != GET)
        return HttpResponse::methodNotAllowed().build();

    file_path = _resolvePath(request);
    if (_file_handler.isDirectory(file_path))
        return HttpResponse::notFound().build();
    if (!_file_handler.fileExists(file_path))
        return HttpResponse::notFound().build();

    body = _file_handler.getFileContents(file_path);
    mime = _file_handler.getMimeType(file_path);
    response = HttpResponse::ok(body, mime);
    return response.build();
}
