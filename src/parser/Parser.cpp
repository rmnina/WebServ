/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:12 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/23 23:43:30 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

Parser::Parser( void) {}

Parser::Parser( Server *server) : _server(server) {}

// Parser::Parser( const Parser &src) {}

// Parser	&Parser::operator=( const Parser &rhs) {}

bool	Parser::fill_method( const std::string &request)
{
	size_t						method_end = request.find(" ");
	std::string 				method = request.substr(0, method_end);
	
	std::string					methods[3] = {"GET", "POST", "DELETE"};
	std::vector<std::string>	tmp;

	for (long unsigned int i = 0; i < methods->size(); i++)
	{
		if (request.find(methods[i]))
		{
			tmp.push_back(methods[i]);
			_request["method"] = tmp;
		}
	}
	if (_request.find("method") == _request.end())
	{
		std::cout << "Error : invalid method" << std::endl;
		return (false);
	}
	else
	{ 
		std::vector<std::string> allowed_method = _server->getConfig()->get_server_value("method");
		if (allowed_method.empty())
			return (true);
		else
		{
			for (std::vector<std::string>::iterator it = allowed_method.begin(); it < allowed_method.end(); it++)
			{
				if (_request["method"][1] == *it)
					return (true); 
			}
			return (false);
		}
	}
}

bool	Parser::fill_path( const std::string &request)
{
	size_t						path_begin = request.find("/");
	size_t						path_end = request.find(" ", path_begin);
	std::string 				path = request.substr(path_begin, path_end - path_begin);

	if (!path.compare("/"))
		path = "index.html";

	std::vector<std::string>	tmp;
	tmp.push_back("www/" + path);
	_request["path"] = tmp;
	
	std::ifstream	resource(path.c_str());
	if (!resource.is_open())
	{
		std::cerr << "Couldnt open resource located at " << path << std::endl;
		return (false);
	}
	resource.close();
	return (true);
}

bool	Parser::fill_content_type( const std::string &request)
{
	size_t	accept_begin = request.find("accept :");
	if (accept_begin == std::string::npos)
		accept_begin = request.find("Accept :");
	size_t	accept_end = request.find(";", accept_begin);

	std::string	tmp = request.substr(accept_begin, accept_end - accept_begin);
	std::vector<std::string> accept = string_to_vector(tmp, ',', 0);
	_request["accept"] = accept;
	return (true);
}

bool	Parser::check_version( const std::string &request)
{
	if (request.find("HTTP/1.1") == std::string::npos)
	{
		std::cout << "Error : wrong HTTP version" << std::endl;
		return (false);
	}
	return (true);		
}

bool	Parser::check_req_size( const std::string &request)
{
	if (request.size() > MAX_REQ_SIZE)
		return (false);
	return (true);
}

std::string Parser::handle_request( void)
{	
	std::cout << ORANGE << _server->getRequest() << RESET << std::endl;
	
	if (!fill_path(_server->getRequest()))
		_error_code = 404; //ERROR PAGE RESOURCE NOT FOUND
	if (!fill_method(_server->getRequest()))
		_error_code = 405; //ERROR PAGE METHOD NOT ALLOWED
	if (!check_version(_server->getRequest()) || !check_req_size(_server->getRequest()))
		_error_code = 400; //ERROR BAD REQUEST
	else
		_error_code = 200;
	build_response();
	return (_response);
}

Parser::~Parser( void) {}