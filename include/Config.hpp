/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:15 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/21 02:08:58 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>
# include <string>
# include <fstream>
# include <exception>

typedef std::map<std::string, std::vector<std::string> >				server_data;
typedef	server_data 													location_i_data;
typedef std::vector<std::map<std::string, std::vector<std::string> > >	location_data;


struct ConfigStruct
{
	enum type
	{
		SERVER_VECTOR,
		LOCATION_MAP
	} ContainerType;

	server_data 	serverData;
	location_data	locationData;

	ConfigStruct(const server_data &map) : ContainerType(SERVER_VECTOR), serverData(map) {}
	ConfigStruct(const location_data &vector): ContainerType(LOCATION_MAP), locationData(vector) {}
};


class Config
{
	private:
		std::vector<ConfigStruct> _servers_conf;
		
		bool	_server_allowed( const std::string &keyword);
		bool	_location_allowed( const std::string &keyword);
		bool	_update_brackets_state( bool brackets);
		
	public:
		Config( void);
		// Config( const Config &src);
		
		// Config	&operator=( const Config &rhs);

		void		get_servers( std::ifstream &conf_file, std::string &line, server_data &server, bool &brackets);
		void		get_locations( std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets);
		void		fill_conf_vector( const std::string &filename);

		~Config( void);
};

std::vector<std::string>	string_to_vector(const std::string &string, const char delimiter, size_t space_pos);

#endif