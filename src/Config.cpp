/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.f.            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:17 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/21 00:58:24 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"

Config::Config( void) {}

// Config::Config( const Config &src) {}

// Config  &Config::operator=( const Config &rhs){}

bool    Config::_server_allowed( const std::string &keyword) 
{
	const std::string			array[9] = {"listen", "server_name", "dir_listing", "error", "method", "upload", "root", "body_size", "index"};
	std::vector<std::string>	allowed(array, array + 9);

	for (std::vector<std::string>::iterator it = allowed.begin(); it < allowed.end(); it++)
	{
		if (!keyword.compare(*it))
			return (true);
	}
	return (false);
}

bool    Config::_location_allowed( const std::string &keyword) 
{
	const std::string			array[7] = {"dir_listing", "error", "method", "upload", "root", "default_file", "cgi",};
	std::vector<std::string>	allowed(array, array + 7);

	for (std::vector<std::string>::iterator it = allowed.begin(); it < allowed.end(); it++)
	{
		if (!keyword.compare(*it))
			return (true);
	}
	return (false);
}

bool	Config::_update_brackets_state( bool brackets)
{
	return (brackets ? false : true);
}

void	Config::get_servers( std::ifstream &conf_file, std::string &line, server_data &server, bool &brackets)
{
	int i = 0;
	
	std::getline(conf_file, line);
	if (!line.empty() && !line.compare(0, 1, "{"))
		brackets = _update_brackets_state(brackets);
	std::getline(conf_file, line);

	for (; line[i] == ' '; i++);
	size_t		space_pos = line.find(" ", i);
	std::string keyword = line.substr(i, space_pos - i);

	while (_server_allowed(keyword))
	{
		std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
		server[keyword] = tmp;
		std::getline(conf_file, line);
		for (i = 0; line[i] == ' '; i++);
		space_pos = line.find(" ", i);
		keyword = line.substr(i, space_pos - i);

	}
	if (!line.empty() && !line.compare(0, 1, "}"))
		brackets = _update_brackets_state(brackets);	
}

void	Config::get_locations( std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets)
{
	location_i_data loc_i;
	size_t		tmp_space_pos = line.find(" ");
	int 		i = 0;

	std::string route = line.substr(0, tmp_space_pos);
	std::vector<std::string> tmp_vect;
	tmp_vect.push_back(route);
	loc_i["route"] = tmp_vect;
	std::getline(conf_file, line);
	
	if (!line.empty() && !line.compare(0, 1, "{"))
		brackets = _update_brackets_state(brackets);
	std::getline(conf_file, line);

	for (; line[i] == ' '; i++);
	size_t		space_pos = line.find(" ", i);
	std::string keyword = line.substr(i, space_pos - i);
	while (_location_allowed(keyword))
	{
		std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
		loc_i[keyword] = tmp;
		std::getline(conf_file, line);
		for (i = 0; line[i] == ' '; i++);
		space_pos = line.find(" ", i);
		keyword = line.substr(i, space_pos - i);
	}
	location.push_back(loc_i);
	if (!line.empty() && !line.compare(0, 1, "}"))
		brackets = _update_brackets_state(brackets);
}

void    Config::fill_conf_vector( const std::string &filename) 
{
	std::ifstream	conf_file(filename.c_str());
	server_data		server;
	location_data	location;
	bool			brackets = false;
	
	if (!conf_file.is_open())
	{
		std::cerr << "Error opening conf file" << std::endl;
		throw std::runtime_error("Could not open file");
	}
	
	std::string	line;
	while (std::getline(conf_file, line))
	{
		if (!line.empty() && !line.compare(0, 6, "server"))
		{
			if (!server.empty())
			{
				_servers_conf.push_back(ConfigStruct(server));
				server.clear();
				_servers_conf.push_back(ConfigStruct(location));
				location.clear();
			}
			get_servers(conf_file, line, server, brackets);
			std::getline(conf_file, line);
		}
		if (!line.empty() && !line.compare(0, 8, "location"))
		{
			get_locations(conf_file, line, location, brackets);
			std::getline(conf_file, line);
		}
		if (!line.empty() && !line.compare("\n"))
			std::getline(conf_file, line);
		else if (!line.empty())
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

Config::~Config( void) {}


std::vector<std::string>	string_to_vector(const std::string &string, const char delimiter, size_t space_pos)
{
	std::vector<std::string>	result;
	size_t						last_pos = space_pos + 1;

	for (size_t i = space_pos; i < string.size(); i++)
	{	
		if (string.at(i) == delimiter)
		{
			result.push_back(string.substr(last_pos, i));
			last_pos = i + 1;
		}
	}
	result.push_back(string.substr(last_pos, string.size() - 1));
	return (result);
}
