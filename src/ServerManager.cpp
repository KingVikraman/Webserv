#include "../inc/ServerManager.hpp"

#include <cerrno>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

ServerManager::ServerManager() : _listen_fd(-1), _max_fd(-1), _running(false)
{
}

ServerManager::~ServerManager()
{
    stop();
}

// Create listening socket
bool ServerManager::setup(const std::string &host, int port)
{
    struct sockaddr_in addr;  // IPV4 address 
    int yes = 1;

    _listen_fd = socket(AF_INET, SOCK_STREAM, 0); // af_inet for ipv4, sock_stream for tcp, 0 for default protocol (tcp in this case)
    if (_listen_fd < 0)
    {
        std::cerr << "socket() failed: " << std::strerror(errno) << std::endl;
        return false;
    }
    // Set SO_REUSEADDR to allow quick reuse of the port after the server is stopped
    if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) // _listen_fd socket to configure, SOL_SOCKET for socket level, SO_REUSEADDR to allow quick reuse of the port, &yes to enable the option, sizeof(yes) for the option value size
    {
        std::cerr << "setsockopt() failed: " << std::strerror(errno) << std::endl;
        close(_listen_fd);
        _listen_fd = -1;
        return false;
    }
    if (!_setNonBlocking(_listen_fd))
    {
        close(_listen_fd);
        _listen_fd = -1;
        return false;
    }

    std::memset(&addr, 0, sizeof(addr));  // Zero out the address 
    addr.sin_family = AF_INET;      // set address family to AF_INET for IPv4
    addr.sin_port = htons(port);    // set port number, htons converts host byte to network byte (network is always big-endian)
    if (host.empty() || host == "0.0.0.0")
        addr.sin_addr.s_addr = INADDR_ANY;  // meaning bind to all addr
    else if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) // since it's not global bind, we try and parse it as an IPv4 address.
    {
        std::cerr << "invalid host: " << host << std::endl;
        close(_listen_fd);
        _listen_fd = -1;
        return false;
    }

    if (bind(_listen_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) // _listen_fd socket to bind, reinterpret_cast to convert sockaddr_in to sockaddr, sizeof(addr) for the address size
    {
        std::cerr << "bind() failed: " << std::strerror(errno) << std::endl;
        close(_listen_fd);
        _listen_fd = -1;
        return false;
    }
    if (listen(_listen_fd, 128) < 0) // ready to accept connection, 128 is max pending connections in the queue
    {
        std::cerr << "listen() failed: " << std::strerror(errno) << std::endl;
        close(_listen_fd);
        _listen_fd = -1;
        return false;
    }

    _max_fd = _listen_fd;  // inntializing later on we will replace higher fd to max_fd ( for efficeincy in select() call)
    _running = true;
    std::cout << "Listening on " << (host.empty() ? "0.0.0.0" : host) << ":" << port << std::endl;
    return true;
}

void ServerManager::run()
{
    while (_running)
    {
        fd_set read_fds;
        fd_set write_fds;
        struct timeval timeout;

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        FD_SET(_listen_fd, &read_fds);
        _max_fd = _listen_fd;

        for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            const int fd = it->first;
            FD_SET(fd, &read_fds);
            if (it->second.readyToWrite())
                FD_SET(fd, &write_fds);
            if (fd > _max_fd)
                _max_fd = fd;
        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        const int ready = select(_max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            std::cerr << "select() failed: " << std::strerror(errno) << std::endl;
            break;
        }

        if (FD_ISSET(_listen_fd, &read_fds))
            _acceptNewConnection();

        std::vector<int> client_fds;
        for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
            client_fds.push_back(it->first);

        for (size_t i = 0; i < client_fds.size(); ++i)
        {
            const int fd = client_fds[i];
            if (_clients.find(fd) == _clients.end())
                continue;
            if (FD_ISSET(fd, &read_fds))
                _readFromClient(fd);
            if (_clients.find(fd) == _clients.end())
                continue;
            if (FD_ISSET(fd, &write_fds))
                _writeToClient(fd);
        }
        _checkTimeouts();
    }
}

void ServerManager::stop()
{
    if (_listen_fd >= 0) // if the listening socket is open, close it
    {
        close(_listen_fd);
        _listen_fd = -1;
    }
    std::vector<int> to_close;
    for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
        to_close.push_back(it->first);  // push client fd
    for (size_t i = 0; i < to_close.size(); ++i)
        _closeClient(to_close[i]);
    _running = false;
}

bool ServerManager::_setNonBlocking(int fd) const
{
    const int flags = fcntl(fd, F_GETFL, 0); // get current file status flags, 0 for default
    if (flags < 0)
    {
        std::cerr << "fcntl(F_GETFL) failed: " << std::strerror(errno) << std::endl;
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) // using bitwise OR to add O_NONBLOCK flag to the existing flags
    {
        std::cerr << "fcntl(F_SETFL) failed: " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void ServerManager::_acceptNewConnection()
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    const int client_fd = accept(_listen_fd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_len);

    if (client_fd < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            std::cerr << "accept() failed: " << std::strerror(errno) << std::endl;
        return;
    }
    if (!_setNonBlocking(client_fd))
    {
        close(client_fd);
        return;
    }
    _clients[client_fd] = Client(client_fd);
}

void ServerManager::_readFromClient(int client_fd)
{
    char buffer[kMessageBufferSize];
    const ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_read == 0)
    {
        _closeClient(client_fd);
        return;
    }
    if (bytes_read < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            _closeClient(client_fd);
        return;
    }

    Client &client = _clients[client_fd];
    client.touch();
    client.rawRequest().append(buffer, static_cast<size_t>(bytes_read));
    client.request().feed(buffer, static_cast<size_t>(bytes_read));

    if (!client.request().parsingCompleted() && client.request().errorCode() == 0)
        return;

    _logRequestTrace(client.rawRequest());
    client.responseBuffer() = _request_handler.buildResponseForRawRequest(client.rawRequest());
    client.setReadyToWrite(true);
    client.setShouldClose(true);
}

void ServerManager::_writeToClient(int client_fd)
{
    Client &client = _clients[client_fd];
    const std::string &response = client.responseBuffer();

    if (client.bytesSent() >= response.size())
    {
        if (client.shouldClose())
            _closeClient(client_fd);
        else
            client.resetResponseState();
        return;
    }

    const char *start = response.c_str() + client.bytesSent();
    const size_t remaining = response.size() - client.bytesSent();
    const ssize_t bytes_sent = send(client_fd, start, remaining, 0);

    if (bytes_sent < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            _closeClient(client_fd);
        return;
    }
    if (bytes_sent == 0)
    {
        _closeClient(client_fd);
        return;
    }

    client.touch();
    client.addBytesSent(static_cast<size_t>(bytes_sent));
    if (client.bytesSent() >= response.size())
    {
        if (client.shouldClose())
            _closeClient(client_fd);
        else
            client.resetResponseState();
    }
}

void ServerManager::_closeClient(int client_fd)
{
    std::map<int, Client>::iterator it = _clients.find(client_fd);

    if (it == _clients.end())
        return; // already closed
    close(client_fd);
    _clients.erase(it); // remove client from the map
}

void ServerManager::_checkTimeouts()
{
    const time_t now = std::time(NULL);
    std::vector<int> to_close;

    for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (now - it->second.lastActivity() > kClientTimeoutSeconds)
            to_close.push_back(it->first);
    }
    for (size_t i = 0; i < to_close.size(); ++i)
        _closeClient(to_close[i]);
}

void ServerManager::_logRequestTrace(const std::string &raw_request) const
{
    std::string request_line = "<malformed request>";
    const size_t line_end = raw_request.find("\r\n");
    if (line_end != std::string::npos && line_end > 0)
        request_line = raw_request.substr(0, line_end);

    const std::time_t now = std::time(NULL);
    struct tm local_tm;
    localtime_r(&now, &local_tm);

    std::ostringstream timestamp;
    timestamp << std::setfill('0')
              << (local_tm.tm_year + 1900) << "-"
              << std::setw(2) << (local_tm.tm_mon + 1) << "-"
              << std::setw(2) << local_tm.tm_mday << " "
              << std::setw(2) << local_tm.tm_hour << ":"
              << std::setw(2) << local_tm.tm_min << ":"
              << std::setw(2) << local_tm.tm_sec;

    std::cout << "[TRACE] " << timestamp.str() << " : " << request_line << std::endl;
}
