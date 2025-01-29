/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BuildResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 21:15:07 by jdufour           #+#    #+#             */
/*   Updated: 2025/01/29 05:26:26 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"
#include <cstdio>

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
	if (_category == "CGI") {
		header << "Content-Type: text/html\r\n";
		header << "Content-Length: " << _body_size << "\r\n";
	}
	else {
		if (get_content_type(_request["path"][0]) == "application/x-httpd-cgi")
		{
			std::cout << "ca verifie le type httpd-cgi" << std::endl;
			header << "Content-Type: text/html\r\n";
		}
		else
			header << "Content-Type: " << get_content_type(_request["path"][0]) << "\r\n";
	}

	//get_content_length(_request["path"][0]);
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

void	Parser::exec_cgi(std::string &filename, int method)
{
// void	Parser::build_POST_response( std::string &filename)
// {
// 	std::string		line;
// 	std::string		content;
// 	std::ifstream	file(filename.c_str());

// 	if (!file.is_open())
// 		std::cerr << "Couldnt open file " << std::endl;
// 	while (std::getline(file, line))
// 		content += line + "\n";
// 	_response += content;
// 	file.close();
// }
	std::cout << "on rentre dans exec_cgi" << std::endl;
	std::cout << "filename est : " << filename << std::endl;
	pid_t pid;
	int pipefd[2];

	if (pipe(pipefd) == -1)
	{std::cerr << "Error creating a pipe\n"; return;}
	pid = fork();
    if (pid == -1) 

	{std::cerr << "Error forking process\n"; return;}

	if (pid == 0) 
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

    std::map<std::string, std::string> env;
		if (method == GET)
        	env["REQUEST_METHOD"] = "GET";
		else
			env["REQUEST_METHOD"] = "POST";
        env["SCRIPT_NAME"] = filename;
        // env["QUERY_STRING"] = _request["query"].empty() ? "" : _request["query"][0];
		// std::ostringstream oss;
		// if (_request["body"].empty())
		// 	oss << 0;
		// else
		// 	oss << _request["body"][0].size();
		// env["CONTENT_LENGTH"] = oss.str();

		char *envp[env.size() + 1];
		int i = 0;
		for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
			std::string env_var = it->first + "=" + it->second;
			envp[i] = strdup(env_var.c_str());
			i++;
		}
		envp[i] = NULL;
		char *argv[] = {strdup(filename.c_str()), NULL};
		execve(filename.c_str(), argv, envp);
		std::cerr << "Error executing CGI script: " << strerror(errno) << "\n";
		exit(1);
	} 

	else 
	{
		close(pipefd[1]);
		char buffer[1024];
		std::ostringstream cgi_output;
		int bytes_read;

		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) 
		{
			buffer[bytes_read] = '\0';
			cgi_output << buffer;
		}
		close(pipefd[0]);
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) 
		{
			int exit_code = WEXITSTATUS(status);
			if (exit_code != 0)
				std::cerr << "CGI script exited with error code: " << exit_code << "\n";
		}

		_body_size = (cgi_output.str()).size();
		//_response = build_response_header();
		// _response += "Content-Length: 100\n\r\n\r";

		_response += cgi_output.str();
		std::cout << "_response a la fin de exec_cgi = " << _response << std::endl;
		std::cout << "_response a la fin de exec_cgi = " << cgi_output.str() << std::endl;
	}
}

bool is_directory(const std::string &path)
{
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
		return false;
	return S_ISDIR(path_stat.st_mode);
}

bool is_file(const std::string &path)
{
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
		return false;
	return S_ISREG(path_stat.st_mode);
}

void	Parser::display_dirlist(std::string path)
{
	DIR *dir;
	struct dirent *entry;

	std::cout << "path dans le display dir = " << path << std::endl;
	if ((dir = opendir(path.c_str())) == NULL)
	{ throw std::runtime_error("Error opening directory for dir_list"); return ;}

	std::ostringstream html;
	html << "<html><head><title>Directory Listing</title></head><body>";
	html << "<h1>Index of " << path << "</h1><ul>";
	std::cout << "on est dans display dirlist\n";
	while ((entry = readdir(dir)) != NULL)
	{
		std::cout << "boucle display dir\n";
		std::string name(entry->d_name);
		if (name != "." && name != "..")
		{
			std::string full_path = path + "/" + name;
			if (is_directory(full_path))
				html << "<li><a href=\"" << full_path.substr(2) << "/\">" << name << "/</a></li>";
			else
				html << "<li><a href=\"" << full_path.substr(2) << "\">" << name << "</a></li>";
		}
	}

	html << "</ul></body></html>";
	closedir(dir);

	_response = build_response_header();

	_response += html.str();
	std::cout << "response fin display_dir = " << _response << std::endl;
}


void	Parser::GETmethod( void)
{	
	std::string	path = _request["path"][0];

	if (path.substr(0, 2) != "./")
		path = "./" + path;
	if (_server_conf.find("dir_listing") != _server_conf.end() &&_server_conf["dir_listing"][0] == "on")
	{
		std::cout << RED << "on a bien trouve le dir_listing" << RESET << std::endl;
		if (path == "./www/index.html")
			display_dirlist("./www");
		else if (path != "./www/favicon.ico")
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

	if (!_category.compare("TEXT") || !_category.compare("IMAGE"))
		build_response_content(path);
	else if (!_category.compare("CGI"))
		exec_cgi(path, POST);
	else
	{
		std::cerr << BOLD RED << "Error getting category : " << _extension << RESET << std::endl;	std::string	path = _request["path"][0];
	}
	std::cout << __func__ << "\tpath = " << path << std::endl;
}

void	Parser::DELETEmethod(void)
{
	std::string path = _request["path"][0];
	size_t pos = path.find("/delete");
	if (pos != std::string::npos)
		path = "www" + path.substr(pos + 7);
	int isFound = path.find("uploaded") != std::string::npos;
	if (!isFound)
	{
		std::cerr << path.c_str() + 4 << ": permission denied" << RESET << std::endl;
		return ;
	}
	int status = remove(path.c_str());
	if (!status)
		std::cout << path.substr(13) << ": successfully deleted" << std::endl;
	else
		std::cerr << path.substr(13) << ": " << strerror(errno) << std::endl;
}

std::string	Parser::build_response( void)
{		
	std::string	method[3] = { "GET", "POST", "DELETE" };
	_response.erase();

	get_location(_request["path"][0]);
	void (Parser::*func_method[])(void) = { &Parser::GETmethod, &Parser::POSTmethod, &Parser::DELETEmethod };

	if (_request["path"][0] == "www/error.html")
	{
		build_response_content(_request["path"][0]);
		restore_error_page();
		return (_response);
	}
	for (long unsigned int i = 0; i < method->size(); i++)
	{
		if (_request.find("method")->second[0] == method[i])
		{
			(this->*func_method[i])();
			std::cout << "\t\t\tmethod[i] " << method[i] << "\n";
			return (_response);
		}
	}
	return ("");
}

