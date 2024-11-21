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

std::vector<ConfigStruct>	Config::get_servers_conf( void) const
{
	return (_servers_conf);
}

bool    Config::_server_allowed( const std::string &keyword) 
{
	const std::string			array[10] = {"listen", "server_name", "dir_listing", "error", "method", "upload", "root", "body_size", "index", "location"};
	std::vector<std::string>	allowed(array, array + 10);

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

void	Config::fill_servers( std::ifstream &conf_file, std::string &line, server_data &server, location_data &locations, bool &brackets)
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
		if (!keyword.compare("location"))
			fill_locations(conf_file, line, locations, brackets);
		else
		{
			std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
			server[keyword] = tmp;
		}
		std::getline(conf_file, line);
		for (i = 0; line[i] == ' '; i++);
		space_pos = line.find(" ", i);
		keyword = line.substr(i, space_pos - i);

	}
	if (!line.empty() && !line.compare(0, 1, "}"))
		brackets = _update_brackets_state(brackets);	
}

void	Config::fill_locations( std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets)
{
	location_i_data loc_i;
	size_t		start, end;
	int 		i = 0;

	for (; line[i] == ' '; i++);
	start = line.find(' ', i);
	end = line.find_last_of(' ');
	std::string route = line.substr(start, end - start);
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

void printConfig(const std::vector<ConfigStruct> &servers_conf)
{	
	for (size_t i = 0; i < servers_conf.size(); ++i)
	{
		const ConfigStruct &config = servers_conf[i];

		std::cout << "ConfigStruct #" << i + 1 << ":\n";

		if (config.ContainerType == ConfigStruct::SERVER_VECTOR)
		{
			std::cout << "  Type: SERVER_VECTOR\n";
			for (server_data::const_iterator it = config.serverData.begin(); it != config.serverData.end(); ++it)
			{
				std::cout << "  Key: " << it->first << "\n";
				std::cout << "  Values: ";
				for (std::vector<std::string>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
				{
					std::cout << *vit << " ";
				}
				std::cout << "\n";
			}
		}
		else if (config.ContainerType == ConfigStruct::LOCATION_MAP)
		{
			std::cout << "  Type: LOCATION_MAP\n";
			for (location_data::const_iterator it = config.locationData.begin(); it != config.locationData.end(); ++it)
			{
				std::cout << "  Location Map:\n";
				for (server_data::const_iterator sit = it->begin(); sit != it->end(); ++sit)
				{
					std::cout << "    Key: " << sit->first << "\n";
					std::cout << "    Values: ";
					for (std::vector<std::string>::const_iterator vit = sit->second.begin(); vit != sit->second.end(); ++vit)
					{
						std::cout << *vit << " ";
					}
					std::cout << "\n";
				}
			}
		}
		std::cout << "\n";
	}
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
			fill_servers(conf_file, line, server, location, brackets);
			std::getline(conf_file, line);
		}
		if (!server.empty())
		{
			_servers_conf.push_back(ConfigStruct(server));
			server.clear();
			_servers_conf.push_back(ConfigStruct(location));
			printConfig(_servers_conf);
			location.clear();
		}
		if (!line.empty() && !line.compare("\n"))
			std::getline(conf_file, line);
		if (!line.empty() && line[0] == ' ')
		{
			int i = 0;
			for (; line[i] == ' '; i++);
			if (!line.compare(i, 1, "}") || !line.compare(i, 1, "{"))
			{
				std::cout << "entering space before brackets " << std::endl;
				_update_brackets_state(brackets);
				std::getline(conf_file, line);
			}
		}
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
	size_t						start = space_pos;
	size_t						end = string.find(delimiter, start);

	while (end != std::string::npos)
	{
		result.push_back(string.substr(start, end - start));
		start = end + 1;
		end = string.find(delimiter, start);
	}
	if (start < string.length())
		result.push_back(string.substr(start));
	return (result);
}
