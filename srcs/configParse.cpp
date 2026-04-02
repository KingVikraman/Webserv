/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:48:33 by zernest           #+#    #+#             */
/*   Updated: 2026/04/02 21:21:44 by zernest          ###   ########.fr       */
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

void configParse::parseLocation(ServerConfig &server)
{
	LocationConfig location;

	if (peek() == ";")
	{
		std::cerr << "Error: missing location path\n";
		exit(1);
	}

	location.path = next();
	expect("{");
	
	while (peek() != "}")
	{
		if (peek() == "allow_methods")
		{
			next();
			while (peek() != ";")
			{
				location.methods.push_back(next());
			}
			expect(";");
		}

		else if (peek() == "root")
		{
			next();
			if (peek() == ";")
			{
				std::cerr << "Error: missing root in location\n";
				exit(1);
			}
			location.root = next();
			expect(";");
		}

		else
		{
			std::cerr << "Error: unknown directive '" << peek() << "' in location\n";
			exit(1);
		}
	}

	expect("{");
	server.locations.push_back(location);
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
			parseLocation(server);
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