/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:38:50 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/06 17:30:15 by eltouma          ###   ########.fr       */
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

		// if (it->get_container_type() != ConfigStruct::SERVER_VECTOR)
		// 	std::cerr << RED BOLD << "not normal to enter there. i failed my calculations didnt i" << RESET << std::endl;
		std::string	name = it->get_server_value("server_name")[0];
		std::string	hostname = "localhost";
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
		_servers.push_back(new Server(name, hostname, port, config, locations));
	}
}

Handler::Handler(const Handler &src)
{
	*this = src;
}

// Handler &Handler::operator=(const Handler &rhs)
// {
// 	this->_epfd = epoll_create1(0);
// 	this->_epfd = rhs._epfd;
// 	for (std::vector<Server *>::iterator it = this->_servers.begin(); it < this->_servers.end(); it++) 
// 		_servers.push_back(new Server((*it)->getName(), (*it)->getHost(), (*it)->getPort(), (*it)->getConfig(), (*it)->getLocation()));
// 	return (*this);
// }

void	Handler::add_event(int fd, int flag) 
{
	struct epoll_event event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event) == -1) 
		throw (std::runtime_error("error on adding event to epoll"));
}

void	Handler::modify_event(int fd, int flag)
{
	struct epoll_event event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event) == -1)
		throw (std::runtime_error("error on modifying event to epoll"));
}

void	Handler::delete_event(int fd)
{
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw (std::runtime_error("error on removing event to epoll"));
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
	std::vector<Server *>::iterator it;
	
	_epfd = epoll_create1(0);

	if (_epfd == -1) 
	{
		std::cerr << "Error on epoll_create" << std::endl;
		return (FAILURE);
	}
	for (it = _servers.begin(); it < _servers.end(); ++it)
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
			for (it = _servers.begin(); it < _servers.end(); it++) 
			{
				if (_events[i].data.fd == (*it)->getSocket())
					(*it)->accept_connection(_epfd);
			}
			if (it == _servers.end())
			{
				for (it = _servers.begin(); it < _servers.end(); it++) 
				{
					int handled = (*it)->handle_existing_client(_events[i].data.fd, _epfd);
					if (handled == FAILURE)
					{
						std::cout << YELLOW BOLD ITALIC << "c'est dans le handled que ca sort????" << RESET << std::endl;
						return (FAILURE);
					}
				}	
			}
		}
	}
	return (SUCCESS);
}

// Server *Handler::operator[](const int index)
// {
// 	if (index < 0 || index >= _nbServ || !_nbServ)
// 		throw(std::out_of_range("Error : invalid access index on Servers"));
// 	return (_servers[index]);
// }

Handler::~Handler()
{
	for (std::vector<Server *>::iterator it = this->_servers.begin(); it < this->_servers.end(); it++) 
		delete (*it);
	close(_epfd);
}
