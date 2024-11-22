/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStruct.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 01:42:14 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:57:53 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSTRUCT_HPP
# define CONFIGSTRUCT_HPP

# include <map>
# include <vector>
# include <string>
# include <exception>
# include <iostream>

typedef std::map<std::string, std::vector<std::string> >				server_data;
typedef	server_data 													location_i_data;
typedef std::vector<std::map<std::string, std::vector<std::string> > >	location_data;


struct ConfigStruct
{
	enum type
	{
		SERVER_VECTOR,
		LOCATION_MAP
	} ContainerType;

	server_data 	serverData;
	location_data	locationData;

	ConfigStruct(const server_data &map);
	ConfigStruct(const location_data &vector);

	type 				get_container_type() const;
	const server_data	&get_server_data() const;

	const std::vector<std::string>							&get_server_value(const std::string &key) const;
	const std::map<std::string, std::vector<std::string> >	&get_location_data(size_t index) const;
	const std::vector<std::string>							&get_location_value(size_t index, const std::string &key) const;
};

#endif