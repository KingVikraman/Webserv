/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:55:06 by zernest           #+#    #+#             */
/*   Updated: 2026/03/17 18:10:17 by zernest          ###   ########.fr       */
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

class configParse
{
private:
	std::vector<std::string> tokens;
	size_t pos;
public:
	configParse(const std::vector<std::string> &tokens);
	std::string peek() const;
	std::string next();
	void parse();
	void expect(const std::string &expected);
	void parseServer();
};

#endif