#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

Server::Server(const Config& config) : _config(config) {}

Server::~Server() {
	for (size_t i = 0; i < _fds.size(); i++)
		close(_fds[i].fd);
}

int Server::_createListenSocket(int port) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) { std::cerr << "socket() failed\n"; return -1; }
	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	fcntl(fd, F_SETFL, O_NONBLOCK);
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		std::cerr << "bind() failed on port " << port << "\n";
		close(fd); return -1;
	}
	if (listen(fd, 128) < 0) { std::cerr << "listen() failed\n"; close(fd); return -1; }
	std::cout << "Listening on port " << port << "\n";
	return fd;
}

bool Server::start() {
	const std::vector<ServerConfig>& servers = _config.getServers();
	for (size_t i = 0; i < servers.size(); i++) {
		int fd = _createListenSocket(servers[i].port);
		if (fd < 0) return false;
		_server_fds.push_back(fd);
		pollfd pfd; pfd.fd = fd; pfd.events = POLLIN; pfd.revents = 0;
		_fds.push_back(pfd);
	}
	_handler.setConfig(&_config, servers[0].port);
	return true;
}

void Server::_acceptConnection(int server_fd) {
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
	if (client_fd < 0) { std::cerr << "accept() failed\n"; return; }
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	pollfd pfd; pfd.fd = client_fd; pfd.events = POLLIN; pfd.revents = 0;
	_fds.push_back(pfd);
	_client_buffers[client_fd] = "";
	_client_responses[client_fd] = "";
	std::cout << "New connection: fd=" << client_fd << "\n";
}

void Server::_readFromClient(size_t index) {
	int fd = _fds[index].fd;
	char buffer[4096];
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0) { _closeConnection(index); return; }
	buffer[bytes] = '\0';
	_client_buffers[fd] += std::string(buffer, bytes);
	std::string& buf = _client_buffers[fd];
	size_t header_end = buf.find("\r\n\r\n");
	if (header_end != std::string::npos) {
		bool complete = true;
		size_t cl_pos = buf.find("Content-Length:");
		if (cl_pos == std::string::npos)
			cl_pos = buf.find("content-length:");
		if (cl_pos != std::string::npos) {
			size_t cl_end = buf.find("\r\n", cl_pos);
			std::string cl_str = buf.substr(cl_pos + 15, cl_end - cl_pos - 15);
			size_t content_length = 0;
			std::istringstream(cl_str) >> content_length;
			size_t body_received = buf.size() - (header_end + 4);
			if (body_received < content_length)
				complete = false;
		}
		if (complete) {
			_client_responses[fd] = _handler.buildResponseForRawRequest(buf);
			buf = "";
			_fds[index].events = POLLOUT;
		}
	}
}



void Server::_writeToClient(size_t index)
{
	int fd = _fds[index].fd;
	std::string& response = _client_responses[fd];
	if (response.empty()) { _closeConnection(index); return; }
	ssize_t bytes = send(fd, response.c_str(), response.size(), 0);
	if (bytes <= 0) { _closeConnection(index); return; }
	response = response.substr(bytes);
	if (response.empty()) _closeConnection(index);
}

void Server::_closeConnection(size_t index)
{
	int fd = _fds[index].fd;
	_client_buffers.erase(fd);
	_client_responses.erase(fd);
	close(fd);
	_fds.erase(_fds.begin() + index);
}

bool Server::_isServerFd(int fd) const {
	for (size_t i = 0; i < _server_fds.size(); i++)
		if (_server_fds[i] == fd) return true;
	return false;
}

void Server::run() {
	std::cout << "Server running. Open http://localhost:"
			  << _config.getServers()[0].port << " in your browser.\n";
	while (true) {
		int ready = poll(&_fds[0], _fds.size(), -1);
		if (ready < 0) { std::cerr << "poll() failed\n"; break; }
		for (size_t i = 0; i < _fds.size(); i++) {
			if (_fds[i].revents == 0) continue;
			if (_fds[i].revents & POLLIN) {
				if (_isServerFd(_fds[i].fd))
					_acceptConnection(_fds[i].fd);
				else {
					_readFromClient(i);
					if (i < _fds.size() && (_fds[i].revents & POLLHUP))
						_closeConnection(i--);
				}
			} else if (_fds[i].revents & POLLOUT) {
				_writeToClient(i);
				if (i >= _fds.size()) break;
			} else if (_fds[i].revents & (POLLERR | POLLHUP)) {
				_closeConnection(i--);
			}
		}
	}
}