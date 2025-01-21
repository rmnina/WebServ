/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahayon <ahayon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 02:54:52 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/20 17:58:09 by ahayon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Config.hpp"

Config::Config( void) {}

// Config::Config( const Config &src) {}

// Config  &Config::operator=( const Config &rhs){}

std::vector<ConfigStruct>	Config::get_servers_conf( void) const
{
	return (_servers_conf);
}

bool    Config::_server_allowed( const std::string &keyword) 
{
	const std::string			array[10] = {"listen", "server_name", "dir_listing", "error", "method",
											"upload", "root", "body_size", "index", "location"};
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
	const std::string			array[8] = {"dir_listing", "error", "method", "upload", "root", 
											"default_file", "cgi", "redirect"};
	std::vector<std::string>	allowed(array, array + 8);

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

void	Config::fill_locations( std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets)
{
	location_i_data loc_i;
	size_t		start, end;
	int 		i;

	std::cout << "line avant le for: " << line << std::endl;
	for (i = 0; line[i] == ' '; i++);
	start = line.find(' ', i);
	end = line.find_last_of(' ');
	std::cout << "start: " << start << " end: " << end << "\n";
	std::string route = line.substr(start, end - start);
	std::cout << "route: " << route << "\n";
	
	std::vector<std::string> tmp_vect;
	tmp_vect.push_back(route);
	loc_i["route"] = tmp_vect;

	/* DISPLAY TMP_VECT DATA */
	// std::cout << "tmp_vect: ";
	// std::vector<std::string>::iterator itt;
	// for (itt = tmp_vect.begin(); itt != tmp_vect.end(); itt++)
	// 	std::cout << *itt << " ";
	
	/* 	TRY TO AVOID HARD DATA*/
	/* 1er essai */
		//loc_i[route] = tmp_vect;	-> ne fonctionne pas
	/* 2eme essai */
	//loc_i[*tmp_vect.begin()] = tmp_vect;
		// std::vector<std::string>::iterator it;
		// for (it = loc_i[*tmp_vect.begin()].begin(); it != loc_i[*tmp_vect.begin()].end(); it++)
		// 	std::cout << *it << " ";
		// std::cout << "\n";

	std::vector<std::string>::iterator it;
	std::cout << "loc_i[\"route\"]: ";
	for (it = loc_i["route"].begin(); it != loc_i["route"].end(); it++)
		std::cout << *it << " ";
	std::cout << "\nline: " << line << std::endl;
	std::getline(conf_file, line);
	std::cout << "After line break: " << line << std::endl;
	
	if (!line.empty() && !line.compare(0, 1, "{"))
		brackets = _update_brackets_state(brackets);
	std::getline(conf_file, line);
	std::cout << "After bracket: " << line << std::endl;
	while (!line.empty())
	{
		for (i = 0; line[i] == ' '; i++);
		line = line.substr(i, line.size() - i);
		std::cout << "!line.empty(): " << line << std::endl;
		if (line[0] == '#')
			std::getline(conf_file, line);
			
		size_t		space_pos = line.find(" ", 0);
		std::string keyword = line.substr(0, space_pos);
		std::cout << "keyword: " << keyword << std::endl;
		if (!_location_allowed(keyword))
		{
			std::cout << "keyword before break: " << keyword << std::endl;
			break ;
		}
		std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
		loc_i[keyword] = tmp;
		std::getline(conf_file, line);
	}
	location.push_back(loc_i);
	for (location_data::iterator vec_it = location.begin(); vec_it != location.end(); ++vec_it) {
        std::cout << "Map: \n";
        for (std::map<std::string, std::vector<std::string> >::iterator map_it = vec_it->begin(); map_it != vec_it->end(); ++map_it) {
            std::cout << "  Key: " << map_it->first << "\n  Values: ";
            for (std::vector<std::string>::iterator vec_str_it = map_it->second.begin(); vec_str_it != map_it->second.end(); ++vec_str_it) {
                std::cout << *vec_str_it << " ";
				       }
            std::cout << "\n";
        }
    }
	std::cout << "\n";
	if (!line.empty() && !line.compare(0, 1, "}"))
		brackets = _update_brackets_state(brackets);
}

void	Config::fill_servers( std::ifstream &conf_file, std::string &line, server_data &server, location_data &locations, bool &brackets)
{
	int i;
	
	std::getline(conf_file, line);
	if (!line.empty() && !line.compare(0, 1, "{"))
		brackets = _update_brackets_state(brackets);
	std::getline(conf_file, line);

	while (!line.empty())
	{
		for (i = 0; line[i] == ' '; i++);
		line = line.substr(i, line.size() - i);
		if (line[0] == '#')
			std::getline(conf_file, line);
		else
		{
			size_t		space_pos = line.find(" ", 0);
			std::string keyword = line.substr(0, space_pos);
			if (!_server_allowed(keyword))
			{
				if (!line.compare(0, 1, "}"))
					_update_brackets_state(brackets);
				break ;
			}
			if (!keyword.compare("location"))
				fill_locations(conf_file, line, locations, brackets);
			else
			{
				std::cout << "keyword != location: " << keyword << "\n";
				std::vector<std::string> tmp(string_to_vector(line, ' ', space_pos));
				server[keyword] = tmp;
			}
			std::cout << "current line is: " << line;
			std::getline(conf_file, line);
			std::cout << ", next line is: " << line << "\n";
		}
	}
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
					std::cout << *vit << " ";
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
						std::cout << *vit << " ";
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
			std::map<std::string, std::vector<std::string> >::iterator it;
            for (it = server.begin(); it != server.end(); it++)
            {
                std::cout << "server key = " << it->first << std::endl;
                for (std::vector<std::string>::iterator vIt = it->second.begin(); vIt != it->second.end(); vIt++)
                    std::cout << "string for " << it->first << " = " << *vIt << "\n";
            }
			std::getline(conf_file, line);
			std::cout << "\033[94mici line " << line << "\033[0m\n\n";
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
