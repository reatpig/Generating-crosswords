#include "C://Users/Timofey/source/repos/Bmp/Bmp/Image.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <windows.h>
#include <iostream>
#include <random>
#include "CImg.h"

using namespace cimg_library;
struct location
{
	unsigned int startX, startY;
	bool horizontal;
};

void insertWord(std::vector<std::string> &crossword, const std::string &word, int x, int y, bool horizontal);
void render(std::vector<std::string> &crossword);
location findHor(std::vector<std::string>& crossword);
location findVer(std::vector<std::string>& crossword);
std::vector<std::string>  cut(std::vector<std::string>& crossword, location &hor, location&ver);
	
float rating(std::vector<std::string>& crossword) {
	float relationship;
	if(crossword.size() > crossword[0].size())
		relationship = (float)crossword[0].size()/crossword.size() ;
	else relationship = (float)crossword.size()/crossword[0].size()  ;

	float count_ = 0;
	for (int i = 0; i < crossword.size(); ++i)
		for (int r = 0; r < crossword[0].size(); ++r)
			if (crossword[i][r]==' ')
				count_+=1;
	count_ *= relationship;
	return count_;
}
int main() {

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(NULL));
	setlocale(LC_ALL, "Russian");
	std::string s;
	std::vector <std::string> allWords;

	

	unsigned int gridX,gridY;
	std::cin >> gridX >> gridY;
	if (gridY > gridX)
		std::swap(gridX, gridY);
	std::vector<std::string> crossword(gridY,std::string(gridX,' '));

	while (std::cin >> s) {
		allWords.push_back(s);
	}
	std::sort(allWords.begin(), allWords.end(),
		[](const std::string& a, const std::string& b)
	{ return a.length() >b.length(); });

	
	std::vector <bool> wordsUsedBool(allWords.size(),false);
	wordsUsedBool.reserve(allWords.size());
	
	std::vector <location> locations(allWords.size(), location{0,0,true});

	std::vector<std::vector<std::string> > allCrossword;
	std::vector<int> ratings;

	if (allWords[0].size() <= gridX) {

		insertWord(crossword, allWords[0],   (gridX- allWords[0].size())/2, gridY / 2, true);
		locations[0] = location{( gridX - allWords[0].size()) / 2, gridY / 2, true };
	}
	
	else std::cerr << "Grid is too small";
	
	wordsUsedBool[0] = true;
	bool flag=false;
	int wordsLeft = allWords.size()-1;
	bool change = false;
	int start=1;
	//Ищём общие буквы

	while (allCrossword.size()<20) {
		for (int wordIndex = 0; wordsLeft != 0; ++wordIndex) {//Перебираем неиспользованные слова
			flag = false;
			if (wordIndex >= allWords.size()) {
				wordIndex = 0;
				if (!change) {
					start++;
					for (int i = 0; i < allWords.size(); ++i)
						wordsUsedBool[i] = false;
					for (int i = 0; i < gridY; ++i)
						for (int r = 0; r < gridX; ++r)
							crossword[i][r] = ' ';
					if (start >= allWords.size())
						start = 0;
					if (allWords[start].size() < gridX) {
						insertWord(crossword, allWords[start], (gridX - allWords[start].size()) / 2, gridY / 2, true);
						wordsUsedBool[start] = true;
						locations[start] = location{ (gridX - allWords[start].size()) / 2, gridY / 2, true };
					}
					
					wordsLeft = allWords.size() - 1;

				}
				else change = false;
			}
			if (!wordsUsedBool[wordIndex])
				for (auto usedWord = 0; usedWord < allWords.size() && !flag; ++usedWord)//Перебираем вставленные слова
					if (wordsUsedBool[usedWord])
						for (int i = 0; i < allWords[wordIndex].size() && !flag; ++i)//Перебираем буквы неиспользованного слова
							for (int r = 0; r < allWords[usedWord].size() && !flag; ++r)//Перебираем буквы вставленного слова
								if (allWords[wordIndex][i] == allWords[usedWord][r] && (float)rand() / RAND_MAX < 0.9) {//Нашли общие буквы

									bool hor = locations[usedWord].horizontal;//Сокращение
									unsigned int startX = locations[usedWord].startX + r * locations[usedWord].horizontal - i * !locations[usedWord].horizontal;
									unsigned int startY = locations[usedWord].startY + r * !locations[usedWord].horizontal - i * locations[usedWord].horizontal;

									if (startX - 1 * !hor < 0 || startY - 1 * hor < 0 || startX + allWords[wordIndex].size() * !hor >= gridX ||
										startY + allWords[wordIndex].size() * hor >= gridY)
										break;
									bool insertB = true;
									if (crossword[startY - 1 * hor][startX - 1 * !hor] != ' ' ||//Проверка клетки до первой
										crossword[startY + allWords[wordIndex].size() * hor][startX + allWords[wordIndex].size() * !hor] != ' ') {//Проверка клетики за последней
										insertB = false;
										break;
									}


									for (int test = 0; test < allWords[wordIndex].size(); ++test) {//Проверка свободны ли боковые клетки
										if ((!hor&& startX + !hor * test == locations[usedWord].startX)//Не пересикает ли проверка вставленное слово 
											|| (startY + hor * test == locations[usedWord].startY&&hor))
											continue;
										if (crossword[startY + hor * test + !hor * 1][startX + !hor * test + hor * 1] != ' ' ||
											crossword[startY + hor * test - !hor * 1][startX + !hor*test - hor * 1] != ' ' ||
											crossword[startY + hor * test][startX + !hor * test] != ' ') {
											insertB = false;
											break;
										}
									}
									if (insertB) {
										change = true;
										wordsLeft--;
										flag = true;
										insertWord(crossword, allWords[wordIndex], startX, startY, !hor);
										wordsUsedBool[wordIndex] = true;
										locations[wordIndex] = location{ startX, startY,!hor };

										//std::cout << std::endl;
										//render(crossword);
										break;
									}
								}
		}
		location ver = findVer(crossword);
		location hor = findHor(crossword);
		std::vector<std::string> copyCrossword  (cut(crossword, hor, ver));
		ratings.push_back(rating(copyCrossword));
		allCrossword.push_back(copyCrossword);

		for (int i = 0; i < allWords.size(); ++i)
			wordsUsedBool[i] = false;
		for (int i = 0; i < gridY; ++i)
			for (int r = 0; r < gridX; ++r)
				crossword[i][r] = ' ';
			start = 0;
		if (allWords[start].size() < gridX) {
			insertWord(crossword, allWords[start], (gridX - allWords[start].size()) / 2, gridY / 2, true);
			wordsUsedBool[start] = true;
			locations[start] = location{ (gridX - allWords[start].size()) / 2, gridY / 2, true };
		}
		
		 wordsLeft = allWords.size() - 1;
	}
	
	 int minRating= ratings[0];
	 int minIndex=0;

	 int maxRating = ratings[0];
	 int maxIndex = 0;

	 for (int i = 0; i < allCrossword.size(); ++i) {
		 if (ratings[i] < minRating) {
			 minRating = ratings[i];
			 minIndex = i;
		 }
		 if (ratings[i] > maxRating) {
			 maxRating = ratings[i];
			 maxIndex = i;
		 }
	 }
	std::cout << std::endl;

	//render(allCrossword[ minIndex]);
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	render(allCrossword[maxIndex]);

	//image.Export("Grid.bmp");
		

	
	return 0;
}

void insertWord(std::vector<std::string> &crossword, const std::string &word, int x, int y, bool horizontal) {
	bool vertical = !horizontal;
	
	for (int i = 0; i < word.size(); ++i) {
		crossword[y + i * vertical][x + i * horizontal] = word[i];
	}
}

void render(std::vector<std::string> &crossword) {
	for (int i = 0; i < crossword.size(); ++i) {
		for (int r = 0; r < crossword[0].size(); ++r)
			std::cout << crossword[i][r];
			std::cout << std::endl;
	}
	CImgList<> font_full = CImgList<>::font(20, false);
	font_full.push_back(CImg<>::vector('а'));
	font_full.push_back(CImg<>::vector('б'));
	font_full.push_back(CImg<>::vector('в'));


	CImg < unsigned char>  img(crossword[0].size()*20+1,crossword.size()*20+1,1,3,255);
	unsigned char black[] = { 0,   0,   0 };
	unsigned char white[] = { 255,   255,   255 };
	for (int i = 0; i < crossword.size(); ++i) 
		for (int r = 0; r < crossword[0].size(); ++r) {
			//const char*const c = static_cast <const char*const >(crossword[i][r]);
			std::string s; 
			s.push_back(crossword[i][r]);
		   
			//img.draw_text(r * 20+5, i * 20,s.c_str(), black,white ,1, font_full);
			img.draw_text(r * 20 + 5, i * 20, s.c_str(), black, 20);
		}

	
	


	for (int i = 0; i < img.width() + 1; ++i)
		img.draw_line(i * 20, 0, i * 20, img.width() * 20, black);// line(i * 20, 0, i * 20, gridY* 20, Color(0, 0, 0));
for (int i = 0; i < img.height()+1; ++i)
	img.draw_line(0, i * 20, img.height() * 20, i * 20, black);
	
	
	img.save("Crossword.bmp");
}
location findVer(std::vector<std::string>& crossword) {
	
	location hor;
	hor.horizontal = true;
	for (int i = 0; i < crossword.size(); ++i)
		for (int r = 0; r < crossword[0].size(); ++r)
			if (crossword[i][r] != ' ')
				hor.startX = i+1;

	for (int i = crossword.size() - 1; i >= 0; --i)
		for (int r = crossword[0].size() - 1; r >= 0; --r)
			if (crossword[i][r] != ' ')
				hor.startY = i;
	return hor;
}
location findHor(std::vector<std::string>& crossword) {
	
	location ver;
	ver.horizontal = false;
	for (int r = 0; r < crossword[0].size(); ++r)
		for (int i = 0; i < crossword.size(); ++i)
			if (crossword[i][r] != ' ')
				ver.startX = r+1;

	for (int r = crossword[0].size() - 1; r >= 0; --r)
		for (int i = crossword.size() - 1; i >= 0; --i)
			if (crossword[i][r] != ' ')
				ver.startY = r;
	return ver;
}
std::vector<std::string>  cut(std::vector<std::string>& crossword, location &hor, location&ver) {
	std::vector<std::string> cutedCrossword( ver.startX - ver.startY, std::string(  hor.startX - hor.startY, ' '));
	for (int i = ver.startY; i < ver.startX; ++i)
		for (int r = hor.startY; r < hor.startX; ++r)
			cutedCrossword[i - ver.startY][r - hor.startY] = crossword[i][r];
	return cutedCrossword;
}