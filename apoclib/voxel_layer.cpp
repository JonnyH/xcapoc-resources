#include <allegro5/allegro.h>
#include <iostream>

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
	ALLEGRO_FILE *datFile = al_fopen(datFileName.c_str(), "r");
	if (!datFile)
	{
		std::cerr << "Failed to open \"" << datFileName << "\"\n";
		return {};
	}
	ALLEGRO_FILE *tabFile = al_fopen(tabFileName.c_str(), "r");
	if (!tabFile)
	{
		std::cerr << "Failed to open \"" << tabFileName << "\"\n";
		return {};
	}

	std::vector<VoxelLayer> layers;

	uint32_t offset;
	int ret = al_fread(tabFile, &offset, sizeof(offset));
	while (ret == sizeof(offset))
	{
		al_fseek(datFile, offset*4, ALLEGRO_SEEK_SET);
		VoxelLayer layer;
		struct lofheader header;
		ret = al_fread(datFile, &header, sizeof(header));
		if (ret != sizeof(header))
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
		for (int y = 0; y < header.bitsY ; y++)
		{
			layer.bitmap[y].resize(layer.bitsX);
			for (int x = 0; x < header.bitsX / 8; x++)
			{
				uint8_t byte;
				ret = al_fread(datFile, &byte, sizeof(byte));
				if (ret != sizeof(byte))
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
		ret = al_fread(tabFile, &offset, sizeof(offset));
	}
	std::cout << "Loaded " << layers.size() << " LOF layers\n";

	al_fclose(tabFile);
	al_fclose(datFile);
	return layers;
}


}; //namespace ApocRes
