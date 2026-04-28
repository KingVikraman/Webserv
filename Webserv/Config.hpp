#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>

class LocationConfig
{
public:
	LocationConfig();
	std::string path;
	std::string root;
	std::string upload_store;
	std::vector<std::string> methods;
	std::vector<std::string> index;
	bool autoindex;
	bool has_return;
	int return_code;
	std::string return_url;
};

class ServerConfig
{
public:
	ServerConfig();
	int port;
	std::vector<std::string> server_names;
	std::vector<std::string> index;
	std::string root;
	size_t client_max_body_size;

	std::map<int, std::string> error_pages;
	std::vector<LocationConfig> locations;
};

// Utility functions
std::string readFile(const std::string& filename);
std::vector<std::string> tokenize(const std::string &text);
void printConfig(const std::vector<ServerConfig> &servers);

// Config class to manage server configuration
class Config
{
private:
	std::vector<ServerConfig> servers;
public:
	Config();
	~Config();
	bool parse(const std::string& config_file);
	const std::vector<ServerConfig>& getServers() const;
};

#endif