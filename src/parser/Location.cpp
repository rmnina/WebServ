/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 21:48:10 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/20 17:02:11 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

location_i_data	Parser::find_location( const std::string &path, location_data::iterator &it)
{
	location_i_data	empty;
	(void)path;
	
	for (; it < _location.end(); it++)
	{
		if ((*it).find("route") != (*it).end())
		{
			std::string location_path = (*it).find("route")->second[0];
			location_path.erase(0, 1);
			if (!location_path.compare("/"))
				location_path = "/index.html";
			location_path = _server_conf["root"][0] + location_path;
			return (*it);
		}
	}
	return (empty);
}

void	Parser::get_location( const std::string &path)
{
	
	for (location_data::iterator it = _location.begin(); it < _location.end(); it++)
	{
		location_i_data	location = find_location(path, it);
		std::string	cmp;
		
		if (location.empty())
			return;
		if (location.find("route") != location.end())
			cmp = _server_conf["root"][0] + "/" + location.find("route")->second[0];
		if (cmp == path)
		{
			for (location_i_data::iterator it = location.begin(); it != location.end(); it++)
			{
				if (it->first == "redirect")
				{
					{
						std::string	tmp = _server_conf["root"][0] + "/" + it->second[0];
						std::vector<std::string>	tmp_vec;
					
						tmp_vec.push_back(tmp);
						_request["path"] = tmp_vec;
						_error_code = 301;
					}
				}
				if (it->first == "method")
					_server_conf["method"] = it->second;
				else
					_server_conf[it->first] = it->second;
			}
		}
	}
}