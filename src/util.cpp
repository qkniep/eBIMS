#include "util.hpp"

#include <iostream>

#include "termcolor/termcolor.hpp"


void util::fatalError(std::string err) {
	std::cerr << termcolor::red << "ERROR" << termcolor::reset
		<< " " << err << std::endl;
	exit(EXIT_FAILURE);
}


void util::generalWarning(std::string warn) {
	std::cout << termcolor::yellow << "Warning" << termcolor::reset
		<< " " << warn << std::endl;
}


void util::successMessage(std::string msg) {
	std::cout << termcolor::green << msg << termcolor::reset << std::endl;
}


void util::info(std::string info) {
	std::cout << termcolor::magenta << "INFO" << termcolor::reset
		<< " " << info << std::endl;
}
