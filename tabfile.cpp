#include "tabfile.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>

TabFile::TabFile(const std::string tabFileName, const std::string dataFileName)
{
	std::ifstream tab(tabFileName);
	std::ifstream dat(dataFileName);
	if (!tab)
	{
		std::cerr << "Failed to open tab file \"" << tabFileName << "\"\n";
		exit(1);
	}
	if (!dat)
	{
		std::cerr << "Failed to open data file \"" << dataFileName << "\"\n";
		exit(1);
	}

	tab.seekg(0, std::ios::end);
	auto tabSize = tab.tellg();
	tab.seekg(0, std::ios::beg);

	dat.seekg(0, std::ios::end);
	auto datSize = dat.tellg();
	dat.seekg(0, std::ios::beg);

	if (tabSize % 4)
	{
		std::cerr << "Tab file length not 4-byte aligned (" << tabSize << ")\n";
	}

	unsigned entries = tabSize / 4;

	std::vector<unsigned> tabOffsets;

	for (unsigned i = 0; i < entries; i++)
	{
		union {
			uint32_t u32;
			char c[4];
		} offset;
		tab.read(offset.c, 4);
		if (offset.u32 >= datSize)
		{
			std::cerr << "Tab idx " << i << " (" << offset.u32 << ") over end of file\n";
			exit(1);
		}
		tabOffsets.push_back(offset.u32);
	}

	for (unsigned i = 1; i <= entries; i++)
	{
		unsigned startOffset = tabOffsets[i-1];
		unsigned endOffset;
		if (i == entries)
			endOffset = datSize;
		else
			endOffset = tabOffsets[i];
		assert(endOffset > startOffset);

		unsigned chunkLength = endOffset - startOffset;

		lengths.push_back(chunkLength);
		data.emplace_back(new char[chunkLength]);
		dat.seekg(startOffset, std::ios::beg);
		dat.read(data.back().get(), chunkLength);
	}
}


unsigned TabFile::entryLength(unsigned idx)
{
	if (idx >= lengths.size())
	{
		std::cerr << "Index " << idx << " out of bounds\n";
		return 0;
	}
	return lengths[idx];
}

unsigned TabFile::getEntries()
{
	return lengths.size();
}

const char* TabFile::entryData(unsigned idx)
{
	if (idx >= data.size())
	{
		std::cerr << "Index " << idx << " out of bounds\n";
		return nullptr;
	}
	return data[idx].get();
}
