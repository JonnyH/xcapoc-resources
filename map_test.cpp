/*
 * =====================================================================================
 *
 *       Filename:  map_test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/10/13 11:41:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jonathan Hamilton (jh), jtrhamilton@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "map.hpp"
#include "pck.hpp"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>

static void printHelp()
{
	std::cout << "Options:\n"
	          << "\"-m mapName\" (required) The name of the map within the mapDirectory to load\n"
	          << "\"-d mapDirectory\" (required) The directory containing the map (.smp,.sdt) files\n"
			  << "\"-u mapunitsDirectory\" (required) The directory containing the map units (pck files, palette)\n"
			  << "\"-o\" (optional) Output the loaded map to a PNG image\n"
			  << "\"-z zLevel\" (optional) Specifies the Z level to dump (default 0, the bottom level)\n";
}

int main(int argc, char **argv)
{
	opterr = 0;
	int c;
	std::string mapDirectory;
	std::string mapUnitsDirectory;
	std::string mapName;
	bool dump = false;
	unsigned int dumpZ = 0;

	const char optionString[] = "m:d:u:oz:";

	c = getopt(argc, argv, optionString);

	while (c != -1)
	{
		switch (c)
		{
			case 'h':
				printHelp();
				return 0;
			case 'm':
				mapName.assign(optarg);
				break;
			case 'd':
				mapDirectory.assign(optarg);
				break;
			case 'u':
				mapUnitsDirectory.assign(optarg);
				break;
			case 'o':
				dump = true;
				break;
			case 'z':
				dumpZ = strtoul(optarg, NULL, 10);
				break;
			case '?':
				std::cerr << "Unknown option:\"" << (char)optopt << "\"\n";
				return 1;
			default:
				std::cerr << "Error parsing option:\"" << (char)c << "\"\n";
				return 1;
		}
		c = getopt(argc, argv, optionString);
	}
	if (mapDirectory == "")
	{
		std::cerr << "Must supply map directory\n";
		printHelp();
		return 1;
	}
	if (mapUnitsDirectory == "")
	{
		std::cerr << "Must supply mapUnits directory\n";
		printHelp();
		return 1;
	}
	if (mapName == "")
	{
		std::cerr << "Must supply map name\n";
		printHelp();
		return 1;
	}

	std::ifstream sdtFile(mapDirectory + "/" + mapName + ".sdt");
	std::ifstream smpFile(mapDirectory + "/" + mapName + ".smp");
	Map map(sdtFile, smpFile);
	if (dump)
	{
		PCKFile floorPCK(mapUnitsDirectory + "/ground.tab", mapUnitsDirectory + "/ground.pck", "xcom.blk", true);
		Palette palette(mapUnitsDirectory + "/palette.pal");
		std::cout << "Total image size: " << map.x * FLOOR_TILE_WIDTH << " x " << map.y * FLOOR_TILE_HEIGHT << "\n";
		png::image<png::rgba_pixel> image(map.x * FLOOR_TILE_WIDTH + 100, map.y * FLOOR_TILE_HEIGHT + 100);
		unsigned int startX = FLOOR_TILE_WIDTH*map.x/2;
		unsigned int startY = 0;
		for (int y = 0; y < map.y; y++)
		{
			for (int x = 0; x < map.x; x++)
			{
				unsigned int XDrawOffset = startX + x*FLOOR_TILE_WIDTH/2;
				XDrawOffset -= y*FLOOR_TILE_WIDTH/2;
				unsigned int YDrawOffset = (x+y)*FLOOR_TILE_HEIGHT/2;
				unsigned int floorTileIndex = map.get(x, y, dumpZ).floorIdx;
				if (floorTileIndex > floorPCK.images.size())
				{
					std::cerr << "Floor index " << floorTileIndex << " larger than pck size (" << floorPCK.images.size() << ")\n";
					floorTileIndex = 0;
				}
				PCKImage &floorImage = floorPCK.images[floorTileIndex];
				floorImage.toPaletteImage().drawAt(image, XDrawOffset, YDrawOffset, palette);
				
			}
		}
		image.write(mapName + ".png");
	}
}
