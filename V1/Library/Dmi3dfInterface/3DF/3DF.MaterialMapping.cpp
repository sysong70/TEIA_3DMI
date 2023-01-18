#include "StdAfx.h"

#include "3DF.MaterialMapping.h"

#include "3DF.Segment.h"

#include <hc.h>
#include <HUtility.h>
#include <HTools.h>
#include <magick/api.h>

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
	m_strTexture = cInThat.Texture();

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

	if(0 != m_strTexture.CompareNoCase(cInThat.Texture())) {
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

CString MaterialMappingKit::Texture() const
{ 
	return m_strTexture; 
}

void MaterialMappingKit::SetTexture(CString strTexture)
{ 
	m_strTexture = strTexture; 
}

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