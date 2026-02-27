#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>


int main() 
{
	// Section 1 : Creating Sockets, if not created return error.
	int server_fd =  socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		std::cerr << "Socket creation failed\n" << std::endl;
		return 1;
	}

	std::cout << "Socket created: fd=" << server_fd << "\n";

	//Section 2 : Setup address structure 
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	//Section 3 : Bind socket to address
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failed\n";
		return 1;
	}

	std::cout << "Socket bound to port 8080\n";

	// Section 4 : Start listening
	if (listen(server_fd, 10) < 0)
	{
		std::cerr << "Listen failed" << std::endl;
		return 1;
	}

	std::cout << "Server listening to port 8080..." << std::endl;
	std::cout << "Waiting for connections..." << std::endl;

	// Section 5 : Accept ONE client connection
	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd < 0)
	{
		std::cerr << "Accept failed" << std::endl;
		return 1;
	}

	std::cout << "Client connected! fd= " << client_fd << std::endl;

	// Section 6 : Echo Loop
	char buffer[1024];
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

		if(bytes_read <= 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		std::cout << "Recieved " << bytes_read << "bytes: " << buffer;
		write(client_fd, buffer, bytes_read);
	}

	close(client_fd);
	close(server_fd);

	std::cout << "Server Closed [/]" << std::endl;
	return 0;
}