/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:54:41 by zernest           #+#    #+#             */
/*   Updated: 2026/04/06 16:08:16 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "configParse.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: invalid number of arguements\n";
		return (1);
	}
	std::string configFile = readFile(av[1]);
	std::vector<std::string> tokens = tokenize(configFile);
	configParse config(tokens);
	for (size_t i = 0; i < tokens.size(); i++) // debug tokenizer start
	{
		std::cout << tokens[i] << std::endl;
	}
	std::cout << "\n\n_______Tokenizer Debug ^_______________\n\n\n" << std::endl; // debug tokenizer end
	config.parse();
	const std::vector<ServerConfig>& servers = config.getServers();
	printConfig(servers);
}