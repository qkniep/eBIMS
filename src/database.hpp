#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

#include <sqlite_modern_cpp.h>

#include "book.hpp"


class Database {
	sqlite::database db;


public:
	Database(std::string filename);
	void printBookTable();
	void sendFindBooks(int socket, char* buffer);
	void insertBook(const struct book& book);
};

#endif /* !DATABASE_HPP */
