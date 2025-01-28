/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:10 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/27 23:05:38 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <cerrno>
# include <cstdlib>
# include <cstring>
# include <fcntl.h>
# include <iostream>
# include <netdb.h>
# include <string>
# include <unistd.h>
# include <sys/epoll.h>
# include "../config/Config.hpp"

# define SUCCESS 0
# define FAILURE 1
# define CONTINUE 2
# define DISCONNECT 3

# define MAXREQUEST 10
# define MAX_REQ_SIZE 20000

class Server
{
	private:		
		server_data					_config;
		location_data				_location;
		
		const std::string			_name;
		const std::string			_hostname;
		const std::string			_port;
		int							_server_socket;
		std::vector<int>			_client_sock;
		std::vector<size_t>			_nb_bytes;
		std::vector<std::string>	_request;
		std::vector<std::string>	_req_body;
		struct addrinfo				*_info;
		struct epoll_event			_event;

		bool						_error_page;

 	public:
		Server(void);
		Server(const std::string &servername, const std::string &hostname, const std::string &port, server_data config, location_data location);
		Server(const Server &src);
		Server &operator=(const Server &src);

		server_data					getConfig(void) const;
		location_data				getLocation(void) const;
		int							getSocket(void) const;
		std::vector<int>			getClientSock(void) const;
		std::vector<size_t>			getNbBytes(void) const;
		std::string					getName(void) const;
		std::string					getHost(void) const;
		std::string					getPort(void) const;
		std::vector<std::string>	getRequest(void) const;
		std::vector<std::string>	getReqBody( void) const;
		bool						getErrorPage( void) const;

		void	add_event(int &epfd, int socket);
		void	modify_event(int &epfd, int socket, uint32_t flag);
		void	delete_event(int &epfd, int socket);
		
		int	create_socket(void);
		int	set_socket(void);
		int	accept_connection(int &epfd);
		int	receive_request(int client_index, int &epfd);
		int	get_client_index(int event_fd);
		int	handle_existing_client(int event_fd, int &epfd);
		int	send_image(unsigned char *response, size_t size, int client_index, int &epfd);
		int	send_response(std::string &response, int client_index, int &epfd);
		

		~Server( void);
};

#endif
