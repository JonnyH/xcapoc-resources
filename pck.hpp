#ifndef __PCK_HPP
#define __PCK_HPP

#include "image.hpp"

#include <cstdint>
#include <vector>
#include <string>

#define PCK_COMPRESSION_MODE_RLE 1
#define PCK_COMPRESSION_MODE_BLK 3
#define PCK_COMPRESSION_MODE_UNIMPLEMENTED1 128

typedef struct
{
	uint8_t compressionMode;
	uint8_t unknown[3];
	uint16_t leftClip;
	uint16_t rightClip;
	uint16_t topClip;
	uint16_t bottomClip;
} pck_header_t;

static_assert(sizeof(pck_header_t) == 12, "pck_header_t not 12 bytes");

typedef struct
{
	uint32_t pixelSkip;
	uint8_t columnSkip;
	uint16_t pixelCount;
	uint8_t pixelCount2;//(pixelCount+1)*4 == pixelcount?
} __attribute__((packed)) pck_rle_header_t;

static_assert(sizeof(pck_rle_header_t) == 8, "pck_rle_header_t not 8 bytes");

typedef struct
{
	uint8_t rowRecords;
	uint16_t unknown;//128-pixels drawn for the first subrecord of the previous row?
	uint8_t row;
	uint8_t subRecords;
} __attribute__((packed)) pck_blk_header_t;

static_assert(sizeof(pck_blk_header_t) == 5, "pck_blk_header_t not 5 bytes");

typedef struct
{
	uint8_t column;
	uint8_t pixelCount;
	uint32_t blkOffset : 24;
} __attribute__((packed)) pck_blk_subheader_t;

static_assert(sizeof(pck_blk_subheader_t) == 5, "pck_blk_subheader_t not 5 bytes");

class PCKImage
{
public:
	unsigned int height;
	unsigned int width;
	unsigned int topSkip;
	unsigned int leftSkip;
	std::vector<uint8_t> data;
	PaletteImage toPaletteImage();
};

class PCKFile
{
public:
	PCKFile(std::string tabFile, std::string pckFile, std::string blkFile, bool tab4);
	~PCKFile();
	int imageCount;
	PCKImage *getImage(int imageNumber);
	std::vector<PCKImage> images;
};

#endif
