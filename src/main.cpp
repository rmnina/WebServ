/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 01:12:29 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/18 18:20:53 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config/Config.hpp"
#include "../include/server/Handler.hpp"

int	main(int argc, char **argv)
{
	(void)argv;

	Config	config;

	std::string filename;

	try {
		if (argc == 2)
			filename = argv[1];
		else
			filename = "test.conf";
		if (MAX_REQ_SIZE < 10)
		{
			std::cerr << BOLD RED << "Error : MAX_REQ_SIZE can't be below 10" << RESET << std::endl;
			return (1);
		}
		config.fill_conf_vector(filename);
		Handler	handler(config.get_servers_conf());	
		try 
		{
			handler.loadServ();
			handler.launchServers();
			if (!handler.handleEvents())
				return (1);
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}

	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
