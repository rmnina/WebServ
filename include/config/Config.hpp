/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:15 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:42:29 by jdufour          ###   ########.fr       */
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
# include "ConfigStruct.hpp"

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

		std::vector<ConfigStruct>	get_servers_conf( void) const;
		ConfigStruct				get_element( int n) const;
		

		void		fill_servers( std::ifstream &conf_file, std::string &line, server_data &server, location_data &locations, bool &brackets);
		void		fill_locations( std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets);
		void		fill_conf_vector( const std::string &filename);

		~Config( void);
};

std::vector<std::string>	string_to_vector(const std::string &string, const char delimiter, size_t space_pos);
void 						printConfig(const std::vector<ConfigStruct> &servers_conf); //tmp print function for debug

#endif