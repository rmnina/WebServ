/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:08 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/05 01:07:17 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"
#include "../../include/parser/Parser.hpp"

Server::Server() : _server_socket(-1) {}

Server::Server(const std::string &servername, const std::string &hostname, const std::string &port, 
				server_data config, location_data location) :
	_config(config),
	_location(location),
	_name(servername),
	_hostname(hostname),
	_port(port),
	_nb_bytes(0),
	_info()
{
	memset(&_info, 0, sizeof(_info));
}

Server::Server(const Server &src) :
	_config(src._config),
	_location(src._location),
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

void	Server::add_event(int &epfd, int fd) 
{
	memset(&_event, 0, sizeof(epoll_event));
	_event.data.fd = fd;
	_event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &_event) == -1) 
		throw (std::runtime_error("error on adding event to epoll here??"));
}

void	Server::modify_event(int &epfd, int fd, uint32_t flag)
{
	memset(&_event, 0, sizeof(epoll_event));
	_event.data.fd = fd;
	_event.events = flag | EPOLLET;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &_event) == -1)
		throw (std::runtime_error("error on modifying event to epoll"));
}

void	Server::delete_event(int &epfd, int fd)
{
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw (std::runtime_error("error on removing event to epoll"));
}

int Server::create_socket()
{
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1)
		std::cerr << "Error on init socket " << _name << std::endl;
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) 
	{
		std::cerr << "Error on set nonblocking on " << _name << std::endl;
		return (FAILURE);
	}
	int val = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &val, sizeof(int)) == -1) 
	{
		std::cerr << "error on setting the port on reusable on " << _name
				  << ": " << strerror(errno) << ". socket value is "
				  << _server_socket << std::endl;
		return (FAILURE);
	}
	std::cout << BLUE << "Server " << BOLD << _name << RESET BLUE << " is launched !" << RESET << std::endl;
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

int	Server::accept_connection(int &epfd)
{
	int			client_sock;
	sockaddr_in	client_addr;
	socklen_t	addr_len = sizeof(client_addr);

	client_sock = accept(_server_socket, (sockaddr *)&client_addr, &addr_len);
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
	add_event(epfd, client_sock);
	_client_sock.push_back(client_sock);
	_request.push_back("");
	_req_body.push_back("");
	_nb_bytes.push_back(0);
	_keep_alive.push_back(false);
	return (SUCCESS);
}

int	Server::receive_request(int client_index, int &epfd)
{
	char	buffer[MAX_REQ_SIZE];
	ssize_t	nb_bytes;
	size_t	pos;
		
	memset(buffer, 0, MAX_REQ_SIZE);
	nb_bytes = recv(_client_sock[client_index], buffer, MAX_REQ_SIZE, 0);
	_nb_bytes[client_index] += nb_bytes;
	_request[client_index].append(buffer);

	size_t conn_pos = _request[client_index].find("Connection: keep-alive");
	if (conn_pos != std::string::npos) 
		_keep_alive[client_index] = true;
	std::cout << ORANGE BOLD << "REQ IS " << std::endl << buffer << RESET << std::endl;

	if (nb_bytes < 0) 
	{
		std::cerr << "Error on recv on " << _name << " Errno is " << errno << std::endl;
		delete_event(epfd, _client_sock[client_index]);
		close(_client_sock[client_index]);
		_client_sock.erase(_client_sock.begin() + client_index);
		_request.erase(_request.begin() + client_index);
		_nb_bytes.erase(_nb_bytes.begin() + client_index);
		return (FAILURE);
	}
	if (nb_bytes == 0)
	{
		delete_event(epfd, _client_sock[client_index]);
		close(_client_sock[client_index]);
		_client_sock.erase(_client_sock.begin() + client_index);
		_request.erase(_request.begin() + client_index);
		_nb_bytes.erase(_nb_bytes.begin() + client_index);
		return (DISCONNECT);	
	}
	pos = _request[client_index].find("Content-Length: ");
	if (pos == std::string::npos)
		pos = _request[client_index].find("content-length: ");
	if (pos != std::string::npos)
	{
		size_t		content_length;
		size_t		body_start;
		std::string	req_body;
		content_length = atoi(_request[client_index].c_str() + pos + strlen("Content-Length: "));
		body_start = _request[client_index].find("\r\n\r\n");
		if (body_start != std::string::npos)
			body_start += 4;
		if (_nb_bytes[client_index] < content_length + body_start)
			return (CONTINUE);
		req_body = _request[client_index].substr(body_start, content_length);
		std::cout << YELLOW BOLD << "BODY IS " << std::endl << req_body << RESET << std::endl;
		_req_body[client_index].append(req_body);
	}	
	return (SUCCESS);
}

int	Server::get_client_index(int event_fd)
{
	long unsigned int i = 0;
	
	if (!_client_sock.size())
		return (-1);
	for (; i < _client_sock.size(); i++)
	{
		if (_client_sock[i] == event_fd)
			return (i);
	}
	return (-1);	
}

int	Server::send_response(std::string &response, int client_index, int &epfd)
{
	unsigned long	bytes_sent = 0;
	size_t			resp_size = response.size();
	
	modify_event(epfd, _client_sock[client_index], EPOLLOUT);
	while (bytes_sent < resp_size)
	{
		size_t	packet_size;
		if (MTU < resp_size - bytes_sent)
			packet_size = MTU;
		else
			packet_size = resp_size - bytes_sent;
		
		std::ostringstream chunk_header;
		chunk_header << std::hex << packet_size << "\r\n";
		std::string chunk = chunk_header.str();
		chunk += response.substr(bytes_sent, packet_size);
		chunk += "\r\n";

		ssize_t	bytes = send(_client_sock[client_index], chunk.c_str(), chunk.size(), 0);
		if (bytes <= 0)
		{
			std::cout << RED BOLD << "Error chunk" << RESET << std::endl;
			return (close (_client_sock[client_index]), 1);
		}
		bytes_sent += packet_size;
	}
	std::string end_chunk = "0\r\n\r\n";
	send(_client_sock[client_index], end_chunk.c_str(), end_chunk.size(), 0);
	if (_keep_alive[client_index])
		modify_event(epfd, _client_sock[client_index], EPOLLIN);
	else
		close(_client_sock[client_index]);
	return (1);
}

int	Server::handle_existing_client( int event_fd, int &epfd)
{
	int	client_index = get_client_index(event_fd);
	
	if (client_index == -1)
		return (-1);
	int received = receive_request(client_index, epfd);
	if (received == SUCCESS)
	{
		Parser	parser(this);
		parser.examine_request(client_index);
		std::string response = parser.build_response_header();
		send(_client_sock[client_index], response.c_str(), response.size(), 0); 
		response = parser.build_response();
		send_response(response, client_index, epfd);
		_request[client_index].clear();
		_nb_bytes[client_index] = 0;
		_req_body[client_index].clear();
		_keep_alive[client_index] = false;
		return (SUCCESS);
	}
	if (received == CONTINUE || received == DISCONNECT)
		return (CONTINUE);
	return (FAILURE);
}

server_data	Server::getConfig( void) const { return (_config); }

location_data	Server::getLocation( void) const { return (_location); }

int Server::getSocket( void) const { return (_server_socket); }

std::vector<int>	Server::getClientSock( void) const { return (_client_sock); }

std::vector<size_t> Server::getNbBytes( void) const { return (_nb_bytes); }

std::string Server::getName( void) const { return (_name); }

std::string Server::getHost( void) const { return (_hostname); }

std::string Server::getPort( void) const { return (_port); }

std::vector<std::string>	Server::getRequest( void) const { return (_request); }

std::vector<std::string>	Server::getReqBody( void) const { return (_req_body); }

std::vector<bool>			Server::getConnectionStatus( void) const { return (_keep_alive); }
 
bool	Server::getErrorPage( void) const { return (_error_page); }


Server::~Server()
{
	close(_server_socket);
	freeaddrinfo(_info);
}
