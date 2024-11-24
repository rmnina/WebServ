/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:38:50 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/24 01:36:59 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Handler.hpp"
#include "../../include/server/Signal.hpp"
#include "../../include/config/Config.hpp"
#include "../../include/parser/Parser.hpp"

volatile int g_sig = 0;

Handler::Handler() : _nbServ(0) {}

Handler::Handler( const std::vector<ConfigStruct> servers_conf) : _servers_conf(servers_conf) {}

void	Handler::loadServ()
{
	printConfig(_servers_conf);
	for (std::vector<ConfigStruct>::iterator it = _servers_conf.begin(); it < _servers_conf.end(); it += 2)
	{
		std::string	name = it->get_server_value("server_name")[1];
		std::string	hostname = "localhost";
		std::string	port = it->get_server_value("listen")[1];
		ConfigStruct *config = &(*it);
		_servers.push_back(new Server(name, hostname, port, config));
	}
}

Handler::Handler(const Handler &src)
{
	*this = src;
}

Handler &Handler::operator=(const Handler &rhs)
{
	this->_epfd = epoll_create1(0);
	this->_nbServ = rhs._nbServ;
	this->_epfd = rhs._epfd;
	for (int i = 0; i < _nbServ; i++) 
		_servers.push_back(new Server(rhs._servers[i]->getName(), rhs._servers[i]->getHost(), rhs._servers[i]->getPort(), rhs._servers[i]->getConfig()));
	return (*this);
}

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
	_epfd = epoll_create1(0);
	if (_epfd == -1) 
	{
		std::cerr << "Error on epoll_create" << std::endl;
		return (FAILURE);
	}

	for (std::vector<Server *>::iterator it = _servers.begin(); it < _servers.end(); ++it) 
	{
		(*it)->create_socket();
		(*it)->set_socket();
		add_event((*it)->getSocket(), EPOLLIN);
	}
	return (SUCCESS);
}

int	Handler::get_client_index( Server &server, int event_fd)
{
	long unsigned int i = 0;
	
	if (!server.getClientSock().size())
		return (-1);
	for (; i < server.getClientSock().size(); i++)
	{
		if (server.getClientSock()[i] == event_fd)
			return (i);
	}
	return (-1);
}

int Handler::handleEvents()
{
	struct epoll_event	events[MAX_EVENTS];
	std::string			response;

	while (!g_sig) 
	{
		initSignal();
		int nfds = epoll_wait(_epfd, events, MAX_EVENTS, -1);
		if (nfds == -1 && !g_sig) 
		{
			std::cout << "Error on epoll_wait" << std::endl;
			return (FAILURE);
		}
		for (int i = 0; i < nfds; ++i) 
		{
			if (events[i].events & EPOLLIN) 
			{
				std::vector<Server *>::iterator it = _servers.begin();
				for (; it < _servers.end(); it++) 
				{
					if (events[i].data.fd == (*it)->getSocket()) 
						(*it)->accept_connection(_epfd);
				}
				if (it == _servers.end())
				{
					std::cout << BOLD GREEN << "yo2" << RESET << std::endl;
					int	client_index = get_client_index((**it), events[i].data.fd);
					if (client_index != -1)
					{
						if ((*it)->receive_request(client_index) == 1)
						{
							Parser	parser(&(**it));
							response = parser.handle_request(client_index);
							modify_event((*it)->getClientSock()[client_index], EPOLLOUT);
							if (send((*it)->getClientSock()[client_index], response.c_str(), strlen(response.c_str()), 0) == -1) 
								std::cerr << "Error on sending response" << std::endl;
							break;
						}
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
	for (int i = 0; i < _nbServ; ++i)
		delete _servers[i];
}