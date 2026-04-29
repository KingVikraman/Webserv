#ifndef CONFIGPARSE_HPP
#define CONFIGPARSE_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

#include "Config.hpp"

class configParse
{
private:
	std::vector<std::string> tokens;
	std::vector<ServerConfig> servers;
	size_t pos;
public:
	configParse(const std::vector<std::string> &tokens);
	std::string peek() const;
	std::string next();
	void parse();
	void parseServer();
	void parseLocation(ServerConfig &server);
	void parseErrorPage(ServerConfig &server);
	void parseReturn(LocationConfig &location);
	void expect(const std::string &expected);
	const std::vector<ServerConfig>& getServers() const;
};

#endif