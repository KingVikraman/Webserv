#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "HttpRequest.hpp"

#include <ctime>
#include <string>

class Client
{
    public:
        Client();
        explicit Client(int fd);
        ~Client();

        int getFd() const;
        void setFd(int fd);

        HttpRequest &request();
        const HttpRequest &request() const;

        std::string &rawRequest();
        const std::string &rawRequest() const;

        std::string &responseBuffer();
        const std::string &responseBuffer() const;

        size_t bytesSent() const;
        void addBytesSent(size_t bytes);
        void resetResponseState();

        bool readyToWrite() const;
        void setReadyToWrite(bool value);

        bool shouldClose() const;
        void setShouldClose(bool value);

        time_t lastActivity() const;
        void touch();

    private:
        int         _fd;                    // socket fd
        HttpRequest _request;               // parsed request data
        std::string _raw_request;           // incomong raw req
        std::string _response_buffer;       // Full outgoing response
        size_t      _bytes_sent;            // how much _response_buffer has been sent so far
        bool        _ready_to_write;        // flag to tell ServerManager to write response to this client
        bool        _should_close;          // flag to tell ServerManager to close connection after writing response
        time_t      _last_activity;         // timestamp of last activity (for timeout handling)
};

#endif
