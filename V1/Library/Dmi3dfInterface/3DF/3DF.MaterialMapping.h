#pragma once
#pragma warning(disable : 4251)

#include "3DF.h"

#include "3DF.Color.h"
#include "3DF.Material.h"

OPEN_3DF_NAMESPACE

#define MATERIAL_MAPPING_MAP_INDEX_SIZE		10

class API_3DF MaterialMappingKit
{
public:
	MaterialMappingKit();

	//----- Color 설정 -----
	MaterialMappingKit & SetColor(RGBAColor const & cInRgbaColor, Material::Color::Type eType = Material::Color::Type::Diffuse);
	MaterialMappingKit & SetGloss(float fGloss);
	bool ShowColor(Material::Color::Type eType, RGBAColor & cOutRgbaColor) const;

	//----- Operator -----
	MaterialMappingKit & operator = (MaterialMappingKit const & cInThat);
	bool operator == (MaterialMappingKit const & cInThat) const;
	bool operator != (MaterialMappingKit const & cInThat) const;

	bool IsAllocate();

	RGBAColor Colors(int nType) const;
	bool SetColorFlags(int nType) const;
	float Gloss() const;
	bool GlossFlag() const;

	bool TextureMirror() const;
	MaterialMappingKit & SetTextureMirror(bool bFlag);

	CString TextureName() const;
	MaterialMappingKit & SetTextureName(CString strTextureName, Material::Texture::Channel eChannel = Material::Texture::Channel::DiffuseTexture, size_t nInLayer = 0);
	
	CString TextureOption() const;
	MaterialMappingKit & SetTextureOption(CString strTextureOption);
	// char * GetMapIndex();

private:
	RGBAColor m_pcColors[(int) Material::Color::Type::Count];
	bool m_pbSetColorFlags[(int) Material::Color::Type::Count];
	float m_fGloss;
	bool m_bGlossFlag;

	bool m_bTextureMirror = false;
	CString m_strTextureName;
	CString m_strTextureOption;
};

class API_3DF MaterialMappingControl
{
public:
	MaterialMappingControl(SegmentKey & cInSegmentKey);

	//== Color 설정 =================================================================================
	MaterialMappingControl & SetMarkerColor(RGBAColor const & cInRgbaColor);

	//== Texture 설정 ===============================================================================
	void InitPopulateTextures();
	void InsertPicture(UINT nIndex, UINT nPixelWidth, UINT nPixelHeight, UCHAR * pucBinaryData);
	void InsertDifaultPicture(UINT nIndex, UINT nSize, UCHAR * pucBinaryData);
	void SetTextureMatrix(float * pfTextureMatrix, char * pchTextureTransformSegment);
	void SetDefineLocalTexture(UINT nIndex, CString strTextureOptions);
	void EndPopulateTextures();

private:
	SegmentKey & m_cInSegmentKey;
	
	MaterialMappingControl & SetColor(CString strGeometry, CString strChannel, RGBAColor const & cInRgbaColor);
	MaterialMappingControl & UnSetColor(CString strInType);
};



CLOSE_3DF_NAMESPACE
