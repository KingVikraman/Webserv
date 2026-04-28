/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:55:06 by zernest           #+#    #+#             */
/*   Updated: 2026/04/07 17:19:02 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
};

class ServerConfig
{
public:
	int port;
	std::vector<std::string> server_names;
	std::vector<std::string> index;
	std::string root;
	size_t client_max_body_size;

	std::map<int, std::string> error_pages;
	std::vector<LocationConfig> locations;
};

std::string readFile(const std::string& filename);
std::vector<std::string> tokenize(const std::string &text);
void printConfig(const std::vector<ServerConfig> &servers);

#endif