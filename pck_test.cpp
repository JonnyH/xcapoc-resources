/*
 * =====================================================================================
 *
 *       Filename:  pck_test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/13 13:20:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jonathan Hamilton (jh), jtrhamilton@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "pck.hpp"
#include "palette.hpp"
#include "image.hpp"
#include <iostream>
#include <unistd.h>
#include <cassert>
#include <fstream>
#include <sstream>

static void printHelp()
{
	std::cout << "Options:\n"
	          << "\"-p pckFileName\" (required)\n"
			  << "\"-t tabFileName\" (required)\n"
			  << "\"-b blkFileName\" (required)\n"
			  << "\"-4\" tab offsets should be multiplied by 4\n"
			  << "\"-d\" dump images to pnm files (requires palette)\n"
			  << "\"-c paletteFileName\" Palette to use for dumping\n";
}

int main(int argc, char **argv)
{
	opterr = 0;
	int c;
	std::string pckFileName;
	std::string tabFileName;
	std::string blkFileName;
	bool tab4 = false;
	bool dump = false;
	std::string paletteFileName;

	const char optionString[] =  "hp:t:b:4dc:";

	c = getopt(argc, argv, optionString);
	while (c != -1)
	{
		switch (c)
		{
			case 'h':
				printHelp();
				return 0;
			case 'p':
				pckFileName.assign(optarg);
				break;
			case 't':
				tabFileName.assign(optarg);
				break;
			case 'b':
				blkFileName.assign(optarg);
				break;
			case '4':
				tab4 = true;
				break;
			case 'd':
				dump = true;
				break;
			case 'c':
				paletteFileName.assign(optarg);
				break;
			case '?':
				std::cerr << "Unknown option:\"" << (char)optopt << "\"\n";
				return 1;
			default:
				std::cerr << "Error parsing option:\":" << (char)c << "\"\n";
		}
		c = getopt(argc, argv, optionString);
	}
	if (pckFileName == "")
	{
		std::cerr << "Must supply pck file name\n";
		printHelp();
		return 1;
	}
	if (tabFileName == "")
	{
		std::cerr << "Must supply tab file name\n";
		printHelp();
		return 1;
	}
	if (blkFileName == "")
	{
		std::cerr << "Must supply blk file name\n";
		printHelp();
		return 1;
	}
	if (paletteFileName == "" && dump)
	{
		std::cerr << "Must supply palette file name when dumping images\n";
		printHelp();
		return 1;
	}
	

	std::cout << "Loading pck = " << pckFileName << std::endl;
	std::cout << "Loading tab = " << tabFileName << std::endl;
	if (tab4)
		std::cout << "\tUsing tab4 format\n";
	std::cout << "Loading blk = " << blkFileName << std::endl;
	PCKFile pckFile(tabFileName, pckFileName, blkFileName, tab4);
	std::cout << "Loaded " << pckFile.images.size() << " images\n";
	if (dump)
	{
		std::cout << "Loading palette \"" << paletteFileName << "\"\n";
		Palette palette(paletteFileName);
		for (unsigned int i = 0; i < pckFile.images.size(); i++)
		{
			std::stringstream outFileName;
			outFileName << pckFileName << i << ".png";
			std::cout << "Writing: \"" << outFileName.str() << std::endl;
			std::ofstream outFile(outFileName.str(), std::ofstream::trunc);
			PaletteImage image = pckFile.images[i].toPaletteImage();

			image.writePNG(outFile, palette);

			outFile.close();
		}
	}
}
