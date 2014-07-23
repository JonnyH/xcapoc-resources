#pragma once

#include <vector>
#include <string>

namespace ApocRes {

class VoxelLayer
{
public:
	int bitsX, bitsY;
	std::vector<std::vector<bool> > bitmap;
	static std::vector<VoxelLayer> loadLayersFromFile(const std::string datFileName, const std::string tabFileName);
};

}; //namespace ApocRes
