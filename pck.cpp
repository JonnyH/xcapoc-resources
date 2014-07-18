/*
 * =====================================================================================
 *
 *       Filename:  pck.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/13 12:51:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jonathan Hamilton (jh), jtrhamilton@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pck.hpp"

#include <fstream>
#include <cassert>
#include <iostream>
#include <cstring>

PaletteImage
PCKImage::toPaletteImage()
{
	PaletteImage image(this->leftSkip + this->width, this->topSkip + this->height);

	memset(image.data.get(), 0, (this->leftSkip+this->width) * (this->topSkip + this->height));

	for (unsigned int row = 0; row < this->height; row++)
	{
		for (unsigned int col = 0; col < this->width; col++)
		{
			unsigned int srcOffset = row*this->width + col;
			unsigned int dstOffset = (row+this->topSkip)*(this->width+this->leftSkip) + (this->leftSkip + col);
			image.data[dstOffset] = this->data[srcOffset];
		}
	}
	return image;
}

static void decodeRLEPCK(std::ifstream &pckFile, PCKImage &image, unsigned int endOffset)
{
	while (true)
	{
		pck_rle_header_t rleHeader;
		if (pckFile.tellg() >= endOffset)
		{
			std::cout << "Reached offset " << pckFile.tellg() << "(max " << endOffset << ")\n";
			break;
		}
		std::cout << "Reading rle offset " << pckFile.tellg() << "\n";
		pckFile.read((char*)&rleHeader, sizeof(rleHeader));
		if (rleHeader.pixelSkip == 0xFFFFFFFF) 
		{
			std::cout << "End pixelskip\n";
			break;
		}
		if (!pckFile)
		{
			std::cout << "EOF in RLE decoder\n";
			break;
		}
		unsigned int row = rleHeader.pixelSkip / 640;
		assert (row >= image.topSkip);
		row -= image.topSkip;
		unsigned int col = rleHeader.pixelSkip % 640;
		assert (col >= image.leftSkip);
		col -= image.leftSkip;
		uint32_t pixelOffset = row*image.width + col;
		assert (col < image.width);
		assert (row < image.height);
		int skipCount = 0;
		if (col + rleHeader.pixelCount > image.width)
		{
			skipCount = rleHeader.pixelCount - (image.width - col);
			std::cout << "Clipping row. (trying to load " << rleHeader.pixelCount << " pixels at col " << col << " - skipping " << skipCount << ")\n";
			rleHeader.pixelCount = image.width - col;

		}
		pckFile.read((char*)image.data.data() + pixelOffset, rleHeader.pixelCount);
		if (skipCount)
			pckFile.seekg(skipCount, pckFile.cur);
	}
}

static void decodeBLKPCK(std::ifstream &pckFile, std::ifstream &blkFile, PCKImage &image)
{
	while (true)
	{
		pck_blk_header_t blkHeader;
		pckFile.read((char*)&blkHeader, sizeof(blkHeader));
		if (blkHeader.rowRecords == 0xff && blkHeader.unknown == 0xffff && blkHeader.row == 0xff)
		{
			//Starts with 0xffffffff, end of pck
			break;
		}
		if (pckFile.eofbit)
		{
			break;
		}
		unsigned int row = blkHeader.row - image.topSkip;
		for (int subRecord = 0; subRecord < blkHeader.rowRecords; subRecord++)
		{
			pck_blk_subheader_t blkSubHeader;
			pckFile.read((char*)&blkSubHeader, sizeof(blkSubHeader));
			unsigned int col = blkSubHeader.column - image.leftSkip;
			blkFile.seekg(blkSubHeader.blkOffset);
			unsigned int imageOffset = row * image.width + col;
			blkFile.read((char*)image.data.data() + imageOffset, blkSubHeader.pixelCount);
		}
	}
}

PCKFile::PCKFile(std::string tabFileName, std::string pckFileName, std::string blkFileName, bool tab4)
{
	std::ifstream tabFile(tabFileName);
	if (!tabFile.good())
	{
		std::cerr << "Tabfile " << tabFileName << " failed to load" << std::endl;
	}
	else
	{
		tabFile.seekg(0, tabFile.end);
		std::cout << "Tabfile length = " << tabFile.tellg() << " bytes\n";
		tabFile.seekg(0, tabFile.beg);
	}
	std::ifstream pckFile(pckFileName);
	if (!pckFile.good())
	{
		std::cerr << "Pckfile " << pckFileName << " failed to load" << std::endl;
	}
	std::ifstream blkFile(blkFileName);
	if (!blkFile.good())
	{
		std::cerr << "Blkfile " << blkFileName << " failed to load" << std::endl;
	}

	while (true)
	{
		std::cout << "Loading image...\n";
		uint32_t offset;
		uint32_t nextOffset;
		tabFile.read((char*)&offset, 4);
		if (!tabFile)
		{
			std::cout << "EOF\n";
			break;
		}
		std::cout << "Offset " << offset << std::endl;
		tabFile.read((char*)&nextOffset, 4);
		if (tab4)
		{
			offset *= 4;
			nextOffset *= 4;
		}
		if (!tabFile)
		{
			std::cout << "EOF reading next...\n";
			nextOffset = 0xffffffff;
		}
		else
		{
			std::cout << "next offset " << nextOffset << std::endl;
		}
		tabFile.seekg(-4, tabFile.cur);
		pckFile.seekg(offset);
		pck_header_t pckHeader;
		pckFile.read((char*)&pckHeader, sizeof(pckHeader));
		if (!pckFile)
		{
			std::cerr << "EOF loading image header\n";
			break;
		}
		if (pckHeader.compressionMode == 0)
		{
			std::cerr << "NULL compression\n";
			continue;
		}
		PCKImage image;
		image.height = pckHeader.bottomClip - pckHeader.topClip;
		image.width = pckHeader.rightClip - pckHeader.leftClip;
		image.topSkip = pckHeader.topClip;
		image.leftSkip = pckHeader.leftClip;
		std::cout << "Image height " << image.height << " width " << image.width << std::endl;
		std::cout << "Compression " << (int)pckHeader.compressionMode << std::endl;
		switch (pckHeader.compressionMode)
		{
			case PCK_COMPRESSION_MODE_RLE:
				image.data.resize(image.height*image.width);
				decodeRLEPCK(pckFile, image, nextOffset);
				break;
			case PCK_COMPRESSION_MODE_BLK:
				image.data.resize(image.height*image.width);
				decodeBLKPCK(pckFile, blkFile, image);
				break;
			default:
				std::cerr << "Unimplemented compression " << (int)pckHeader.compressionMode << std::endl;
				break;
		}
		images.push_back(image);
	}
	std::cout << "Finished loading\n";
}

PCKFile::~PCKFile()
{

}
