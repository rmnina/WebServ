/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:15 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/24 15:56:08 by eltouma          ###   ########.fr       */
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

# define RED		"\x1b[31m"
# define BLUE		"\x1b[34m"
# define RESET		"\x1b[0m"
# define GREEN		"\x1b[32m"
# define PINK		"\x1b[95m"
# define YELLOW 	"\x1b[33m"
# define ORANGE 	"\x1b[38;5;208m"
# define PURPLE 	"\x1b[38;5;129m"
# define TURQUOISE	"\x1b[38;5;80m"
# define MAGENTA	"\x1b[35m"
# define BOLD		"\x1b[1m"
# define ITALIC		"\x1b[3m"

class Config
{
	private:
		std::vector<ConfigStruct> _servers_conf;
		
		bool	_server_allowed(const std::string &keyword);
		bool	_location_allowed(const std::string &keyword);
		bool	_update_brackets_state(bool brackets);
		
	public:
		Config(void);
		// Config( const Config &src);
		// Config	&operator=( const Config &rhs);

		std::vector<ConfigStruct>	get_servers_conf(void) const;
		ConfigStruct				get_element(int n) const;
		

		void		fill_servers(std::ifstream &conf_file, std::string &line, server_data &server, location_data &locations, bool &brackets);
		void		fill_locations(std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets);
		void		fill_conf_vector(const std::string &filename);

		~Config(void);
};

std::vector<std::string>	string_to_vector(const std::string &string, const char delimiter, size_t space_pos);
void 						printConfig(const std::vector<ConfigStruct> &servers_conf); //tmp print function for debug

#endif
