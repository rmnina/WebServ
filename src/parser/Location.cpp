/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahayon <ahayon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 21:48:10 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/21 15:51:13 by ahayon           ###   ########.fr       */
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
		_server_conf[it->first] = it->second;
}