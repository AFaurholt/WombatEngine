#include <iostream>
// #include <cstddef>
// #include <chrono>
// #include <vector>

#include "Wombat/Legacy/Instance.h"

int main()
{
	Wombat::Instance instance{Wombat::Instance("TestApp", true)};

	instance.OpenDebugWindow();

	/*
	Input: words[] = { "may", "student", "students", "dog",
				 "studentssess", "god", "cat", "act",
				 "tab", "bat", "flow", "wolf", "lambs",
				 "amy", "yam", "balms", "looped",
				 "poodle"};
	Output :
	looped, poodle,
	lambs, balms,
	flow, wolf,
	tab, bat,
	may, amy, yam,
	student, students, studentssess,
	dog, god,
	cat, act,

	All words with same set of characters are printed
	together in a line.

	*/
	// std::vector<std::string> allWords
	// {
	// 	"may", "student", "students", "dog",
	// 	"studentssess", "god", "cat", "act",
	// 	"tab", "bat", "flow", "wolf", "lambs",
	// 	"amy", "yam", "balms", "looped",
	// 	"poodle"
	// };

	// struct CharSetWordPair
	// {
	// 	std::string charSet;
	// 	std::vector<std::string> words{};
	// };

	// std::vector<CharSetWordPair> charSetWordPairs{};

	// for (auto&& word : allWords)
	// {
	// 	//sort word
	// 	std::string sortedWord{ word };
	// 	std::sort(sortedWord.begin(), sortedWord.end());
	// 	std::string::iterator uniqueIterator = std::unique(sortedWord.begin(), sortedWord.end());
	// 	std::string sortedUnique{ sortedWord.substr(0, uniqueIterator - sortedWord.begin()) };

	// 	if (charSetWordPairs.size() > 0)
	// 	{
	// 		bool matchFound{ false };
	// 		for (auto&& pair : charSetWordPairs)
	// 		{
	// 			if (pair.charSet.compare(sortedUnique) == 0)
	// 			{
	// 				pair.words.push_back(word);
	// 				matchFound = true;
	// 				break;
	// 			}
	// 		}
	// 		if (!matchFound)
	// 		{
	// 			CharSetWordPair newPair{};
	// 			newPair.charSet = sortedUnique;
	// 			newPair.words.push_back(word);

	// 			charSetWordPairs.push_back(newPair);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		CharSetWordPair newPair{};
	// 		newPair.charSet = sortedUnique;
	// 		newPair.words.push_back(word);

	// 		charSetWordPairs.push_back(newPair);
	// 	}
	// }

	// std::sort
	// (
	// 	charSetWordPairs.begin()
	// 	, charSetWordPairs.end()
	// 	, [](CharSetWordPair a, CharSetWordPair b)
	// 	{
	// 		return a.charSet < b.charSet;
	// 	}
	// );

	// for (auto&& pair : charSetWordPairs)
	// {
	// 	printf("%s: ", pair.charSet.c_str());
	// 	for (auto&& word : pair.words)
	// 	{
	// 		printf("%s, ", word.c_str());
	// 	}
	// 	printf("\n");
	// }

	std::cin.get();

	return 0;
}
