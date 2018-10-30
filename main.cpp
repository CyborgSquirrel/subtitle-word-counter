#include "srtparser.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

std::string getEntireDialogue(SubtitleParser * parser)
{
	std::vector<SubtitleItem*> subtitles = parser->getSubtitles();
	std::string dialogue = "";
	for(auto subtitle : subtitles){
		dialogue += subtitle->getDialogue();
		dialogue += " ";
	}
	return dialogue;
}

bool isInvalid(char c)
{
	const static std::string invalid = ".!?,;[](){}/\\*\"";
	return invalid.find(c) != std::string::npos;
}

char fixChar(char c){
	if(isInvalid(c)){
		return ' ';
	}else{
		return c;
	}
}

std::string formatDialogue(std::string dialogue)
{
	std::transform(dialogue.begin(), dialogue.end(), dialogue.begin(), ::tolower);
	std::transform(dialogue.begin(), dialogue.end(), dialogue.begin(), fixChar);
	return dialogue;
}

std::map<std::string, int> createWordAppearancesMap(std::istringstream & dialogue)
{
	std::map<std::string, int> wordAppearances;
	std::istream_iterator<std::string> eos;
	std::string word;
	for(std::istream_iterator<std::string> iterator(dialogue); iterator != eos; iterator++){
		word = *iterator;
		if(wordAppearances.count(word) == 0){
			wordAppearances[word] = 1;
		}else{
			wordAppearances[word]++;
		}
	}
	return wordAppearances;
}

std::vector<std::pair<std::string, int>> createWordAppearancesVector(const std::map<std::string, int> & wordAppearancesMap){
	std::vector<std::pair<std::string, int>> wordAppearances;
	std::copy(wordAppearancesMap.begin(), wordAppearancesMap.end(), std::back_inserter(wordAppearances));
	return wordAppearances;
}

int main(int argc, char * argv[])
{
	std::ostream * out;
	if(argc <= 1){
		std::cout << "Please pass the subtitle file as an argument" << std::endl;
	}else{
		if(argc == 2){
			out = &std::cout;
		}else{
			out = new std::ofstream(argv[2]);
		}
		SubtitleParserFactory * subParserFactory = new SubtitleParserFactory(argv[1]);
		SubtitleParser * parser = subParserFactory->getParser();
		std::istringstream dialogue(formatDialogue(getEntireDialogue(parser)));
		std::vector<std::pair<std::string, int>> wordAppearances = createWordAppearancesVector(createWordAppearancesMap(dialogue));
		std::sort(wordAppearances.begin(), wordAppearances.end(), [](const std::pair<std::string, int> & a, const std::pair<std::string, int> & b){return a.second > b.second;});
		for(auto wordAppearance : wordAppearances){
			*out << wordAppearance.second << "\t" << wordAppearance.first << std::endl;
		}
	}
	return 0;
}