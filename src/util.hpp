#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>


namespace util {
	void fatalError(std::string err);
	void generalWarning(std::string warn);
	void successMessage(std::string msg);
	void info(std::string info);
}

#endif /* !UTIL_HPP */
