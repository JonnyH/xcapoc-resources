#pragma once

#include "../apoclib/palette.h"
#include "../apoclib/palette_image.h"
#include "../apoclib/image.h"

#include <vector>
#include <string>
#include <memory>

namespace Render {

class Palette
{
public:
	virtual ~Palette() = 0;
};

class Image
{
public:
	virtual ~Image() = 0;
};

class PaletteImage
{
public:
	virtual ~PaletteImage() = 0;
};

class CitymapRenderer;

class CitymapRendererFactory
{
	friend class CitymapRenderer;
public:
	virtual ~CitymapRendererFactory() = 0;
	virtual bool isSupported() = 0;
	virtual CitymapRenderer *create() = 0;
	virtual std::string getName() = 0;
	virtual float getPreference() = 0;
};

class CitymapRenderer
{
	friend class CitymapRendererFactory;
private:
public:
	static std::vector<std::unique_ptr<CitymapRendererFactory> > registeredRenderers;
	virtual ~CitymapRenderer();

	virtual Palette* loadPalette(const ApocRes::Palette &pal) = 0;
	virtual Image* loadImage(const ApocRes::Image &img) = 0;
	virtual PaletteImage* loadPaletteImage(const ApocRes::PaletteImage &img) = 0;

	virtual void setPalette(const Palette &pal,
		const Palette *pal2 = nullptr,
		float paletteMix = 0) = 0;

	/* x/y in pixels (Defines what a 'city' coordinate is) */
	virtual void setTileSize(float x, float y, float z) = 0;
	/* x/y/z in 'city coordinates' */
	virtual void draw(float x, float y, float z, const PaletteImage &pal) = 0;
	virtual void draw(float x, float y, float z, const Image &image) = 0;

	virtual void finishDraw() = 0;
	/* Clears the screen, offset in 'city coordinates' */
	virtual void startDraw(float xScreenOffset, float yScreenOffset) = 0;

	virtual bool supportsPalette() = 0;
	virtual bool supportsPaletteMix() = 0;
	virtual std::string getName() = 0;

	/* Just give me the 'best' renderer available on the system */
	static CitymapRenderer* getRenderer();
	static void registerRenderer(CitymapRendererFactory *fact);


};

}; //namespace Render
