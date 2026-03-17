/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:54:41 by zernest           #+#    #+#             */
/*   Updated: 2026/03/17 16:33:54 by zernest          ###   ########.fr       */
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
	// for(size_t i = 0; i < tokens.size(); i++)
	// {
	// 	std::cout << tokens[i] << std::endl;
	// }
}