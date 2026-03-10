/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:55:06 by zernest           #+#    #+#             */
/*   Updated: 2026/03/10 21:07:55 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

class LocationConfig
{
public:
	std::string path;
	std::vector<std::string> methods;
	std::string root;
	std::string index;
};

class ServerConfig
{
public:
	int port;
	std::string server_name;
	std::string root;
	std::string index;
	size_t client_max_body_size;

	std::map<int, std::string> error_pages;
	std::vector<LocationConfig> locations;
};