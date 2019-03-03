#ifndef GOODREADS_HPP
#define GOODREADS_HPP

#include <chrono>
#include <optional>
#include <string>

#include "book.hpp"


class Goodreads {
	std::chrono::time_point<std::chrono::steady_clock> lastRequestTime;
	std::string developerKey;


public:
	Goodreads(std::string devKey);
	std::optional<struct book> search(std::string str);

private:
	bool isReady();
};

#endif /* !GOODREADS_HPP */
