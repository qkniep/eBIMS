#include "database.hpp"


Database::Database(std::string filename) : db(filename) {
	try {
		db <<
			"CREATE TABLE IF NOT EXISTS books ("
			"    _id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
			"    title TEXT,"
			"    author TEXT"
			");";
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void Database::insertBook(std::string title, std::string author) {
	try {
		db << "INSERT INTO books (_id, title, author) VALUES (?,?,?);"
			<< title
			<< author;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void Database::printBookTable() {
	try {
		std::cout << "Current State of Table (books):" << std::endl;
		db << "SELECT _id, title, author FROM books;"
			>> [&](int id, std::string title, std::string author) {
				std::cout << id << ' ' << title << ' ' << author << std::endl;
			};
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void Database::printFindBooks(char* buffer) {
	try {
		db << "SELECT title FROM books WHERE title LIKE ?;"
			<< buffer
			>> [&](std::string title) {
				std::cout << title << std::endl;
				//send(new_socket, title.c_str(), title.length(), 0);
			};
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}
