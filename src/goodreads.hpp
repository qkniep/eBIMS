#ifndef GOODREADS_HPP
#define GOODREADS_HPP

#include <chrono>
#include <string>


class Goodreads {
	std::chrono::time_point<std::chrono::steady_clock> lastRequestTime;
	std::string developerKey;


public:
	Goodreads(std::string devKey);
	bool search(std::string str);

private:
	bool isReady();
};

#endif /* !GOODREADS_HPP */
