#pragma once

#include "3DF.h"
#include "Kit.h"
#include "Definition.h"
#include "Math.h"

namespace H3DF
{
	class ImageKit;

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
			DXT5,				//!< Compressed, DXT5 Format.
			None
		};

		class API_3DF ImportOptionsKit : public Kit
		{
		public:
			ImportOptionsKit();
			ImportOptionsKit(ImportOptionsKit const & cInKit);

			static const H3DF::Type staticType = H3DF::Type::ImageImportOptionsKit;
			H3DF::Type ObjectType() const { return staticType; };

			ImportOptionsKit const & operator = (ImportOptionsKit const & cInKit);

			void Show(ImportOptionsKit & cOutKit) const;

			bool Empty() const;

			bool operator == (ImportOptionsKit const & cInKit) const;
			bool operator != (ImportOptionsKit const & cInKit) const;

			ImportOptionsKit & SetSize(UINT nInWidth, UINT nInHeight);
			ImportOptionsKit & SetFormat(Image::Format eInFormat);

			ImportOptionsKit & UnsetSize();
			ImportOptionsKit & UnsetFormat();
			ImportOptionsKit & UnsetEverything();

			bool ShowSize(UINT & nOutWidth, UINT & nOutHeight) const;
			bool ShowFormat(Image::Format & eOutFormat) const;
		};

		class API_3DF File
		{
		public:
			static ImageKit Import(CString strFilePathName, ImportOptionsKit const & cInOptions);

		private:
			File();
		};

	private:
		Image() = default;
	};

	class API_3DF ImageKit : public Kit
	{
	public:
		ImageKit();
		ImageKit(ImageKit const & cInKit);
		ImageKit(ImageKit const & cInKit, H3DF::Image::Format eInFormat);

		static const H3DF::Type staticType = H3DF::Type::ImageKit;
		H3DF::Type ObjectType() const { return staticType; };

		ImageKit(ImageKit && cInThat) noexcept;
		ImageKit & operator = (ImageKit && cInThat) noexcept;

		ImageKit const & operator = (ImageKit const & cInKit);

		void Show(ImageKit & cOutKit) const;

		bool Empty() const;

		bool operator == (ImageKit const & cInKit) const;
		bool operator != (ImageKit const & cInKit) const;

		ImageKit & SetSize(UINT nInWidth, UINT nInHeight);
		ImageKit & SetData(ByteArray const & arInImageData);
		ImageKit & SetData(size_t nByteCount, byte const pInImageData[]);

		ImageKit & UnsetFormat();
		ImageKit & UnsetEverything();

		bool ShowSize(UINT & nOutWidth, UINT & nOutheight) const;
		bool ShowData(ByteArray & arOutImageData) const;
		bool ShowFormat(Image::Format & cOutFormat) const;
	};

	class API_3DF ImageDefinition : public Definition {
	public:
		ImageDefinition();
		ImageDefinition(Definition const & cInThat);
		ImageDefinition(ImageDefinition const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::ImageDefinition;
		H3DF::Type ObjectType() const { return staticType; };

		ImageDefinition(ImageDefinition && cInThat) noexcept;
		ImageDefinition & operator = (ImageDefinition && cInThat) noexcept;

		ImageDefinition const & operator = (ImageDefinition const & cInKit);

		void Set(ImageKit const & cInKit);
		void Show(ImageKit & cOutKit) const;
	};
}