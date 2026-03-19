/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:55:06 by zernest           #+#    #+#             */
/*   Updated: 2026/03/19 22:00:08 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSE_HPP
#define CONFIGPARSE_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>

#include "config.hpp"

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
	void expect(const std::string &expected);
	const std::vector<ServerConfig>& getServers() const;
};

#endif