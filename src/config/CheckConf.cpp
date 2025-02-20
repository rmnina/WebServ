/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConf.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahayon <ahayon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:07:16 by ahayon            #+#    #+#             */
/*   Updated: 2025/02/20 19:06:25 by ahayon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Config.hpp"
#include "../../include/parser/Parser.hpp"
#include <fstream>


bool Config::check_valid_nb(const std::string& str) 
{
	if (str.empty()) 
		return (false); 
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] < '0' || str[i] > '9')
			return (false); 
	}
	return true;
}

bool Config::check_valid_ip(const std::string& ip) 
{
	if (ip.empty()) 
		return (false);

	char ipCopy[16];
	std::strncpy(ipCopy, ip.c_str(), 15);
	ipCopy[15] = '\0'; 

	int parts = 0;
	char* token = std::strtok(ipCopy, "."); 
	while (token) {
		std::string part(token);

		if (!check_valid_nb(part)) 
			return (false);

		int num = std::atoi(part.c_str());
		if (num < 0 || num > 255) 
			return (false);

		parts++;
		token = std::strtok(NULL, ".");
	}
	return (parts == 4);
}

bool	Config::check_keyword_validity(std::string keyword, std::vector<std::string> tmp)
{
    std::string errors[9] = {"400", "403", "404", "405", "406", "410", "413", "504", "418"};
    
	if (keyword == "listen") {
		char *endptr;
		for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
			long nb = strtol(it->c_str(), &endptr, 10);
			if (*endptr != '\0' || nb < 1024 || nb > 65535)
				return (false);
		}
		return (true);
	} 
	else if (keyword == "host") {
		return (tmp.size() == 1 && (tmp[0] == "localhost" || check_valid_ip(tmp[0])));
	} 
	else if (keyword == "dir_listing") {
		return (tmp.size() == 1 && (tmp[0] == "on" || tmp[0] == "off"));
	} 
	else if (keyword == "index") {
		if (tmp.size() != 1 || tmp[0].substr(tmp[0].size() - 5) != ".html")
			return (false);
		// std::ifstream file(("www/" + tmp[0]).c_str());
		// return file.good();
	} 
	else if (keyword == "body_size") {
		if (tmp.size() != 1)
			return (false);
		char *endptr;
		long nb = strtol(tmp[0].c_str(), &endptr, 10);
		return (*endptr == '\0' && nb >= 1 && nb <= 424242);
	} 
	else if (keyword == "method") {
		if (tmp.empty() || tmp.size() > 3)
			return (false);
		for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
			if (*it != "GET" && *it != "POST" && *it != "DELETE")
				return (false);
		}
	} 
	else if (keyword == "upload") {
		return (tmp.size() == 2 && tmp[0] == "on");
	} 
	else if (keyword == "server_name") {
		return (tmp.size() == 1);
	}
    else if (keyword == "error") {
        if (tmp.size() != 2) {
            std::cout << "c'est size\n";
            return (false);
        }
        std::vector<std::string>::iterator it = tmp.begin();
        for (int i = 0; i < 9; i++) {
            if (*it == errors[i])
                break ;
            if (i == 8) {
                std::cout << "c'est i\n";
                return (false);
            }
        }
        it++;
        std::cout << "*it = " << *it << std::endl;
        std::ifstream file((*it).c_str());
        if (!file.is_open()) {
            std::cout << "c'est file\n";
            return (false);
        }
        file.close();
    }
	return true;
}