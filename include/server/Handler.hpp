/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:40:57 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/27 23:04:46 by jdufour          ###   ########.fr       */
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
		Handler(void);
		Handler(const std::vector<ConfigStruct> servers_conf);

		Handler(const Handler &src);
//		Handler &operator=(const Handler &rhs);

		void	loadServ( void);
		int		launchServers( void);
		int		handleEvents( void);

		void	add_event(int fd, int event);
		void	modify_event(int fd, int event);
		void	delete_event(int fd);

		~Handler(void);
};

#endif
