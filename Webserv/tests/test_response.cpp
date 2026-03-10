#include "../includes/HttpResponse.hpp"
#include <iostream>

int main()
{
	// Test 1: Basic response
	HttpResponse resp1;
	resp1.setStatus(200);
	resp1.setHeader("Content-Type" , "text/html");
	resp1.setBody("<h1>Hello World</h1>");

	std::string output1 = resp1.build();
	
	if (output1.find("HTTP/1.1 200 OK") != std::string::npos)
		std::cout << "✓ Status line correct!" << std::endl;
	else
	 	std::cout << "✗ Status line wrong!" << std::endl;

	if (output1.find("Content-Type: text/html") != std::string::npos)
		std::cout << "✓ Content-Type header present" << std::endl;
	else
		std::cout << "✗ Content-Type header missing" << std::endl;

	if (output1.find("Content-Length: 20") != std::string::npos)
		std::cout << "✓ Content-Length auto-set" << std::endl;
	else
		std::cout << "✗ Content-Length wrong" << std::endl;

	if (output1.find("<h1>Hello World</h1>") != std::string::npos)
		std::cout << "✓ Body present" << std::endl;
	else
		std::cout << "✗ Body missing" << std::endl;


	// Test 2: Convenience method
	HttpResponse resp2 = HttpResponse::ok("Test body", "text/plain");
	std::string output2 = resp2.build();

	if (output2.find("200 OK") != std::string::npos)
		std::cout << "✓ ok() method works" << std::endl;
	else
		std::cout << "✗ ok() method failed" << std::endl;

	// Test 3 : 404 response
	HttpResponse resp3 = HttpResponse::notFound();
	std::string output3 = resp3.build();

	if (output3.find("404 Not Found") != std::string::npos)
		std::cout << "✓ notFound() method works" << std::endl;
	else
		std::cout << "✗ notFound() method failed" << std::endl;

	// Test 4 : Multiple Headers
	HttpResponse resp4;
	resp4.setHeader("X=Custom-Header", "CustomValue");
	resp4.setHeader("Cache-Control", "no-cache");
	std::string output4 = resp4.build();

	if (output4.find("X=Custom-Header: CustomValue") != std::string::npos &&
		output4.find("Cache-Control: no-cache") != std::string::npos)
		std::cout << "✓ Multiple headers work" << std::endl;
	else
		std::cout << "✗ Multiple headers failed" <<std::endl;

	return (0);
}