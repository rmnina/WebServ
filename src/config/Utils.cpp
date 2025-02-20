#include "../../include/config/Config.hpp"
#include "../../include/parser/Parser.hpp"

std::vector<ConfigStruct>	Config::get_servers_conf(void) const
{
	return (_servers_conf);
}

bool    Config::_server_allowed(const std::string &keyword) 
{
	const std::string			array[13] = {"listen", "Cross-Origin-Resource-Policy", "server_name", "dir_listing", "error", "method",
											"upload", "root", "body_size", "index", "location", "host"};
	std::vector<std::string>	allowed(array, array + 13);

	for (std::vector<std::string>::iterator it = allowed.begin(); it < allowed.end(); it++)
	{
		if (!keyword.compare(*it))
			return (true);
	}
	return (false);
}

bool    Config::_location_allowed(const std::string &keyword) 
{
	const std::string			array[8] = {"dir_listing", "error", "method", "upload", "root", 
											"default_file", "cgi", "redirect"};
	std::vector<std::string>	allowed(array, array + 8);

	for (std::vector<std::string>::iterator it = allowed.begin(); it < allowed.end(); it++)
	{
		if (!keyword.compare(*it))
			return (true);
	}
	return (false);
}

bool	Config::_update_brackets_state(bool brackets) 
{ 
	return (brackets ? false : true); 
}

std::vector<std::string>	string_to_vector(const std::string &string, const char delimiter, size_t space_pos)
{
	std::vector<std::string>	result;

	if (space_pos >= string.size())
		return (result);

	size_t	start = space_pos;

	if (string[start] == delimiter)
		start++;

	size_t	end = string.find(delimiter, start);

	while (end != std::string::npos)
	{
		result.push_back(string.substr(start, end - start));
		start = end + 1;
		end = string.find(delimiter, start);
	}
	if (start < string.length())
		result.push_back(string.substr(start));
	return (result);
}
