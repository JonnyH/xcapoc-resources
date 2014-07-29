#include "render/citymap_render.h"
#include "apoclib/citymap.h"
#include "apoclib/pck.h"
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <SDL2/SDL.h>

static void print_usage()
{
	std::cout << "Usage: citymap_render_test resource_dir\n"
		<< "'resource_dir' being the extracted/mounted XCOM apoc iso 'xcom3' directory\n";
}

int main(int argc, char **argv)
{
	bool quit = false;
	if (argc != 2)
	{
		print_usage();
		return EXIT_FAILURE;
	}
	int viewOffsetX = 0;
	int viewOffsetY = 0;
	std::string dirPrefix = std::string(argv[1]);
	std::string citymapFileName = dirPrefix + "/ufodata/citymap1";
	std::ifstream citymapFile(citymapFileName, std::ios::in);
	if (!citymapFile)
	{
		std::cerr << "Failed to open city map \"" << citymapFileName << "\"\n";
		return EXIT_FAILURE;
	}
	std::unique_ptr<ApocRes::CityMap> city(ApocRes::CityMap::loadFromFile(citymapFile, "citymap1"));

	std::string dayPalFileName = dirPrefix + "/ufodata/pal_01.dat";
	std::ifstream dayPalFile(dayPalFileName, std::ios::in);
	if (!dayPalFile)
	{
		std::cerr << "Failed to open city palette \"" << dayPalFileName << "\"\n";
		return EXIT_FAILURE;
	}
	std::unique_ptr<ApocRes::Palette> dayPal(ApocRes::Palette::loadFromFile(dayPalFile));

	std::string imgPckFileName = dirPrefix + "/ufodata/city.pck";
	std::ifstream imgPckFile(imgPckFileName, std::ios::in);
	if (!imgPckFile)
	{
		std::cerr << "Failed to PCK file \"" << imgPckFileName << "\"\n";
		return EXIT_FAILURE;
	}

	std::string imgTabFileName = dirPrefix + "/ufodata/city.tab";
	std::ifstream imgTabFile(imgTabFileName, std::ios::in);
	if (!imgTabFile)
	{
		std::cerr << "Failed to TAB file \"" << imgTabFileName << "\"\n";
		return EXIT_FAILURE;
	}
	std::unique_ptr<ApocRes::PCK> pck(ApocRes::PCK::loadFromFile(imgPckFile, imgTabFile));


	SDL_Init(0);

	{

		std::unique_ptr<Render::CitymapRenderer> renderer(Render::CitymapRenderer::getRenderer());
		//FIXME: EVIL way of tracking renderer resources!!
		std::unordered_map<ApocRes::PaletteImage*, std::unique_ptr<Render::PaletteImage> > renderImages;
		std::unordered_map<ApocRes::Palette*, std::unique_ptr<Render::Palette> > renderPalettes;


		std::cout << "Using renderer:\"" << renderer->getName() << "\"\n";

		assert(renderer->supportsPalette());
		assert(renderer->supportsPaletteMix());

		while (!quit)
		{
			SDL_Event ev;
			while (SDL_PollEvent(&ev))
			{
				switch (ev.type)
				{
					case SDL_QUIT:
						quit = true;
						break;
					case SDL_KEYDOWN:
						switch(ev.key.keysym.sym)
						{
							case SDLK_UP:
								viewOffsetX--;
								viewOffsetY--;
								break;
							case SDLK_DOWN:
								viewOffsetX++;
								viewOffsetY++;
								break;
							case SDLK_LEFT:
								viewOffsetX--;
								viewOffsetY++;
								break;
							case SDLK_RIGHT:
								viewOffsetX++;
								viewOffsetY--;
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
			}
		}

		//Render
		if (renderPalettes.find(dayPal.get()) == renderPalettes.end())
			renderPalettes[dayPal.get()] =std::unique_ptr<Render::Palette>(renderer->loadPalette(*dayPal));
		renderer->setTileSize(64, 32, 12);
		renderer->setPalette(*renderPalettes[dayPal.get()]);
		renderer->startDraw(viewOffsetX, viewOffsetY);
		for (int z = 0; z < city->sizeZ; z++)
		{
			for (int y = 0; y < city->sizeY; y++)
			{
				for (int x = 0; x < city->sizeX; x++)
				{
					int idx = city->tiles[x][y][z];
					if (!idx)
						continue;
					if (renderImages.find(&pck->images[idx]) == renderImages.end())
						renderImages[&pck->images[idx]] = std::unique_ptr<Render::PaletteImage>(renderer->loadPaletteImage(pck->images[idx]));


					renderer->draw(x, y, z, *renderImages[&pck->images[idx]]);
				}
			}
		}
		renderer->finishDraw();

	}
	SDL_Quit();
}
