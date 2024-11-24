/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:05 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/24 01:18:31 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <exception>
# include <ctime>
# include <iomanip>
# include <sstream>
# include "../server/Server.hpp"
# include "../server/Handler.hpp"
# include "../config/Config.hpp"

class Parser
{
	private:
		std::map<std::string, std::vector<std::string> >	_request;
		std::string											_response;

		std::string											_extension;
		int													_error_code;

		Server												*_server;
		
	public:
		std::map<std::string, std::string> 					mime_types;			

		Parser( void);
		Parser( Server *server);
		// Parser( const Parser &src);
		// Parser	&operator=( const Parser &rhs);

		void		init_mime_types( void);

		void		build_raw_text( std::ifstream &resource);
		void		build_image( std::ifstream &resource);
		void		exec_cgi( std::ifstream &resource);

		void		GETmethod( void);
		void		POSTmethod( void);
		void		DELETEmethod( void);

		std::string	get_content_type( const std::string &filename);
		size_t		get_content_length( const std::string &filename);
		void		build_response_header( void);
		void		build_response( void);

		std::string	handle_request( int client_index);
		
		bool		fill_method( const std::string &request);
		bool		fill_path( const std::string &request);
		bool		fill_content_type( const std::string &request);
		bool		check_version( const std::string &request);
		bool		check_req_size( const std::string &request);

		

		~Parser( void);
	
};

std::string							get_time(void);

#endif