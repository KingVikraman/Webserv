/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:48:15 by zernest           #+#    #+#             */
/*   Updated: 2026/03/11 22:48:27 by zernest          ###   ########.fr       */
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