#include "StdAfx.h"

#include "ImageImpl.h"

#include <magick/api.h>

#include <HUtility.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

void H3DF::ImageKitImpl::Reset()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nLength = 0;
	m_eFormat = Image::Format::None;
	m_arImageData.clear();
}

bool H3DF::ImageKitImpl::FileToImage(CString strFilePathName, Image::ImportOptionsKit const & cInOptions)
{ 
	FILE * pcInFile = _wfopen(strFilePathName, L"rb");
	
	bool bFailFlag = true;
	int nReadCount = 0;
	int nSize = 0;
	size_t nCount = 0;
	CStringA strFormat;

	if (!pcInFile) {
		m_strInformation = L"Error: Unable to open file for reading";
		goto CleanUpLabel;
	}

	nReadCount = fseek(pcInFile, 0, SEEK_END);
	if (nReadCount == -1) {
		goto CleanUpLabel;
	}

	nSize = ftell(pcInFile);
	if (nSize <= 0)
		goto CleanUpLabel;

	nReadCount = fseek(pcInFile, 0, SEEK_SET);
	if (nReadCount == -1)
		goto CleanUpLabel;

	m_arImageData.resize(nSize);

	if (true == m_arImageData.empty()) {
		goto CleanUpLabel;
	}

	nCount = fread(m_arImageData.data(), 1, nSize, pcInFile);

	if (nCount != nSize) {
		goto CleanUpLabel;
	}

	Image::Format eFormat;
	cInOptions.ShowFormat(eFormat);

	if (eFormat == Image::Format::Jpeg) {
		strFormat = "jpg";
	}
	else if (eFormat == Image::Format::Png) {
		strFormat = "png";
	}
	else if (eFormat == Image::Format::Targa) {
		strFormat = "tga";
	}
	else if (eFormat == Image::Format::DXT1) {
		strFormat = "dxt1";
	}
	else if (eFormat == Image::Format::DXT3) {
		strFormat = "dxt3";
	}
	else if (eFormat == Image::Format::DXT5) {
		strFormat = "dxt5";
	}
	else if (eFormat == Image::Format::Bmp) {
		strFormat = "bmp";
	}

	if (false == OutsourceToImageMagick(nCount, strFormat)) {
		goto CleanUpLabel;
	}

	bFailFlag = false;

CleanUpLabel:

	if (nullptr != pcInFile) {
		fclose(pcInFile);
	}

	if (true == bFailFlag) {
		Reset();
		m_strInformation = "Error: Unable to open file for reading";
		
	}

	return !bFailFlag;
}

bool H3DF::ImageKitImpl::OutsourceToImageMagick(size_t nInCount, CStringA strFormat)
{
	::ImageType eImageType;
	bool bFailFlag = true; // 정상적으로 진행되면 값이 false로 변경됨
	char const * pchMagickFormat = nullptr;
	int nPixelSize = 0;

	InitializeMagick(".");

	ExceptionInfo cException;
	GetExceptionInfo(&cException);

	ImageInfo * pcImageInfo = CloneImageInfo((ImageInfo *) nullptr);
	strcpy(pcImageInfo->magick, strFormat);
	::Image * pcImage = BlobToImage(pcImageInfo, m_arImageData.data(), nInCount, &cException);

	if (cException.severity != UndefinedException && cException.severity != CoderWarning) {
		CatchException(&cException);
		goto CleanUpLabel;
	}

	if (!pcImage) {
		goto CleanUpLabel;
	}

	eImageType = GetImageType(pcImage, &cException);

	switch (eImageType)
	{
		case PaletteMatteType:
		case GrayscaleMatteType:
		case TrueColorMatteType:
			pchMagickFormat = "RGBA";
			m_eFormat = Image::Format::RGBA;
			nPixelSize = 4;
			break;

		case GrayscaleType:
			pchMagickFormat = "I";
			m_eFormat = Image::Format::Grayscale;
			nPixelSize = 1;
			break;

		case TrueColorType:
		default:
			if (pcImage->compression == JPEGCompression) {
				pchMagickFormat = nullptr;
				m_eFormat = Image::Format::Jpeg;
				nPixelSize = 0;
			}
			else if (strieq(pcImage->magick, "tga")) {
				pchMagickFormat = nullptr;
				m_eFormat = Image::Format::Targa;
				nPixelSize = 0;
			}
			else {
				pchMagickFormat = "RGB";
				m_eFormat = Image::Format::RGB;
				nPixelSize = 3;
			}
			break;
	}

	m_nWidth = pcImage->magick_columns;
	m_nHeight = pcImage->magick_rows;

	if (nPixelSize) {
		m_nLength = m_nWidth * m_nHeight * nPixelSize;
		m_arImageData.resize(m_nLength);

		if (m_arImageData.empty()) {
			goto CleanUpLabel;
		}

		ExportImagePixels(pcImage, 0, 0, m_nWidth, m_nHeight, pchMagickFormat, CharPixel, m_arImageData.data(), &cException);
	}

	if ((strieq(strFormat, "tif") || strieq(strFormat, "tiff")) && pcImage->depth == 1) {
		ImageAttribute const * pcAttribute = GetImageAttribute(pcImage, "Photometric");
		if (pcAttribute && streq(pcAttribute->value, "min-is-black")) {
			for (UINT nIndex = 0; nIndex < m_nLength; nIndex++) {
				m_arImageData[nIndex] = ~m_arImageData[nIndex];
			}
		}
	}

	if (cException.severity != UndefinedException && cException.severity != CoderWarning) {
		CatchException(&cException);
		goto CleanUpLabel;
	}

	bFailFlag = false;

CleanUpLabel:

	if (pcImage)
		DestroyImage(pcImage);

	if (pcImageInfo)
		DestroyImageInfo(pcImageInfo);

	DestroyExceptionInfo(&cException);
	DestroyMagick();

	return !bFailFlag;
}