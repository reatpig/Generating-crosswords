#pragma once
#include <map>
#include <vector>
#include <string>
void generateCrossword(std::vector <std::string>const & allWords,
	std::multimap<int, std::vector<std::string> >& allCrossword, size_t howMany=1);
struct location
{
	unsigned long long startX, startY;
	bool horizontal;
};
