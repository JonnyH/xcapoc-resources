/*
 * =====================================================================================
 *
 *       Filename:  palette.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/10/13 12:39:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jonathan Hamilton (jh), jtrhamilton@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "palette.hpp"

#include <fstream>
#include <iostream>

Palette::Palette()
{

}

Palette::Palette(const std::string fileName)
{
	unsigned char paletteData[256][3];
	static_assert(sizeof(paletteData) == 768, "sizeof(paletteData) != 768");
	std::ifstream inFile(fileName);
	inFile.read((char*)&paletteData[0][0], sizeof(paletteData));
	if (!inFile)
	{
		std::cerr << "Failed to load palette file\n";
		return;
	}
	for (int i = 0; i < 256; i++)
	{
		this->palette[i] = Color(paletteData[i][0], paletteData[i][1],
		                         paletteData[i][2], 255);
	}
	//Palette index 0 is transparent
	this->palette[0] = Color(0,0,0,0);
	//Last 16 colors are shades of gray
	//TODO: Check this is correct - values stolen from UFO
	unsigned char grayValues[16][3] = 
	{
		{0x8c, 0x96, 0x94},
		{0x84, 0x8a, 0x8c},
		{0x73, 0x7d, 0x84},
		{0x6b, 0x75, 0x7b},
		{0x5a, 0x69, 0x6b},
		{0x52, 0x5d, 0x63},
		{0x4a, 0x51, 0x5a},
		{0x39, 0x45, 0x52},
		{0x31, 0x38, 0x42},
		{0x29, 0x30, 0x39},
		{0x21, 0x24, 0x31},
		{0x18, 0x1c, 0x21},
		{0x10, 0x14, 0x18},
		{0x08, 0x0c, 0x10},
		{0x00, 0x04, 0x08},
		{0x00, 0x00, 0x00},
	};
	for (int i = 240; i < 256; i++)
	{
		this->palette[i] = Color(grayValues[i-240][0],
		                         grayValues[i-240][1],
								 grayValues[i-240][2],
								 255);
	}
}
