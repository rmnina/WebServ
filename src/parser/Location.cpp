/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 21:48:10 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/29 02:43:11 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

location_i_data	Parser::find_location( const std::string &path)
{
	location_i_data	empty;
	
	for (location_data::iterator it = _location.begin(); it < _location.end(); it++)
	{
		if ((*it).find("route") != (*it).end())
		{
			std::string location_path = (*it).find("route")->second[0];
			location_path.erase(0, 1);
			if (!location_path.compare("/"))
				location_path = "/index.html";
			location_path = "www" + location_path;
			if (location_path == path)
				return (*it);
		}
	}
	return (empty);
}

void	Parser::get_location( const std::string &path)
{
	location_i_data	location = find_location(path);

	if (location.empty())
		return;

	for (location_i_data::iterator it = location.begin(); it != location.end(); it++)
	{
		if (it->first == "redirect")
		{
			_request["path"] = it->second;
			_error_code = 301;
		}
		if (it->first == "method")
			_server_conf["method"] = it->second;
		else
			_server_conf[it->first] = it->second;
	}
}