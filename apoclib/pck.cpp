#include "pck.h"
#include <cassert>
#include "palette_image.h"
#include <cstdint>

namespace {
#include "xcom_blk.h"
}

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
struct pck_rle_header_a
{
	uint32_t pixelSkip;
	uint8_t columnSkip;
	uint16_t pixelCount;
	uint8_t padding;
};
#pragma pack(pop)
static_assert(sizeof(struct pck_rle_header_a) == 8, "pck_rle_header_a not 8 bytes");

#pragma pack(push,1)
struct pck_rle_header_b
{
	uint32_t pixelSkip;
	uint16_t columnSkip;
	uint16_t pixelCount;
	uint16_t padding;
	uint16_t unknown;
};
#pragma pack(pop)
static_assert(sizeof(struct pck_rle_header_b) == 12, "pck_rle_header_b not 12 bytes");

#pragma pack(push,1)
struct pck_blk_header
{
	uint8_t rowRecords;
	uint16_t unknown;
	uint8_t row;
	uint8_t subRecords;
};
#pragma pack(pop)
static_assert(sizeof(struct pck_blk_header) == 5, "pck_blk_header not 5 bytes");

#pragma pack(push,1)
struct pck_blk_subheader
{
	uint8_t column;
	uint8_t pixelCount;
	uint32_t blkOffset : 24;
};
#pragma pack(pop)
static_assert(sizeof(struct pck_blk_subheader) == 5, "pck_blk_subheader not 5 bytes");

static PaletteImage
loadRLEImageA(struct pck_header &header, std::ifstream &file)
{
	PaletteImage img(header.rightClip, header.bottomClip);

	while (file)
	{
		struct pck_rle_header_a rleHeader;
		file.read((char*)&rleHeader, sizeof(rleHeader));
		if (rleHeader.pixelSkip == 0xFFFFFFFF || file.eof())
			break;

		unsigned int row = rleHeader.pixelSkip / 640;
		unsigned int col = rleHeader.pixelSkip % 640;
		if(col != rleHeader.columnSkip)
		{
			std::cerr << "Unexpected columnSkip - maybe RLE type B?\n";
			break;
		}
		for (unsigned int i = 0; i < rleHeader.pixelCount; i++)
		{
			uint8_t idx;
			unsigned int x = (col + i)%640;
			unsigned int y = row + (i / 640);
			file.read((char*)&idx, sizeof(idx));
			if (file.eof())
			{
				std::cerr << "Unexpected EOF reading PCK pixels - maybe RLE type B?\n";
				break;
			}
			if (x < header.rightClip && y < header.bottomClip)
				img.setPixel(x, y, idx);
		}
	}
	return img;
}

static PaletteImage
loadRLEImageB(struct pck_header &header, std::ifstream &file)
{
	PaletteImage img(header.rightClip, header.bottomClip);

	while (file)
	{
		struct pck_rle_header_b rleHeader;
		file.read((char*)&rleHeader, sizeof(rleHeader));
		if (file.eof())
		{
			std::cerr << "Unexpected EOF - maybe RLE type A?\n";
			break;
		}
		if (rleHeader.pixelSkip == 0xFFFFFFFF)
			break;

		unsigned int row = rleHeader.pixelSkip / 640;
		unsigned int col = rleHeader.pixelSkip % 640;
		if(col != rleHeader.columnSkip)
		{
			std::cerr << "Unexpected columnSkip - maybe RLE type A?\n";
			break;
		}
		for (unsigned int i = 0; i < rleHeader.pixelCount; i++)
		{
			uint8_t idx;
			unsigned int x = (col + i)%640;
			unsigned int y = row + (i / 640);
			file.read((char*)&idx, sizeof(idx));
			if (file.eof())
			{
				std::cerr << "Unexpected EOF reading PCK pixels - maybe RLE type A?\n";
				break;
			}
			if (x < header.rightClip && y < header.bottomClip)
				img.setPixel(x, y, idx);
		}
	}
	return img;
}

static PaletteImage
loadBLKImage(struct pck_header &header, std::ifstream &file)
{
	PaletteImage img(header.rightClip, header.bottomClip);
	while (file)
	{
		struct pck_blk_header blkHeader;
		file.read((char*)&blkHeader, sizeof(blkHeader));
		if (file.eof())
		{
			std::cerr << "Unexpected EOF reading BLK header\n";
			break;
		}
		if (blkHeader.rowRecords == 0xff &&
		    blkHeader.unknown == 0xffff &&
			blkHeader.row == 0xff)
			break;
		for (unsigned r = 0; r < blkHeader.subRecords; r++)
		{
			struct pck_blk_subheader subHeader;
			file.read((char*)&subHeader, sizeof(subHeader));
			if (file.eof())
			{
				std::cerr << "Unexpected EOF reading BLK subheader\n";
				break;
			}
			unsigned int y = blkHeader.row;
			for (unsigned int i = 0; i < subHeader.pixelCount; i++)
			{
				unsigned int blkOffset = subHeader.blkOffset + i;
				unsigned int x = subHeader.column + i;
				assert(blkOffset < xcom_blk.size);
				if (x < header.rightClip && y < header.bottomClip)
					img.setPixel(x, y, xcom_blk.data[blkOffset]);
			}
		}
	}
	return img;
}

static PCK*
loadFromFile1(PCK *pck, std::ifstream &pckFile, std::ifstream &tabFile, PCK::RLEType type)
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
				switch (type)
				{
					case PCK::RLEType::typeA:
						pck->images.push_back(loadRLEImageA(header, pckFile));
						break;
					case PCK::RLEType::typeB:
						pck->images.push_back(loadRLEImageB(header, pckFile));
						break;
				}
				break;
			case 3:
				pck->images.push_back(loadBLKImage(header, pckFile));
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
PCK::loadFromFile(std::ifstream &pckFile, std::ifstream &tabFile, RLEType rleType)
{
	PCK *pck = new PCK();
	uint16_t version;
	pckFile.read((char*)&version, sizeof(version));
	switch (version)
	{
		case 1:
			return loadFromFile1(pck, pckFile, tabFile, rleType);
		default:
			assert(0);
	}
	return pck;
}

}; //namespace ApocRes
