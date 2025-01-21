/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahayon <ahayon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:05 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/20 16:46:41 by ahayon           ###   ########.fr       */
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
# include <dirent.h>

# define MTU 1500

class Parser
{
	private:
		std::map<std::string, std::vector<std::string> >	_request;
		std::string											_response;
		std::vector<unsigned char>							_image_response;
		
		size_t												_resp_size;
		std::string											_extension;
		std::string											_category;
		int													_error_code;

		Server												*_server;
		server_data											_server_conf;
		location_data										_location;
		
	public:
		std::map<std::string, std::string> 					mime_types;			

		Parser( void);
		Parser( Server *server);
		// Parser( const Parser &src);
		// Parser	&operator=( const Parser &rhs);

		std::vector<unsigned char>							getImageResponse( void) const;
		std::map<std::string, std::vector <std::string> >	getRequest( void) const;
		std::string											getCategory( void) const;
		size_t												getRespSize( void) const;
		
		void		init_mime_types( void);

		void		build_response_content( std::string &filename);
		void		exec_cgi( std::string &filename);

		void		GETmethod( void);
		void		POSTmethod( void);
		void		DELETEmethod( void);

		std::string					get_content_type( const std::string &filename);
		void						get_content_category( void);
		size_t						get_content_length( const std::string &filename);
		location_i_data				find_location( const std::string &path);
		void						get_location( const std::string &filename);
		std::string					build_response_header( void);
		std::string					build_response( void);

		std::string	examine_request( int client_index);
		
		bool		fill_method( const std::string &request);
		bool		fill_path( const std::string &request);
		bool		fill_content_type( const std::string &request);
		bool		check_version( const std::string &request);
		bool		check_req_size( const std::string &request);

		void		display_dirlist(std::string path);

		~Parser( void);
	
};

std::string							get_time(void);

#endif