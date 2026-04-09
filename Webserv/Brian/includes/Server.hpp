#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <string>
#include <poll.h>
#include "../../Zep/includes/Config.hpp"
#include "RequestHandler.hpp"

class Server {
public:
    Server(const Config& config);
    ~Server();

    bool start();
    void run();

private:
    const Config&               _config;
    RequestHandler              _handler;
    std::vector<pollfd>         _fds;           // everything poll watches
    std::vector<int>            _server_fds;    // listening sockets only
    std::map<int, std::string>  _client_buffers;   // incoming data per fd
    std::map<int, std::string>  _client_responses; // outgoing data per fd

    int  _createListenSocket(int port);
    void _acceptConnection(int server_fd);
    void _readFromClient(size_t index);
    void _writeToClient(size_t index);
    void _closeConnection(size_t index);
    bool _isServerFd(int fd) const;
};

#endif