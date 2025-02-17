/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:20:49 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/17 15:51:21 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

void	Parser::GETmethod( void)
{	
	std::string	path = _request["path"][0];
	if (path.substr(0, 2) != "./")
		path = "./" + path;
	if (_server_conf.find("dir_listing") != _server_conf.end() &&_server_conf["dir_listing"][0] == "on")
	{
		if (path == "./" + _server_conf["root"][0] + "/" + "index.html")
			display_dirlist("./" + _server_conf["root"][0]);
		else
		{
			if (is_directory(path))
				display_dirlist(path);
			else if (is_file(path))
				build_response_content(path);
		}
	}
	else if (!_category.compare("TEXT") || !_category.compare("IMAGE"))
		build_response_content(path);
	else if (!_category.compare("CGI"))
		exec_cgi(path, GET);
	else
		print_log(CERR, RED, "Error", _server->getName(), "Error getting category for ", _extension);
}

void	Parser::POSTmethod( void)
{
	std::string	path = _request["path"][0];

	if (_request_body.find("filename=") != std::string::npos)
	{
		upload();
		build_response_upload();
	}
	else if (!_category.compare("CGI"))
	{
		exec_cgi(path, POST);
	}
	else
		build_response_content(path);
}

void	Parser::upload(void)
{	
	if (!fill_content_type_multipart(_request_body))
	{
		_error_code = 400;
		print_log(CERR, RED, "Error", _server->getName(), "Invalid upload request headers", " ");
		return;
	}

	if (!_request["File_Type"][0].compare("text/plain"))
	{
		_error_code = 400;
		print_log(CERR, RED, "Error", _server->getName(), "Invalid file. Server only accepts text/plain", " ");
		return;
	}
	
	std::string			filename;
	std::vector<char>	content;
	
	content.push_back('\0');
	if (!get_file_name(_request_body, filename) ||
		!get_file_content(_request_body, content))
	{
		_error_code = 400;
		print_log(CERR, RED, "Error", _server->getName(), "Could not exctact file from request :", filename);
		return;
	}

	if (_server_conf.find("upload") != _server_conf.end() && 
		_server_conf["upload"][0] == "on")
		_upload_dir = _server_conf["root"][0] + "/" + _server_conf["upload"][1];
	else
	{
		print_log(CERR, RED, "Error", _server->getName(), "Upload not configured", " ");
		_error_code = 500;
	}

	struct stat info;
	if (stat(_upload_dir.c_str(), &info) != 0)
	{
		if (mkdir(_upload_dir.c_str(), 0755) != 0)
		{
			print_log(CERR, RED, "Error", _server->getName(), "Could not create directory :", _upload_dir);
			_error_code = 500;
		}
	}
	std::string	filepath = _upload_dir + "/" + filename;
	std::ofstream	file(filepath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		print_log(CERR, RED, "Error", _server->getName(), "Cannot create file :", filepath.c_str() + 4);
		_error_code = 500;
	}
	file.write(content.data(), content.size());
	file.close();
	print_log(COUT, YELLOW, "Log", _server->getName(), "File successfuly uploaded to server : ", filename);
	_error_code = 201;
}

void	Parser::DELETEmethod(void)
{
	std::string path = _request["path"][0];
	size_t pos = path.find("/delete");
	if (pos != std::string::npos)
		path = _server_conf["root"][0] + path.substr(pos + 7);
	int isFound = path.find(_upload_dir) != std::string::npos;
	if (!isFound)
	{
		_error_code = 403;
		print_log(CERR, RED, "Error", _server->getName(), "Delete is forbidden on path ", path.c_str() + 4);
		return ;
	}
	int status = remove(path.c_str());
	if (!status)
		print_log(COUT, MAGENTA, "Log", _server->getName(), "File successfully deleted : ", path.substr(13));
	else
		print_log(CERR, RED, "Error", _server->getName(), strerror(errno), path.substr(13));
}
