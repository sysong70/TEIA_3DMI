#include "../3DF.h"
#include "../Image.h"
#include "../Kit.h"
#include "../Math.h"

namespace H3DF
{
	class ImageImportOptionsKitImpl : public Impl
	{
	public:
		ImageImportOptionsKitImpl() { m_eType = H3DF::Type::ImageImportOptionsKit; }

		void Copy(ImageImportOptionsKitImpl * pcInThat) {
			m_nWidth = pcInThat->m_nWidth;
			m_nHeight = pcInThat->m_nHeight;
			m_eFormat = pcInThat->m_eFormat;
		}

		UINT m_nWidth = 0;
		UINT m_nHeight = 0;

		Image::Format m_eFormat = Image::Format::None;
	};

	class HImageInfo {
	public:
		char * m_data;
		int m_len, m_width, m_height;
		char m_format[35];

		HImageInfo() : m_data(0), m_width(-1), m_height(-1) { strcpy(m_format, ""); }

		~HImageInfo()
		{
			if (m_data)
				free(m_data);
		}
	};

	class ImageKitImpl : public Impl
	{
	public:
		ImageKitImpl() { m_eType = H3DF::Type::ImageKit; }

		void Copy(ImageKitImpl * pcInThat) {
			m_nWidth = pcInThat->m_nWidth;
			m_nHeight = pcInThat->m_nHeight;
			m_eFormat = pcInThat->m_eFormat;
			m_arImageData = pcInThat->m_arImageData;
		}

		UINT m_nWidth = 0;
		UINT m_nHeight = 0;
		UINT m_nLength = 0;

		Image::Format m_eFormat = Image::Format::None;

		ByteArray m_arImageData;
		
		CString m_strInformation;

		void Reset();

		bool FileToImage(CString strFilePathName, Image::ImportOptionsKit const & cInOptions);
		bool OutsourceToImageMagick(size_t nInCount, CStringA strInImplicitFormat);
	};
}

