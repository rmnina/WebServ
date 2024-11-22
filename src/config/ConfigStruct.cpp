/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStruct.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 01:42:05 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:20:46 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ConfigStruct.hpp"

ConfigStruct::ConfigStruct(const server_data &map) : ContainerType(SERVER_VECTOR), serverData(map) {}

ConfigStruct::ConfigStruct(const location_data &vector) : ContainerType(LOCATION_MAP), locationData(vector) {}

ConfigStruct::type  ConfigStruct::get_container_type() const { return (ContainerType); }

const server_data    &ConfigStruct::get_server_data() const { return (serverData); }

const std::vector<std::string>	&ConfigStruct::get_server_value(const std::string &key) const
{
	if (ContainerType != SERVER_VECTOR)
		throw std::runtime_error("no serverData");
	server_data::const_iterator it = serverData.find(key);
	if (it != serverData.end())
		return (it->second);
	throw std::out_of_range("key not found in server");
}

const std::map<std::string, std::vector<std::string> >	&ConfigStruct::get_location_data(size_t index) const
{
	if (ContainerType != LOCATION_MAP)
		throw std::runtime_error("no locationData");
	if (index < locationData.size())
		return (locationData[index]);
	throw std::out_of_range("index out of bound in location");
}

const std::vector<std::string>	&ConfigStruct::get_location_value(size_t index, const std::string &key) const
{
	const std::map<std::string, std::vector<std::string> > locationMap = get_location_data(index);

	std::map<std::string, std::vector<std::string> >::const_iterator it = locationMap.find(key);
	if (it != locationMap.end())
		return (it->second);
	throw std::out_of_range("key not found in location");
}