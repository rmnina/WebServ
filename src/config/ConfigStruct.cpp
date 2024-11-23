/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStruct.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 01:42:05 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/23 19:52:05 by jdufour          ###   ########.fr       */
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
	static const std::vector<std::string> empty;
	if (it != serverData.end())
		return (it->second);
	return (empty);
}

const server_data	&ConfigStruct::get_location_data(size_t index) const
{
	if (ContainerType != LOCATION_MAP)
		throw std::runtime_error("no locationData");
	if (index < locationData.size())
		return (locationData[index]);
	throw std::out_of_range("index out of bound in location");
}

const std::vector<std::string>	&ConfigStruct::get_location_value(size_t index, const std::string &key) const
{
	const location_i_data locationMap = get_location_data(index);

	location_i_data::const_iterator it = locationMap.find(key);
	static const std::vector<std::string> empty;
	if (it != locationMap.end())
		return (it->second);
	return (empty);
}