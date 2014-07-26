#pragma once
#include <vector>
#include <fstream>

namespace ApocRes {

class PaletteImage;

class PCK
{
	private:
		PCK();
	public:
		std::vector<PaletteImage> images;
		static PCK *loadFromFile(std::ifstream &pckFile, std::ifstream &tabFile);
};

}; //namespace ApocRes
