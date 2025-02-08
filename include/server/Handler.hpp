/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahayon <ahayon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:40:57 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/07 21:05:57 by ahayon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP

# include <csignal>
# include <exception>
# include <sys/epoll.h>
# include <vector>
# include <sys/stat.h>
# include <sstream>
# include "Server.hpp"
# include "../config/Config.hpp"

# define MAX_EVENTS 1000
# define MTU		1500
# define CREATE_FOLDER 1
# define DELETE_FOLDER 2
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
		void	handle_upload_folders(std::vector<Server *> servers, int action);

		std::vector<Server *> &getServers();

		~Handler(void);
};

#endif
