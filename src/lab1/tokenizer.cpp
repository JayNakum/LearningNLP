#include "tokenizer.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

static std::vector<std::string> WordTokenizer(const std::string& data, const char delimeter = ' ')
{
	std::vector<std::string> tokens;

	std::stringstream tokensStream(data);
	std::string token;
	while (std::getline(tokensStream, token, delimeter))
		tokens.push_back(token);

	return tokens;
}

static std::vector<std::string> SentenceTokenizer(const std::string& data)
{
	const unsigned int THRESH = 3;

	std::vector<std::string> tokens;

	std::stringstream lineStream(data);
	std::string line;
	while (std::getline(lineStream, line, '\n'))
	{
		std::string token = "";
		for (const char& t : line)
		{
			if (t == '.')
			{
				if (token.size() < THRESH) continue; // possibility of sentence starting with a shortform

				const std::vector<std::string> words = WordTokenizer(token);
				if (words.at(words.size() - 1).size() < THRESH) continue; // possibility of sentence having a shortform

				tokens.push_back(token);
				token = "";
			}
			token += t;
		}
	}

	return tokens;
}

namespace Lab1
{
	void Main()
	{
		const std::string data = "Formula One (more commonly known as Formula 1 or F1) is the highest class of international racing for open-wheel single-seater formula racing cars "
			"sanctioned by the Fédération Internationale de l'Automobile (FIA).\n"
			"The FIA Formula One World Championship has been one of the premier forms of racing around the world since its inaugural season in 1950.\n"
			"The word formula in the name refers to the set of rules to which all participants' cars must conform.\n"
			"A Formula One season consists of a series of races, known as Grands Prix.\n"
			"Grands Prix take place in multiple countries and continents around the world on either purpose - built circuits or closed public roads.";

		for (const std::string& sentence : SentenceTokenizer(data))
		{
			std::cout << sentence << std::endl;
			for (const std::string& word : WordTokenizer(sentence))
			{
				std::cout << "\t" << word << std::endl;
			}
			std::cout << std::endl;
		}
	}
}
