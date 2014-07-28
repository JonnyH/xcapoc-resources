#include "pck.h"
#include <cassert>
#include "palette_image.h"
#include <cstdint>

namespace ApocRes {

PCK::PCK()
{
}

struct pck_header
{
	uint16_t compressionMode;
	uint16_t unknown;
	uint16_t leftClip;
	uint16_t rightClip;
	uint16_t topClip;
	uint16_t bottomClip;
};

static_assert(sizeof(struct pck_header) == 12, "pck_header not 12 bytes");

#pragma pack(push,1)
struct pck_rle_header
{
	uint32_t pixelSkip;
	uint8_t columnSkip;
	uint16_t pixelCount;
	uint8_t padding;
};
#pragma pack(pop)
static_assert(sizeof(struct pck_rle_header) == 8, "pck_rle_header not 8 bytes");

static PaletteImage
loadRLEImage(struct pck_header &header, std::ifstream &file)
{
	PaletteImage img(header.rightClip, header.bottomClip);

	while (file)
	{
		struct pck_rle_header rleHeader;
		file.read((char*)&rleHeader, sizeof(rleHeader));
		if (rleHeader.pixelSkip == 0xFFFFFFFF)
			break;

		unsigned int y = rleHeader.pixelSkip / 640;
		unsigned int col = rleHeader.pixelSkip % 640;
		assert(col == rleHeader.columnSkip);
		for (unsigned int i = 0; i < rleHeader.pixelCount; i++)
		{
			uint8_t idx;
			unsigned int x = col + i;
			file.read((char*)&idx, sizeof(idx));
			if (x < header.rightClip)
				img.setPixel(x, y, idx);
		}
	}


	return img;
}

static PCK*
loadFromFile1(PCK *pck, std::ifstream &pckFile, std::ifstream &tabFile)
{
	uint32_t offset;

	tabFile.read((char*)&offset, sizeof(offset));

	while (tabFile)
	{
		pckFile.seekg(offset*4, std::ios::beg);
		struct pck_header header;

		pckFile.read((char*)&header, sizeof(header));

		switch (header.compressionMode)
		{
			case 1:
				pck->images.push_back(loadRLEImage(header, pckFile));
				break;
			default:
				std::cerr << "Unknown compression mode:" << header.compressionMode << "\n";
				assert(0);
		}
		tabFile.read((char*)&offset, sizeof(offset));
	}

	return pck;
}


PCK *
PCK::loadFromFile(std::ifstream &pckFile, std::ifstream &tabFile)
{
	PCK *pck = new PCK();
	uint16_t version;
	pckFile.read((char*)&version, sizeof(version));
	switch (version)
	{
		case 1:
			return loadFromFile1(pck, pckFile, tabFile);
		default:
			assert(0);
	}
	return pck;
}

}; //namespace ApocRes
