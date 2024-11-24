/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:10 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/24 03:31:52 by jdufour          ###   ########.fr       */
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

# define MAXREQUEST 10
# define MAX_REQ_SIZE 2000

class Server
{
	private:		
		ConfigStruct		*_config;
		
		const std::string			_name;
		const std::string			_hostname;
		const std::string			_port;
		const int					_server_socket;
		std::vector<int>			_client_sock;
		std::vector<size_t>			_nb_bytes;
		std::vector<std::string>	_request;
		struct addrinfo				*_info;

 	public:
		Server( void);
		Server(const std::string &servername, const std::string &hostname, const std::string &port, ConfigStruct *config);
		Server(const Server &src);
		Server &operator=(const Server &src);

		ConfigStruct				*getConfig( void) const;
		int							getSocket( void) const;
		std::vector<int>			getClientSock( void) const;
		std::vector<size_t>			getNbBytes( void) const;
		std::string					getName( void) const;
		std::string					getHost( void) const;
		std::string					getPort( void) const;
		std::vector<std::string>	getRequest( void) const;

		void	add_event(int epfd, int socket, int flag);
		void	modify_event(int epfd, int socket, int flag);
		void	delete_event(int epfd, int socket);
		
		int	create_socket( void);
		int	set_socket( void);
		int	accept_connection( int epfd);
		int	receive_request( int client_index);
		int	send_response( void);

		~Server( void);
};

#endif