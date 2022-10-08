#include "Crossword generator.h"
#include <algorithm>
#include <random>
#include <cmath>
#include <mutex>
#include <stdlib.h>

void insertWord(std::vector<std::string>& crossword, const std::string& word, location loc) {
	bool vertical = !loc.horizontal;
	for (int i = 0; i < word.size(); ++i) {
		crossword[loc.startY + i * vertical][loc.startX + i * !vertical] = word[i];
	}
}

void startOver(std::vector <std::string>const& allWords, std::vector <bool>& wordsUsedBool, std::vector<std::string>& crossword,
	std::vector <location> &locations, unsigned int gridY, unsigned int gridX, unsigned int start, int& wordsLeft) {
		for (int i = 0; i < allWords.size(); ++i) {
			wordsUsedBool[i] = false;
			locations[i] = { 0,0,false };
		}
	for (int i = 0; i < gridY; ++i)
		for (int r = 0; r < gridX; ++r)
			crossword[i][r] = ' ';
	if (allWords[start].size() < gridX) {
		locations[start] = location{ (gridX - allWords[start].size()) / 2, gridY / 2, true };
		insertWord(crossword, allWords[start], locations[start]);
		wordsUsedBool[start] = true;
	}
	wordsLeft = allWords.size() - 1;
}
//Find edge without empty row
location findVer(std::vector<std::string>& crossword) {
	location hor{ 0,0,true };
	//Upper edge
	for (int i = 0; i < crossword.size(); ++i) {
		for (int r = 0; r < crossword[0].size(); ++r)
			if (crossword[i][r] != ' ')
				hor.startX = i ;
		if (hor.startX != 0)
			break;
	}
	//Bottom edge
	for (int i = crossword.size() - 1; i >= 0; --i) {
		for (int r = crossword[0].size() - 1; r >= 0; --r)
			if (crossword[i][r] != ' ')
				hor.startY = i+1;
		if (hor.startY != 0)
			break;
	}
	return hor;
}
//Find edge without empty column
location findHor(std::vector<std::string>& crossword) {

	location ver{ 0,0,false };
	//Left edge
	for (int r = 0; r < crossword[0].size(); ++r) {
		for (int i = 0; i < crossword.size(); ++i)
			if (crossword[i][r] != ' ') {
				ver.startX = r ;
				break;
			}
		if (ver.startX != 0)
			break;
	}
	//Right edge
	for (int r = crossword[0].size() - 1; r >= 0; --r) {
		for (int i = crossword.size() - 1; i >= 0; --i)
			if (crossword[i][r] != ' '){
				ver.startY = r+1;
				break;
				}
		if (ver.startY != 0)
			break;
	}
	return ver;
}
std::vector<std::string>  cut(std::vector<std::string>& crossword, location& hor, location& ver) {
	std::vector<std::string> cutedCrossword(ver.startY - ver.startX, std::string(hor.startY - hor.startX, ' '));
	for (int i = ver.startX; i < ver.startY; ++i)
		for (int r = hor.startX; r < hor.startY; ++r)
			cutedCrossword[i - ver.startX][r - hor.startX] = crossword[i][r];
	return cutedCrossword;
}
//Count gridSize for worst case
size_t countGridSize(std::vector <std::string>const & allWords) {
	size_t letterCount = 0;
	//In worst case only half longest word affect on grid's size;
	for (size_t i = 0; i < allWords.size() / 2; ++i)
		letterCount += allWords[i].size();
	return letterCount;
}
//Rate how good crossword
float rating(std::vector<std::string>& crossword) {
	float relationship;
	if (crossword.size() > crossword[0].size())
		relationship = (float)crossword[0].size() / crossword.size();
	else relationship = (float)crossword.size() / crossword[0].size();

	float count_ = 0;
	for (int i = 0; i < crossword.size(); ++i)
		for (int r = 0; r < crossword[0].size(); ++r)
			if (crossword[i][r] == ' ')
				count_ += 1;
	//count_ *= atan(relationship);
	return count_;
}

std::mutex g_lock;

void generateCrossword(std::vector <std::string> const & allWords,
	std::multimap<int, std::vector<std::string> >&allCrossword, size_t howMany )
{
	auto thd_id = std::this_thread::get_id();
	std::mt19937_64 rand_engine{ std::hash<std::thread::id> {}(std::this_thread::get_id()) };
	std::uniform_real_distribution<double> rand_distr{ 0, RAND_MAX};

	unsigned int gridSize = countGridSize(allWords);
	unsigned int gridX{ gridSize }, gridY{ gridSize };
	std::vector<std::string> crossword(gridY, std::string(gridX, ' '));
	std::vector <bool> wordsUsedBool(allWords.size(), false);
	wordsUsedBool.reserve(allWords.size());
	std::vector <location> locations(allWords.size(), location{ 0,0,true });

	//Use the random long word how central
	size_t startWord =(int) rand_distr(rand_engine) % (allWords.size() / 4);
	if (allWords[startWord].size() < gridX) {
		locations[startWord] = location{ (gridX - allWords[startWord].size()) / 2, gridY / 2, true };
		insertWord(crossword, allWords[startWord], locations[startWord]);
		wordsUsedBool[startWord] = true;
	}

	bool flag = false;
	int wordsLeft = allWords.size() - 1;
	bool change = false;
	unsigned int  start = 1;

	//Search of common word
	g_lock.lock();
	size_t sizeC = allCrossword.size();
	g_lock.unlock();

	while (sizeC < howMany) {

		for (int wordIndex = 0; wordsLeft != 0; ++wordIndex) {//Перебираем неиспользованные слова
			flag = false;
			//If didn't find where insert a word
			if (wordIndex >= allWords.size()) {
				wordIndex = 0;
				if (!change) {
					//Start over with new central word
					
					start+= (int)rand_distr(rand_engine)%4;
					if (start >= allWords.size())
						start = 0;
					startOver(allWords, wordsUsedBool, crossword, locations, gridY, gridX, start, wordsLeft);
				}
				else change = false;
			}
			if (!wordsUsedBool[wordIndex])
				for (auto usedWord = 0; usedWord < allWords.size() && !flag; ++usedWord)//Looping over inserted words
					if (wordsUsedBool[usedWord])
						for (int i = 0; i < allWords[wordIndex].size() && !flag; ++i)//Looping through the letters of an unused word
							for (int r = 0; r < allWords[usedWord].size() && !flag; ++r)//Looping through the letters of an inserted word
								if (allWords[wordIndex][i] == allWords[usedWord][r] && (float)rand_distr(rand_engine) / RAND_MAX < 0.8) {//Some random to generate various crosswords
									bool hor = locations[usedWord].horizontal;
									unsigned int startX = locations[usedWord].startX + r * locations[usedWord].horizontal - i * !locations[usedWord].horizontal;
									unsigned int startY = locations[usedWord].startY + r * !locations[usedWord].horizontal - i * locations[usedWord].horizontal;

									bool insertNewWord = true;
									//Check if word goes beyond the borders
									if (startX - 1 * !hor < 0 || startY - 1 * hor < 0 || startX + allWords[wordIndex].size() * !hor >= gridX ||
										startY + allWords[wordIndex].size() * hor >= gridY)
										break;
									
									//Check if inserted word and old word form straight
									if (crossword[startY - 1 * hor][startX - 1 * !hor] != ' ' ||
										crossword[startY + allWords[wordIndex].size() * hor][startX + allWords[wordIndex].size() * !hor] != ' ') {
										insertNewWord = false;
										break;
									}
									//Check if side cells are free
									for (int test = 0; test < allWords[wordIndex].size(); ++test) {
										//Does the check intersect the inserted word
										if ((!hor && startX + !hor * test == locations[usedWord].startX)
											|| (startY + hor * test == locations[usedWord].startY && hor))
											continue;
										if (crossword[startY + hor * test + !hor * 1][startX + !hor * test + hor * 1] != ' ' ||
											crossword[startY + hor * test - !hor * 1][startX + !hor * test - hor * 1] != ' ' ||
											crossword[startY + hor * test][startX + !hor * test] != ' ') {
											insertNewWord = false;
											break;
										}
									}
									if (insertNewWord) {
										change = true;
										wordsLeft--;
										flag = true;
										locations[wordIndex] = location{ startX, startY,!hor };
										insertWord(crossword, allWords[wordIndex], locations[wordIndex]);
										wordsUsedBool[wordIndex] = true;
										break;
									}
								}
		}
		//Cut empty cells
		location ver = findVer(crossword);
		location hor = findHor(crossword);

	
		std::vector<std::string> copyCrossword(cut(crossword, hor, ver));
		
		//Save prepared crossword and ready to new 
		g_lock.lock();
		allCrossword.insert({ rating(copyCrossword), copyCrossword });
		g_lock.unlock();
		sizeC++;
		start = 0;
		startOver(allWords, wordsUsedBool, crossword, locations, gridY, gridX, start, wordsLeft);
	}
}

