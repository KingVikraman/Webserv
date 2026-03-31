#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Client.hpp"
#include "RequestHandler.hpp"

#include <map>
#include <string>

class ServerManager
{
    public:
        ServerManager();
        ~ServerManager();

        bool setup(const std::string &host, int port);
        void run();
        void stop();

    private:
        enum
        {
            kMessageBufferSize = 8192,
            kClientTimeoutSeconds = 60
        };

        int _listen_fd;
        int _max_fd;
        bool _running;
        RequestHandler _request_handler;
        std::map<int, Client> _clients;

        bool _setNonBlocking(int fd) const;
        void _acceptNewConnection();
        void _readFromClient(int client_fd);
        void _writeToClient(int client_fd);
        void _closeClient(int client_fd);
        void _checkTimeouts();
        void _logRequestTrace(const std::string &raw_request) const;
};

#endif
