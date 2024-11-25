/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:05 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/25 23:04:36 by jdufour          ###   ########.fr       */
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

		void		build_raw_text( std::string &filename);
		void		build_image( std::string &filename);
		void		exec_cgi( std::string &filename);

		void		GETmethod( void);
		void		POSTmethod( void);
		void		DELETEmethod( void);

		std::string					get_content_type( const std::string &filename);
		void						get_content_category( void);
		size_t						get_content_length( const std::string &filename);
		std::string					get_location( const std::string &filename);
		void						build_response_header( void);
		unsigned char				*build_img_response( void);
		std::string					build_response( void);

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