#include "../inc/FileHandler.hpp"

#include <fstream>
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
    FileHandler fh;
    std::ofstream test("tests/test_asset.txt");
    std::string contents;

    test << "runtime merge test";
    test.close();

    expectTrue(fh.fileExists("tests/test_asset.txt"), "fileExists() sees test asset");
    expectTrue(!fh.fileExists("tests/nonexistent.txt"), "fileExists() rejects missing asset");
    expectTrue(fh.getFileSize("tests/test_asset.txt") == 18, "getFileSize() reports expected size");

    contents = fh.getFileContents("tests/test_asset.txt");
    expectTrue(contents == "runtime merge test", "getFileContents() reads file contents");
    expectTrue(fh.getMimeType("tests/test_asset.txt") == "text/plain", "getMimeType() detects txt files");

    if (g_failures != 0)
        return 1;
    std::cout << "All file handler tests passed." << std::endl;
    return 0;
}
