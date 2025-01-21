/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:40:57 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/21 15:13:24 by eltouma          ###   ########.fr       */
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

# define MAX_EVENTS 1000
# define MTU		1500

class Handler
{
	private:
		std::vector<ConfigStruct>	_servers_conf;		
		std::vector<Server *>		_servers;
		int							_epfd;
		struct epoll_event			_events[MAX_EVENTS];

	public:
		Handler( void);
		Handler( const std::vector<ConfigStruct> servers_conf);

		Handler(const Handler &src);
//		Handler &operator=(const Handler &rhs);

		void	loadServ( void);
		int		launchServers( void);
		int		get_client_index( Server &server, int event_fd);
		int		handle_existing_client( Server &server, int event_fd);
		int		send_response( Server &server, std::string &response, int client_index);
		int		send_image( Server &server, unsigned char *response, size_t size, int client_index);
		int		handleEvents( void);

		void	add_event(int fd, int event);
		void	modify_event(int fd, int event);
		void	delete_event(int fd);

		~Handler( void);
};

#endif
