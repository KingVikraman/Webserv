#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed\n";
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Server listening on port 8080\n";

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        std::cerr << "Accept failed\n";
        return 1;
    }

    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    read(client_fd, buffer, sizeof(buffer));

    std::cout << "Client sent:\n" << buffer << std::endl;

    const char* response = "Hello from server\n";
    write(client_fd, response, std::strlen(response));

    close(client_fd);
    close(server_fd);

    return 0;
}