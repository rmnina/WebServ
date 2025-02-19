/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:08 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/19 16:10:21 by jdufour          ###   ########.fr       */
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
	_nb_bytes(0)
{
	_info = NULL;
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;     
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
	_event.events = EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &_event) == -1) 
		print_log(CERR, RED, "Error", _name, "Failed on adding epoll event. Socket fd: ", fd);
}

void	Server::modify_event(int &epfd, int fd, uint32_t flag)
{
	memset(&_event, 0, sizeof(epoll_event));
	_event.data.fd = fd;
	_event.events = flag;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &_event) == -1)
		print_log(CERR, RED, "Error", _name, "Failed on modifying epoll event. Socket fd: ", fd);
}

void	Server::delete_event(int &epfd, int fd)
{
	std::ostringstream fdstring;

	fdstring << fd;
	
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		print_log(CERR, RED, "Error", _name, "Failed on deleting epoll event. Socket fd: ", fd);
}

int Server::create_socket()
{
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1)
	{
		print_log(CERR, RED, "Error", _name, "The following socket initialisation failed. Socket fd: ", _server_socket);
		return (FAILURE);
	}
	print_log(COUT, BLUE, "Log :", _name, "Socket has been successfuly initialized. Socket fd: ", _server_socket);
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) 
	{
		print_log(CERR, RED, "Error: ", _name, "The server socket flagging failed: Socket fd: ", _server_socket);
		return (FAILURE);
	}
	int val = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &val, sizeof(int)) == -1) 
	{
		print_log(CERR, RED, "Error", _name, "The server socket port config failed. Socket fd: ", _port);
		return (FAILURE);
	}
	print_log(COUT, BLUE, "Log :", _name, "Successfully launched with host ", _hostname);
	std::cout << TURQUOISE << "Server " << BOLD << _name << RESET TURQUOISE << " with host " << _hostname << " is launched on port " << _port << RESET << std::endl;
	if (getaddrinfo(_hostname.c_str(), _port.c_str(), &_hints, &_info) != 0) 
	{
		print_log(CERR, RED, "Error", _name, "Could not retrieve address info for port ", _port);
		return (FAILURE);
	}
	return (SUCCESS);
}

int Server::set_socket()
{
	if (bind(_server_socket, _info->ai_addr, _info->ai_addrlen) == -1) 
	{
		print_log(CERR, RED, "Error", _name, "Socket binding failed. Socket fd: ", _server_socket);
		return (FAILURE);
	}
	print_log(COUT, BLUE, "Log", _name, "Socket has been successfuly binded on port ", _port);
	if (listen(_server_socket, MAXREQUEST) == -1) 
		return (FAILURE);
	return (SUCCESS);
}

int	Server::accept_connection(int &epfd)
{
	int			client_sock;
	sockaddr_in	client_addr;
	socklen_t	addr_len = sizeof(client_addr);

	client_sock = -1;
	
	client_sock = accept(_server_socket, (sockaddr *)&client_addr, &addr_len);
	if (client_sock == -1) 
	{
		print_log(CERR, RED, "Error", _name, "Accept failed. Socket fd :", _port);
		return (FAILURE);
	}
	if (fcntl(client_sock, F_SETFL, O_NONBLOCK) == -1) 
	{
		print_log(CERR, RED, "Error", _name, "The client socket flagging failed. Socket fd: ", _port);
		return (FAILURE);
	}
	for (std::vector<int>::iterator it = _client_sock.begin(); it != _client_sock.end(); ++it) 
	{
		if (*it == client_sock) 
		{
			print_log(CERR, RED, "Error", _name, "Client socket already exists. Socket fd: ", client_sock);
			close(client_sock);
			return (CONTINUE);
		}
	}
	add_event(epfd, client_sock);

	std::vector<char>	tmp;
	tmp.push_back('\0');

	_client_sock.push_back(client_sock);
	print_log(COUT, GREEN, "Log", _name, "Client socket has been successfuly created. Socket fd: ", client_sock);
	_request.push_back("");
	_req_body.push_back("");
	_req_binary.push_back(tmp);
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

	if (nb_bytes < 0) 
	{
		print_log(CERR, RED, "Error", _name, "Recv failed. Socket fd: ", _client_sock[client_index]);
		std::cerr << "Error on recv on " << _name << " Errno is " << errno << " fd is " << _client_sock[client_index] << std::endl;
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
		size_t				content_length;
		size_t				body_start;
		std::string			req_body;
		std::vector<char>	req_binary;

		content_length = atoi(_request[client_index].c_str() + pos + strlen("Content-Length: "));
		body_start = _request[client_index].find("\r\n\r\n");
		if (body_start != std::string::npos)
			body_start += 4;
		if (_nb_bytes[client_index] < content_length + body_start)
			return (CONTINUE);

		req_body = _request[client_index].substr(body_start, content_length);
		_req_body[client_index].append(req_body);

		for (size_t i = 0; buffer[i] && i < content_length; i++)
			req_binary.push_back(buffer[i]);
		_req_binary[client_index].insert(_req_binary[client_index].end(), req_binary.begin(), req_binary.end());
	}	
	return (SUCCESS);
}

bool	Server::has_client(int fd) const 
{
	for (std::vector<int>::const_iterator it = _client_sock.begin(); it != _client_sock.end(); ++it) 
	{
		if (*it == fd) 
			return true;
	}
	return false;
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
			print_log(CERR, RED, "Error", _name, "Error sending chunks. Socket fd: ", _client_sock[client_index]);
			return (close (_client_sock[client_index]), 1);
		}
		bytes_sent += packet_size;
	}
	std::string end_chunk = "0\r\n\r\n";
	send(_client_sock[client_index], end_chunk.c_str(), end_chunk.size(), 0);
	if (_keep_alive[client_index])
		modify_event(epfd, _client_sock[client_index], EPOLLIN);
	else
	{
		delete_event(epfd, _client_sock[client_index]);
		close(_client_sock[client_index]);
		_client_sock.erase(_client_sock.begin() + client_index);
	}
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
		_req_binary[client_index].clear();
		_keep_alive[client_index] = false;
		return (SUCCESS);
	}
	if (received == CONTINUE || received == DISCONNECT)
		return (CONTINUE);
	return (FAILURE);
}

/******----------------***** GETTERS *****----------------******/

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

std::vector<std::vector<char> >	Server::getReqBinary( void) const { return (_req_binary);}

std::vector<bool>			Server::getConnectionStatus( void) const { return (_keep_alive); }
 
bool	Server::getErrorPage( void) const { return (_error_page); }


Server::~Server()
{
	close(_server_socket);
	for (std::vector<int>::iterator it = _client_sock.begin(); it < _client_sock.end(); ++it)
		close(*it);
	freeaddrinfo(_info);
}
