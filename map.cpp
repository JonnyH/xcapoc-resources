/*
 * =====================================================================================
 *
 *       Filename:  map.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/10/13 12:01:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jonathan Hamilton (jh), jtrhamilton@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "map.hpp"

#include <iostream>
#include <cassert>

//FIXME: Order of x/y/z guessed
typedef struct
{
	uint32_t XChunks;
	uint32_t YChunks;
	uint32_t ZChunks;
	uint32_t unknown1;
	uint32_t unknown2;
} sdt_file_t;
static_assert(sizeof(sdt_file_t) == 20, "sizeof(sdt_file_t) != 20");



Map::Map(std::ifstream &sdtInput, std::ifstream &smpInput)
{
	sdt_file_t sdt;
	sdtInput.read((char*)&sdt, sizeof(sdt));
	if (!sdtInput)
	{
		std::cerr << "Error loading sdt file\n";
		return;
	}

	std::cout << "Loading " << sdt.XChunks << "x" << sdt.YChunks << "x" << sdt.ZChunks << "chunks\n";
	std::cout << "\tUnknown1 = " << sdt.unknown1 << "\n\tUnknown2 = " << sdt.unknown2 << "\n";

	this->x = sdt.XChunks * XChunkSize;
	this->y = sdt.YChunks * YChunkSize;
	this->z = sdt.ZChunks * ZChunkSize;

	this->data.reset( new smp_unit_t[this->x*this->y*this->z]);

	smpInput.read((char*)this->data.get(), this->x * this->y * this->z * sizeof(smp_unit_t));
	if (!smpInput)
	{
		std::cerr << "Failed loading smp file data\n";
		return;
	}
}

smp_unit_t
Map::get(unsigned int x, unsigned int y, unsigned int z)
{
	assert(x < this->x);
	assert(y < this->y);
	assert(z < this->z);

	//FIXME: smp ordering of x/y/z guessed
	unsigned int ZStride = this->x * this->y;
	unsigned int offset = z*(ZStride) + y*this->y + x;
	return this->data[offset];
}
