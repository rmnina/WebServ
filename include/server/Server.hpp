/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:10 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:57:17 by jdufour          ###   ########.fr       */
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
		const std::string	_name;
		const std::string	_hostname;
		const std::string	_port;
		const int			_server_socket;
		int					_new_socket;
		int					_nb_bytes;
		std::string			_request;
		struct addrinfo		*_info;

 	public:
		Server();
		Server(const std::string &servername, const std::string &hostname, const std::string &port);
		Server(const Server &src);
		Server &operator=(const Server &src);

		int			getSocket() const;
		int			getNbBytes() const;
		std::string	getName() const;
		std::string	getHost() const;
		std::string	getPort() const;
		std::string	getRequest() const;

		int	createSocket();
		int	setSocket();
		int	receiveRequest();
		int	sendResponse();

		~Server();
};

#endif