#include <iostream>
#include <fstream>
#include <sstream>

#include "tabfile.h"

struct chunk
{
	uint32_t startOffset;
	uint32_t endOffset;
};

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: split_tab input.tab input.file\n";
		return 1;
	}

	TabFile f(argv[1], argv[2]);
	std::ifstream inTab(argv[1]);
	for (unsigned i = 0; i < f.getEntries(); i++)
	{
		std::stringstream ss;
		ss << argv[2] << ":" << i;
		std::ofstream oFile(ss.str());
		oFile.write(f.entryData(i), f.entryLength(i));
	}
}
