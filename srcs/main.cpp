/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:54:41 by zernest           #+#    #+#             */
/*   Updated: 2026/03/18 16:49:38 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "configParse.hpp"

int main()
{
	std::string configFile = readFile("config.conf");
	std::vector<std::string> tokens = tokenize(configFile);
	configParse config(tokens);
	config.parse();
	const std::vector<ServerConfig>& servers = config.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "Server " << i << ":\n";
		std::cout << "  Port: " << servers[i].port << "\n";
		std::cout << "  Name: " << servers[i].server_name << "\n";
		std::cout << "  Root: " << servers[i].root << "\n";
		std::cout << "  Index: " << servers[i].index << "\n";
	}
}