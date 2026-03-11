#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <sys/types.h>

#define private public
#include "../inc/HttpRequest.hpp"
#undef private

static int g_failures = 0;

static void expectTrue(bool condition, const std::string &name)
{
    if (condition)
        std::cout << "[PASS] " << name << std::endl;
    else
    {
        std::cout << "[FAIL] " << name << std::endl;
        ++g_failures;
    }
}

static void expectEqual(const std::string &actual, const std::string &expected, const std::string &name)
{
    if (actual == expected)
        std::cout << "[PASS] " << name << std::endl;
    else
    {
        std::cout << "[FAIL] " << name << " (expected \"" << expected << "\", got \"" << actual << "\")" << std::endl;
        ++g_failures;
    }
}

static void expectEqual(int actual, int expected, const std::string &name)
{
    if (actual == expected)
        std::cout << "[PASS] " << name << std::endl;
    else
    {
        std::cout << "[FAIL] " << name << " (expected " << expected << ", got " << actual << ")" << std::endl;
        ++g_failures;
    }
}

static void expectEqual(size_t actual, size_t expected, const std::string &name)
{
    if (actual == expected)
        std::cout << "[PASS] " << name << std::endl;
    else
    {
        std::cout << "[FAIL] " << name << " (expected " << expected << ", got " << actual << ")" << std::endl;
        ++g_failures;
    }
}

static void feedString(HttpRequest &req, const std::string &raw)
{
    req.feed(const_cast<char *>(raw.c_str()), raw.size());
}

static void expectMethodPrefix(const std::string &token, HttpMethod expected, const std::string &label)
{
    HttpRequest req;

    feedString(req, token);

    expectEqual(req._error_code, 0, label + " keeps error_code at 0");
    expectEqual(static_cast<int>(req._method), static_cast<int>(expected), label + " sets method enum");
    expectEqual(static_cast<int>(req._state), static_cast<int>(Request_Line_First_Space), label + " reaches Request_Line_First_Space");
}

static void expectRequestDefaults(const HttpRequest &req, const std::string &label)
{
    expectEqual(static_cast<int>(req._method), static_cast<int>(NONE), label + " resets method");
    expectEqual(req._error_code, 0, label + " resets error_code");
    expectEqual(static_cast<int>(req._state), static_cast<int>(Request_Line), label + " resets state");
    expectEqual(req._method_index, 1, label + " resets method_index");
    expectEqual(req._body_length, static_cast<size_t>(0), label + " resets body length");
    expectEqual(req._chunk_length, static_cast<size_t>(0), label + " resets chunk length");
    expectEqual(req._path, "", label + " clears path");
    expectEqual(req._query, "", label + " clears query");
    expectEqual(req._fragment, "", label + " clears fragment");
    expectEqual(req._storage, "", label + " clears storage");
    expectEqual(req._key_storage, "", label + " clears key storage");
    expectEqual(req._server_name, "", label + " clears server name");
    expectEqual(req._boundary, "", label + " clears boundary");
    expectEqual(req._body_str, "", label + " clears body string");
    expectEqual(req._body.size(), static_cast<size_t>(0), label + " clears body bytes");
    expectTrue(req._request_headers.empty(), label + " clears headers");
    expectTrue(req._fields_done_flag == false, label + " resets fields_done flag");
    expectTrue(req._body_flag == false, label + " resets body flag");
    expectTrue(req._body_done_flag == false, label + " resets body_done flag");
    expectTrue(req._chunked_flag == false, label + " resets chunked flag");
    expectTrue(req._multiform_flag == false, label + " resets multiform flag");
}

static void test_constructor_defaults()
{
    HttpRequest req;

    expectRequestDefaults(req, "Constructor");
    expectTrue(req.getHeaders().empty(), "Constructor leaves headers map empty");
    expectTrue(req.parsingCompleted() == false, "Constructor does not start completed");
    expectEqual(req.errorCode(), 0, "errorCode() mirrors constructor default");
    expectEqual(req.getPath(), "", "getPath() starts empty");
    expectEqual(req.getQuery(), "", "getQuery() starts empty");
    expectEqual(req.getFragment(), "", "getFragment() starts empty");
    expectEqual(req.getBody(), "", "getBody() starts empty");
    expectEqual(req.getBoundary(), "", "getBoundary() starts empty");
    expectTrue(req.getMultiformFlag() == false, "getMultiformFlag() starts false");
}

static void test_method_prefixes()
{
    expectMethodPrefix("GET", GET, "GET");
    expectMethodPrefix("POST", POST, "POST");
    expectMethodPrefix("PUT", PUT, "PUT");
    expectMethodPrefix("DELETE", DELETE, "DELETE");
    expectMethodPrefix("HEAD", HEAD, "HEAD");
}

static void test_bad_method_and_spacing_errors()
{
    HttpRequest badStart;
    HttpRequest badPost;
    HttpRequest badToken;
    HttpRequest badSpace;

    feedString(badStart, "X");
    feedString(badPost, "PX");
    feedString(badToken, "GEZ");
    feedString(badSpace, "GETX");

    expectEqual(badStart._error_code, 501, "Invalid first byte sets 501");
    expectEqual(badPost._error_code, 501, "Invalid P* prefix sets 501");
    expectEqual(badToken._error_code, 501, "Invalid method token sets 501");
    expectEqual(badSpace._error_code, 400, "Non-space after method sets 400");
}

static void test_complete_get_request()
{
    HttpRequest req;
    std::string raw = "GET /index.html HTTP/1.1\r\n\r\n";

    feedString(req, raw);

    expectTrue(req.parsingCompleted(), "Simple GET completes parsing");
    expectEqual(req.errorCode(), 0, "Simple GET keeps error_code at 0");
    expectEqual(static_cast<int>(req.getMethod()), static_cast<int>(GET), "Simple GET sets method");
    expectEqual(req.getMethodStr(), "GET", "Simple GET resolves method string");
    expectEqual(req.getPath(), "/index.html", "Simple GET parses path");
    expectEqual(req.getQuery(), "", "Simple GET leaves query empty");
    expectEqual(req.getFragment(), "", "Simple GET leaves fragment empty");
    expectEqual(static_cast<int>(req._state), static_cast<int>(Parsing_Done), "Simple GET reaches Parsing_Done");
    expectTrue(req.getHeaders().empty(), "Simple GET keeps headers empty");
}

static void test_query_fragment_and_headers()
{
    HttpRequest req;
    std::string raw = "HEAD /docs/readme.txt?lang=en#top HTTP/1.1\r\n"
                      "Host: Example.com:8080\r\n"
                      "X-Test:  value with spaces \r\n"
                      "\r\n";

    feedString(req, raw);

    expectTrue(req.parsingCompleted(), "HEAD with headers completes parsing");
    expectEqual(req.errorCode(), 0, "HEAD with headers keeps error_code at 0");
    expectEqual(static_cast<int>(req.getMethod()), static_cast<int>(HEAD), "HEAD request sets method");
    expectEqual(req.getMethodStr(), "HEAD", "HEAD request resolves method string");
    expectEqual(req.getPath(), "/docs/readme.txt", "HEAD request parses path");
    expectEqual(req.getQuery(), "lang=en", "HEAD request parses query");
    expectEqual(req.getFragment(), "top", "HEAD request parses fragment");
    expectEqual(req.getHeader("host"), "Example.com:8080", "Host header is stored");
    expectEqual(req.getHeader("x-test"), "value with spaces", "Header values are trimmed");
    expectEqual(req.getServerName(), "Example.com", "Host header populates server name");
    expectEqual(req.getHeaders().size(), static_cast<size_t>(2), "Two headers are parsed");
}

static void test_fixed_length_body_across_multiple_feeds()
{
    HttpRequest req;
    std::string firstHalf = "POST /submit HTTP/1.1\r\n"
                            "Host: site.test\r\n"
                            "Content-Length: 11\r\n"
                            "\r\n"
                            "hello";
    std::string secondHalf = " world";

    feedString(req, firstHalf);

    expectTrue(req.parsingCompleted() == false, "Partial body does not complete parsing");
    expectEqual(req.errorCode(), 0, "Partial body keeps error_code at 0");
    expectEqual(static_cast<int>(req.getMethod()), static_cast<int>(POST), "POST body request sets method");
    expectEqual(req.getMethodStr(), "POST", "POST body request resolves method string");
    expectEqual(req.getPath(), "/submit", "POST body request parses path");
    expectEqual(req.getServerName(), "site.test", "POST body request parses host");
    expectTrue(req._body_flag, "Content-Length enables body parsing");
    expectEqual(req._body_length, static_cast<size_t>(11), "Content-Length is parsed");
    expectEqual(req._body.size(), static_cast<size_t>(5), "Partial body stores received bytes");
    expectEqual(req.getBody(), "", "Body string is empty until parsing completes");

    feedString(req, secondHalf);

    expectTrue(req.parsingCompleted(), "Full body completes parsing");
    expectTrue(req._body_done_flag, "Full body marks body_done flag");
    expectEqual(req.getBody(), "hello world", "Full body is exposed through getBody()");
    expectEqual(req._body.size(), static_cast<size_t>(11), "Full body stores all bytes");
}

static void test_setters_and_manual_accessors()
{
    HttpRequest req;
    HttpMethod method = PUT;
    std::string name = "Content-Type";
    std::string value = " \ttext/plain \t";

    req.setMethod(method);
    req.setHeader(name, value);
    req.setMaxBodySize(128);
    req.setBody("payload");

    expectEqual(static_cast<int>(req.getMethod()), static_cast<int>(PUT), "setMethod updates method");
    expectEqual(req.getMethodStr(), "PUT", "setMethod affects method string");
    expectEqual(req.getHeader("content-type"), "text/plain", "setHeader lowercases key and trims value");
    expectEqual(req._max_body_size, static_cast<size_t>(128), "setMaxBodySize stores the configured limit");
    expectEqual(req.getBody(), "payload", "setBody updates body string");
    expectEqual(req._body.size(), static_cast<size_t>(7), "setBody updates raw body bytes");
}

static void test_multipart_request_extracts_boundary()
{
    HttpRequest req;
    std::string raw = "POST /upload HTTP/1.1\r\n"
                      "Host: files.local:9000\r\n"
                      "Content-Type: multipart/form-data; boundary=----XYZ\r\n"
                      "Content-Length: 1\r\n"
                      "\r\n"
                      "x";

    feedString(req, raw);

    expectTrue(req.parsingCompleted(), "Multipart request completes parsing");
    expectEqual(req.errorCode(), 0, "Multipart request keeps error_code at 0");
    expectTrue(req.getMultiformFlag(), "Multipart request enables multiform flag");
    expectEqual(req.getBoundary(), "----XYZ", "Multipart request extracts boundary");
    expectEqual(req.getServerName(), "files.local", "Multipart request parses server name");
    expectEqual(req.getBody(), "x", "Multipart request body is stored");
}

static void test_chunked_headers_and_invalid_chunk_size()
{
    HttpRequest req;
    std::string headers = "POST /stream HTTP/1.1\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n";

    feedString(req, headers);

    expectTrue(req.parsingCompleted() == false, "Chunked headers alone do not complete parsing");
    expectEqual(req.errorCode(), 0, "Chunked headers keep error_code at 0");
    expectTrue(req._body_flag, "Transfer-Encoding enables body parsing");
    expectTrue(req._chunked_flag, "Transfer-Encoding enables chunked mode");
    expectEqual(static_cast<int>(req._state), static_cast<int>(Chunked_Length_Begin), "Chunked parser waits for chunk length");

    feedString(req, "g");

    expectEqual(req.errorCode(), 400, "Invalid chunk size digit sets 400");
}

static void test_request_line_and_header_errors()
{
    HttpRequest badVersion;
    HttpRequest badHeader;
    std::ostringstream longUri;
    std::string raw;

    feedString(badVersion, "GET / HTTP/1.a\r\n\r\n");
    expectEqual(badVersion.errorCode(), 400, "Invalid HTTP version sets 400");

    feedString(badHeader, "GET / HTTP/1.1\r\nBad Header: value\r\n\r\n");
    expectEqual(badHeader.errorCode(), 400, "Invalid header field name sets 400");

    longUri << "GET /" << std::string(MAX_URI_LENGTH + 8, 'a') << " HTTP/1.1\r\n\r\n";
    raw = longUri.str();
    HttpRequest tooLong;
    feedString(tooLong, raw);
    expectEqual(tooLong.errorCode(), 414, "URI longer than MAX_URI_LENGTH sets 414");
}

static void test_clear_resets_parser_state()
{
    HttpRequest req;
    std::string raw = "POST /clear HTTP/1.1\r\n"
                      "Host: clear.test\r\n"
                      "Content-Length: 4\r\n"
                      "\r\n"
                      "data";

    feedString(req, raw);
    expectTrue(req.parsingCompleted(), "clear() setup request completes parsing");

    req.clear();

    expectRequestDefaults(req, "clear()");
    expectTrue(req.parsingCompleted() == false, "clear() resets parsingCompleted()");
    expectEqual(req.errorCode(), 0, "clear() resets errorCode()");
}

int main()
{
    std::cout << "== HttpRequest Parser Tests ==" << std::endl;

    test_constructor_defaults();
    test_method_prefixes();
    test_bad_method_and_spacing_errors();
    test_complete_get_request();
    test_query_fragment_and_headers();
    test_fixed_length_body_across_multiple_feeds();
    test_setters_and_manual_accessors();
    test_multipart_request_extracts_boundary();
    test_chunked_headers_and_invalid_chunk_size();
    test_request_line_and_header_errors();
    test_clear_resets_parser_state();

    if (g_failures == 0)
    {
        std::cout << "All tests passed." << std::endl;
        return 0;
    }

    std::cout << g_failures << " test(s) failed." << std::endl;
    return 1;
}
