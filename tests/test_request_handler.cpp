#include "../inc/RequestHandler.hpp"

#include <iostream>
#include <string>

static int g_failures = 0;

static void expectTrue(bool condition, const std::string &message)
{
    if (condition)
        std::cout << "[PASS] " << message << std::endl;
    else
    {
        std::cout << "[FAIL] " << message << std::endl;
        ++g_failures;
    }
}

int main()
{
    RequestHandler handler;
    std::string response;
    std::string long_path;

    response = handler.buildResponseForRawRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    expectTrue(response.find("HTTP/1.1 200 OK") != std::string::npos, "GET / returns 200");
    expectTrue(response.find("Content-Type: text/html") != std::string::npos, "GET / returns html content type");
    expectTrue(response.find("Raja Backend Works!") != std::string::npos, "GET / serves public/index.html");

    response = handler.buildResponseForRawRequest("GET /style.css HTTP/1.1\r\nHost: localhost\r\n\r\n");
    expectTrue(response.find("HTTP/1.1 200 OK") != std::string::npos, "GET /style.css returns 200");
    expectTrue(response.find("Content-Type: text/css") != std::string::npos, "GET /style.css returns css content type");

    response = handler.buildResponseForRawRequest("GET /missing.css HTTP/1.1\r\nHost: localhost\r\n\r\n");
    expectTrue(response.find("HTTP/1.1 404 Not Found") != std::string::npos, "Missing file returns 404");

    response = handler.buildResponseForRawRequest("GETX / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    expectTrue(response.find("HTTP/1.1 400 Bad Request") != std::string::npos, "Malformed request returns 400");

    response = handler.buildResponseForRawRequest("HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    expectTrue(response.find("HTTP/1.1 405 Method Not Allowed") != std::string::npos, "Unsupported HEAD returns 405");

    long_path.assign(MAX_URI_LENGTH + 5, 'a');
    response = handler.buildResponseForRawRequest("GET /" + long_path + " HTTP/1.1\r\nHost: localhost\r\n\r\n");
    expectTrue(response.find("HTTP/1.1 414 URI Too Long") != std::string::npos, "Parser 414 propagates to response");

    if (g_failures != 0)
        return 1;
    std::cout << "All request handler tests passed." << std::endl;
    return 0;
}
