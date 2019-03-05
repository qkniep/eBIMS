#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>

#include "termcolor/termcolor.hpp"


void fatalError(std::string err) {
	std::cerr << termcolor::red << "ERROR" << termcolor::reset
		<< " " << err << std::endl;
	exit(EXIT_FAILURE);
}


void generalWarning(std::string warn) {
	std::cout << termcolor::yellow << "Warning" << termcolor::reset
		<< " " << warn << std::endl;
}


void successMessage(std::string msg) {
	std::cout << termcolor::green << msg << termcolor::reset << std::endl;
}

#endif /* !UTIL_HPP */
