/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:38:50 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/17 15:59:44 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Handler.hpp"
#include "../../include/server/Signal.hpp"
#include "../../include/config/Config.hpp"
#include "../../include/parser/Parser.hpp"

volatile int g_sig = 0;

Handler::Handler() {}

Handler::Handler(const std::vector<ConfigStruct> servers_conf) : _servers_conf(servers_conf) {}

void	Handler::loadServ()
{
	
	printConfig(_servers_conf);
	for (std::vector<ConfigStruct>::iterator it = _servers_conf.begin(); it < _servers_conf.end();)
	{
		std::vector<ConfigStruct>::iterator loc = it + 1;
		server_data				 			config;
		location_data						locations;

		std::string	name = it->get_server_value("server_name")[0];
		std::string	hostname = it->get_server_value("host")[0];
		std::string	port = it->get_server_value("listen")[0];
		config = (*it).serverData;
		if (loc != _servers_conf.end())
		{
			for (; loc->get_container_type() == ConfigStruct::LOCATION_MAP; loc++);
			if (loc != it + 1)
			{
				it++;
				locations = (*it).locationData;
			}
		}
		it++;
		// iterqtion sur les serveurs pour verifier que hosntae + port 
		//if 
			//tg
		//else
			_servers.push_back(new Server(name, hostname, port, config, locations));
	}
}

void	Handler::add_event(int fd, int flag) 
{
	struct epoll_event event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event) == -1) 
		print_log(CERR, RED, "Error", "EPOLL", "Failed on adding epoll event. Socket fd: ", fd);
}

int Handler::launchServers()
{
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); ++it) 
	{
		(*it)->create_socket();
		(*it)->set_socket();
	}
	return (SUCCESS);
}

int Handler::handleEvents()
{
	_epfd = epoll_create1(0);

	if (_epfd == -1) 
	{
		std::cerr << "Error on epoll_create" << std::endl;
		return (FAILURE);
	}
	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); ++it)
		(*it)->add_event(_epfd, (*it)->getSocket());

	while (!g_sig) 
	{
		initSignal();
		int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, -1);
		if (nfds == -1 && !g_sig) 
		{
			std::cerr << "Error on epoll_wait" << std::endl;
			return (FAILURE);
		}

		for (int i = 0; i < nfds; i++) 
		{
			bool handled = false;
			for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); ++it) 
			{
				if (_events[i].data.fd == (*it)->getSocket()) 
				{
					if ((*it)->accept_connection(_epfd) == FAILURE)
						return (FAILURE);
					handled = true;
					break;
				}
			}
			if (!handled) 
			{
				for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); ++it) 
				{
					if ((*it)->has_client(_events[i].data.fd)) 
					{
						if ((*it)->handle_existing_client(_events[i].data.fd, _epfd) == FAILURE) 
							return (FAILURE);
						break;
					}
				}
			}
		}
	}
	return (SUCCESS);
}

Handler::~Handler()
{
	for (std::vector<Server *>::iterator it = this->_servers.begin(); it < this->_servers.end(); it++) 
	{
		print_log(COUT, TURQUOISE, "EXIT", (*it)->getName(), "Closing server...", " ");
		delete (*it);
	}
	close(_epfd);
}

std::vector<Server *>& Handler::getServers()
{
	return (this->_servers);
}
