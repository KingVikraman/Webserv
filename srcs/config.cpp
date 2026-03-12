/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:48:15 by zernest           #+#    #+#             */
/*   Updated: 2026/03/12 22:16:30 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

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
