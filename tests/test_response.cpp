#include "../inc/HttpResponse.hpp"

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
    HttpResponse resp1;
    std::string output1;
    HttpResponse created;
    std::string created_output;
    HttpResponse no_content;
    std::string no_content_output;

    resp1.setStatus(200);
    resp1.setHeader("Content-Type", "text/html");
    resp1.setBody("<h1>Hello World</h1>");
    output1 = resp1.build();

    expectTrue(output1.find("HTTP/1.1 200 OK") != std::string::npos, "Status line correct");
    expectTrue(output1.find("Content-Type: text/html") != std::string::npos, "Content-Type header present");
    expectTrue(output1.find("Content-Length: 20") != std::string::npos, "Content-Length auto-set");
    expectTrue(output1.find("<h1>Hello World</h1>") != std::string::npos, "Body present");

    expectTrue(HttpResponse::ok("Test body", "text/plain").build().find("200 OK") != std::string::npos,
        "ok() helper works");
    expectTrue(HttpResponse::notFound().build().find("404 Not Found") != std::string::npos,
        "notFound() helper works");
    expectTrue(HttpResponse::uriTooLong().build().find("414 URI Too Long") != std::string::npos,
        "uriTooLong() helper works");
    expectTrue(HttpResponse::notImplemented().build().find("501 Not Implemented") != std::string::npos,
        "notImplemented() helper works");

    created = HttpResponse::created("Created body");
    created_output = created.build();
    expectTrue(created_output.find("HTTP/1.1 201 Created") != std::string::npos,
        "created() returns 201 Created");

    no_content = HttpResponse::noContent();
    no_content_output = no_content.build();
    expectTrue(no_content_output.find("HTTP/1.1 204 No Content") != std::string::npos,
        "noContent() returns 204 No Content");
    expectTrue(no_content_output.find("Content-Length: 0") != std::string::npos,
        "noContent() sets Content-Length to 0");

    if (g_failures != 0)
        return 1;
    std::cout << "All response tests passed." << std::endl;
    return 0;
}
