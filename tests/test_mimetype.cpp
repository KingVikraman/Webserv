#include "../inc/FileHandler.hpp"

#include <iostream>
#include <string>

static int g_failures = 0;

static void expectEqual(const std::string &actual, const std::string &expected, const std::string &message)
{
    if (actual == expected)
        std::cout << "[PASS] " << message << std::endl;
    else
    {
        std::cout << "[FAIL] " << message << " (expected \"" << expected << "\", got \"" << actual << "\")" << std::endl;
        ++g_failures;
    }
}

int main()
{
    FileHandler fh;

    expectEqual(fh.getMimeType("public/index.html"), "text/html", "HTML mime type");
    expectEqual(fh.getMimeType("public/style.css"), "text/css", "CSS mime type");
    expectEqual(fh.getMimeType("public/images/logo.png"), "image/png", "PNG mime type");

    if (g_failures != 0)
        return 1;
    std::cout << "All mime type tests passed." << std::endl;
    return 0;
}
