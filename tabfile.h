#pragma once

#include <memory>
#include <vector>

class TabFile
{
public:
	TabFile(const std::string tabName, const std::string dataFileName);
	unsigned getEntries();
	const char *entryData(unsigned idx);
	unsigned entryLength(unsigned idx);
private:
	std::vector<unsigned> lengths;
	std::vector<std::unique_ptr<char[]> > data;
};
