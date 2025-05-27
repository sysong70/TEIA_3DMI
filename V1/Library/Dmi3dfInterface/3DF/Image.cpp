#include "StdAfx.h"

#include "Image.h"
#include "Impl/ImageImpl.h"

#include "Object.h"

#include <Path.h>

#include <hc.h>
#include <HIOUtilityMagick.h>

using namespace H3DF;

//== Image ImportOptionsKit class ==================================================================

H3DF::Image::ImportOptionsKit::ImportOptionsKit()
{
	m_pcImpl = new ImageImportOptionsKitImpl();
}

H3DF::Image::ImportOptionsKit::ImportOptionsKit(ImportOptionsKit const & cInKit)
{
	m_pcImpl = new ImageImportOptionsKitImpl();
	Set(cInKit);
}

void H3DF::Image::ImportOptionsKit::Set(ImportOptionsKit const & cInKit)
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	ImageImportOptionsKitImpl * ccInKitImpl = static_cast<ImageImportOptionsKitImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(ccInKitImpl);

	pcImpl->Copy(ccInKitImpl);
}

Image::ImportOptionsKit const & H3DF::Image::ImportOptionsKit::operator = (ImportOptionsKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

void H3DF::Image::ImportOptionsKit::Show(ImportOptionsKit & cOutKit) const
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	ImageImportOptionsKitImpl * pcOutKitImpl = static_cast<ImageImportOptionsKitImpl *>(cOutKit.m_pcImpl);
	DEBUG_VALID(pcOutKitImpl);

	pcOutKitImpl->Copy(pcImpl);
}

bool H3DF::Image::ImportOptionsKit::Empty() const
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (pcImpl->m_eFormat != Image::Format::None) {
		return false;
	}

	return true;
}

bool H3DF::Image::ImportOptionsKit::operator == (ImportOptionsKit const & cInKit) const
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	ImageImportOptionsKitImpl * pccInKitImpl = static_cast<ImageImportOptionsKitImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pccInKitImpl);

	if (pcImpl->m_eFormat != pccInKitImpl->m_eFormat) {
		return false;
	}

	return true;
}

bool H3DF::Image::ImportOptionsKit::operator != (ImportOptionsKit const & cInKit) const
{
	return !(*this == cInKit);
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::SetSize(UINT nInWidth, UINT nInHeight)
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = nInWidth;
	pcImpl->m_nHeight = nInHeight;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::SetFormat(Image::Format eInFormat)
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_eFormat = eInFormat;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::UnsetSize()
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = 0;
	pcImpl->m_nHeight = 0;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::UnsetFormat()
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_eFormat = Image::Format::None;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::UnsetEverything()
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = 0;
	pcImpl->m_nHeight = 0;
	pcImpl->m_eFormat = Image::Format::None;

	return *this;
}

bool H3DF::Image::ImportOptionsKit::ShowSize(UINT & nOutWidth, UINT & nOutHeight) const
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	if (0 == pcImpl->m_nWidth || 0 == pcImpl->m_nHeight) {
		return false;
	}

	nOutWidth = pcImpl->m_nWidth;
	nOutHeight = pcImpl->m_nHeight;

	return true;
}

bool H3DF::Image::ImportOptionsKit::ShowFormat(Image::Format & eOutFormat) const
{
	ImageImportOptionsKitImpl * pcImpl = static_cast<ImageImportOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	if (Image::Format::None == pcImpl->m_eFormat) {
		return false;
	}

	eOutFormat = pcImpl->m_eFormat;

	return true;
}

//== Image Kit class ===============================================================================
H3DF::ImageKit::ImageKit()
{
	m_pcImpl = new ImageKitImpl();
}

H3DF::ImageKit::ImageKit(ImageKit const & cInKit)
{
	m_pcImpl = new ImageKitImpl();
	Set(cInKit);
}

H3DF::ImageKit::ImageKit(ImageKit const & cInKit, H3DF::Image::Format eInFormat)
{
	m_pcImpl = new ImageKitImpl();
	Set(cInKit);
}

H3DF::ImageKit::ImageKit(ImageKit && cInThat) noexcept :
	Kit(std::move(cInThat))
{
}

ImageKit & H3DF::ImageKit::operator = (ImageKit && cInThat) noexcept
{
	this->Kit::operator = (std::move(cInThat));
	return *this;
}


void H3DF::ImageKit::Set(ImageKit const & cInKit)
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	ImageKitImpl * pcInKitImpl = static_cast<ImageKitImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pcInKitImpl);

	pcImpl->Copy(pcInKitImpl);

}

ImageKit const & H3DF::ImageKit::operator = (ImageKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

void H3DF::ImageKit::Show(ImageKit & cOutKit) const
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	ImageKitImpl * pcOutKitImpl = static_cast<ImageKitImpl *>(cOutKit.m_pcImpl);
	DEBUG_VALID(pcOutKitImpl);

	pcOutKitImpl->Copy(pcImpl);
}

bool H3DF::ImageKit::Empty() const
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	return true;
}

bool H3DF::ImageKit::operator == (ImageKit const & cInKit) const
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	ImageKitImpl * pccInKitImpl = static_cast<ImageKitImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pccInKitImpl);

	return true;
}

bool H3DF::ImageKit::operator != (ImageKit const & cInKit) const
{
	return !(*this == cInKit);
}

ImageKit & H3DF::ImageKit::SetSize(UINT nInWidth, UINT nInHeight)
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = nInWidth;
	pcImpl->m_nHeight = nInHeight;

	return *this;
}

ImageKit & H3DF::ImageKit::SetData(ByteArray const & arInImageData)
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_arImageData = arInImageData;

	return *this;
}

ImageKit & H3DF::ImageKit::SetData(size_t nByteCount, byte const pInImageData[])
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_arImageData.clear();
	pcImpl->m_arImageData.insert(pcImpl->m_arImageData.end(), pInImageData, pInImageData + nByteCount);

	return *this;
}

ImageKit & H3DF::ImageKit::UnsetFormat()
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	pcImpl->m_eFormat = Image::Format::None;

	return *this;
}

ImageKit & H3DF::ImageKit::UnsetEverything()
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	pcImpl->m_nWidth = 0;
	pcImpl->m_nHeight = 0;
	pcImpl->m_eFormat = Image::Format::None;
	pcImpl->m_arImageData.clear();

	return *this;
}

bool H3DF::ImageKit::ShowSize(UINT & nOutWidth, UINT & nOutheight) const
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	if (0 == pcImpl->m_nWidth || 0 == pcImpl->m_nHeight) {
		return false;
	}
	nOutWidth = pcImpl->m_nWidth;
	nOutheight = pcImpl->m_nHeight;

	return true;
}

bool H3DF::ImageKit::ShowData(ByteArray & arOutImageData) const
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);
	if (pcImpl->m_arImageData.empty()) {
		return false;
	}

	arOutImageData = pcImpl->m_arImageData;

	return true;
}

bool H3DF::ImageKit::ShowFormat(Image::Format & cOutFormat) const
{
	ImageKitImpl * pcImpl = static_cast<ImageKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (Image::Format::None == pcImpl->m_eFormat) {
		return false;
	}

	cOutFormat = pcImpl->m_eFormat;
	return true;
}

//== Image File class ==============================================================================
H3DF::Image::File::File()
{

}

ImageKit H3DF::Image::File::Import(CString strFilePathName, ImportOptionsKit const & cInOptions)
{
	ImageKit cOutKit;

	// FileToImage 함수를 호출하여 ImageKit에 정보를 저장한다.
	((ImageKitImpl *)cOutKit.GetImpl())->FileToImage(strFilePathName, cInOptions);

	return std::move(cOutKit);
}


//== ImageDefinition class =========================================================================
H3DF::ImageDefinition::ImageDefinition()
{
	m_pcImpl = new ImageDefinitionImpl();
}

H3DF::ImageDefinition::ImageDefinition(Definition const & cInThat)
{
	if (H3DF::Type::ImageDefinition != cInThat.Type())
	{
		DEBUG_STOP;
		return;
	}

	m_pcImpl = new ImageDefinitionImpl();
	Set(cInThat);
}

H3DF::ImageDefinition::ImageDefinition(ImageDefinition const & cInThat)
{
	m_pcImpl = new ImageDefinitionImpl();
	Set(cInThat);
}

H3DF::ImageDefinition::ImageDefinition(ImageDefinition && cInThat) noexcept :
	Definition(cInThat)
{
}

ImageDefinition & H3DF::ImageDefinition::operator = (ImageDefinition && cInThat) noexcept
{
	this->ImageDefinition::operator = (cInThat);
	return *this;
}

void H3DF::ImageDefinition::Set(ImageDefinition const & cInKit)
{
	ImageDefinitionImpl * pcImpl = static_cast<ImageDefinitionImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	ImageDefinitionImpl * pcInKitImpl = static_cast<ImageDefinitionImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pcInKitImpl);

	pcImpl->Set(pcInKitImpl);
}

ImageDefinition const & H3DF::ImageDefinition::operator = (ImageDefinition const & cInKit)
{
	Set(cInKit);
	return *this;
}

void H3DF::ImageDefinition::Set(ImageKit const & cInKit)
{
	ImageDefinitionImpl * pcImpl = static_cast<ImageDefinitionImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_cImageKit = cInKit;
}

void H3DF::ImageDefinition::Show(ImageKit & cOutKit) const
{
	ImageDefinitionImpl * pcImpl = static_cast<ImageDefinitionImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutKit = pcImpl->m_cImageKit;
}
