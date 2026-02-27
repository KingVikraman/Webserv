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

static void test_method_get_reaches_first_space()
{
    HttpRequest req;
    char data[] = "GET";
    req.feed(data, 3);

    expectTrue(req._error_code == 0, "GET keeps error_code at 0");
    expectTrue(req._method == GET, "GET sets method enum");
    expectTrue(req._state == Request_Line_First_Space, "GET reaches Request_Line_First_Space");
}

static void test_method_post_reaches_first_space()
{
    HttpRequest req;
    char data[] = "POST";
    req.feed(data, 4);

    expectTrue(req._error_code == 0, "POST keeps error_code at 0");
    expectTrue(req._method == POST, "POST sets method enum");
    expectTrue(req._state == Request_Line_First_Space, "POST reaches Request_Line_First_Space");
}

static void test_method_put_reaches_first_space()
{
    HttpRequest req;
    char data[] = "PUT";
    req.feed(data, 3);

    expectTrue(req._error_code == 0, "PUT keeps error_code at 0");
    expectTrue(req._method == PUT, "PUT sets method enum");
    expectTrue(req._state == Request_Line_First_Space, "PUT reaches Request_Line_First_Space");
}

static void test_bad_start_byte_sets_501()
{
    HttpRequest req;
    char data[] = "X";
    req.feed(data, 1);

    expectTrue(req._error_code == 501, "Invalid first byte sets 501");
}

static void test_bad_post_prefix_sets_501()
{
    HttpRequest req;
    char data[] = "PX";
    req.feed(data, 2);

    expectTrue(req._error_code == 501, "Invalid P* prefix sets 501");
}

static void test_bad_method_token_sets_501()
{
    HttpRequest req;
    char data[] = "GEZ";
    req.feed(data, 3);

    expectTrue(req._error_code == 501, "Invalid method token sets 501");
}

static void test_first_space_accepts_space()
{
    HttpRequest req;
    char data[] = "GET ";
    req.feed(data, 4);

    expectTrue(req._error_code == 0, "Space after method keeps error_code at 0");
    expectTrue(req._state == Request_Line_URI_Path_Slash, "Space transitions to Request_Line_URI_Path_Slash");
}

static void test_first_space_rejects_non_space()
{
    HttpRequest req;
    char data[] = "GETX";
    req.feed(data, 4);

    expectTrue(req._error_code == 400, "Non-space after method sets 400");
}

int main()
{
    std::cout << "== Request-Line Partial Parser Tests ==" << std::endl;

    test_method_get_reaches_first_space();
    test_method_post_reaches_first_space();
    test_method_put_reaches_first_space();
    test_bad_start_byte_sets_501();
    test_bad_post_prefix_sets_501();
    test_bad_method_token_sets_501();
    test_first_space_accepts_space();
    test_first_space_rejects_non_space();

    if (g_failures == 0)
    {
        std::cout << "All tests passed." << std::endl;
        return 0;
    }

    std::cout << g_failures << " test(s) failed." << std::endl;
    return 1;
}
