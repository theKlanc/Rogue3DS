#include <string>
#include <sstream>
#include "core.h"

using namespace std;

string get_string(int number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

bool fsIsDirectory(const std::string path)
{
	DIR* dir = opendir(path.c_str());
	if (dir) {
		closedir(dir);
		return true;
	}
	return false;
}

bool fsExists(const std::string path)
{
	FILE* fd = fopen(path.c_str(), "r");
	if (fd) {
		fclose(fd);
		return true;
	}
	return fsIsDirectory(path);
}

bool fsCreateDir(const std::string path)
{
	if (fsExists(path)) {
		errno = EEXIST;
		return false;
	}
	return (mkdir(path.c_str(), 0777) == 0);
}