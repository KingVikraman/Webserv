/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:48:33 by zernest           #+#    #+#             */
/*   Updated: 2026/03/12 22:59:58 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParse.hpp"

configParse::configParse(const std::vector<std::string> &tokens) : tokens(tokens), pos(0) {}

void configParse::parse()
{
	while (pos < tokens.size())
	{
		std::string token = tokens[pos];

		if (token == "server")
		{
			// write parseServer function
			parseServer();
		}
		pos++;
	}
}