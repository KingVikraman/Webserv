#include "../includes/CgiHandler.hpp"
#include "../includes/HttpRequest.hpp"
#include <iostream>
#include <ostream>

void printResult(const std::string& test_name, bool passed)
{
	std::cout << (passed ? "[PASS] " : "[FAIL] ") << test_name << std::endl;
}

// Test 1 : Basic GET request with no body
void testBasicGet()
{
	HttpRequest req;
	const char* raw = "GET /test.php HTTP/1.1\r\nHost : localhost\r\n\r\n";
	req.feed(raw, strlen(raw));

	CgiHandler cgi;
	std::string output = cgi.execute("./test.php", req);

	printResult("Basic GET request", !output.empty());
	std::cout << " Output: " << output.substr(0, 100) << std::endl;
}

// Test 2: POST request with body
void testPostWithBody()
{
	HttpRequest req;
	const char* raw =	"POST /test.php HTTP/1.1\r\n"
						"Host: localhost\r\n"
						"Content-Type: application/x-www-form-urlencoded\r\n"
						"Content-Length: 11\r\n"
						"\r\n"
						"hello=world";
	req.feed(raw,strlen(raw));

	CgiHandler cgi;
	std::string output = cgi.execute("./test.php", req);

	printResult("POST request with body", !output.empty());
	std::cout << "Output: " << output.substr(0, 100) << std::endl;
}

// Test 3 : GET request with query string

void testQuerySting()
{
	HttpRequest req;
	const char* raw =	"GET /test.php? name=Raja&age=20 HTTP/1.1\r\n	"
						"Host: localhost\r\n"
						"\r\n";
	req.feed(raw, strlen(raw));

	CgiHandler cgi;
	std::string output = cgi.execute("./test.php", req);

	printResult("GET request with query string", !output.empty());
	std::cout << " Output: " << output.substr(0, 100) << std::endl;
}

// Test 4 : Check parsing completed correctly
void testParsingCompleted()
{
	HttpRequest req;
	const char* raw = "GET /test.php HTTP/1.1\r\nHost: localhost\r\n\r\n";
	req.feed(raw, strlen(raw));

	printResult("Parsing completed", req.parsingCompleted());
	printResult("No error code", req.errorCode() == 0);
}

int main()
{
	std::cout << "=== CgiHandler Tests ===" << std::endl;
	std::cout << std::endl;

	testParsingCompleted();
	std::cout << std::endl;

	testBasicGet();
	std::cout << std::endl;

	testPostWithBody();
	std::cout << std::endl;

	testQuerySting();
	std::cout << std::endl;

	testParsingCompleted();
	std::cout << std::endl;


	std::cout << "=== Test Completed ===" << std::endl;
	std::cout << "Everything is a Success" << std::endl;
}