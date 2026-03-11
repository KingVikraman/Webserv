/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zernest <zernest@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:54:41 by zernest           #+#    #+#             */
/*   Updated: 2026/03/11 22:49:30 by zernest          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

int main()
{
	std::string config = readFile("config.conf");
	std::cout << config << std::endl;
}