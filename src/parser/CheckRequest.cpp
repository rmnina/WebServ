/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:12 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/29 02:38:00 by jdufour          ###   ########.fr       */
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
	
	for (long unsigned int i = 0; i < sizeof(methods) / sizeof(std::string); i++)
	{
		if (!method.compare(methods[i]))
		{
			tmp.push_back(method);
			_request["method"] = tmp;
		}
	}
	if (_request.find("method") == _request.end())
	{
		std::cerr << "Error : invalid method" << std::endl;
		return (false);
	}
	else
	{ 
		std::vector<std::string>	allowed_method;
		server_data::iterator		pos = _server_conf.find("method");

		if (pos == _server_conf.end())
			return (true);
		allowed_method = pos->second;
		for (std::vector<std::string>::iterator it = allowed_method.begin(); it < allowed_method.end(); it++)
		{
			if (_request["method"][0] == *it)
				return (true); 
		}
		return (false);
	}
}

bool	Parser::fill_path( const std::string &request)
{
	size_t						path_begin = request.find("/");
	size_t						path_end = request.find(" ", path_begin);
	std::string 				path = request.substr(path_begin, path_end - path_begin);

	if (!path.compare("/"))
		path = "/index.html";
	else if (!path.compare("/delete"))
		path = "/delete.html";
	path = "www" + path;
	
	std::vector<std::string>	tmp;
	tmp.push_back(path);
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
		std::cerr << "Error : wrong HTTP version" << std::endl;
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

void	Parser::examine_request( int client_index)
{	
	_server_conf = _server->getConfig();
	_location = _server->getLocation();
	// _request_body = _server->getReqBody()[client_index];
	
	std::string	request = _server->getRequest()[client_index];
	if (request.empty())
		return ;

	init_mime_types();
	if (!fill_path(request))
		_error_code = 404; //ERROR PAGE RESOURCE NOT FOUND
	if (!fill_method(request))
		_error_code = 405; //ERROR PAGE METHOD NOT ALLOWED
	if (!check_version(request) || !check_req_size(request))
		_error_code = 400; //ERROR BAD REQUEST
	else
		_error_code = 200;
	throw_error_page();
}

std::vector<unsigned char>							Parser::getImageResponse( void) const { return (_image_response); }
std::map<std::string, std::vector<std::string> >	Parser::getRequest( void) const { return (_request); }
std::string											Parser::getCategory( void) const { return (_category); }
size_t												Parser::getRespSize( void) const { return (_resp_size); }
int													Parser::getErrorCode( void) const { return (_error_code); }

Parser::~Parser( void) {}
