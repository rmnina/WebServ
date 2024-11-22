/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 01:12:29 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:44:11 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config/Config.hpp"
#include "../include/server/Handler.hpp"

int	main(void)
{
	std::string	filename = "test.conf";
	Config	config;

	config.fill_conf_vector(filename);
	Handler	handler(config.get_servers_conf());
	
	try 
	{
		// printConfig(handler.get_servers_conf());
		handler.loadServ();
		handler.launchServers();
		handler.handleEvents();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}