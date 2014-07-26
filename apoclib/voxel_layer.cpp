#include <iostream>
#include <fstream>

#include "voxel_layer.h"

namespace ApocRes {

struct lofheader
{
	uint32_t bitsX;
	uint32_t bitsY;
} __attribute__((packed));

static_assert(sizeof(struct lofheader) == 8, "lofheader not 8 bytes");

std::vector<VoxelLayer>
VoxelLayer::loadLayersFromFile(const std::string datFileName, const std::string tabFileName)
{
	std::ifstream datFile (datFileName, std::ios_base::in);
	if (!datFile)
	{
		std::cerr << "Failed to open \"" << datFileName << "\"\n";
		return {};
	}
	std::ifstream tabFile (tabFileName, std::ios_base::in);
	if (!tabFile)
	{
		std::cerr << "Failed to open \"" << tabFileName << "\"\n";
		return {};
	}

	std::vector<VoxelLayer> layers;

	uint32_t offset;
	tabFile.read((char*)&offset, sizeof(offset));
	while (tabFile)
	{
		datFile.seekg(offset*4, std::ios_base::beg);
		VoxelLayer layer;
		struct lofheader header;
		datFile.read((char*)&header, sizeof(header));
		if (!datFile)
		{
			std::cerr << "Failed to load LOF bitmap\n";
			return {};
		}
		if (header.bitsX % 8 ||
		    header.bitsY % 8)
		{
			std::cout << "Invalid LOF bitmap header (x=" << header.bitsX <<
				",y=" << header.bitsY << ",tab=" << offset << ")\n";
				return {};
		}
		layer.bitsY = header.bitsY;
		layer.bitsX = header.bitsX;
		layer.bitmap.resize(layer.bitsY);
		for (unsigned int y = 0; y < header.bitsY ; y++)
		{
			layer.bitmap[y].resize(layer.bitsX);
			for (unsigned int x = 0; x < header.bitsX / 8; x++)
			{
				uint8_t byte;
				datFile.read((char*)&byte, sizeof(byte));
				if (!datFile)
				{
					std::cerr << "EOF in LOF bitmap\n";
					return {};
				}
				for (int b = 0; b < 8; b++)
				{
					bool bit = (byte >> b) & 1;
					layer.bitmap[y][(x*8)+b] = bit;

				}
			}
		}
		layers.push_back(layer);
		tabFile.read((char*)&offset, sizeof(offset));
	}
	std::cout << "Loaded " << layers.size() << " LOF layers\n";

	return layers;
}


}; //namespace ApocRes
