/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 17:03:57 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/06 18:20:33 by eltouma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

int Parser::build_error_page( void)
{	
	std::ostringstream os;
	os << _error_code;
	std::string	code_str = os.str();
	
	int	code[8] = {400, 403, 404, 405, 406, 410, 504, 418};
	std::string	message[8] = {"Bad request syntax", "Forbidden", "Resource not found",
							"Method not allowed", "Format not acceptable", "Resource gone", "Gateway Timeout", "I'm a teapot"};
	
	std::ifstream   error_file("www/error.html");
	if (!error_file.is_open())
		std::cerr << "Error opening error file" << std::endl;
	
	std::ostringstream	tmp;
	tmp << error_file.rdbuf();
	std::string	content = tmp.str();
	_error_page = tmp.str();
	error_file.close();

	size_t	pos = 0;
	std::ofstream	output_file("www/error.html");
	std::string		image("images/" + code_str + ".jpg");
	if (!output_file.is_open())
		std::cerr << "Error opening error file in writing mode" << std::endl;
	if ((pos = content.find("{{ERROR_IMG}}")) != std::string::npos)
		content.replace(pos, strlen("{{ERROR_IMG}}"), image);
	if ((pos = content.find("{{ERROR_CODE}}")) != std::string::npos)
		content.replace(pos, strlen("{{ERROR_CODE}}"), code_str);
	if ((pos = content.find("{{ERROR_MESSAGE}}")) != std::string::npos)
	{
		for (long unsigned int i = 0; i < sizeof(code) / sizeof(int); i++)
		{
			if (_error_code == code[i])
				content.replace(pos, strlen("{{ERROR_MESSAGE}}"), message[i]);
		}
	}
	output_file << content;
	output_file.close();
	return (SUCCESS);
}

int	Parser::throw_error_page( void)
{
	int	error_code[6] = {400, 403, 404, 405, 406, 410};

	for (long unsigned int i = 0; i < sizeof(error_code) / sizeof(int); i++)
	{
		if (_error_code == error_code[i])
		{
			build_error_page();
			_request["path"][0] = "www/error.html";
			return (FAILURE);
		}
	}
	return (SUCCESS);
}

int	Parser::restore_error_page( void)
{
	std::ofstream	output_file("www/error.html");

	if (!output_file.is_open())
		std::cerr << "Error opening error file in writing mode" << std::endl;
	output_file << _error_page;
	
	output_file.close();
	return (SUCCESS);
}
