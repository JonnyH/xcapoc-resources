#include <memory>
#include "apoclib/palette_image.h"
#include "apoclib/palette.h"
#include <fstream>
#include <iostream>
#include <cstdint>

using namespace ApocRes;

struct PcxHeader
{
	uint8_t Identifier;      /* PCX Id Number (Always 0x0A) */
	uint8_t Version;         /* Version Number */
	uint8_t Encoding;        /* Encoding Format */
	uint8_t BitsPerPixel;    /* Bits per Pixel */
	uint16_t XStart;         /* Left of image */
	uint16_t YStart;         /* Top of Image */
	uint16_t XEnd;           /* Right of Image */
	uint16_t YEnd;           /* Bottom of image */
	uint16_t HorzRes;        /* Horizontal Resolution */
	uint16_t VertRes;        /* Vertical Resolution */
	uint8_t Palette[48];     /* 16-Color EGA Palette */
	uint8_t Reserved1;       /* Reserved (Always 0) */
	uint8_t NumBitPlanes;    /* Number of Bit Planes */
	uint16_t BytesPerLine;   /* Bytes per Scan-line */
	uint16_t PaletteType;    /* Palette Type */
	uint16_t HorzScreenSize; /* Horizontal Screen Size */
	uint16_t VertScreenSize; /* Vertical Screen Size */
	uint8_t Reserved2[54];   /* Reserved (Always 0) */
};

static const uint8_t PcxIdentifier = 0x0A;

static_assert(sizeof(struct PcxHeader) == 128, "PcxHeader unexpected size");

static png::image<png::rgba_pixel> loadPCX(std::istream &pcxFile)
{
	pcxFile.seekg(0, std::ios::end);
	auto size = pcxFile.tellg();
	pcxFile.seekg(0, std::ios::beg);

	std::cerr << "Size: " << size << "\n";

	if (size < sizeof(struct PcxHeader) + 256 * 3)
	{
		std::cerr << "Size too small for header + palette\n";
		return {1,1};
	}

	struct PcxHeader header;
	pcxFile.read((char*)&header, sizeof(header));

	if (header.Identifier != PcxIdentifier)
	{
		std::cerr << "Wrong identifier " << (int)header.Identifier << " : expected " << (int)PcxIdentifier << "\n";
		return {1,1};
	}

	if (header.Encoding != 1)
	{
		std::cerr << "Unknown encoding " << (int)header.Encoding << "\n";
		return {1,1};
	}

	if (header.BitsPerPixel != 8)
	{
		std::cerr << "Unsupported bits per plane " << (int)header.BitsPerPixel << "\n";
		return {1,1};
	}

	if (header.NumBitPlanes != 1)
	{
		std::cerr << "Unsupported plane count " << (int)header.NumBitPlanes << "\n";
		return {1,1};
	}

	pcxFile.seekg(0, std::ios::end);
	pcxFile.seekg(-(256*3 + 1), std::ios::cur);
	uint8_t b;
	pcxFile.read((char*)&b, 1);
	if (b != 0x0C)
	{
		std::cerr << "Byte at offset (end - 769) not 12 to indicate palette but " << (int)b << "\n";
		return {1,1};
	}

	char palette[256*3];
	pcxFile.read(palette, 256*3);
	pcxFile.seekg(sizeof(struct PcxHeader), std::ios::beg);

	std::unique_ptr<Palette> pal(Palette::loadFromMemory(palette, 256*3, false));

	//FIXUP: pcx index '0' is not transparent
	auto idx0 = pal->getPixel(0);
	idx0.alpha = 255;
	pal->setPixel(0, idx0);



	std::cout << "XStart:\t" << (int)header.XStart << "\n";
	std::cout << "YStart:\t" << (int)header.YStart << "\n";
	std::cout << "XEnd:\t" << (int)header.XEnd << "\n";
	std::cout << "YEnd:\t" << (int)header.YEnd << "\n";
	std::cout << "HRes:\t" << (int)header.HorzRes << "\n";
	std::cout << "VRes:\t" << (int)header.VertRes << "\n";

	PaletteImage img{header.HorzRes, header.VertRes};

	for (int y = header.YStart; y <= header.YEnd; y++)
	{
		int x = header.XStart;
		while (x <= header.XEnd)
		{
			uint8_t b;
			int run_length;
			uint8_t idx;
			pcxFile.read((char*)&b, 1);
			if (!pcxFile)
			{
				std::cerr << "Unexpected EOF\n";
				return {1,1};
			}

			//If the two top bits are set it's a run of the bottom 6 bits
			if ((b & 0xC0) == 0xC0)
			{
				run_length = b & 0x3F;
				pcxFile.read((char*)&idx, 1);
			}
			//Otherwise it's just a single index (I suppose this means you
			//can't use palette indices above 0xC0 outside a run?)
			else
			{
				run_length = 1;
				idx = b;
			}
			while (run_length > 0 && x <= header.XEnd)
			{
				img.setPixel(x, y, idx);
				x++;
				run_length--;
			}
		}
	}

	return img.getPNG(*pal);
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: pcx2pal in.pcx\n";
		return -1;
	}
	auto pcxFile = std::ifstream(argv[1]);
	auto img = loadPCX(pcxFile);
	img.write("out.png");
	return 0;
}
