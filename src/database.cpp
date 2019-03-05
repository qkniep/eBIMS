#include "database.hpp"

#include <unistd.h>
#include <sstream>


Database::Database(std::string filename) : db(filename) {
	try {
		db <<
			"CREATE TABLE IF NOT EXISTS books ("
			"    gr_id INTEGER PRIMARY KEY NOT NULL,"
			"    title TEXT,"
			"    author TEXT"
			");";
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void Database::insertBook(const struct book& book) {
	try {
		db << "INSERT INTO books (gr_id, title, author) VALUES (?,?,?);"
			<< book.goodreadsID
			<< book.title
			<< book.author;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void Database::printBookTable() {
	try {
		std::cout << "Current State of Table (books):" << std::endl;
		db << "SELECT gr_id, title, author FROM books;"
			>> [&](int id, std::string title, std::string author) {
				std::cout << id << ' ' << title << ' ' << author << std::endl;
			};
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void Database::sendFindBooks(int socket, char* buffer) {
	std::stringstream ss, ss_i;
	try {
		db << "SELECT title FROM books WHERE title LIKE ?;"
			<< buffer
			>> [&](std::string title) {
				ss << title << "\n";
			};
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	ss_i << ss.str().length();
	write(socket, ss_i.str().c_str(), 1024);
	write(socket, ss.str().c_str(), ss.str().length());
}
