/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:12 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/05 02:44:23 by jdufour          ###   ########.fr       */
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

bool	Parser::fill_content_type_multipart(const std::string &request)
{
	size_t	first_line_end = request.find("\r\n");
	if (first_line_end == std::string::npos)
		return (false);

	std::string	boundary = request.substr(0, first_line_end);
	
	std::vector<std::string>	tmp;
	tmp.push_back("multipart/form-data; boundary=" + boundary);
	_request["Content-Type"] = tmp;

	std::vector<std::string>	boundary_vec;
	boundary_vec.push_back(boundary);
	_request["boundary"] = boundary_vec;

	return (true);
}

bool	Parser::fill_content_length(const std::string &request)
{
	size_t	length_begin = request.find("Content-Length:");
	if (length_begin == std::string::npos)
		length_begin = request.find("content-length:");

	if (length_begin == std::string::npos)
	{
		std::cerr << "Error: no Content-Length header found" << std::endl;
		return (false);
	}
	
	size_t	length_end = request.find("\r\n", length_begin);
	if (length_end == std::string::npos)
	{
		std::cerr << "Error: malformed Content-Length header" << std::endl;
		return (false);
	}
	std::string	length_str = request.substr(length_begin + 15, 
		length_end - (length_begin + 15));
	while (length_str[0] == ' ')
		length_str = length_str.substr(1);
	std::vector<std::string> tmp;
	tmp.push_back(length_str);
	_request["Content-Length"] = tmp;
	return (true);
}

bool	Parser::get_file_name(const std::string &body, std::string &filename)
{
	size_t	filename_pos = body.find("filename=\"");
	if (filename_pos == std::string::npos)
	{
		std::cerr << "Error: no filename in request" << std::endl;
		return (false);
	}

	size_t	filename_end = body.find("\"", filename_pos + 10);
	if (filename_end == std::string::npos)
	{
		std::cerr << "Error: malformed filename" << std::endl;
		return (false);
	}

	filename = body.substr(filename_pos + 10, filename_end - (filename_pos + 10));
	
	if (filename.find('/') != std::string::npos || 
		filename.find("..") != std::string::npos)
	{
		std::cerr << "Error: invalid filename" << std::endl;
		return (false);
	}
	return (true);
}

bool	Parser::get_file_content(const std::string &body, std::vector<char> &content)
{
	size_t	MAX_FILE_SIZE = 3000;

	std::cout << BLUE BOLD << "BODY LENGTH IS " << body.length() << RESET << std::endl;
	
	if (body.length() > MAX_FILE_SIZE)
	{
		std::cerr << "File too heavy (> 10Mo)" << std::endl;
		return (false);
	}

	if (_request.find("boundary") == _request.end())
	{
		std::cout << GREEN BOLD << "boundary" << RESET << std::endl;
		return (false);
	}

	std::string	boundary = _request["boundary"][0];

	size_t	content_begin = body.find("\r\n\r\n");
	if (content_begin == std::string::npos)
	{
		std::cout << GREEN BOLD << "no begin" << RESET << std::endl;
		return (false);
	}
	content_begin += 4;

	size_t	content_end = body.find(boundary, content_begin);
	if (content_end == std::string::npos)
		content_end = body.length();
	else
		content_end -= 2;

	content.assign(body.begin() + content_begin, body.begin() + content_end);
	std::cout << BOLD BLUE << "content is";
	for (std::vector<char>::iterator it = content.begin(); it < content.end(); it++)
	{
		std::cout << *it;
	}
	std::cout << RESET << std::endl;
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
	_request_body = _server->getReqBody()[client_index];
	_keep_alive = _server->getConnectionStatus()[client_index];
	
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
