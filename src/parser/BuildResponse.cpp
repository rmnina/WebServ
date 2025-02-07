/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BuildResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skiam <skiam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 21:15:07 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/07 02:17:32 by eltouma          ###   ########.fr       */
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
	mime_types[".php"] = "application/x-httpd-cgi";
	mime_types[".c"] = "text/x-c";
	mime_types[".cpp"] = "text/x-c";
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
	std::string		cgi[6] = {".py", ".sh", ".php", ".cgi", ".c", ".cpp"};

	for (long unsigned int i = 0; i < sizeof(raw_text_files) / sizeof(std::string); i++)
	{
		if (raw_text_files[i] == _extension)
		{
			_category = "TEXT";
			return;
		}
	}
	for (long unsigned int i = 0; i < sizeof(image_files) / sizeof(std::string); i++)
	{
		if (image_files[i] == _extension)
		{
			_category = "IMAGE";
			return;
		}
	}
	for (long unsigned int i = 0; i < sizeof(cgi) / sizeof(std::string); i++)
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
		if (_server_conf.find("dir_listing") != _server_conf.end() && _server_conf["dir_listing"][0] == "on" && is_directory(_request["path"][0]))
			header << "Content-Type: text/html\r\n";
		else if (get_content_type(_request["path"][0]) == "application/x-httpd-cgi")
			header << "Content-Type: text/html\r\n";
		else
			header << "Content-Type: " << get_content_type(_request["path"][0]) << "\r\n";
	}

	//get_content_length(_request["path"][0]);
	// header << "Content-Length: " << get_content_length(_request["path"][0]) << "\r\n";
	
	if (_keep_alive)
		header << "Connection: keep-alive\r\n";
	else
		header << "Connection: close\r\n";

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

static void	handle_cgi_error(int *status, pid_t pid)
{
	int	time = 0;
	while (time < 2000000)
	{
		pid_t ret = waitpid(pid, status, WNOHANG);
		if (ret == pid)
			break ;
		time += 1000;
		usleep(1000);
		if (time >= 2000000) 
		{
			kill(pid, SIGKILL);
			break ;
		}
	}
	return ;
}

std::string	trim_req_body(std::string str)
{
	size_t pos1 = str.find('\n');
	size_t pos2 = str.find('\n', pos1 + 1);
	size_t pos3 = str.find('\n', pos2 + 1);
	size_t pos4 = str.find('\n', pos3 + 1);
	
	return (str.substr(pos3 + 1, pos4 - pos3 - 1));
}

/*
void Parser::exec_cgi(std::string &filename, int method)
{
    std::cout << "Request body: " << _request_body << std::endl;

    pid_t pid;
    int pipe_in[2], pipe_out[2];

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        std::cerr << "Error creating pipes\n";
        return;
    }

    pid = fork();
    if (pid == -1) {
        std::cerr << "Error forking process\n";
        return;
    }

    if (pid == 0) {
        // --- Processus enfant ---
        close(pipe_in[1]);  // On lit dans pipe_in
        close(pipe_out[0]); // On écrit dans pipe_out

        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[0]);
        close(pipe_out[1]);

        // --- Préparation des variables d'environnement ---
        char content_length[20];
        sprintf(content_length, "CONTENT_LENGTH=%lu", (unsigned long)_request_body.size());

        char *env[] = {
            (char *)(method == GET ? "REQUEST_METHOD=GET" : "REQUEST_METHOD=POST"),
            content_length,
            (char *)"CONTENT_TYPE=application/x-www-form-urlencoded",
	    (char *)"REDIRECT_STATUS=1",
            NULL
        };

        char *argv[] = { (char *)filename.c_str(), NULL };
        execve(filename.c_str(), argv, env);
        std::cerr << "Error executing CGI script\n";
        exit(1);
    } else {

		//handle_cgi_error(&status, pid); 
        // --- Processus parent ---
        close(pipe_in[0]);
        close(pipe_out[1]);

        // --- Envoi des données POST via STDIN ---
        if (method == POST) {
            write(pipe_in[1], _request_body.c_str(), _request_body.size());
        }
        close(pipe_in[1]);

        // --- Lecture de la sortie du script ---
        char buffer[1024];
        std::string cgi_output;
        int bytes_read;
        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            cgi_output.append(buffer);
        }
        close(pipe_out[0]);

        // --- Attente du processus enfant ---
        int status;
        waitpid(pid, &status, 0);

        _response = cgi_output;
        std::cout << "CGI Response: " << _response << std::endl;
    }
}
*/

void Parser::exec_cgi(std::string &filename, int method) {
    std::cout << "Entering exec_cgi with filename: " << filename << std::endl;

    pid_t pid;
    int input_pipe[2], output_pipe[2];

//std::cout << "req body before trim = " << _request_body << std::endl;
//_request_body = "sign=" + trim_req_body(_request_body);
std::cout << "req body after = " << _request_body << std::endl;

    if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1) {
        std::cerr << "Error creating pipes\n";
        return;
    }

    pid = fork();
    if (pid == -1) {
        std::cerr << "Error forking process\n";
        return;
    }

    if (pid == 0) {
        close(input_pipe[1]);
        close(output_pipe[0]);

        dup2(input_pipe[0], STDIN_FILENO);   // Redirect input pipe to stdin
        dup2(output_pipe[1], STDOUT_FILENO); // Redirect stdout to output pipe

        close(input_pipe[0]);
        close(output_pipe[1]);

        std::vector<std::string> env_list;
	if (method == GET)
			env_list.push_back("REQUEST_METHOD=GET");
	else if (method == POST) {
				env_list.push_back("REQUEST_METHOD=POST");
				std::stringstream ss;
				ss << _request_body.size();
	//std::cout << "content length = " << ss.str() << std::endl;
				env_list.push_back("CONTENT_LENGTH=" + ss.str());
				env_list.push_back("CONTENT_TYPE=application/x-sh");
        }

	env_list.push_back("REDIRECT_STATUS=200");
	env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_list.push_back("QUERY_STRING=");
			env_list.push_back("SCRIPT_NAME=" + filename);
	env_list.push_back("SCRIPT_FILENAME=" + filename);

        // Convert to char* array for execve
    char **envp = new char*[env_list.size() + 1];
        for (size_t i = 0; i < env_list.size(); i++) {
            envp[i] = strdup(env_list[i].c_str());
//std::cout << "env[i] = " << envp[i] << std::endl;
}
        envp[env_list.size()] = NULL;

        char *argv[] = { strdup(filename.c_str()), NULL };
std::cout << "\n\nrequest_body " << _request_body << "filename " << filename << "\n";
        execve(filename.c_str(), argv, envp);

        std::cerr << "Error executing CGI script: " << strerror(errno) << "\n";
        exit(1);
    }
else { // Parent process
	int status;

			close(input_pipe[0]);
			close(output_pipe[1]);

	//std::cout << "request body dans parent = " << _request_body << std::endl;
			if (method == POST)
	{
	std::cerr << "ATTENTION TOUT LE MODE, JE SUIS UN POST\n";
				write(input_pipe[1], _request_body.c_str(), _request_body.size());
	}
			close(input_pipe[1]); // Close after writing
	handle_cgi_error(&status, pid);
			char buffer[1024];
			std::string cgi_output;
			int bytes_read;
			while ((bytes_read = read(output_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
				buffer[bytes_read] = '\0';
				cgi_output += buffer;
			std::cout << "ON PASSE ICI: cgi_output: " << cgi_output << "\n";
        }
        close(output_pipe[0]);
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            std::cerr << "CGI script exited with error code: " << WEXITSTATUS(status) << "\n";
        }

        _response = cgi_output;
        std::cout << "CGI Output:\n" << _response << std::endl;
    }
}
// void Parser::exec_cgi(std::string &filename, int method) {
//     std::cout << "Entering exec_cgi with filename: " << filename << std::endl;

//     pid_t pid;
//     int input_pipe[2], output_pipe[2];

// 	_request_body = "sign=" + trim_req_body(_request_body) + "\r\n";
// 	std::cout << "req body after = " << _request_body << std::endl;
//     if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1) {
//         std::cerr << "Error creating pipes\n";
//         return;
//     }

//     pid = fork();
//     if (pid == -1) {
//         std::cerr << "Error forking process\n";
//         return;
//     }

//     if (pid == 0) { // Child process
//         close(input_pipe[1]);  // Close unused write end of input pipe
//         close(output_pipe[0]); // Close unused read end of output pipe

//         dup2(input_pipe[0], STDIN_FILENO);   // Redirect input pipe to stdin
//         dup2(output_pipe[1], STDOUT_FILENO); // Redirect stdout to output pipe

//         close(input_pipe[0]);
//         close(output_pipe[1]);

//         // Set up environment variables
//         std::vector<std::string> env_list;
// 		if (method == GET)
//         	env_list.push_back("REQUEST_METHOD=GET");
// 		else
// 			env_list.push_back("REQUEST_METHOD=POST");
//         if (method == POST) {
//             std::stringstream ss;
//             ss << _request_body.size();
//             env_list.push_back("CONTENT_LENGTH=" + ss.str());
// 			env_list.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
//         }

//         env_list.push_back("SCRIPT_NAME=" + filename);

//         // Convert to char* array for execve
//         char **envp = new char*[env_list.size() + 1];
//         for (size_t i = 0; i < env_list.size(); i++) {
//             envp[i] = strdup(env_list[i].c_str());
// 			//std::cout << "env[i] = " << envp[i] << std::endl;
// 		}
//         envp[env_list.size()] = NULL;

//         char *argv[] = { strdup(filename.c_str()), NULL };
//         execve(filename.c_str(), argv, envp);

//         std::cerr << "Error executing CGI script: " << strerror(errno) << "\n";
//         exit(1);
//     } 
// 	else { // Parent process
// 		int status;
// 		handle_cgi_error(&status, pid); 
//         close(input_pipe[0]);  // Close unused read end of input pipe
//         close(output_pipe[1]); // Close unused write end of output pipe

// 		std::cout << "request body dans parent = " << _request_body << std::endl;
//         if (method == POST)
//             write(input_pipe[1], _request_body.c_str(), _request_body.size());
//         close(input_pipe[1]); // Close after writing

//         char buffer[1024];
//         std::string cgi_output;
//         int bytes_read;
//         while ((bytes_read = read(output_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
//             buffer[bytes_read] = '\0';
//             cgi_output += buffer;
//         }
//         close(output_pipe[0]);
//         waitpid(pid, &status, 0);
//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             std::cerr << "CGI script exited with error code: " << WEXITSTATUS(status) << "\n";
//         }

//         _response = cgi_output;
//         std::cout << "CGI Output:\n" << _response << std::endl;
//     }
// }

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
		std::cout << "name = " << name << std::endl;
		if (name != "." && name != "..")
		{
			std::string full_path = path + "/" + name;
			if (is_directory(full_path))
				html << "<li><a href=\"" << name << "/\">" << name << "/</a></li>";
			else
				html << "<li><a href=\"" << name << "\">" << name << "</a></li>";
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
		upload();
	if (!_category.compare("TEXT") || !_category.compare("IMAGE"))
		build_response_content(path);
	else if (!_category.compare("CGI"))
	{
		std::cout << "on est bien dans le cgi post\n";
		exec_cgi(path, POST);
	}
	else
		std::cerr << BOLD RED << "Error getting category : " << _extension << RESET << std::endl;	
	std::cout << __func__ << "\tpath = " << path << std::endl;
}

void	Parser::upload(void)
{
	if (!fill_content_type_multipart(_request_body))
		std::cerr << "Invalid upload request headers" << std::endl;

	std::string	filename;
	std::string	content;
	
	if (!get_file_name(_request_body, filename) ||
		!get_file_content(_request_body, content))
		std::cerr << "Could not extract file from request" << std::endl;

	std::string	upload_dir;
	if (_server_conf.find("upload") != _server_conf.end() && 
		_server_conf["upload"][0] == "on")
		upload_dir = _server_conf["upload"][1];
	else
		std::cerr << "Upload not configured" << std::endl;

	struct stat info;
	if (stat(upload_dir.c_str(), &info) != 0)
	{
		if (mkdir(upload_dir.c_str(), 0755) != 0)
			std::cerr << "Error creating directory" << std::endl;
	}
	std::string	filepath = upload_dir + "/" + filename;
	std::cout << BOLD YELLOW << "filepath is " << filepath << RESET << std::endl;
	std::ofstream	file(filepath.c_str(), std::ios::binary);
	if (!file.is_open())
		std::cerr << "Cannot create file" << std::endl;

	file.write(content.c_str(), content.length());
	file.close();
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
	std::cout << RED << "BODY :" << _request_body << RESET << std::endl;
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

