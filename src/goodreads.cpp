#include "goodreads.hpp"

#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>

#include "pugixml/pugixml.hpp"


#define DEV_KEY "nMJozBknjHBc7NdCxG88Dg"


auto Goodreads::lastRequestTime = std::chrono::steady_clock::now();


bool Goodreads::search(const std::string str) {
	if (!isReady()) return false;

	try {
		curlpp::Cleanup cleaner;

		// Call the Goodreads API
		std::stringstream url;
		url << "https://www.goodreads.com/search/index.xml";
		url << "?q=" << curlpp::escape(str);
		url << "&key=" << DEV_KEY;
		std::stringstream xmlData;
		xmlData << curlpp::options::Url(url.str());

		lastRequestTime = std::chrono::steady_clock::now();

		// Parse the XML result
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load(xmlData);
		if (!result) return false;

		pugi::xml_node searchResult = doc.child("GoodreadsResponse")
				.child("search").child("results").child("work")
				.child("best_book");

		const char* id = searchResult.child("id").text().get();
		const char* title = searchResult.child("title").text().get();
		const char* author = searchResult.child("author").child("name").text().get();

		std::cout << "[" << id << "]  " << title << " – " << author << std::endl;

		return true;
	} catch (curlpp::LogicError& e) {
		std::cout << e.what() << std::endl;
	} catch (curlpp::RuntimeError& e) {
		std::cout << e.what() << std::endl;
	}
	return false;
}


bool Goodreads::isReady() {
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> deltaTime = currentTime - lastRequestTime;
	return deltaTime.count() > 1;
}
