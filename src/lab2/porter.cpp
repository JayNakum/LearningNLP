#include "porter.h"

#include <iostream>
#include <string>
#include <vector>

static bool IsVowel(const char letter)
{
	if (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u')
		return true;

	if (letter == 'y')
		return true;

	return false;
}

static bool IsConsonant(const char letter)
{
	return !(IsVowel(letter));
}

static bool XvX(const std::string& stem)
{
	for (const char letter : stem)
		if (IsVowel(letter)) 
			return true;

	return false;
}

static bool Xd(const std::string& stem)
{
	const char l1 = (stem.substr(0, stem.size() - 1)).back();
	const char l2 = stem.back();

	return (IsConsonant(l1) && IsConsonant(l2));
}

static bool Xo(const std::string& stem)
{
	if (stem.size() < 3) return false;

	const char c1 = (stem.substr(0, stem.size() - 2)).back();
	const char v = (stem.substr(0, stem.size() - 1)).back();
	const char c2 = stem.back();

	if (c2 == 'w' || c2 == 'x' || c2 == 'y')
		return false;

	return (IsConsonant(c1) && IsVowel(v) && IsConsonant(c2));
}

static std::string GetForm(const std::string& stem)
{
	std::string form = " ";

	for (const char letter : stem)
	{
		if (IsVowel(letter))
		{
			if (form.back() != 'v')
				form += 'v';
		}
		else
		{
			if (form.back() != 'c')
				form += 'c';
		}
	}

	return form;
}

static unsigned int M(const std::string& stem)
{
	unsigned int count = 0;

	std::string form = GetForm(stem);
	for (size_t offset = form.find("vc"); offset != std::string::npos; offset = form.find("vc", offset + 2))
	{
		++count;
	}

	return count;
}

static inline bool EndsWith(const std::string& str, const std::string& ending)
{
	if (ending.size() > str.size())
		return false;

	return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

static inline bool Replace(std::string& str, const std::string& from, const std::string& to)
{
	// size_t startPos = str.find(from);
	if (str.find(from) == std::string::npos)
		return false;

	str = str.substr(0, str.size() - from.size());
	str += to;

	// str.replace(startPos, from.length(), to);
	return true;
}


static void Step1(std::string& stem)
{
	if (EndsWith(stem, "sses"))
	{
		Replace(stem, "sses", "ss");
	}
	else if (EndsWith(stem, "ies"))
	{
		Replace(stem, "ies", "i");
	}
	else if (EndsWith(stem, "ss"))
	{
		// Replace(stem, "ss", "ss");
	}
	else if (EndsWith(stem, "s"))
	{
		Replace(stem, "s", "\0");
	}
}

static void Step2b(std::string& stem)
{
	if (EndsWith(stem, "at"))
		stem += 'e';
	
	else if (EndsWith(stem, "bl"))
		stem += 'e';

	else if (Xo(stem) && !(EndsWith(stem, "l") || EndsWith(stem, "s") || EndsWith(stem, "z")))
		stem = stem.substr(0, stem.size() - 1);

	else if ((M(stem) > 1) && Xo(stem))
		stem += 'e';
}

static void Step2(std::string& stem)
{
	if ((M(stem) > 1) && EndsWith(stem, "eed"))
	{
		stem = stem.substr(0, stem.size() - 1);
	}
	else if (XvX(stem) && EndsWith(stem, "ed"))
	{
		Replace(stem, "ed", "\0");
		Step2b(stem);
	}
	else if (XvX(stem) && EndsWith(stem, "ing"))
	{
		Replace(stem, "ing", "\0");
		Step2b(stem);
	}
}

static void Step3(std::string& stem)
{
	if (XvX(stem) && EndsWith(stem, "y"))
		Replace(stem, "y", "i");
}

static void Step4(std::string& stem)
{
	if (M(stem))
	{
		if (EndsWith(stem, "ational"))
			Replace(stem, "ational", "ate");
		else if (EndsWith(stem, "ization"))
			Replace(stem, "ization", "ize");
		else if (EndsWith(stem, "biliti"))
			Replace(stem, "biliti", "ble");
	}
}

static void Step5(std::string& stem)
{
	if (M(stem))
	{
		if (EndsWith(stem, "icate"))
			Replace(stem, "icate", "ic");
		else if (EndsWith(stem, "ful"))
			Replace(stem, "ful", "\0");
		else if (EndsWith(stem, "ness"))
			Replace(stem, "ness", "\0");
	}
}

static void Step6(std::string& stem)
{
	if (M(stem))
	{
		if (EndsWith(stem, "ance"))
			Replace(stem, "ance", "\0");
		else if (EndsWith(stem, "ent"))
			Replace(stem, "ent", "\0");
		else if (EndsWith(stem, "ive"))
			Replace(stem, "ive", "\0");
		else if (EndsWith(stem, "ize"))
			Replace(stem, "ize", "\0");
	}
}

static void Step7(std::string& stem)
{
	if ((M(stem) > 1) && EndsWith(stem, "e"))
		stem = stem.substr(0, stem.size() - 1);
	else if (((M(stem) == 1) && !Xo(stem)) && EndsWith(stem, "ness"))
		Replace(stem, "ness", "\0");

	if ((M(stem) > 1) && Xd(stem) && EndsWith(stem, "l"))
		stem = stem.substr(0, stem.size() - 1);
}

static std::string PorterStemmer(const std::string& word)
{
	std::string stem = word;
	Step1(stem);
	Step2(stem);
	Step3(stem);
	Step4(stem);
	Step5(stem);
	Step6(stem);
	Step7(stem);
	return stem;
}

namespace Lab2
{
	void Main()
	{
		std::vector<std::string> tokens = {"computers", "singing", "controlling", "generalizations", "elephants", "doing"};
		for (const std::string& token : tokens)
		{
			std::cout << PorterStemmer(token) << std::endl;
		}
	}
}
