#include "StdAfx.h"

#include "3DF.MaterialMappingControl.h"

#include "3DF.Segment.h"

#include <HUtility.h>
#include <HTools.h>

USING_3DF_NAMESPACE

//== MaterialMappingKit ============================================================================

MaterialMappingKit::MaterialMappingKit()
{
	for(auto & pbSetColor : m_pbSetColors) {
		for(bool & bSetColor : pbSetColor) {
			bSetColor = false;
		}
	}

	for(auto & eColorType : m_eColorType) {
		eColorType = Material::Type::None;
	}

	for(auto & dShininess : m_fGloss) {
		dShininess = 1.0;
	}

	for(auto & bShininessFlag : m_bGlossFlag) {
		bShininessFlag = false;
	}


/*
	for(int nTypeIndex = 0; nTypeIndex < (int) TypeIndex::Count; nTypeIndex++) {
		for(int nColorIndex = 0; nColorIndex < (int) ColorIndex::Count; nColorIndex++) {
			m_pbSetColors[nTypeIndex][nColorIndex] = false;
		}
	}
*/
}

//== Face Color 설정 ================================================================================

MaterialMappingKit & MaterialMappingKit::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	return SetColor(TypeIndex::Face, (Material::Channel) eInChannel, cInRgbaColor);
}

MaterialMappingKit & MaterialMappingKit::SetFaceGloss(float fGloss)
{
	return SetGloss(TypeIndex::Face, fGloss);
}

bool MaterialMappingKit::ShowFaceChannel(Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const
{
	return ShowChannel(TypeIndex::Face, eInChannel, eOutType, cOutRgbaColor, strOutTextureName, fOutValue);
}

//== Line Color 설정 ================================================================================

MaterialMappingKit & MaterialMappingKit::SetLineColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	return SetColor(TypeIndex::Line, (Material::Channel) eInChannel, cInRgbaColor);
}

bool MaterialMappingKit::ShowLineChannel(Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const
{
	return ShowChannel(TypeIndex::Line, eInChannel, eOutType, cOutRgbaColor, strOutTextureName, fOutValue);
}

MaterialMappingKit & MaterialMappingKit::operator = (MaterialMappingKit const & cInThat)
{
	for(int nTypeIndex = 0; nTypeIndex < (int) TypeIndex::Count; nTypeIndex++) {
		for(int nColorIndex = 0; nColorIndex < (int) Material::Channel::Count; nColorIndex++) {
			m_pcColors[nTypeIndex][nColorIndex] = cInThat.Colors(nTypeIndex, nColorIndex);
			m_pbSetColors[nTypeIndex][nColorIndex] = cInThat.SetColors(nTypeIndex, nColorIndex);
		}
	}

	for(int nTypeIndex = 0; nTypeIndex < (int) TypeIndex::Count; nTypeIndex++) {
		m_fGloss[nTypeIndex] = cInThat.Gloss()[nTypeIndex];
		m_bGlossFlag[nTypeIndex] = cInThat.GlossFlag()[nTypeIndex];
	}

	return *this;
}

bool MaterialMappingKit::operator == (MaterialMappingKit const & cInThat) const
{
	for(int nTypeIndex = 0; nTypeIndex < (int) TypeIndex::Count; nTypeIndex++) {
		for(int nColorIndex = 0; nColorIndex < (int) Material::Channel::Count; nColorIndex++) {
			if(m_pbSetColors[nTypeIndex][nColorIndex] != cInThat.SetColors(nTypeIndex, nColorIndex)) {
				return false;
			}

			if(m_pcColors[nTypeIndex][nColorIndex] != cInThat.Colors(nTypeIndex, nColorIndex)) {
				return false;
			}
		}
	}

	for(int nTypeIndex = 0; nTypeIndex < (int) TypeIndex::Count; nTypeIndex++) {
		if(m_bGlossFlag[nTypeIndex] != cInThat.GlossFlag()[nTypeIndex]) {
			return false;
		}
	}

	for(int nTypeIndex = 0; nTypeIndex < (int) TypeIndex::Count; nTypeIndex++) {
		if(m_fGloss[nTypeIndex] != cInThat.Gloss()[nTypeIndex]) {
			return false;
		}
	}

	return true;
}

bool MaterialMappingKit::IsAllocate()
{
	for(auto & pbSetColor : m_pbSetColors) {
		for(bool & bSetColor : pbSetColor) {
			if(true == bSetColor) {
				return true;
			}
		}
	}

	for(auto & bShininessFlag : m_bGlossFlag) {
		if(true == bShininessFlag) {
			return true;
		}
	}

	return false;

}

MaterialMappingKit & MaterialMappingKit::SetColor(TypeIndex nTypeIndex, Material::Channel eInChannel, RGBAColor const & cInRgbaColor)
{
	m_eColorType[(int) nTypeIndex] = Material::Type::RGBAColor;
	m_pcColors[(int) nTypeIndex][(int) eInChannel] = cInRgbaColor;
	m_pbSetColors[(int) nTypeIndex][(int) eInChannel] = true;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetGloss(TypeIndex nTypeIndex, float fGloss)
{
	m_eColorType[(int) nTypeIndex] = Material::Type::GlossValue;
	m_fGloss[(int) nTypeIndex] = fGloss;
	m_bGlossFlag[(int) nTypeIndex] = true;

	return *this;
}

MaterialMappingKit & MaterialMappingKit::UnsetColor(TypeIndex nTypeIndex, Material::Channel eInChannel)
{
	m_eColorType[(int) nTypeIndex] = Material::Type::None;
	m_pbSetColors[(int) nTypeIndex][(int) eInChannel] = false;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::UnsetGloss(TypeIndex nTypeIndex)
{
	m_eColorType[(int) nTypeIndex] = Material::Type::None;
	m_bGlossFlag[(int) nTypeIndex] = false;

	return *this;
}

bool MaterialMappingKit::ShowChannel(TypeIndex nTypeIndex, Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const
{
	if(true == m_pbSetColors[(int) nTypeIndex][(int)eInChannel]) {
		eOutType = m_eColorType[(int) nTypeIndex];
		cOutRgbaColor = m_pcColors[(int) nTypeIndex][(int) eInChannel];
		return true;
	}

	if(true == m_bGlossFlag[(int) nTypeIndex]) {
		fOutValue = m_fGloss[(int) nTypeIndex];
		return true;
	}

	return false;
}

RGBAColor MaterialMappingKit::Colors(int nTypeIndex, int nInChannel) const
{
	return m_pcColors[nTypeIndex][nInChannel];
}

bool MaterialMappingKit::SetColors(int nTypeIndex, int nInChannel) const
{
	return m_pbSetColors[nTypeIndex][nInChannel];
}

float * MaterialMappingKit::Gloss() const
{
	return (float *) m_fGloss;
}

bool * MaterialMappingKit::GlossFlag() const
{ 
	return (bool *) m_bGlossFlag; 
}


//== MaterialMappingControl ========================================================================

MaterialMappingControl::MaterialMappingControl(SegmentKey & cInSegmentKey) :
	m_cInSegmentKey(cInSegmentKey)
{

}

MaterialMappingControl & MaterialMappingControl::SetWindows(bool bInValue)
{
	return SetSelectability("windows", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetEdges(bool bInValue)
{
	return SetSelectability("edges", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetFaces(bool bInValue)
{
	return SetSelectability("faces", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetLights(bool bInValue) 
{
	return SetSelectability("lights", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetLines(bool bInValue) 
{
	return SetSelectability("lines", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetMarkers(bool bInValue)
{
	return SetSelectability("markers", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetVertices(bool bInValue)
{
	return SetSelectability("vertices", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetText(bool bInValue)
{
	return SetSelectability("text", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetGeometry(bool bInValue)
{
	return SetSelectability("geometry", bInValue);
}

MaterialMappingControl & MaterialMappingControl::SetEverything(bool bInValue)
{
	return SetSelectability("everything", bInValue);
}

MaterialMappingControl & MaterialMappingControl::UnsetWindows()
{
	return UnSetSelectability("windows");
}

MaterialMappingControl & MaterialMappingControl::UnsetEdges()
{
	return UnSetSelectability("edges");
}

MaterialMappingControl & MaterialMappingControl::UnsetFaces()
{
	return UnSetSelectability("faces");
}

MaterialMappingControl & MaterialMappingControl::UnsetLights()
{
	return UnSetSelectability("lights");
}

MaterialMappingControl & MaterialMappingControl::UnsetLines()
{
	return UnSetSelectability("lines");
}

MaterialMappingControl & MaterialMappingControl::UnsetMarkers() 
{
	return UnSetSelectability("markers");
}

MaterialMappingControl & MaterialMappingControl::UnsetVertices()
{
	return UnSetSelectability("vertices");
}

MaterialMappingControl & MaterialMappingControl::UnsetText()
{
	return UnSetSelectability("text");
}

MaterialMappingControl & MaterialMappingControl::UnsetGeometry() 
{
	return UnSetSelectability("geometry");
}

MaterialMappingControl & MaterialMappingControl::UnsetEverything()
{
	m_cInSegmentKey.Open();

	HC_UnSet_Selectability();

	m_cInSegmentKey.Close();

	return *this;
}

MaterialMappingControl & MaterialMappingControl::SetSelectability(CString strInType, bool bInValue)
{
	m_cInSegmentKey.Open();

	CString strList;
	strList.Format(L"%s = %s", strInType, (true == bInValue ? L"on" : L"off"));
	HC_Set_Selectability(H_ASCII_TEXT(strList));

	m_cInSegmentKey.Close();

	return *this;
}

MaterialMappingControl & MaterialMappingControl::UnSetSelectability(CString strInType)
{
	m_cInSegmentKey.Open();

	HC_UnSet_One_Selectability(H_ASCII_TEXT(strInType));

	m_cInSegmentKey.Close();

	return *this;
}