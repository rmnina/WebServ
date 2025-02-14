/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahayon <ahayon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 02:54:52 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/14 14:00:40 by ahayon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Config.hpp"

Config::Config(void) {}

// Config::Config( const Config &src) {}

// Config  &Config::operator=( const Config &rhs){}

std::vector<ConfigStruct>	Config::get_servers_conf(void) const
{
	return (_servers_conf);
}

bool    Config::_server_allowed(const std::string &keyword) 
{
	const std::string			array[13] = {"listen", "Cross-Origin-Resource-Policy", "server_name", "dir_listing", "error", "method",
											"upload", "root", "body_size", "index", "location", "host"};
	std::vector<std::string>	allowed(array, array + 13);

	for (std::vector<std::string>::iterator it = allowed.begin(); it < allowed.end(); it++)
	{
		if (!keyword.compare(*it))
			return (true);
	}
	return (false);
}

bool    Config::_location_allowed(const std::string &keyword) 
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

bool	Config::_update_brackets_state(bool brackets) 
{ 
	return (brackets ? false : true); 
}

void	Config::fill_locations(std::ifstream &conf_file, std::string &line, location_data &location, bool &brackets)
{
	location_i_data loc_i;
	size_t		start, end;
	int 		i;

	std::cout << "line avant le for: " << line << std::endl;
	for (i = 0; line[i] == ' '; i++);
	start = line.find(' ', i);
	end = line.find_last_of(' ') + 1;
	std::cout << "start: " << start << " end: " << end << "\n";
	std::string route = line.substr(start, end - start);
	std::cout << "route: " << route << "\n";
	
	std::vector<std::string> tmp_vect;
	tmp_vect.push_back(route);
	loc_i["route"] = tmp_vect;

	std::vector<std::string>::iterator it;
	std::cout << "loc_i[\"route\"]: ";
	for (it = loc_i["route"].begin(); it != loc_i["route"].end(); it++)
		std::cout << *it << " ";
	std::cout << "\nline: " << line << std::endl;
	std::getline(conf_file, line);
	std::cout << "After line break: " << line << std::endl;
	for (i = 0; line[i] == ' '; i++);
	if (!line.empty() && !line.compare(i, 1, "{"))
		brackets = _update_brackets_state(brackets);
	else
		throw std::invalid_argument("C'est non 2");
	std::getline(conf_file, line);
	std::cout << "After bracket: " << line << std::endl;
	while (!line.empty())
	{
		for (i = 0; line[i] == ' '; i++);
		line = line.substr(i, line.size() - i);
		std::cout << "!line.empty(): " << line << std::endl;
		if (line[0] == '#')
			std::getline(conf_file, line);
			
		size_t		space_pos = line.find(" ");
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

bool Config::check_valid_nb(const std::string& str) 
{
    if (str.empty()) 
		return (false); 
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] < '0' || str[i] > '9')
			return (false); 
    }
    return true;
}

bool Config::check_valid_ip(const std::string& ip) 
{
    if (ip.empty()) 
		return false;

    char ipCopy[16];
    std::strncpy(ipCopy, ip.c_str(), 15);
    ipCopy[15] = '\0'; 

    int parts = 0;
    char* token = std::strtok(ipCopy, "."); 
    while (token) {
        std::string part(token);

        if (!check_valid_nb(part)) 
			return (false);

        int num = std::atoi(part.c_str());
        if (num < 0 || num > 255) 
			return (false);

        parts++;
        token = std::strtok(NULL, ".");
    }
    return (parts == 4);
}

bool	Config::check_keyword_validity(std::string keyword, std::vector<std::string> tmp)
{
    if (keyword == "listen") {
        char *endptr;
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
            long nb = strtol(it->c_str(), &endptr, 10);
            if (*endptr != '\0' || nb < 1024 || nb > 65535)
                return false;
        }
        return true;
    } 
    else if (keyword == "host") {
        return (tmp.size() == 1 && (tmp[0] == "localhost" || check_valid_ip(tmp[0])));
    } 
    else if (keyword == "dir_listing") {
        return (tmp.size() == 1 && (tmp[0] == "on" || tmp[0] == "off"));
    } 
    else if (keyword == "index") {
        if (tmp.size() != 1 || tmp[0].substr(tmp[0].size() - 5) != ".html")
            return false;
        std::ifstream file(("www/" + tmp[0]).c_str());
        return file.good();
    } 
    else if (keyword == "body_size") {
        if (tmp.size() != 1)
            return false;
        char *endptr;
        long nb = strtol(tmp[0].c_str(), &endptr, 10);
        return (*endptr == '\0' && nb >= 1 && nb <= 424242);
    } 
    else if (keyword == "method") {
        if (tmp.empty() || tmp.size() > 3)
            return false;
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
            if (*it != "GET" && *it != "POST" && *it != "DELETE")
                return false;
        }
    } 
    else if (keyword == "upload") {
        return (tmp.size() == 2 && tmp[0] == "on");
    } 
    else if (keyword == "server_name") {
        return (tmp.size() == 1);
    }
    return true;
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
					std::cout << "keyword qui foire = " << keyword << std::endl;
					throw std::invalid_argument("Unauthorized argument in conf file");
				}
				server[keyword] = tmp;
			}
			std::getline(conf_file, line);
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


void    Config::fill_conf_vector(const std::string &filename) 
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
			/*
            for (it = server.begin(); it != server.end(); it++)
            {
                std::cout << "server key = " << it->first << std::endl;
                for (std::vector<std::string>::iterator vIt = it->second.begin(); vIt != it->second.end(); vIt++)
                    std::cout << "string for " << it->first << " = " << *vIt << "\n";
            }
	    */
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


std::vector<std::string>	string_to_vector(const std::string &string, const char delimiter, size_t space_pos)
{
	std::vector<std::string>	result;

	if (space_pos >= string.size())
		return (result);

	size_t	start = space_pos;

	if (string[start] == delimiter)
		start++;

	size_t	end = string.find(delimiter, start);

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
