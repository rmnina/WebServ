/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:10 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/23 23:05:34 by jdufour          ###   ########.fr       */
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
# include "../config/Config.hpp"

# define SUCCESS 0
# define FAILURE 1

# define MAXREQUEST 10
# define MAX_REQ_SIZE 100

class Server
{
	private:		
		ConfigStruct		*_config;
		
		const std::string	_name;
		const std::string	_hostname;
		const std::string	_port;
		const int			_server_socket;
		int					_client_sock;
		int					_nb_bytes;
		std::string			_request;
		struct addrinfo		*_info;

 	public:
		Server( void);
		Server(const std::string &servername, const std::string &hostname, const std::string &port, ConfigStruct *config);
		Server(const Server &src);
		Server &operator=(const Server &src);

		ConfigStruct	*getConfig( void) const;
		int				getSocket( void) const;
		int				getClientSock( void) const;
		int				getNbBytes( void) const;
		std::string		getName( void) const;
		std::string		getHost( void) const;
		std::string		getPort( void) const;
		std::string		getRequest( void) const;

		int	createSocket( void);
		int	setSocket( void);
		int	receiveRequest( void);
		int	sendResponse( void);

		~Server( void);
};

#endif