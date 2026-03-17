/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:48:33 by zernest           #+#    #+#             */
/*   Updated: 2026/03/17 21:19:55 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParse.hpp"

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
	expect("{");
	
	while (peek() != "}")
	{
		std::cout << "Inside Server: " << peek() << std::endl;
		next();
	}

	expect("}");
}

void configParse::parse()
{
	while (pos < tokens.size())
	{
		if (peek() == "server")
		{
			next();
			// write parseServer functionn
			std::cout << "Server Block Found\n";
			parseServer();
		}
		else
		{
			next();
		}
	}
}