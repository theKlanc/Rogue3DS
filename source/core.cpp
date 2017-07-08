#include <string>

#include <sstream>
#include "../include/core.h"

using namespace std;

bool testing() { return true; }

string get_string(int number)
{
	stringstream ss;
	ss << number;
	return ss.str();
}