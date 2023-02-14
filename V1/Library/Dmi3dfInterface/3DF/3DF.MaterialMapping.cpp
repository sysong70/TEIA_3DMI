#include "StdAfx.h"

#include "3DF.MaterialMapping.h"

#include "3DF.Segment.h"

#include <hc.h>
#include <HUtility.h>
#include <HTools.h>
#include <magick/api.h>
#include <varray.h>

USING_3DF_NAMESPACE

//== MaterialMappingKit ============================================================================

MaterialMappingKit::MaterialMappingKit()
{
	for(auto & pbSetColor : m_pbSetColorFlags) {
		pbSetColor = false;
	}

	m_bGlossFlag = false;
	m_fGloss = 1.0;
}

//== Color 설정 =====================================================================================

MaterialMappingKit & MaterialMappingKit::SetColor(RGBAColor const & cInRgbaColor, Material::Color::Type eType)
{
	m_pcColors[(int) eType] = cInRgbaColor;
	m_pbSetColorFlags[(int) eType] = true;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetGloss(float fGloss)
{
	m_fGloss = fGloss;
	m_bGlossFlag = true;
	return *this;
}

bool MaterialMappingKit::ShowColor(Material::Color::Type eType, RGBAColor & cOutRgbaColor) const
{
	if(false == m_pbSetColorFlags[(int) eType]) {
		return false;
	}

	cOutRgbaColor = m_pcColors[(int) eType];
	return true;
}

MaterialMappingKit & MaterialMappingKit::operator = (MaterialMappingKit const & cInThat)
{
	for(int nTypeIndex = 0; nTypeIndex < (int) Material::Color::Type::Count; nTypeIndex++) {
		m_pcColors[nTypeIndex] = cInThat.Colors(nTypeIndex);
		m_pbSetColorFlags[nTypeIndex] = cInThat.SetColorFlags(nTypeIndex);
	}

	m_fGloss = cInThat.Gloss();
	m_bGlossFlag = cInThat.GlossFlag();
	
	m_bTextureMirror = cInThat.TextureMirror();
	m_strTextureName = cInThat.TextureName();
	m_strTextureOption = cInThat.TextureOption();

	return *this;
}

bool MaterialMappingKit::operator == (MaterialMappingKit const & cInThat) const
{
	for(int nTypeIndex = 0; nTypeIndex < (int) Material::Color::Type::Count; nTypeIndex++) {
		if(m_pbSetColorFlags[nTypeIndex] != cInThat.SetColorFlags(nTypeIndex)) {
			return false;
		}

		if(true == m_pbSetColorFlags[nTypeIndex]) {
			if(false == m_pcColors[nTypeIndex].Equals(cInThat.Colors(nTypeIndex))) {
				return false;
			}
		}
	}

	if(m_bGlossFlag != cInThat.GlossFlag()) {
		return false;
	}

	if(true == m_bGlossFlag) {
		if(m_fGloss != cInThat.Gloss()) {
			return false;
		}
	}

	if(m_bTextureMirror != cInThat.TextureMirror()) {
		return false;
	}

	if(0 != m_strTextureName.CompareNoCase(cInThat.TextureName())) {
		return false;
	}

	if(0 != m_strTextureOption.CompareNoCase(cInThat.TextureOption())) {
		return false;
	}

	return true;
}

bool MaterialMappingKit::operator != (MaterialMappingKit const & cInThat) const
{
	if(*this == cInThat) {
		return false;
	}

	return true;
}

bool MaterialMappingKit::IsAllocate()
{
	for(auto & pbSetColor : m_pbSetColorFlags) {
		if(true == pbSetColor) {
			return true;
		}
	}

	if(true == m_bGlossFlag) {
		return true;
	}

	return false;

}

RGBAColor MaterialMappingKit::Colors(int nTypeIndex) const
{
	return m_pcColors[nTypeIndex];
}

bool MaterialMappingKit::SetColorFlags(int nTypeIndex) const
{
	return m_pbSetColorFlags[nTypeIndex];
}

float MaterialMappingKit::Gloss() const
{
	return m_fGloss;
}

bool MaterialMappingKit::GlossFlag() const
{ 
	return m_bGlossFlag; 
}

bool MaterialMappingKit::TextureMirror() const
{
	return m_bTextureMirror;
}

MaterialMappingKit & MaterialMappingKit::SetTextureMirror(bool bFlag)
{
	m_bTextureMirror = bFlag;
	return *this;
}

CString MaterialMappingKit::TextureName() const
{ 
	return m_strTextureName;
}

MaterialMappingKit & MaterialMappingKit::SetTextureName(CString strTextureName, Material::Texture::Channel eChannel, size_t nInLayer)
{
	m_strTextureName = strTextureName;
	return *this;
}

CString MaterialMappingKit::TextureOption() const
{
	return m_strTextureOption;
}

MaterialMappingKit & MaterialMappingKit::SetTextureOption(CString strTextureOption)
{
	m_strTextureOption = strTextureOption;
	return *this;
}

/*

char * MaterialMappingKit::GetMapIndex()
{
	char pchMapIndex[MATERIAL_MAPPING_MAP_INDEX_SIZE];
	int nIndex = 0;
	pchMapIndex[nIndex++] = 

}

*/

//== MaterialMappingControl ========================================================================

MaterialMappingControl::MaterialMappingControl(SegmentKey & cInSegmentKey) :
	m_cInSegmentKey(cInSegmentKey)
{

}

//== Color 설정 =====================================================================================
MaterialMappingControl & MaterialMappingControl::SetMarkerColor(RGBAColor const & cInRgbaColor)
{
	return SetColor(L"markers", "diffuse", cInRgbaColor);
}


MaterialMappingControl & MaterialMappingControl::SetColor(CString strGeometry, CString strChannel, RGBAColor const & cInRgbaColor)
{
	m_cInSegmentKey.Open();

	CString strColorText;
	strColorText.Format(L"%s = (%s = (r=%f g=%f b=%f))", strGeometry, strChannel, cInRgbaColor.red, cInRgbaColor.green, cInRgbaColor.blue);
	HC_Set_Color(H_ASCII_TEXT(strColorText));

	m_cInSegmentKey.Close();

	return *this;
}

MaterialMappingControl & MaterialMappingControl::UnSetColor(CString strInType)
{
	m_cInSegmentKey.Open();

	HC_UnSet_One_Selectability(H_ASCII_TEXT(strInType));

	m_cInSegmentKey.Close();

	return *this;
}

void MaterialMappingControl::InitPopulateTextures()
{
	InitializeMagick(".");

	m_cInSegmentKey.Open();

	HC_Set_Visibility("image = off");
	
	m_cInSegmentKey.Close();
}

void MaterialMappingControl::InsertPicture(UINT nIndex, UINT nPixelWidth, UINT nPixelHeight, UCHAR * pucBinaryData)
{
	m_cInSegmentKey.Open();
	
	//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), nPixelWidth, nPixelHeight, pucBinaryData);
	HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), nPixelWidth, nPixelHeight, pucBinaryData);

	m_cInSegmentKey.Close();
}

void MaterialMappingControl::InsertDifaultPicture(UINT nIndex, UINT nSize, UCHAR * pucBinaryData)
{
	VArray<unsigned char> anPixels;

	ExceptionInfo exception;
	GetExceptionInfo(&exception);
	ImageInfo * image_info = CloneImageInfo((ImageInfo *) nullptr);
	Image * image = BlobToImage(image_info, (void *) pucBinaryData, nSize, &exception);
	if(image != nullptr)
	{
		unsigned long width = image->magick_columns;
		unsigned long height = image->magick_rows;
		anPixels.EnsureSize(width * height * 4);

		ExportImagePixels(image, 0, 0, width, height, "RGBA", CharPixel, &anPixels[0], &exception);
		DestroyImage(image);
		DestroyImageInfo(image_info);
		DestroyExceptionInfo(&exception);

		m_cInSegmentKey.Open();

		//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), width, height, &anPixels[0]);
		HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), width, height, &anPixels[0]);

		m_cInSegmentKey.Close();
	}
}

void MaterialMappingControl::SetTextureMatrix(float * pfTextureMatrix, char * pchTextureTransformSegment)
{
	m_cInSegmentKey.Open();
	
	HC_Compute_Matrix_Inverse(pfTextureMatrix, pfTextureMatrix);

	char textureTransformSegment[4096] = "";
	HC_Open_Segment("texture_transformations");
	{
		HC_Open_Segment("");
		{
			HC_Show_Segment(HC_Create_Segment("."), pchTextureTransformSegment);
			HC_Set_Texture_Matrix(pfTextureMatrix);
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	m_cInSegmentKey.Close();
}

void MaterialMappingControl::SetDefineLocalTexture(UINT nIndex, CString strTextureOptions)
{
	CString strText;
	strText.Format(L"texture_%u", nIndex);
	
	m_cInSegmentKey.Open();

	HC_Define_Local_Texture(H_ASCII_TEXT(strText), H_ASCII_TEXT(strTextureOptions));

	m_cInSegmentKey.Close();
}

void MaterialMappingControl::EndPopulateTextures()
{
	DestroyMagick();
}
