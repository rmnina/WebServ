/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:38:50 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/27 23:06:12 by jdufour          ###   ########.fr       */
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

// int	Handler::get_client_index( Server &server, int event_fd)
// {

// }

// int	Handler::send_response( Server &server, std::string &response, int client_index)
// {
// 	unsigned long	bytes_sent = 0;
// 	size_t			packets_sent = 0;
// 	size_t			resp_size = response.size();
	
// 	modify_event(server.getClientSock()[client_index], EPOLLOUT);
// 	while (bytes_sent < resp_size)
// 	{
// 		size_t	packet_size;
// 		if (MTU < resp_size - bytes_sent)
// 			packet_size = MTU;
// 		else
// 			packet_size = resp_size - bytes_sent;
// 		ssize_t	bytes = send(server.getClientSock()[client_index], response.c_str() + bytes_sent, packet_size, 0);
// 		if (bytes <= 0)
// 			return (close (server.getClientSock()[client_index]), 1);
// 		bytes_sent += bytes;
// 		packets_sent++;
// 	}
// 	modify_event(server.getClientSock()[client_index], EPOLLIN);
// 	return (1);
// }

// int	Handler::send_image( Server &server, unsigned char *response, size_t size, int client_index)
// {
// 	unsigned long	bytes_sent = 0;
// 	size_t			packets_sent = 0;
	
// 	modify_event(server.getClientSock()[client_index], EPOLLOUT);
// 	while (bytes_sent < size)
// 	{
// 		size_t	packet_size;
// 		if (MTU < size - bytes_sent)
// 			packet_size = MTU;
// 		else
// 			packet_size = size - bytes_sent;
// 		ssize_t	bytes = send(server.getClientSock()[client_index], response + bytes_sent, packet_size, 0);
// 		if (bytes <= 0)
// 			return (close (server.getClientSock()[client_index]), 1);
// 		bytes_sent += bytes;
// 		packets_sent++;
// 	}
// 	modify_event(server.getClientSock()[client_index], EPOLLIN);
// 	return (1);
// }

// int	Handler::handle_existing_client( Server &server, int event_fd)
// {
// 	int	client_index = get_client_index(server, event_fd);
// 	if (client_index != -1)
// 	{
// 		if (server.receive_request(client_index, _epfd) == 1)
// 		{
// 			Parser	parser(&server);
// 			std::string response = parser.handle_request(client_index);
// 			if (!parser.getCategory().compare("IMAGE"))
// 			{
// 				send_response(server, response, client_index);
// 				unsigned char *img_response = parser.build_img_response();
// 				send_image(server, img_response, parser.getRespSize(), client_index);
// 				return (SUCCESS);
// 			}
// 			else
// 			{
// 				response += parser.build_response();
// 				send_response(server, response, client_index);
// 				return (SUCCESS);
// 			}
// 		}
// 		return (FAILURE);
// 	}
// 	return (FAILURE);
// }

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
					(*it)->handle_existing_client(_events[i].data.fd, _epfd);
					// if (handled == FAILURE)
					// 	return (FAILURE);
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
}
