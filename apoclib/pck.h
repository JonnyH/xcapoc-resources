#pragma once
#include <vector>
#include <fstream>

namespace ApocRes {

class PaletteImage;

class PCK
{
	public:
		enum class PCKType
		{
			RLEA, //has 8-byte RLE headers
			RLEB, //has 12-byte RLE headers
			BLK,
			SHADOW,
		};
		static PaletteImage load(std::istream &pckFile, PCKType type);
};

}; //namespace ApocRes
