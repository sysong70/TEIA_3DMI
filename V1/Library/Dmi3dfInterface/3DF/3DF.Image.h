#pragma once

#include "3DF.h"
#include "Math.h"

OPEN_3DF_NAMESPACE

class API_3DF Image
{
public:
	/*! \enum Format
	 * 	Enumerates the various image formats that Visualize supports. */
	enum class Format : unsigned int
	{
		RGB,				//!< Uncompressed, one unsigned byte per channel, three unsigned bytes per pixel, RGB order.
		RGBA,				//!< Uncompressed, one unsigned byte per channel, four unsigned bytes per pixel, RGBA order.
		ARGB,				//!< Uncompressed, one unsigned byte per channel, four unsigned bytes per pixel, ARGB order.
		Mapped8,			//!< Uncompressed, one unsigned byte per pixel, each unsigned byte is an index into a material palette.
		Grayscale,			//!< Uncompressed, one unsigned byte per pixel.
		Bmp,				//!< Uncompressed, 1bpp (Mono), 8 bpp (Greyscale), 16 bpp (ARGB/RGB), 24bpp (RGB), 32bpp (ARGB).
		Jpeg,				//!< Compressed, JPEG format.
		Png,				//!< Compressed, PNG format.
		Targa,				//!< Compressed, TARGA Format.
		DXT1,				//!< Compressed, DXT1 Format.
		DXT3,				//!< Compressed, DXT3 Format.
		DXT5				//!< Compressed, DXT5 Format.
	};
private:
	Image() {}
};

class API_3DF ImageKit
{
public:
	ImageKit() {} ;

// 	ImageKit & SetSize(unsigned int in_width, unsigned int in_height);
// 	ImageKit & SetData(ByteArray const & in_image_data);
// 	ImageKit & SetData(size_t in_byte_count, byte const in_image_data[]);
// 	ImageKit & SetFormat(Image::Format in_format);
};

CLOSE_3DF_NAMESPACE