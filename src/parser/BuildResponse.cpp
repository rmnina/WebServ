/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BuildResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 21:15:07 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/28 23:28:28 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

void  Parser::init_mime_types( void)
{
	mime_types[".html"] = "text/html";
	mime_types[".htm"] = "text/html";
	mime_types[".css"] = "text/css";
	mime_types[".scss"] = "text/x-scss";
	mime_types[".woff"] = "font/woff";
	mime_types[".woff2"] = "font/woff2";
	mime_types[".ttf"] = "font/ttf";
	mime_types[".js"] = "application/javascript";
	mime_types[".json"] = "application/json";
	mime_types[".py"] = "application/py";
	mime_types[".jpg"] = "image/jpeg";
	mime_types[".jpeg"] = "image/jpeg";
	mime_types[".png"] = "image/png";
	mime_types[".gif"] = "image/gif";
	mime_types[".ico"] = "image/x-icon";
	mime_types[".svg"] = "image/svg+xml";
	mime_types[".txt"] = "text/plain";
	mime_types[".cgi"] = "application/x-httpd-cgi";
	mime_types[".xml"] = "application/xml";
	mime_types[".pdf"] = "application/pdf";
}

std::string get_time(void)
{
	std::time_t	now = std::time(NULL);
	std::tm*	gmt = std::gmtime(&now);

	const char*	days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	const char*	months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
							"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	std::ostringstream	oss;
	oss << days[gmt->tm_wday] << ", " << std::setw(2) << std::setfill('0') << gmt->tm_mday
		<< " " << months[gmt->tm_mon] << " " << (gmt->tm_year + 1900) << " "
		<< std::setw(2) << std::setfill('0') << gmt->tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << gmt->tm_min << ":"
		<< std::setw(2) << std::setfill('0') << gmt->tm_sec << " GMT";

	return oss.str();
}

std::string	Parser::get_content_type( const std::string &filename)
{	
	std::size_t	dot = filename.rfind('.');
	if (dot != std::string::npos)
		_extension = filename.substr(dot);
	else 
		_extension = "";
	get_content_category();
	std::map<std::string, std::string>::const_iterator it = mime_types.find(_extension);
	if (it != mime_types.end())
		return (it->second);
	return ("application/octet-stream");
}

void	Parser::get_content_category( void)
{
	std::string		raw_text_files[11] = {".html", ".css", ".scss", ".js", ".txt", ".xml", ".json", ".csv",
										".woff", ".woff2", ".ttf"};
	std::string		image_files[6] = {".jpg", ".jpeg", ".png", ".gif", ".ico", ".svg"};
	std::string		cgi[4] = {".py", ".sh", ".php", ".cgi"};

	for (long unsigned int i = 0; i <= sizeof(raw_text_files) / sizeof(std::string); i++)
	{
		if (raw_text_files[i] == _extension)
		{
			_category = "TEXT";
			return;
		}
	}
	for (long unsigned int i = 0; i <= sizeof(image_files) / sizeof(std::string); i++)
	{
		if (image_files[i] == _extension)
		{
			_category = "IMAGE";
			return;
		}
	}
	for (long unsigned int i = 0; i <= sizeof(cgi) / sizeof(std::string); i++)
	{
		if (cgi[i] == _extension)
		{
			_category = "CGI";
			return;
		}
	}
}

size_t	Parser::get_content_length( const std::string &filename)
{
	std::ifstream	content(filename.c_str(), std::ios::binary);

	if (!content)
		std::cerr << "File couldnt be opened : " << filename << std::endl;
	content.seekg(0, std::ios::end);
	_resp_size = content.tellg();
	content.seekg(0, std::ios::beg);

	content.close();
	return (_resp_size);
}

std::string	Parser::build_response_header( void)
{
	std::ostringstream	header;
	header << "HTTP/1.1 " <<_error_code;
	if (_error_code == 200 || _error_code == 201 || _error_code == 204)
		header <<" OK\r\n";
	header << "Date: " << get_time() << "\r\n";
	if (_category == "IMAGE")
		header << "Accept-Ranges: bytes\r\n";
	header << "Content-Type: " << get_content_type(_request["path"][0]) << "\r\n";
	get_content_length(_request["path"][0]);
	// header << "Content-Length: " << get_content_length(_request["path"][0]) << "\r\n";
	header << "Connection: keep-alive\r\n";
	header << "Transfer-Encoding: chunked\r\n";
	header << "Server: WebServ\r\n\r\n";
	
	_response += header.str();
	return (_response);
}

void	Parser::build_response_content( std::string &filename)
{
	std::string		line;
	std::string		content;
	std::ifstream	file(filename.c_str());

	if (!file.is_open())
		std::cerr << "Couldnt open file " << std::endl;
	while (std::getline(file, line))
		content += line + "\n";
	_response += content;
	file.close();
}

void	Parser::exec_cgi( std::string &filename)
{
	(void)filename;
}

void	Parser::GETmethod( void)
{	
	std::string	path = _request["path"][0];

	if (!_category.compare("TEXT") || !_category.compare("IMAGE"))
		build_response_content(path);
	else if (!_category.compare("CGI"))
		exec_cgi(path);
	else
		std::cerr << BOLD RED << "Error getting category : " << _extension << RESET << std::endl;
}

void	Parser::POSTmethod( void)
{
	
}

void	Parser::DELETEmethod( void)
{
	
}

std::string	Parser::build_response( void)
{		
	std::string	method[3] = { "GET", "POST", "DELETE" };
	_response.erase();
	
	get_location(_request["path"][0]);
	void (Parser::*func_method[])(void) = { &Parser::GETmethod, &Parser::POSTmethod, &Parser::DELETEmethod };

	for (long unsigned int i = 0; i < method->size(); i++)
	{
		if (_request.find("method")->second[0] == method[i])
		{
			(this->*func_method[i])();
			return (_response);
		}
	}
	return ("");
}

