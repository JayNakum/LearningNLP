#include "termIncidence.h"

#include <iostream>
#include <string>
#include <sstream>

#include <vector>
#include <set>

#include <algorithm>

static std::vector<std::string> WordTokenizer(const std::string& data, const char delimeter = ' ')
{
	std::vector<std::string> tokens;

	std::stringstream tokensStream(data);
	std::string token;
	while (std::getline(tokensStream, token, delimeter))
		tokens.push_back(token);

	return tokens;
}

namespace Lab8
{
	void Main()
	{
		std::vector<std::string> documents = {"I am a cow", "cow is what I am", "today is tuesday"};

		std::set<std::string> wordSet;
		for (const std::string& doc : documents)
		{
			for (const std::string& token : WordTokenizer(doc))
				wordSet.insert(token);
		}

		std::vector<std::string> words;
		words.reserve(wordSet.size());
		for (const std::string& word : wordSet)
			words.push_back(word);
		std::sort(words.begin(), words.end());

		std::vector<std::vector<bool>> matrix;
		matrix.reserve(documents.size());

		std::cout << '\t';
		for (const std::string& word : words)
			std::cout << word << '\t';

		unsigned int id = 0;
		for (const std::string& doc : documents)
		{
			std::cout << std::endl << ++id << '\t';

			std::vector<bool> row;
			row.reserve(words.size());

			const auto& tokens = WordTokenizer(doc);
			for (const std::string& word : words)
			{
				if (std::find(tokens.begin(), tokens.end(), word) != tokens.end())
				{
					row.push_back(true);
					std::cout << "1" << '\t';
				}
				else
				{
					row.push_back(false);
					std::cout << "0" << '\t';
				}
			}

			matrix.push_back(row);
		}		
	}
}
