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
		enum class RLEType
		{
			typeA, //has 8-byte RLE headers
			typeB, //has 12-byte RLE headers
		};
		std::vector<PaletteImage> images;
		static PCK *loadFromFile(std::ifstream &pckFile, std::ifstream &tabFile, RLEType rleType = RLEType::typeA);
};

}; //namespace ApocRes
