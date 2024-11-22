/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:08 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:50:16 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"

Server::Server() : _server_socket(-1), _new_socket(0), _nb_bytes(0), _info() {}

Server::Server(const std::string &servername, const std::string &hostname, const std::string &port) :
	_name(servername),
	_hostname(hostname),
	_port(port),
	_server_socket(socket(AF_INET, SOCK_STREAM, 0)),
	_new_socket(0),
	_nb_bytes(0),
	_info()
{
}

Server::Server(const Server &src) :
	_name(src._name),
	_hostname(src._hostname),
	_port(src._port),
	_server_socket(src._server_socket),
	_new_socket(src._new_socket),
	_nb_bytes(src._nb_bytes),
	_request(src._request),
	_info(src._info)
{
}

Server &Server::operator=(const Server &src)
{
	if (this != &src) 
	{
		_new_socket = src._new_socket;
		_nb_bytes = src._nb_bytes;
		_request = src._request;
		_info = src._info;
	}
	return (*this);
}

int Server::createSocket()
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
	std::cout << "Server " << _name << " is launched !" << std::endl;
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

int Server::setSocket()
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

int Server::receiveRequest()
{
	char buff[MAX_REQ_SIZE];

	_new_socket = accept(_server_socket, NULL, NULL);
	if (_new_socket == -1) 
    {
		std::cerr << "Error on awaiting connection (accept) on " << _name << std::endl;
		return (FAILURE);
	}
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) 
    {
		std::cerr << "Error on set nonblocking on " << _name << std::endl;
		return (FAILURE);
	}
	while (true) 
    {
		for (int i = 0; i < MAX_REQ_SIZE; ++i) 
			buff[i] = 0;
		_nb_bytes =
			static_cast< int >(recv(_new_socket, buff, MAX_REQ_SIZE, 0));
		if (_nb_bytes == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) 
        {
			std::cerr << "Error on recv on " << _name << std::endl;
			close(_new_socket);
		} 
        else if (_nb_bytes == 0) 
        {
			std::cout << "client disconnected on " << _name << std::endl;
			close(_new_socket);
		}
		_request.append(buff, _nb_bytes);
		if (_nb_bytes < MAX_REQ_SIZE)
			break;
	}
	return (SUCCESS);
}

int Server::getSocket() const { return (_server_socket); }

int Server::getNbBytes() const { return (_nb_bytes); }

std::string Server::getName() const { return (_name); }

std::string Server::getHost() const { return (_hostname); }

std::string Server::getPort() const { return (_port); }

std::string Server::getRequest() const { return (_request); }

Server::~Server()
{
	close(_server_socket);
	freeaddrinfo(_info);
}
