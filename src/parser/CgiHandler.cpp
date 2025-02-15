/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:19:27 by jdufour           #+#    #+#             */
/*   Updated: 2025/02/14 23:21:43 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser/Parser.hpp"
#include <cstdio>

void	Parser::handle_cgi_error(int *status, pid_t pid)
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
			_error_code = 504;
			print_log(RED, "Error", _server->getName(), "Gateway Timeout", ' ');
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

void	Parser::exec_cgi(std::string &filename, int method) 
{
	pid_t pid;
	int input_pipe[2], output_pipe[2];

	if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1) 
	{
		print_log(RED, "Error", _server->getName(), "Pipe creation failed in ", __func__);
		return;
	}
	pid = fork();
	if (pid == -1) 
	{
		print_log(RED, "Error", _server->getName(), "Pipe creation failed in ", __func__);
		return;
	}
	if (pid == 0) { // Child
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
			env_list.push_back("CONTENT_LENGTH=" + ss.str());
			env_list.push_back("CONTENT_TYPE=application/x-sh");
		}


		env_list.push_back("REDIRECT_STATUS=200");
		env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");
		env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
		env_list.push_back("QUERY_STRING=");
		env_list.push_back("SCRIPT_NAME=" + filename);
		env_list.push_back("SCRIPT_FILENAME=" + filename);
		env_list.push_back("UPLOAD_PATH=" + _upload_dir);
		env_list.push_back("PORT=" + _server->getPort());

		char **envp = new char*[env_list.size() + 1];
		for (size_t i = 0; i < env_list.size(); i++) {
			char *buff_env = new char[env_list[i].size() + 1];
			envp[i] = strcpy(buff_env, env_list[i].c_str());
		}
		envp[env_list.size()] = NULL;

		char *buff_argv = new char[filename.size() + 1];
		char *argv[] = { strcpy(buff_argv, filename.c_str()), NULL };
		execve(filename.c_str(), argv, envp);
		print_log(RED, "Error", _server->getName(), "CGI script could not be executed. ", strerror(errno));
		delete argv[0];
		for (int j = 0; envp[j]; j++)
			delete envp[j];
		exit(1);
	}
	else { // Parent process
		int status;

		close(input_pipe[0]);
		close(output_pipe[1]);

		if (method == POST)
			write(input_pipe[1], _request_body.c_str(), _request_body.size());
		close(input_pipe[1]); // Close after writing
		handle_cgi_error(&status, pid);
		char buffer[1024];
		std::string cgi_output;
		int bytes_read;
		while ((bytes_read = read(output_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes_read] = '\0';
			cgi_output += buffer;
		}
		close(output_pipe[0]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			print_log(RED, "Error", _server->getName(), "CGI script exited with error code: ", WEXITSTATUS(status));

		_response = cgi_output;
	}
}
