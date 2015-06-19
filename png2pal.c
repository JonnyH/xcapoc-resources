#include <png.h>
#include <stdio.h>
#include <stdlib.h>

/* If set, reduces the range to 6 bits (divides png-stored values by 4) */
#define VGA_RANGE

void print_usage(void)
{
	fprintf(stderr, "Usage: png2pal input.png output.pal\n");
}

void user_error_fn(png_structp png_ptr, const char *msg)
{
	fprintf(stderr, "PNG error: \"%s\"\n", msg);
	abort();
}

void user_warning_fn(png_structp png_ptr, const char *msg)
{
	fprintf(stderr, "PNG warning: \"%s\"\n", msg);
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		print_usage();
		return EXIT_FAILURE;
	}
	FILE *fp = fopen(argv[1], "rb");
	if (!fp)
	{
		fprintf(stderr, "Failed to open input file: \"%s\"\n", argv[1]);
		return EXIT_FAILURE;
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		(png_voidp)NULL,
		user_error_fn,
		user_warning_fn);

	if(!png_ptr)
	{
		fprintf(stderr, "Failed to create png_ptr\n");
		return EXIT_FAILURE;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
	{
		fprintf(stderr, "Failed to create info_ptr\n");
		return EXIT_FAILURE;
	}

	png_init_io(png_ptr, fp);

	png_read_info(png_ptr, info_ptr);

	int bit_depth, color_type;

	png_get_IHDR(png_ptr, info_ptr, NULL, NULL, &bit_depth, &color_type, NULL, NULL, NULL);

	if ((color_type & PNG_COLOR_MASK_PALETTE) == 0)
	{
		fprintf(stderr, "Image \"%s\" does not have a palette (color type = %d)\n", argv[1], color_type);
		return EXIT_FAILURE;
	}

	int palette_size;
	switch (bit_depth)
	{
		default:
			fprintf(stderr, "Image \"%s\" has unsupported palette size %d - assuming 8 bit\n", argv[1], bit_depth);
		case 8:
			palette_size = 256;
			break;
	}
	png_color *pal;
	int returned_palette_size = 0;
	png_get_PLTE(png_ptr, info_ptr, &pal, &returned_palette_size);
	fprintf(stdout, "Got %d palette entries\n", returned_palette_size);
	FILE *pal_out = fopen(argv[2], "wb");
	if (!pal_out)
	{
		fprintf(stderr, "Failed to open output file \"%s\"\n", argv[2]);
		return EXIT_FAILURE;
	}
	int x;
	for (x = 0; x < palette_size; x++)
	{
		unsigned char b = 0;
		b = pal[x].red;
#ifdef VGA_RANGE
	if (b%4)
		fprintf(stderr, "idx %d: R value %d isn't 6-bit?\n", x, (int)b);
		b /= 4;
#endif
		fwrite(&b, 1, 1, pal_out);
		b = pal[x].green;
#ifdef VGA_RANGE
	if (b%4)
		fprintf(stderr, "idx %d: G value %d isn't 6-bit?\n", x, (int)b);
		b /= 4;
#endif
		fwrite(&b, 1, 1, pal_out);
		b = pal[x].blue;
#ifdef VGA_RANGE
	if (b%4)
		fprintf(stderr, "idx %d: B value %d isn't 6-bit?\n", x, (int)b);
		b /= 4;
#endif
		fwrite(&b, 1, 1, pal_out);
	}
	fclose(pal_out);
	fclose(fp);
}
