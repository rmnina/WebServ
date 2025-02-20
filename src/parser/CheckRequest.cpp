/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:49:12 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/20 16:03:57 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"
#include "../../include/config/Config.hpp"
#include <cstring>

Parser::Parser( void) {}

Parser::Parser( Server *server) : _server(server) {}

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
		print_log(CERR, RED, "Error", _server->getName(), "Bad method. Use GET, POST or DELETE.", ' ');
		return (false);
	}
	else
	{ 
		std::vector<std::string>	allowed_method;
		server_data::iterator		pos = _server_conf.find("method");

		if (pos == _server_conf.end())
			return (true);
		allowed_method = pos->second;
		for (std::vector<std::string>::iterator it = allowed_method.begin(); it < allowed_method.end(); ++it)
			std::cout << BOLD YELLOW << "METHOD " << *it << RESET << std::endl;
		for (std::vector<std::string>::iterator it = allowed_method.begin(); it < allowed_method.end(); it++)
		{
			if (_request["method"][0] == *it)
				return (true); 
		}
		print_log(CERR, RED, "Error", _server->getName(), "Unallowed method : ", _request["method"][0]);
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
	path = _server_conf["root"][0] + path;
	
	std::vector<std::string>	tmp;
	tmp.push_back(path);
	_request["path"] = tmp;
	
	std::ifstream	resource(path.c_str());
	if (!resource.is_open())
	{
		print_log(CERR, RED, "Error", _server->getName(), "Could not open ressource located at ", path);
		return (false);
	}
	resource.close();
	get_location(_request["path"][0]);
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

	size_t	filetype_pos = request.find("Content-Type: ");
	if (filetype_pos == std::string::npos)
		return (false);
	
	size_t	filetype_end = request.find("\r\n", filetype_pos);
	std::string	file_type = request.substr(filetype_pos + 14, filetype_end);
	
	std::vector<std::string>	file_type_vec;
	file_type_vec.push_back(file_type);
	_request["File_Type"] = file_type_vec;

	return (true);
}

bool	Parser::fill_content_length(const std::string &request)
{
	size_t	length_begin = request.find("Content-Length:");
	if (length_begin == std::string::npos)
		length_begin = request.find("content-length:");

	if (length_begin == std::string::npos)
	{
		print_log(CERR, RED, "Error", _server->getName(), "no Content-Length header found", ' ');
		return (false);
	}
	
	size_t	length_end = request.find("\r\n", length_begin);
	if (length_end == std::string::npos)
	{
		print_log(CERR, RED, "Error", _server->getName(), "malformed Content-Length header", ' ');
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

static bool	removeSpaces(std::string filename)
{
	for (size_t i = 0; i < filename.length(); i++)
		if (filename[i] == ' ')
			return (false);
	return (true);
}

bool	Parser::get_file_name(const std::string &body, std::string &filename)
{
	size_t	filename_pos = body.find("filename=\"");
	if (filename_pos == std::string::npos)
	{
		print_log(CERR, RED, "Error", _server->getName(), "no filename in upload request", ' ');
		return (false);
	}

	size_t	filename_end = body.find("\"", filename_pos + 10);
	if (filename_end == std::string::npos)
	{
		print_log(CERR, RED, "Error", _server->getName(), "malformed filename in upload request", ' ');
		return (false);
	}

	filename = body.substr(filename_pos + 10, filename_end - (filename_pos + 10));
	
	if (filename.find('/') != std::string::npos || 
		filename.find("..") != std::string::npos)
	{
		print_log(CERR, RED, "Error", _server->getName(), "invalid filename in upload request", ' ');
		return (false);
	}
	if (!removeSpaces(filename))
		return (false);
	return (true);
}

size_t	ft_strlen(const char *str)
{
	int i = 0;

	for (; str && str[i]; i++);
	return (i);
}

bool	Parser::get_file_content(const std::string &body, std::vector<char> &content)
{
	size_t	MAX_FILE_SIZE = 3000;
	char	*body_binary = &_req_binary[1];

	std::stringstream ss;
	ss << MAX_FILE_SIZE;

	std::cout << BOLD YELLOW << body_binary << "\nsize is "<< strlen(body_binary) << RESET << std::endl;

	if (strlen(body.c_str()) > MAX_FILE_SIZE || strlen(body_binary) > MAX_FILE_SIZE)
	{
		std::string log = "File should contain under " + ss.str();
		log.append(" characters. Current : ");
		print_log(CERR, RED, "Error", _server->getName(), log.c_str(), strlen(body_binary));
		return (false);
	}
	if (_request.find("boundary") == _request.end())
	{
		print_log(CERR, RED, "Error", _server->getName(), "Boundary not find in body", " ");
		return (false);
	}
	std::string	boundary = _request["boundary"][0];
	
	char	*content_start = std::strstr(body_binary, "\r\n\r\n");
	if (!content_start)
	{
		print_log(CERR, RED, "Error", _server->getName(), "Invalid format in request body", " ");
		return (false);
	}
	content_start += 4;

	std::string end_boundary = boundary + "--";
	char	*content_end = std::strstr(content_start, end_boundary.c_str());
	
	if (!content_end)
		content_end = body_binary + strlen(body_binary);
	content.assign(content_start, content_end);

	return (true);
}

bool	Parser::check_version( const std::string &request)
{
	if (request.find("HTTP/1.1") == std::string::npos)
	{
		print_log(CERR, RED, "Error", _server->getName(), "wrong HTTP version. Expected : HTTP/1.1", ' ');
		return (false);
	}
	return (true);		
}

bool	Parser::check_req_size( const std::string &request)
{
	if (request.size() > MAX_REQ_SIZE)
	{
		print_log(CERR, RED, "Error", _server->getName(), "Request too big.", " ");
		return (false);
	}
	return (true);
}

bool	Parser::check_body_size( void)
{
	server_data::iterator	pos = _server_conf.find("body_size");
	if (pos == _server_conf.end())
		return (true);
	size_t	body_size = std::atoi(pos->second[0].c_str());
	
	std::ofstream	file("tmp", std::ios::binary);
	std::ostringstream tmp;
	
	std::string	content = tmp.str();
	file.write(content.c_str(), content.size());
	file.close();
	if (_request_body.size() > body_size)
	{
		print_log(CERR, RED, "Error", _server->getName(), "Request body too big.", " ");
		return (false);
	}
	return (true);
}

std::string trim_request(std::string request)
{
	size_t first_space = request.find(' ');
	if (first_space == std::string::npos)  
		return "";

	size_t second_space = request.find(' ', first_space + 1);
	if (second_space == std::string::npos)  
		return "";

	return request.substr(first_space + 1, second_space - first_space - 1);
}

void	Parser::examine_request( int client_index)
{	
	// get_location();
	bool	autoindex = false;
	
	_error_code = -1;
	_server_conf = _server->getConfig();
	_location = _server->getLocation();
	_request_body = _server->getReqBody()[client_index];
	_keep_alive = _server->getConnectionStatus()[client_index];
	_req_binary = _server->getReqBinary()[client_index];

	if (_server_conf.find("dir_listing") != _server_conf.end())
	{
		if (_server_conf["dir_listing"][0] == "on")
			autoindex = true;
	}
	
	std::string	request = _server->getRequest()[client_index];
	if (request.empty())
		return ;
	std::string trim_req = _server_conf["root"][0] + trim_request(request);
	DIR	*dir = opendir(trim_req.c_str());
	init_mime_types();
	if (!fill_path(request))
		_error_code = 404; //ERROR PAGE RESOURCE NOT FOUND
	else if (trim_req != _server_conf["root"][0] + "/" && dir != NULL && autoindex == false)
		_error_code = 418;
	else if (!fill_method(request))
		_error_code = 405; //ERROR PAGE METHOD NOT ALLOWED
	else if (!check_version(request)) 
		_error_code = 400; //ERROR BAD REQUEST
	else if (!check_req_size(request) || !check_body_size())
		_error_code = 413;
	else if (_error_code != 301)
		_error_code = 200;
	closedir(dir);
	throw_error_page();
}


/******----------------***** GETTERS *****----------------******/

std::vector<unsigned char>							Parser::getImageResponse( void) const { return (_image_response); }
std::map<std::string, std::vector<std::string> >	Parser::getRequest( void) const { return (_request); }
std::string											Parser::getCategory( void) const { return (_category); }
size_t												Parser::getRespSize( void) const { return (_resp_size); }
int													Parser::getErrorCode( void) const { return (_error_code); }

Parser::~Parser( void) {}
