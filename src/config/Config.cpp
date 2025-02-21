/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skiam <skiam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 02:54:52 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/20 22:51:48 by skiam            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Config.hpp"
#include "../../include/parser/Parser.hpp"

Config::Config(void) {}

// Config::Config( const Config &src) {}

// Config  &Config::operator=( const Config &rhs){}


void	Config::fill_locations(std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets)
{
	location_i_data loc_i;
	size_t		start, end;
	int 		i;

	for (i = 0; line[i] == ' '; i++);
	start = line.find(' ', i) + 1;
	end = line.find_last_of('\n');
	std::string route = line.substr(start, end - start);
	
	std::vector<std::string> tmp_vect;
	tmp_vect.push_back(route);
	loc_i["route"] = tmp_vect;

	std::getline(conf_file, line);
	for (i = 0; line[i] == ' '; i++);
	if (!line.empty() && !line.compare(i, 1, "{"))
		brackets = _update_brackets_state(brackets);
	else
		throw std::invalid_argument("C'est non 2");
	std::getline(conf_file, line);
	while (!line.empty())
	{
		for (i = 0; line[i] == ' '; i++);
		line = line.substr(i, line.size() - i);
		if (line[0] == '#')
			std::getline(conf_file, line);
			
		size_t		space_pos = line.find(" ");
		std::string keyword = line.substr(0, space_pos);
		if (!_location_allowed(keyword))
		{
			break ;
		}
		std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
		loc_i[keyword] = tmp;
		std::getline(conf_file, line);
	}
	location.push_back(loc_i);
	if (!line.empty() && !line.compare(0, 1, "}"))
		brackets = _update_brackets_state(brackets);
}

void	Config::fill_servers(std::ifstream &conf_file, std::string &line, server_data &server, location_data &locations, bool &brackets)
{
	int i;
	
	std::getline(conf_file, line);
	if (!line.empty() && !line.compare(0, 1, "{"))
		brackets = _update_brackets_state(brackets);
	else
		throw std::invalid_argument("Problem with brackets in the conf file");
	std::getline(conf_file, line);

	while (!line.empty())
	{
		for (i = 0; line[i] == ' '; i++);
		line = line.substr(i, line.size() - i);
		if (line[0] == '#')
			std::getline(conf_file, line);
		else
		{
			size_t		space_pos = line.find(" ");
			std::string keyword = line.substr(0, space_pos);
			if (!_server_allowed(keyword))
			{
				if (!line.compare(0, 1, "}"))
					_update_brackets_state(brackets);
				else
					throw std::invalid_argument("");
				break ;
			}
			if (!keyword.compare("location"))
				fill_locations(conf_file, line, locations, brackets);
			else
			{
				std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
				if (!check_keyword_validity(keyword, tmp)) {
					throw std::invalid_argument("Unauthorized argument in conf file for " + keyword);
				}
				server[keyword] = tmp;
			}
			std::getline(conf_file, line);
		}
	}
	if (!line.empty() && !line.compare(0, 1, "}")) {
		brackets = _update_brackets_state(brackets);
	}
	if (server.find("listen") == server.end() || server.find("host") == server.end() || server.find("root") == server.end())
		throw std::invalid_argument("Error: make sure to to add a 'listen'/'host/'root' value in your conf file");
	std::vector<std::string> tmp2, tmp3;
	if (server.find("server_name") == server.end()) {
		tmp2.push_back("");
		server["server_name"] = tmp2;
	}
	if (server.find("body_size") == server.end()) {
		tmp3.push_back("20000");
		server["body_size"] = tmp3;
	}
}

void    Config::fill_conf_vector(const std::string &filename) 
{
	std::ifstream	conf_file(filename.c_str());
	server_data		server;
	location_data	location;
	bool			brackets = false;
	
	if (!conf_file.is_open())
	{
		print_log(CERR, RED, "Error", "Config", "Could not open conf file :", filename.c_str());
		throw std::runtime_error("Could not open file");
	}
	
	std::string	line;
	while (std::getline(conf_file, line))
	{
		if (!line.empty() && !line.compare(0, 6, "server"))
		{
			fill_servers(conf_file, line, server, location, brackets);
			std::map<std::string, std::vector<std::string> >::iterator it;
			std::getline(conf_file, line);
		}
		if (!server.empty())
		{
			_servers_conf.push_back(ConfigStruct(server));
			server.clear();
			if (!location.empty())
			{
				_servers_conf.push_back(ConfigStruct(location));
				location.clear();
			}
		}
		if (!line.empty() && line[0] == ' ')
		{
			int i = 0;
			for (; line[i] == ' '; i++);
			line = line.substr(i, line.size() - i);
		}
		if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '}')
			continue;
		else
		{
			std::cout << line << std::endl;
			throw std::runtime_error("Unauthorized parameter in conf file");
		}
	}
	if (brackets)
	{
		std::cerr << "Unclosed brackets" << std::endl;
		throw std::runtime_error("Unclosed brackets in conf file");
	}
}

Config::~Config(void) {}