#include "StdAfx.h"

#include "3DF.MaterialMapping.h"

#include "3DF.Segment.h"

#include <HUtility.h>
#include <HTools.h>

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