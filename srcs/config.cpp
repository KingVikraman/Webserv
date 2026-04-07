/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:48:15 by zernest           #+#    #+#             */
/*   Updated: 2026/04/07 17:33:24 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

LocationConfig::LocationConfig() : autoindex(false) {}

std::string readFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
		throw std::runtime_error("Could not open config file");

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::vector<std::string> tokenize(const std::string &text)
{
	std::vector<std::string> tokens;
	std::string token;
	
	for (size_t i = 0; i < text.size(); i++)
	{
		char c = text[i];
		if(isspace(c))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else if (c == '{' || c == '}' || c == ';')
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
			
			tokens.push_back(std::string(1, c));
		}
		else
		{
			token += c;
		}
	}
	if (!token.empty())
		tokens.push_back(token);
	return (tokens);
}

void printVector(const std::vector<std::string>& vec)
{
	for (size_t i = 0; i < vec.size(); i++)
		std::cout << vec[i] << " ";
}

void printLocation(const LocationConfig& loc)
{
	std::cout << "    location " << loc.path << " {\n";

	if (!loc.root.empty())
		std::cout << "        root " << loc.root << ";\n";

	if (!loc.index.empty())
	{
		std::cout << "        index ";
		printVector(loc.index);
		std::cout << ";\n";
	}

	if (!loc.methods.empty())
	{
		std::cout << "        allow_methods ";
		printVector(loc.methods);
		std::cout << ";\n";
	}

	if (!loc.upload_store.empty())
		std::cout << "        upload_store " << loc.upload_store << ";\n";

	if (loc.autoindex)
		std::cout << "        autoindex on;\n";

	std::cout << "    }\n";
}

void printServer(const ServerConfig& s)
{
	std::cout << "server {\n";

	std::cout << "    listen " << s.port << ";\n";

	if (!s.server_names.empty())
	{
		std::cout << "    server_name ";
		printVector(s.server_names);
		std::cout << ";\n";
	}

	if (!s.root.empty())
		std::cout << "    root " << s.root << ";\n";

	if (!s.index.empty())
	{
		std::cout << "    index ";
		printVector(s.index);
		std::cout << ";\n";
	}

	if (s.client_max_body_size > 0)
		std::cout << "    client_max_body_size "
				  << s.client_max_body_size << ";\n";

	for (size_t i = 0; i < s.locations.size(); i++)
	{
		printLocation(s.locations[i]);
	}

	std::cout << "}\n\n";
}

void printConfig(const std::vector<ServerConfig> &servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		printServer(servers[i]);
	}
}

// void printConfig(const std::vector<ServerConfig> &servers)
// {
// 	for (size_t i = 0; i < servers.size(); i++)
// 	{
// 		std::cout << "Server " << i << ":\n";
// 		std::cout << "  Port: " << servers[i].port << "\n";
// 		std::cout << "  Name: ";
// 		for(size_t j = 0; j < servers[i].server_names.size(); j++)
// 		{
// 			std::cout << servers[i].server_names[j] << " ";
// 		}
// 		std::cout << std::endl;
// 		std::cout << "  Root: " << servers[i].root << "\n";
// 		std::cout << "  Index: " << servers[i].index << "\n";
// 		for(size_t j = 0; j < servers[i].locations.size();j++ )
// 		{
// 			std::cout << "  Location " << j << ":\n";
// 			std::cout << "    Path: " << servers[i].locations[j].path << "\n";
// 			std::cout << "    Root: " << servers[i].locations[j].root << "\n";
// 			std::cout << "    Index: ";
// 			for (size_t k = 0; k < servers[i].locations[j].index.size(); k++)
// 			{
// 				std::cout << servers[i].locations[j].index[k] << " ";
// 			}
// 			std::cout << "\n";
// 			std::cout << "    Autoindex: " << (servers[i].locations[j].autoindex ? "on" : "off") << "\n";
// 			std::cout << "    Methods: ";
// 			for (size_t k = 0; k < servers[i].locations[j].methods.size(); k++)
// 			{
// 				std::cout << servers[i].locations[j].methods[k] << " ";
// 			}
// 			std::cout << "\n";
// 			std::cout << "    Upload Store: " << servers[i].locations[j].upload_store << "\n";
// 		}
// 	}
// }