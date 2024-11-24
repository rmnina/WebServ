/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:40:57 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/24 19:37:30 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP

# include <csignal>
# include <exception>
# include <sys/epoll.h>
# include <vector>
# include "Server.hpp"
# include "../config/Config.hpp"

# define MAX_EVENTS 10

class Handler
{
	private:
		std::vector<ConfigStruct>	_servers_conf;		
		std::vector<Server *>		_servers;
		int							_epfd;

	public:
		Handler( void);
		Handler( const std::vector<ConfigStruct> servers_conf);

		Handler(const Handler &src);
		Handler &operator=(const Handler &rhs);

		void	loadServ( void);
		int		launchServers( void);
		int		get_client_index( Server &server, int event_fd);
		int		handleEvents( void);

		void	add_event(int fd, int event);
		void	modify_event(int fd, int event);
		void	delete_event(int fd);

		~Handler( void);
};

#endif