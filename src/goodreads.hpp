#ifndef GOODREADS_HPP
#define GOODREADS_HPP

#include <chrono>
#include <string>


class Goodreads {
	static std::chrono::time_point<std::chrono::steady_clock> lastRequestTime;


public:
	static bool search(std::string str);

private:
	static bool isReady();
};

#endif /* !GOODREADS_HPP */
