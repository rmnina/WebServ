/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 01:12:29 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/21 02:32:50 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"

int	main(void)
{
	Config		config;
	std::string	filename = "test.conf";
	
	try 
	{
		config.fill_conf_vector(filename);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}