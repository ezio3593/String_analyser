// string_analyzer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include "StringParser.h"
#include <time.h>

void printHelp(const char*);
void printIncParamsError();
void printStat(const std::map<std::string, int>* stat);

int main(int argc, char* argv[])
{
	StringParser stringParser;
	stringParser.addState(new NumberState());
	stringParser.addState(new WordState());
	stringParser.addState(new SymbolState());
	stringParser.addState(new SpaceState());

	std::string input;
	if (argc >= 2)
	{
		std::string mode = argv[1];

		if (argc == 3 && mode == "-f")
		{
			std::ifstream ifs(argv[2]);

			if (!ifs.fail())
			{
				clock_t time = clock();
				while (!ifs.eof()) 
				{
					std::getline(ifs, input);
					stringParser.Parse(&input);
					input.clear();
				}
				time = clock() - time;
				std::cout << std::endl << std::endl << "Time: " << time << std::endl;
				printStat(stringParser.getStat());
			} else
			{
				std::cerr << "Cannot open file: " << argv[2] << std::endl;
				return 1;
			}

		} else if (mode == "-h")
		{
			printHelp(argv[0]);
		} else if (mode[0] == '-')
		{
			printIncParamsError();
		} else 
		{
			input = argv[1];
			stringParser.Parse(&input);
			printStat(stringParser.getStat());
		}
	} else
	{
		std::cout << "Please, enter string: ";
		std::cin.peek();
		std::getline(std::cin, input);
		stringParser.Parse(&input);
		stringParser.getStat();
		printStat(stringParser.getStat());
		std::cout << std::endl << "Press any key to exit" << std::endl;
		std::cin.get();
	}	
	return 0;
}

void printHelp(const char* programName)
{
	std::cerr << "USAGE: " << programName << " [options] [input string]" << std::endl << std::endl;
	std::cerr << "Options: " <<  std::endl;
	std::cerr << "	-f FILENAME	" << "Using text from file with FILENAME for analyse" << std::endl;
}

void printIncParamsError()
{
	std::cerr << "Error: Incorrect parameters. Print '-h' to view help." << std::endl;
}

void printStat(const std::map<std::string, int>* stat)
{
	typedef std::map<std::string, int>::const_iterator const_iter;

	if (stat == NULL) return;

	
	const_iter it =  stat->cbegin();
	unsigned int maxLen = (it->first).length();
	for (; it != stat->cend(); ++it)
	{
		if ((it ->first).length() > maxLen) maxLen = (it ->first).length();
	}

	std::cout << std::endl; 

	for (it =  stat->cbegin(); it != stat->cend(); ++it)
	{
		std::cout << std::setw(maxLen) << it->first  << ": " << it->second << std::endl;
	}
}

