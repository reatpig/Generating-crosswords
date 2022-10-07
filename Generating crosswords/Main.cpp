#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <iostream>
#include "CImg.h"
#include <map>
#include "Crossword generator.h"
#include <fstream>
#include <thread>

using namespace cimg_library;

void render(std::vector<std::string> &crossword);
std::vector <std::string>& wordFromFile(char* path,  std::vector < std::string>&);

int main(int argc, char* argv[]) {
	//Russian language
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
	setlocale(LC_ALL, "Russian");

	std::string s;
	std::vector <std::string> allWords;

	switch (argc)
	{case 1://Without argument input from keybourd 
		std::cout << "Input words. Write ctrl+z+enter, if you will want to stop" << std::endl;
		while (std::cin >> s) {
			allWords.push_back(s);
		}
		break;
	case 2://With argument input from file 
	 wordFromFile(argv[1], allWords );
		break;
	default:
		std::cerr << "Too many command line arguments";
		return 1;
	}

	std::multimap<int, std::vector<std::string> >allCrossword;
	std::sort(allWords.begin(), allWords.end(),
		[](const std::string& a, const std::string& b)
		{ return a.length() > b.length(); });

    generateCrossword( allWords,allCrossword, 50);
	std::cout << std::endl;
	render(allCrossword.begin()->second);
	return 0;
}
std::vector <std::string>& wordFromFile(char * path, std::vector < std::string>& allWords) {
	std::ifstream file(path);
	std::string s;
	while (!file.eof()) {
		file >> s;
		allWords.push_back(s);
	 }
	return allWords;
}
void render(std::vector<std::string>& crossword) {
	for (int i = 0; i < crossword.size(); ++i) {
		for (int r = 0; r < crossword[0].size(); ++r)
			std::cout << crossword[i][r];
		std::cout << std::endl;
	}
	//Word drawing
	CImg < unsigned char>  imgWithAnswer(crossword[0].size() * 20 + 1, crossword.size() * 20 + 1, 1, 3, 255);
	CImg < unsigned char>  imgWithoutAnswer(crossword[0].size() * 20 + 1, crossword.size() * 20 + 1, 1, 3, 255);
	unsigned char black[] = { 0,   0,   0 };
	unsigned char white[] = { 255,   255,   255 };
	for (int i = 0; i < crossword.size(); ++i)
		for (int r = 0; r < crossword[0].size(); ++r) {
			std::string s({ crossword[i][r] });
			if (s[0] != ' ') {
				imgWithAnswer.draw_text(r * 20 + 5, i * 20, s.c_str(), black, white, 1, 22);
				//Grid drawing
				imgWithAnswer.draw_line(r * 20, i*20, r * 20, (i+1) * 20, black);
				imgWithAnswer.draw_line(r * 20, i * 20, (r+1) * 20, i  * 20, black);
				imgWithAnswer.draw_line(r * 20, (i +1) * 20, (r+1) * 20, (i + 1) * 20, black);
				imgWithAnswer.draw_line((r+1) * 20, i  * 20, (r + 1) * 20, (i + 1) * 20, black);

				imgWithoutAnswer.draw_line(r * 20, i * 20, r * 20, (i + 1) * 20, black);
				imgWithoutAnswer.draw_line(r * 20, i * 20, (r + 1) * 20, i * 20, black);
				imgWithoutAnswer.draw_line(r * 20, (i + 1) * 20, (r + 1) * 20, (i + 1) * 20, black);
				imgWithoutAnswer.draw_line((r + 1) * 20, i * 20, (r + 1) * 20, (i + 1) * 20, black);

			}
		}
	std::string name = "Crossword with answer.bmp";
	imgWithAnswer.save(name.c_str());
	imgWithoutAnswer.save("Crossword without answer.bmp");
}