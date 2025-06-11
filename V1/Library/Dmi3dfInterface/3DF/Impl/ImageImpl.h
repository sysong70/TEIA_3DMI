#include "../3DF.h"
#include "../Image.h"
#include "../Kit.h"
#include "../Math.h"

#include "DefinitionImpl.h"

namespace H3DF
{
	class ImageImportOptionsKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ImageImportOptionsKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ImageImportOptionsKitImpl * pcInThat) {
			m_nWidth = pcInThat->m_nWidth;
			m_nHeight = pcInThat->m_nHeight;
			m_eFormat = pcInThat->m_eFormat;
		}

		UINT m_nWidth = 0;
		UINT m_nHeight = 0;

		Image::Format m_eFormat = Image::Format::None;
	};

	class ImageKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ImageKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ImageKitImpl * pcInThat) {
			m_nWidth = pcInThat->m_nWidth;
			m_nHeight = pcInThat->m_nHeight;
			m_eFormat = pcInThat->m_eFormat;
			m_arImageData = pcInThat->m_arImageData;
			m_strInformation = pcInThat->m_strInformation;
		}

		bool Equals(const ImageKitImpl * pcInThat) const {
			return (m_nWidth == pcInThat->m_nWidth &&
				m_nHeight == pcInThat->m_nHeight &&
				m_eFormat == pcInThat->m_eFormat &&
				m_arImageData == pcInThat->m_arImageData &&
				m_strInformation == pcInThat->m_strInformation);
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

	class ImageDefinitionImpl : public DefinitionImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ImageDefinitionImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ImageDefinitionImpl * pcInThat) {
			m_strSource = pcInThat->m_strSource;
			m_cImageKit = pcInThat->m_cImageKit; // Copy
		}

		bool Equals(const ImageDefinitionImpl * pcInThat) const {
			return (m_strSource == pcInThat->m_strSource &&
				m_cImageKit == pcInThat->m_cImageKit);
		}

		CStringA m_strSource;
		ImageKit m_cImageKit;
	};
}

