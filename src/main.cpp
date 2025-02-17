/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 01:12:29 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/15 19:10:33 by jdufour          ###   ########.fr       */
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
