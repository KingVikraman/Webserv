#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <iostream>


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

	// Section 


}