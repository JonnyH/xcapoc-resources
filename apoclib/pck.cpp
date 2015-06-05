#include "pck.h"
#include <cassert>
#include "palette_image.h"
#include <cstdint>
#include <map>

namespace {
#include "xcom_blk.h"
}

namespace ApocRes {

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
};
#pragma pack(pop)
static_assert(sizeof(struct pck_blk_header) == 4, "pck_blk_header not 4 bytes");

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
loadRLEA(std::istream &file)
{
	struct pck_header header;
	file.read((char*)&header, sizeof(header));
	if (!file)
	{
		std::cerr << "Unexpected EOF reading RLE PCK header\n";
		assert(0);
	}
	if (header.compressionMode != 1)
	{
		std::cerr << "RLE header expected compression mode '1', got '" << (int)header.compressionMode << "'\n";
		assert(0);
	}
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
loadRLEB(std::istream &file)
{
	struct pck_header header;
	file.read((char*)&header, sizeof(header));
	if (!file)
	{
		std::cerr << "Unexpected EOF reading RLE PCK header\n";
		assert(0);
	}
	if (header.compressionMode != 1)
	{
		std::cerr << "RLE header expected compression mode '1', got '" << (int)header.compressionMode << "'\n";
		assert(0);
	}
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
loadBLK(std::istream &file)
{
	struct pck_header header;
	file.read((char*)&header, sizeof(header));
	if (!file)
	{
		std::cerr << "Unexpected EOF reading BLK PCK header\n";
		assert(0);
	}
	if (header.compressionMode != 3)
	{
		std::cerr << "BLK header expected compression mode '3', got '" << (int)header.compressionMode << "'\n";
		assert(0);
	}

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
		for (unsigned r = 0; r < blkHeader.rowRecords; r++)
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

#pragma pack(push,1)
struct pck128_header
{
	uint8_t h1;
	uint8_t h2;
	uint16_t unknown1;
	uint16_t width;
	uint16_t height;
};
#pragma pack(pop)
static_assert(sizeof(struct pck128_header) == 8, "pck_header not 8 bytes");

//244 is the closest-to-black I could find
#define B 244

std::vector<std::vector<int>> ditherLut =
{
	{0, 0, 0, 0},
	{B, 0, B, 0},
	{0, B, 0, B},
	{B, 0, 0, 0},
	{0, B, 0, 0},
	{0, 0, B, 0},
	{0, 0, 0, B},
};
#undef B

static PaletteImage
loadShadow(std::istream &file)
{
	struct pck128_header header;
	file.read((char*)&header, sizeof(header));
	if (!file)
	{
		std::cerr << "Unexpected EOF reading BLK PCK header\n";
		assert(0);
	}
	PaletteImage img(header.width, header.height);
	uint8_t b = 0;
	file.read((char*)&b, 1);
	int pos = 0;
	while (b != 0xff)
	{
		uint8_t count = b;
		file.read((char*)&b, 1);
		if (!file)
		{
			std::cerr << "Unexpected EOF reading shadow data\n";
			assert(0);
			break;
		}
		uint8_t idx = b;

		if (idx == 0)
			pos += count * 4;
		else
		{
			assert(idx < 7);

			while (count--)
			{
				for (int i = 0; i < 4; i++)
				{
					#define STRIDE 640
					int x = pos % STRIDE;
					int y = pos / STRIDE;
					if (x < header.width && y < header.height)
					{
						img.setPixel(x, y, ditherLut[idx][i]);
					}
					pos++;
				}
			}
		}
		file.read((char*)&b, 1);
		if (!file)
		{
			std::cerr << "Unexpected EOF reading shadow data\n";
			assert(0);
			break;
		}
	}
	return img;
}

PaletteImage
PCK::load(std::istream &pckFile, PCKType type)
{
	switch (type)
	{
		case PCKType::RLEA:
			return loadRLEA(pckFile);
		case PCKType::RLEB:
			return loadRLEB(pckFile);
		case PCKType::BLK:
			return loadBLK(pckFile);
		case PCKType::SHADOW:
			return loadShadow(pckFile);
		default:
			std::cerr << "Unexpected PCKType\n";
			assert(0);
	}
}

}; //namespace ApocRes
