/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:20:49 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/09 19:45:53 by eltouma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

void	Parser::GETmethod( void)
{	
	std::string	path = _request["path"][0];
	std::cout << "path before = " << path << std::endl;
	if (path.substr(0, 2) != "./")
		path = "./" + path;
	if (_server_conf.find("dir_listing") != _server_conf.end() &&_server_conf["dir_listing"][0] == "on")
	{
		std::cout << RED << "on a bien trouve le dir_listing" << RESET << std::endl;
		if (path == "./www/index.html")
			display_dirlist("./www");
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
		std::cerr << BOLD RED << "Error getting category : " << _extension << RESET << std::endl;
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
	std::cout << __func__ << "\tpath = " << path << std::endl;
}

void	Parser::upload(void)
{
	if (!fill_content_type_multipart(_request_body))
	{
		_error_code = 400;
		std::cerr << "Invalid upload request headers" << "\nError code: " << _error_code << std::endl;
		return;
	}

	std::string			filename;
	std::vector<char>	content;
	
	if (!get_file_name(_request_body, filename) ||
		!get_file_content(_request_body, content))
	{
		_error_code = 400;
		std::cerr << "Could not extract file from request" << "\nError code: " << _error_code << std::endl;
		return;
	}

	if (_server_conf.find("upload") != _server_conf.end() && 
		_server_conf["upload"][0] == "on")
		_upload_dir = "www/" + _server_conf["upload"][1];
	else
	{
		std::cerr << "Upload not configured" << std::endl;
		_error_code = 500;
	}

	struct stat info;
	if (stat(_upload_dir.c_str(), &info) != 0)
	{
		if (mkdir(_upload_dir.c_str(), 0755) != 0)
		{
			std::cerr << "Error creating directory" << std::endl;
			_error_code = 500;
		}
	}
	std::string	filepath = _upload_dir + "/" + filename;
	std::cout << BOLD YELLOW << "filepath is " << filepath << RESET << std::endl;
	std::ofstream	file(filepath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Cannot create file" << std::endl;
		_error_code = 500;
	}
	file.write(content.data(), content.size());
	file.close();
	_error_code = 201;
}

void	Parser::DELETEmethod(void)
{
	std::string path = _request["path"][0];
	size_t pos = path.find("/delete");
	if (pos != std::string::npos)
		path = "www" + path.substr(pos + 7);
	int isFound = path.find(_upload_dir) != std::string::npos;
	if (!isFound)
	{
		_error_code = 403;
		std::cerr << "Remove " << path.c_str() + 4 << ": Forbidden" << RESET << "\nError code: " << _error_code << std::endl;
		return ;
	}
	int status = remove(path.c_str());
	if (!status)
		std::cout << path.substr(13) << ": successfully deleted" << "\nCode: " << _error_code << std::endl;
	else
		std::cerr << path.substr(13) << ": " << strerror(errno) << std::endl;
}
