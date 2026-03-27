#include "../inc/RequestHandler.hpp"

#include <sstream>

RequestHandler::RequestHandler()
{
}

RequestHandler::~RequestHandler()
{
}

HttpResponse RequestHandler::_buildErrorResponse(int status_code) const
{
    if (status_code == 400)
        return HttpResponse::badRequest();
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
    Route route_result;
    std::string body;
    std::string mime;

    request.feed(raw_request.c_str(), raw_request.size()); // passing pointer to raw_request data and size of raw_request

    if (request.errorCode() != 0)
        return _buildErrorResponse(request.errorCode()).build(); // using build to convert HttpResponse to string
    if (!request.parsingCompleted())
        return HttpResponse::badRequest().build();

    route_result = _router.route(request);
    if (route_result.error_code != 0)
        return _buildErrorResponse(route_result.error_code).build();
    if (route_result.type == ROUTE_CGI || route_result.type == ROUTE_WRITE_FILE || route_result.type == ROUTE_DELETE)
        return HttpResponse::notImplemented().build();
    if (route_result.type != ROUTE_STATIC_FILE)
        return HttpResponse::internalError().build();
    if (_file_handler.isDirectory(route_result.file_path))
        return HttpResponse::notFound().build();
    if (!_file_handler.fileExists(route_result.file_path))
        return HttpResponse::notFound().build();

    body = _file_handler.getFileContents(route_result.file_path);
    mime = _file_handler.getMimeType(route_result.file_path);
    if (request.getMethod() == HEAD)
    {
        std::ostringstream content_length;

        content_length << body.length();
        response.setStatus(200);
        response.setHeader("Content-Type", mime);
        response.setHeader("Content-Length", content_length.str());
        return response.build();
    }
    response = HttpResponse::ok(body, mime);
    return response.build();
}
