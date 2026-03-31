#include "../inc/Client.hpp"

Client::Client()
    : _fd(-1), _bytes_sent(0), _ready_to_write(false), _should_close(false), _last_activity(std::time(NULL))
{
}

Client::Client(int fd)
    : _fd(fd), _bytes_sent(0), _ready_to_write(false), _should_close(false), _last_activity(std::time(NULL))
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

void Client::setFd(int fd)
{
    _fd = fd;
}

HttpRequest &Client::request()
{
    return _request;
}

const HttpRequest &Client::request() const
{
    return _request;
}

std::string &Client::rawRequest()
{
    return _raw_request;
}

const std::string &Client::rawRequest() const
{
    return _raw_request;
}

std::string &Client::responseBuffer()
{
    return _response_buffer;
}

const std::string &Client::responseBuffer() const
{
    return _response_buffer;
}

size_t Client::bytesSent() const
{
    return _bytes_sent;
}

void Client::addBytesSent(size_t bytes)
{
    _bytes_sent += bytes;
}

void Client::resetResponseState()
{
    _response_buffer.clear();
    _bytes_sent = 0;
    _ready_to_write = false;
}

bool Client::readyToWrite() const
{
    return _ready_to_write;
}

void Client::setReadyToWrite(bool value)
{
    _ready_to_write = value;
}

bool Client::shouldClose() const
{
    return _should_close;
}

void Client::setShouldClose(bool value)
{
    _should_close = value;
}

time_t Client::lastActivity() const
{
    return _last_activity;
}

void Client::touch()
{
    _last_activity = std::time(NULL);
}
