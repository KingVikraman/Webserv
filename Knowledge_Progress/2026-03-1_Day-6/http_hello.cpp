#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <cstring>

void setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


int main()
{
	// Creating a server socket.
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	// Enabling the address reuse.
	int opt = -1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// Non-blocking function call.
	setNonBlocking(server_fd);
	
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	bind(server_fd, (struct sockaddr*)&address, sizeof(address));
	listen(server_fd, 10);

	std::cout << "Multi-client server on port 8080" << std::endl;

	// Array of file descriptors to monitor.
	std::vector<struct pollfd> poll_fds;

	// Adding server sockets to the array of polls.
	struct pollfd server_pollfd;
	server_pollfd.fd = server_fd;		//"Watch ticket" for server socket.
	server_pollfd.events = POLLIN;		// Watch for incoming data/connections.
	poll_fds.push_back(server_pollfd); // Add new polls to watchlist.

	// If true the server runs forever.
	while (true)
	{
		// Wait for any events on either one socket.
		int activity = poll(poll_fds.data(), poll_fds.size(), -1);

		if(activity < 0)
		{
			std::cerr << "Poll error " << std::endl;
			break;
		}

		// Loops though all the checked polls.
		for( size_t i = 0; i < poll_fds.size(); i++)
		{
			if(poll_fds[i].revents == 0)
				continue;

			// New connections.
			if(poll_fds[i].fd == server_fd)
			{
				int client_fd = accept(server_fd, NULL, NULL);
				if (client_fd < 0)
				{
					std::cerr << "Accept failed " << std::endl;
					continue;
				}

				setNonBlocking(client_fd);
				std::cout << "New client: fd= " << client_fd << std::endl;

				struct pollfd client_pollfd;
				client_pollfd.fd = client_fd;
				client_pollfd.events = POLLIN;
				poll_fds.push_back(client_pollfd);
			}

			else
			{
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));

				int bytes_read = read(poll_fds[i].fd, buffer, sizeof(buffer) - 1);

				if (bytes_read <= 0)
				{
					std::cout << "Client disconnected: fd = " << poll_fds[i].fd << std::endl;
					close(poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					i--;
				}
				else
				{
					std::string response = 
							"HTTP/1.1 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: 57\r\n"
							"\r\n"
							"<html><body><h1>Hello from our server!</h1></body></html>";

					write(poll_fds[i].fd, response.c_str(), response.length());
					std::cout << "HTTP response sent to fd= " << poll_fds[i].fd << "\n";
					close(poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					i--;
				}
			}
		}
	}
}