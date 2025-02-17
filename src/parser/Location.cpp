/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 21:48:10 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/17 01:23:19 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

location_i_data	Parser::find_location( const std::string &path)
{
	location_i_data	empty;
	(void)path;
	
	for (location_data::iterator it = _location.begin(); it < _location.end(); it++)
	{
		if ((*it).find("route") != (*it).end())
		{
			std::string location_path = (*it).find("route")->second[0];
			location_path.erase(0, 1);
			if (!location_path.compare("/"))
				location_path = "/index.html";
			location_path = "www" + location_path;
			return (*it);
		}
	}
	return (empty);
}

void	Parser::get_location( const std::string &path)
{
	location_i_data	location = find_location(path);
	std::string	cmp;

	if (location.empty())
		return;
	if (location.find("route") != location.end())
		cmp = "www/" + location.find("route")->second[0];
	for (location_i_data::iterator it = location.begin(); it != location.end(); it++)
	{
		if (it->first == "redirect")
		{
			if (cmp == path)
			{
				std::string	tmp = "www/" + it->second[0];
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