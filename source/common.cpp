#include <string>
#include <sstream>
#include "common.h"

using namespace std;

string get_string(int number) {
	stringstream ss;
	ss << number;
	return ss.str();
}