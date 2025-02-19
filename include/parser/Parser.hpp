/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:05 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/19 16:26:18 by jdufour          ###   ########.fr       */
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
# include <sys/stat.h>
# include <sys/wait.h>

# define MTU 1500
# define GET	0
# define POST	1

# define COUT	0
# define CERR	1

class Parser
{
	private:
		std::string											_req_str;
		std::map<std::string, std::vector<std::string> >	_request;
		std::string											_request_body;
		std::vector<char>									_req_binary;
		std::string											_response;
		std::vector<unsigned char>							_image_response;
		int													_port_code;
		std::string											_port_page;

		size_t												_body_size;
		bool												_keep_alive;
		
		size_t												_resp_size;
		std::string											_extension;
		std::string											_category;
		int													_error_code;
		std::string											_error_page;
		std::string											_upload_dir;

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
		int													getErrorCode( void) const;
		
		void		init_mime_types( void);

		void		build_response_content( std::string &filename);
		void		build_POST_response( std::string &filename);
		void		exec_cgi( std::string &filename, int method);
		void		handle_cgi_error(int *status, pid_t pid);

		void		GETmethod( void);
		void		POSTmethod( void);
		void		DELETEmethod( void);
		void		upload( void);

		std::string					get_content_type( const std::string &filename);
		void						get_content_category( void);
		size_t						get_content_length( const std::string &filename);
		location_i_data				find_location( const std::string &path, location_data::iterator &it);
		void						get_location( const std::string &filename);
		std::string					build_response_header( void);
		std::string					build_response( void);
		void						build_response_upload();

		void		examine_request( int client_index);
		
		bool		fill_method( const std::string &request);
		bool		fill_path( const std::string &request);
		bool		fill_content_type( const std::string &request);
		bool		check_version( const std::string &request);
		bool		check_req_size( const std::string &request);
		bool		check_body_size( void);

		bool		fill_content_type_multipart( const std::string &request);
		bool		fill_content_length( const std::string &request);
		bool		get_file_name( const std::string &body, std::string &filename);
		bool		get_file_content( const std::string &body, std::vector<char> &content);

		int			build_error_page( void);
		int			throw_error_page( void);
		int			restore_error_page( void);
		void		display_dirlist(std::string path);
		
		void        display_code(void);
        bool        is_directory(const std::string &path);
        bool        is_file(const std::string &path);

		int			build_delete_page( void);
		int			restore_delete_page( void);
		~Parser( void);
	
};

std::string	get_time(void);

bool 		is_directory(const std::string &path);
bool 		is_file(const std::string &path);

template <typename T>
void	print_log(int output, const char *color, const char *status, const std::string &servername, const char *message, const T &value) 
{
	switch (output)
	{
		case CERR:
		{
			std::cerr << color << ITALIC << status << RESET << color <<" on " << BOLD << servername << " : " << RESET << color
			<< message << BOLD << value << RESET << std::endl;
			break;
		}
		case COUT:
		{
			std::cout << color << ITALIC << status << RESET << color << " on " << BOLD << servername << " : " << RESET << color
			<< message << BOLD << value << RESET << std::endl;
			break;
		}
	}
}

#endif
