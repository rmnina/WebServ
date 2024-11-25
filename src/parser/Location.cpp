/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 21:48:10 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/25 23:45:41 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

location_i_data	find_location( location_data &locations, std::string &path)
{
	location_i_data	empty;
	for (location_data::iterator it = locations.begin(); it < locations.end(); it++)
	{
		if ((*it).find("route") != (*it).end())
		{
			std::string location_path = (*it).find("route")->second[0];
			if (location_path == path)
				return (*it);
		}
	}
	return (empty);
}


// std::string	Parser::get_location( const std::string &path)
// {
// 	location_data	location_tab = _server->getLocation();
	
// 	location_i_data	location = find_location(location_tab, _request["path"][0]);
// 	if (location.empty())
// 		return ("");
// 	const std::string	array[8] = {"dir_listing", "error", "method", "upload", 
// 									"root", "default_file", "cgi", "redirect"};
	
// }