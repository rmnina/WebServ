/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 01:12:29 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/20 11:52:44 by eltouma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config/Config.hpp"
#include "../include/server/Handler.hpp"

int	main(void)
{
	std::string	filename = "test.conf";
	Config	config;

	try {
		config.fill_conf_vector(filename);
		Handler	handler(config.get_servers_conf());	
		try 
		{
			// printConfig(handler.get_servers_conf());
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
