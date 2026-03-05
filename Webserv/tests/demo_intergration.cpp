#include "../includes/FileHandler.hpp"
#include <iostream>

int main()
{
	FileHandler fh;

	// Simulate frontend request parsing;
	std::string request_path = "/style.css"; // Brian parses from "GET /index.html HTTP/1.1"

	std::cout << "Partner parsed request: " << request_path << std::endl;

	// Brian calls my API:
	if (fh.fileExists("public" + request_path))
	{
		std::string content = fh.getFileContents("public" + request_path);
		std::string mime = fh.getMimeType("public" + request_path);
		size_t size = fh.getFileSize("public" + request_path);

		std::cout << "\n FileHandler SUCCESS: " << std::endl;
		std::cout << "File Exist : YES\n";
		std::cout << "Size: " << size << " bytes\n";
		std::cout << "MIME: " << mime << "\n";
		std::cout << "Content preview: " << content.substr(0,50) << "...\n";


		// Brians response using the the data that i Provide:
		std::cout << "\n Partner builds HTTP response: " << std::endl;
		std::cout << "HTTP/ 1.1 200 OK\r\n";
		std::cout << "Content-Type: " << mime << "\r\n";
		std::cout << "Content-Lenght: " << size << "\r\n";
		std::cout << "\r\n";
		std::cout << content << std::endl; 
	}
	else
	{
		std::cout << "Error 404 : File Not Found !!!" << std::endl;
	}
	return (0);
}
