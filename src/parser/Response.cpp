/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 21:15:07 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/24 00:06:41 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"

void  Parser::init_mime_types( void)
{
	mime_types[".html"] = "text/html";
	mime_types[".htm"] = "text/html";
	mime_types[".css"] = "text/css";
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
	std::map<std::string, std::string>::const_iterator	it = mime_types.find(_extension);
	if (it != mime_types.end())
		return (it->second);
	return ("application/octet-stream");
}

size_t	Parser::get_content_length( const std::string &filename)
{
	std::ifstream	content(filename.c_str(), std::ios::binary | std::ios::ate);
	if (!content)
		std::cerr << "File couldnt be opened : " << filename << std::endl;
	return (content.close(), content.tellg());
}

void	Parser::build_response_header( void)
{
	std::ostringstream	header;
	header << "HTTP/1.1 " <<_error_code;
	if (_error_code == 200 || _error_code == 201 || _error_code == 204)
		header <<" OK ";
	header << "\ndate: " << get_time();
	header << "\ncontent_type: " << get_content_type(_request["path"][0]);
	header << "\ncontent-length: " << get_content_length(_request["path"][0]);
	header << "\nserver: the best Webserv you will see\r\n\r\n";
	
	_response += header.str();
}

void	Parser::build_raw_text( std::ifstream &resource)
{
	std::string	line;
	std::string	content;
	
	while (std::getline(resource, line))
		content += line + "\n";
	
	build_response_header();
	_response += content;
	resource.close();
}

void	Parser::build_image( std::ifstream &resource)
{
	std::streampos		size = get_content_length(_request["path"][0]);
	std::vector<char>	buffer(size);

	resource.read(&buffer[0], size);
	
	std::string	content(buffer.begin(), buffer.end());
	resource.close();
}

void	Parser::exec_cgi( std::ifstream &resource)
{
	(void)resource;
}

void	Parser::GETmethod( void)
{	
	std::string	path = _request["path"][0];
	std::ifstream	resource(path.c_str());
	std::string		raw_text_files[7] = {".html", ".css", ".js", ".txt", ".xml", ".json", ".csv"};
	std::string		image_files[5] = {".jpg", ".jpeg", ".png", ".gif", ".ico"};
	std::string		cgi[4] = {".py", ".sh", ".php", ".cgi"};

	get_content_type(path);
	if (!resource.is_open())
		std::cerr << "Ressource " << path << "could not be opened" << std::endl;
	for (long unsigned int i = 0; i < raw_text_files->size(); i++)
	{
		if (raw_text_files[i] == _extension)
		{
			build_raw_text(resource);
			return;
		}
	}
	for (long unsigned int i = 0; i < image_files->size(); i++)
	{
		if (image_files[i] == _extension)
		{
			build_image(resource);
			return;
		}
	}
	for (long unsigned int i = 0; i < cgi->size(); i++)
	{
		if (cgi[i] == _extension)
		{
			exec_cgi(resource);
			return;
		}
	}
}

void	Parser::POSTmethod( void)
{
	
}

void	Parser::DELETEmethod( void)
{
	
}

void	Parser::build_response( void)
{	
	std::string	method[3] = { "GET", "POST", "DELETE" };
	void (Parser::*func_method[])(void) = { &Parser::GETmethod, &Parser::POSTmethod, &Parser::DELETEmethod };

	for (long unsigned int i = 0; i < method->size(); i++)
	{
		if (_request.find("method")->second[0] == method[i])
		{
			(this->*func_method[i])();
			return;
		}
	}
}

