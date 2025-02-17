#include <iostream>
#include <cstdlib>
#include <dirent.h>
#include <cstring>

int main()
{
	const char	*upload_path = getenv("UPLOAD_PATH");
	const char	*root = getenv("ROOT");
	if (!upload_path)
	{
		std::cout << "Content-Type: application/json\r\n\r\n[]";
		return 1;
	}

	DIR	*dir = opendir(upload_path);
	if (!dir)
	{
		std::cout << "[]";
		return 1;
	}

	struct	dirent *entry;
	bool	first = true;

	std::string upload_dir = upload_path;
	size_t pos = upload_dir.find(root);
	if (pos == 0)
		upload_dir = upload_dir.substr(4);

	std::cout << "{ \"upload_dir\": \"" << upload_dir << "\", \"files\": [";
	while ((entry = readdir(dir)) != NULL)
	{
		if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
			continue;
		if (!first)
			std::cout << ",";
		std::cout << "\"" << entry->d_name << "\"";
		first = false;
	}
	std::cout << "]}\n"; 

	closedir(dir);
	return 0;
}
