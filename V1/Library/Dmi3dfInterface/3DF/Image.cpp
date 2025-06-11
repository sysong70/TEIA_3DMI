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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ImageImportOptionsKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Image::ImportOptionsKit::ImportOptionsKit(ImportOptionsKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.GetImpl()) ? cInKit.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

Image::ImportOptionsKit const & H3DF::Image::ImportOptionsKit::operator = (ImportOptionsKit const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::Image::ImportOptionsKit::Show(ImportOptionsKit & cOutKit) const
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutKit.m_pcImpl = pcImpl->Clone();
	DEBUG_VALID(cOutKit.m_pcImpl);
}

bool H3DF::Image::ImportOptionsKit::Empty() const
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (pcImpl->m_eFormat != Image::Format::None) {
		return false;
	}

	return true;
}

bool H3DF::Image::ImportOptionsKit::operator == (ImportOptionsKit const & cInKit) const
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	ImageImportOptionsKitImpl * pccInKitImpl = static_cast<ImageImportOptionsKitImpl *>(cInKit.m_pcImpl.get());
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
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = nInWidth;
	pcImpl->m_nHeight = nInHeight;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::SetFormat(Image::Format eInFormat)
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eFormat = eInFormat;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::UnsetSize()
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = 0;
	pcImpl->m_nHeight = 0;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::UnsetFormat()
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eFormat = Image::Format::None;

	return *this;
}

Image::ImportOptionsKit & H3DF::Image::ImportOptionsKit::UnsetEverything()
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = 0;
	pcImpl->m_nHeight = 0;
	pcImpl->m_eFormat = Image::Format::None;

	return *this;
}

bool H3DF::Image::ImportOptionsKit::ShowSize(UINT & nOutWidth, UINT & nOutHeight) const
{
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<ImageImportOptionsKitImpl *>(m_pcImpl.get());
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ImageKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ImageKit::ImageKit(ImageKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.GetImpl()) ? cInKit.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::ImageKit::ImageKit(ImageKit const & cInKit, H3DF::Image::Format eInFormat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.GetImpl()) ? cInKit.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eFormat = eInFormat;
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

ImageKit const & H3DF::ImageKit::operator = (ImageKit const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ImageKit::Show(ImageKit & cOutKit) const
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutKit.m_pcImpl = pcImpl->Clone();
	DEBUG_VALID(cOutKit.m_pcImpl);
}

bool H3DF::ImageKit::Empty() const
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return true;
}

bool H3DF::ImageKit::operator == (ImageKit const & cInKit) const
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInKitImpl = static_cast<ImageKitImpl *>(cInKit.m_pcImpl.get());
	DEBUG_VALID(pcInKitImpl);

	return pcImpl->Equals(pcInKitImpl);
}

bool H3DF::ImageKit::operator != (ImageKit const & cInKit) const
{
	return !(*this == cInKit);
}

ImageKit & H3DF::ImageKit::SetSize(UINT nInWidth, UINT nInHeight)
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWidth = nInWidth;
	pcImpl->m_nHeight = nInHeight;

	return *this;
}

ImageKit & H3DF::ImageKit::SetData(ByteArray const & arInImageData)
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arImageData = arInImageData;

	return *this;
}

ImageKit & H3DF::ImageKit::SetData(size_t nByteCount, byte const pInImageData[])
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arImageData.clear();
	pcImpl->m_arImageData.insert(pcImpl->m_arImageData.end(), pInImageData, pInImageData + nByteCount);

	return *this;
}

ImageKit & H3DF::ImageKit::UnsetFormat()
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->m_eFormat = Image::Format::None;

	return *this;
}

ImageKit & H3DF::ImageKit::UnsetEverything()
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->m_nWidth = 0;
	pcImpl->m_nHeight = 0;
	pcImpl->m_eFormat = Image::Format::None;
	pcImpl->m_arImageData.clear();

	return *this;
}

bool H3DF::ImageKit::ShowSize(UINT & nOutWidth, UINT & nOutheight) const
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	if (pcImpl->m_arImageData.empty()) {
		return false;
	}

	arOutImageData = pcImpl->m_arImageData;

	return true;
}

bool H3DF::ImageKit::ShowFormat(Image::Format & cOutFormat) const
{
	auto pcImpl = static_cast<ImageKitImpl *>(m_pcImpl.get());
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ImageDefinitionImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ImageDefinition::ImageDefinition(Definition const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<ImageDefinitionImpl>();
	auto pcImpl = static_cast<ImageDefinitionImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const DefinitionImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->DefinitionImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::ImageDefinition::ImageDefinition(ImageDefinition const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
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

ImageDefinition const & H3DF::ImageDefinition::operator = (ImageDefinition const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ImageDefinition::Set(ImageKit const & cInKit)
{
	auto pcImpl = static_cast<ImageDefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cImageKit = cInKit;
}

void H3DF::ImageDefinition::Show(ImageKit & cOutKit) const
{
	auto pcImpl = static_cast<ImageDefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutKit = pcImpl->m_cImageKit;
}
