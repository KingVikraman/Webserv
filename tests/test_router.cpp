#include "../inc/Router.hpp"

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

static void feedString(HttpRequest &req, const std::string &raw)
{
    req.feed(raw.c_str(), raw.size());
}

int main()
{
    Router router;
    HttpRequest req;
    Route route;
    std::string long_path;

    feedString(req, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_STATIC_FILE, "GET / routes to static file");
    expectTrue(route.file_path == "public/index.html", "GET / maps to public/index.html");

    req.clear();
    feedString(req, "GET /style.css HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_STATIC_FILE, "GET /style.css routes to static file");
    expectTrue(route.file_path == "public/style.css", "GET /style.css maps to public/style.css");

    req.clear();
    feedString(req, "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_STATIC_FILE, "HEAD / routes to static file");
    expectTrue(route.file_path == "public/index.html", "HEAD / maps to public/index.html");

    req.clear();
    feedString(req, "GET /cgi-bin/env.py HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_CGI, "CGI path routes to CGI");
    expectTrue(route.cgi_path == "cgi-bin/env.py", "CGI path strips leading slash");

    req.clear();
    feedString(req, "GET /cgi-binX/tool HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_STATIC_FILE, "Non-matching /cgi-bin prefix stays static");

    req.clear();
    feedString(req, "POST /upload/file.txt HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_WRITE_FILE, "Upload path routes to write handler");
    expectTrue(route.upload_dir == "public/uploads", "Upload path keeps default upload dir metadata");
    expectTrue(route.file_path == "public/upload/file.txt", "Upload path keeps target file path");

    req.clear();
    feedString(req, "POST /uploadx/file.txt HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_WRITE_FILE, "Non-matching upload prefix still routes to write handler");
    expectTrue(route.upload_dir.empty(), "Non-matching upload prefix does not set upload dir metadata");

    req.clear();
    feedString(req, "POST /notes.txt HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_WRITE_FILE, "POST non-upload path still routes to write handler");
    expectTrue(route.file_path == "public/notes.txt", "POST non-upload path maps to public target file");
    expectTrue(route.upload_dir.empty(), "POST non-upload path does not force upload dir");

    req.clear();
    feedString(req, "PUT /drafts/report.txt HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_WRITE_FILE, "PUT path routes to write handler");
    expectTrue(route.file_path == "public/drafts/report.txt", "PUT path maps to public target file");

    req.clear();
    feedString(req, "POST /cgi-bin/env.py HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_CGI, "POST CGI path routes to CGI");
    expectTrue(route.cgi_path == "cgi-bin/env.py", "POST CGI path strips leading slash");

    req.clear();
    feedString(req, "DELETE /old.txt HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_DELETE, "DELETE path routes to delete");
    expectTrue(route.file_path == "public/old.txt", "DELETE maps to public target file");

    req.clear();
    feedString(req, "GET /../../etc/passwd HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_ERROR && route.error_code == 400, "Path traversal is rejected with 400");

    req.clear();
    feedString(req, "GET /safe..name.txt HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_STATIC_FILE, "Literal dots in filename are not treated as traversal");

    req.clear();
    feedString(req, "GET /x HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_STATIC_FILE, "Short paths do not break CGI suffix checks");

    req.clear();
    long_path.assign(MAX_URI_LENGTH + 2, 'a');
    feedString(req, "GET /" + long_path + " HTTP/1.1\r\nHost: localhost\r\n\r\n");
    route = router.route(req);
    expectTrue(route.type == ROUTE_ERROR && route.error_code == 414,
        "Parser error 414 propagates through router");

    req.clear();
    feedString(req, "GET /unsupported HTTP/1.1\r\nHost: localhost\r\n\r\n");
    HttpMethod unsupported = NONE;
    req.setMethod(unsupported);
    route = router.route(req);
    expectTrue(route.type == ROUTE_ERROR && route.error_code == 405,
        "Unsupported methods are rejected with 405");

    if (g_failures != 0)
        return 1;
    std::cout << "All router tests passed." << std::endl;
    return 0;
}
