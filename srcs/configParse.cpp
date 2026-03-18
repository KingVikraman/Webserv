/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:48:33 by zernest           #+#    #+#             */
/*   Updated: 2026/03/18 23:19:00 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParse.hpp"
#include "config.hpp"

configParse::configParse(const std::vector<std::string> &tokens) : tokens(tokens), pos(0) {}

std::string configParse::peek() const
{
	return tokens[pos];
}

std::string configParse::next()
{
	return tokens[pos++];
}

void configParse::expect(const std::string &expected)
{
	if (peek() != expected)
	{
		std::cerr << "Error: Expected '" << expected << "' but got: '" << peek() << "'\n";
		exit(1);
	}
	next();
}

void configParse::parseServer()
{
	ServerConfig server;

	expect("{");
	
	while (pos < tokens.size() && peek() != "}")
	{
		if (peek() == "listen")
		{
			next();
			
			if (peek() == ";")
			{
				std::cerr << "Error: missing value for port.\n";
				exit(1);
			}
			std::string portStr = next();
			server.port = std::atoi(portStr.c_str());
			expect(";");
		}
		else if (peek() == "server_name") // handles single server name for now
		{
			next();

			if (peek() == ";")
			{
				std::cerr << "Error: missing server name.\n";
				exit(1);
			}
			std::string serverStr = next();
			server.server_name = serverStr;
			expect(";");
		}
		else if (peek() == "root")
		{
			next();

			if (peek() == ";")
			{
				std::cerr << "Error: missing root address.\n";
				exit(1);
			}
			std::string rootStr = next();
			server.root = rootStr;
			expect(";");
		}
		else if (peek() == "index")
		{
			next();

			if (peek() == ";")
			{
				std::cerr << "Error: missing index.\n";
				exit(1);
			}
			std::string indexStr = next();
			server.index = indexStr;
			expect(";");
		}
		else if (peek() == "location")
		{
			next(); // consume "location"
			std::cout << "Found location block\n";
		}
		else
		{
			std::cerr << "Error: unknown directive '" << peek() << "'\n";
			exit(1);
		}
	}

	if (pos >= tokens.size())
	{
		std::cerr << "Error: missing '}' for server block\n";
		exit(1);
	}

	expect("}");
	servers.push_back(server);
}

void configParse::parse()
{
	while (pos < tokens.size())
	{
		if (peek() == "server")
		{
			next();
			parseServer();
		}
		else
		{
			next();
		}
	}
}

const std::vector<ServerConfig>& configParse::getServers() const
{
	return servers;
}