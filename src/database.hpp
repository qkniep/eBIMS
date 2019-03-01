#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

#include <sqlite_modern_cpp.h>


class Database {
	sqlite::database db;


public:
	Database(std::string filename);
	void printBookTable();
	void printFindBooks(char* buffer);
	void insertBook(std::string title, std::string author);
};

#endif /* !DATABASE_HPP */
