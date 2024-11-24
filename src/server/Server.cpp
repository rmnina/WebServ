/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:08 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/24 20:01:30 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"

Server::Server() : _config(NULL), _server_socket(-1), _client_sock(0), _nb_bytes(0), _info() {}

Server::Server(const std::string &servername, const std::string &hostname, const std::string &port, ConfigStruct *config) :
	_config(config),
	_name(servername),
	_hostname(hostname),
	_port(port),
	_server_socket(socket(AF_INET, SOCK_STREAM, 0)),
	_nb_bytes(0),
	_info()
{
}

Server::Server(const Server &src) :
	_config(src._config),
	_name(src._name),
	_hostname(src._hostname),
	_port(src._port),
	_server_socket(src._server_socket),
	_client_sock(src._client_sock),
	_nb_bytes(src._nb_bytes),
	_request(src._request),
	_info(src._info)
{
}

Server &Server::operator=(const Server &src)
{
	if (this != &src) 
	{
		_client_sock = src._client_sock;
		_nb_bytes = src._nb_bytes;
		_request = src._request;
		_info = src._info;
	}
	return (*this);
}

void	Server::add_event(int epfd, int fd, int flag) 
{
	struct epoll_event event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) 
		throw (std::runtime_error("error on adding event to epoll"));
}

void	Server::modify_event(int epfd, int fd, int flag)
{
	struct epoll_event event;
	event.events = flag;
	event.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == -1)
		throw (std::runtime_error("error on modifying event to epoll"));
}

void	Server::delete_event(int epfd, int fd)
{
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw (std::runtime_error("error on removing event to epoll"));
}


int Server::create_socket()
{
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) 
	{
		std::cerr << "Error on set nonblocking on " << _name << std::endl;
		return (FAILURE);
	}
	int val = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1) 
	{
		std::cerr << "error on setting the port on reusable on " << _name
				  << ": " << strerror(errno) << ". socket value is "
				  << _server_socket << std::endl;
		return (FAILURE);
	}
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(int)) == -1) 
	{
		std::cerr << "error on setting the port on reusable on " << _name
				  << ": " << strerror(errno) << std::endl;
		return (FAILURE);
	}
	std::cout << BLUE << "Server " << BOLD << _name << RESET BLUE << " is launched !" << RESET << std::endl;
	if (_server_socket == -1) 
	{
		std::cerr << "socket failed on " << _name << std::endl;
		return (FAILURE);
	}
	if (getaddrinfo(_hostname.c_str(), _port.c_str(), NULL, &_info) != 0) 
	{
		std::cerr << "getaddrinfo failed on" << _name << std::endl;
		return (FAILURE);
	}
	return (SUCCESS);
}

int Server::set_socket()
{
	if (bind(_server_socket, _info->ai_addr, _info->ai_addrlen) == -1) 
	{
		std::cerr << _name << ": bind failed on " << _name << std::endl;
		return (FAILURE);
	}
	if (listen(_server_socket, MAXREQUEST) == -1) 
		return (FAILURE);
	return (SUCCESS);
}

int	Server::accept_connection( int epfd)
{
	int		client_sock;

	client_sock = accept(_server_socket, NULL, NULL);
	
	if (client_sock == -1) 
	{
		std::cerr << "Error on awaiting connection (accept) on " << _name << std::endl;
		return (FAILURE);
	}
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) 
	{
		std::cerr << "Error on set nonblocking on " << _name << std::endl;
		return (FAILURE);
	}	
	int optval = 1;
	if (setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(int)) == -1)
	{
		std::cerr << "Error on set nonblocking on " << _name << std::endl;
		return (FAILURE);
	}
	add_event(epfd, client_sock, EPOLLIN);
	_client_sock.push_back(client_sock);
	_request.push_back("");
	_nb_bytes.push_back(0);
	return (SUCCESS);
}

int	Server::receive_request( int client_index)
{
	char	buffer[MAX_REQ_SIZE];
	size_t	nb_bytes;
	size_t	pos;
		
	memset(buffer, 0, MAX_REQ_SIZE);
	nb_bytes = recv(_client_sock[client_index], buffer, MAX_REQ_SIZE, 0);
	_nb_bytes[client_index] += nb_bytes;
	_request[client_index].append(buffer);
	std::cout << GREEN << _request[client_index] << RESET << std::endl;
	if (nb_bytes <= 0) 
	{
		std::cerr << "Error on recv on " << _name << std::endl;
		close(_client_sock[client_index]);
	} 
	if (nb_bytes > 0)
	{
		pos = _request[client_index].find("Content-Length: ");
		if (pos == std::string::npos)
			pos = _request[client_index].find("content-length: ") == std::string::npos;
		if (pos != std::string::npos)
		{
			size_t	content_length;
			size_t	body_start;

			content_length = atoi(_request[client_index].c_str() + pos + 16);
			body_start = _request[client_index].find("\r\n\r\n");
			if (body_start != std::string::npos)
				body_start += 4;
			else
			{
				body_start = _request[client_index].find("\n\n");
				if (body_start != std::string::npos)
					body_start += 2;
			}
			if (_nb_bytes[client_index] < content_length + body_start)
				return (0);
			else
				return (1);	
		}
		else
			return (1);
	}
	return (1);
}

// int Server::receive_request( void)
// {
// 	char buff[MAX_REQ_SIZE];

// 	_client_sock = accept(_server_socket, NULL, NULL);
	
// 	if (_client_sock == -1) 
// 	{
// 		std::cerr << "Error on awaiting connection (accept) on " << _name << std::endl;
// 		return (FAILURE);
// 	}
// 	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) 
// 	{
// 		std::cerr << "Error on set nonblocking on " << _name << std::endl;
// 		return (FAILURE);
// 	}
// 	int optval = 1;
// 	if (setsockopt(_client_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(int)) == -1)
// 	{
// 		std::cerr << "Error on set nonblocking on " << _name << std::endl;
// 		return (FAILURE);
// 	}
// 	while (true) 
// 	{
// 		for (int i = 0; i < MAX_REQ_SIZE; ++i) 
// 			buff[i] = 0;
// 		_nb_bytes =
// 			static_cast< int >(recv(_client_sock, buff, MAX_REQ_SIZE, 0));
// 		if (_nb_bytes == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) 
// 		{
// 			std::cerr << "Error on recv on " << _name << std::endl;
// 			close(_client_sock);
// 		} 
// 		else if (_nb_bytes == 0) 
// 		{
// 			std::cout << "client disconnected on " << _name << std::endl;
// 			close(_client_sock);
// 		}
// 		_request.append(buff, _nb_bytes);
// 		if (_nb_bytes < MAX_REQ_SIZE)
// 			break;
// 	}
// 	return (SUCCESS);
// }

ConfigStruct	*Server::getConfig() const { return (_config); }

int Server::getSocket() const { return (_server_socket); }

std::vector<int>	Server::getClientSock() const { return (_client_sock); }

std::vector<size_t> Server::getNbBytes() const { return (_nb_bytes); }

std::string Server::getName() const { return (_name); }

std::string Server::getHost() const { return (_hostname); }

std::string Server::getPort() const { return (_port); }

std::vector<std::string> Server::getRequest() const { return (_request); }

Server::~Server()
{
	close(_server_socket);
	freeaddrinfo(_info);
}
