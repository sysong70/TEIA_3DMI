#include "StdAfx.h"

#include "3DF.Selectability.h"

#include "3DF.Segment.h"

#include <HUtility.h>
#include <HTools.h>

USING_3DF_NAMESPACE

SelectabilityKit::SelectabilityKit() 
{
	for(bool & nFlag : m_bSelectabilityFlag) {
		nFlag = false;
	}
}

bool SelectabilityKit::Set(char * pchValue)
{
	if (nullptr == pchValue) {
		RETURN_FALSE;
	}

	if (0 == strcmp(pchValue, "windows")) {
		SetWindows(true);
	}
	if (0 == strcmp(pchValue, "edges")) {
		SetEdges(true);
	}
	if (0 == strcmp(pchValue, "faces")) {
		SetFaces(true);
	}
	if (0 == strcmp(pchValue, "lights")) {
		SetLights(true);
	}
	if (0 == strcmp(pchValue, "lines")) {
		SetLines(true);
	}
	if (0 == strcmp(pchValue, "markers")) {
		SetMarkers(true);
	}
	if (0 == strcmp(pchValue, "vertices")) {
		SetVertices(true);
	}
	if (0 == strcmp(pchValue, "text")) {
		SetText(true);
	}
	if (0 == strcmp(pchValue, "geometry")) {
		SetGeometry(true);
	}
	if (0 == strcmp(pchValue, "everything")) {
		SetEverything(true);
	}

	return true;
}

//== Set Selectability Kit =========================================================================

SelectabilityKit & SelectabilityKit::SetWindows(bool bInValue)
{
	return SetSelectability(SelectabilityType::Windows, true);
}

SelectabilityKit & SelectabilityKit::SetEdges(bool bInValue)
{
	return SetSelectability(SelectabilityType::Edges, true);
}

SelectabilityKit & SelectabilityKit::SetFaces(bool bInValue)
{
	return SetSelectability(SelectabilityType::Edges, true);
}

SelectabilityKit & SelectabilityKit::SetLights(bool bInValue)
{
	return SetSelectability(SelectabilityType::Lights, true);
}

SelectabilityKit & SelectabilityKit::SetLines(bool bInValue)
{
	return SetSelectability(SelectabilityType::Lines, true);
}

SelectabilityKit & SelectabilityKit::SetMarkers(bool bInValue)
{
	return SetSelectability(SelectabilityType::Markers, true);
}

SelectabilityKit & SelectabilityKit::SetVertices(bool bInValue)
{
	return SetSelectability(SelectabilityType::Vertices, true);
}

SelectabilityKit & SelectabilityKit::SetText(bool bInValue)
{
	return SetSelectability(SelectabilityType::Text, true);
}

SelectabilityKit & SelectabilityKit::SetGeometry(bool bInValue)
{
	return SetSelectability(SelectabilityType::Geometry, true);
}

SelectabilityKit & SelectabilityKit::SetEverything(bool bInValue)
{
	return SetSelectability(SelectabilityType::Everything, true);
}

//== Unset Selectability Kit =======================================================================

SelectabilityKit & SelectabilityKit::UnsetWindows()
{
	return SetSelectability(SelectabilityType::Windows, false);
}

SelectabilityKit & SelectabilityKit::UnsetEdges()
{
	return SetSelectability(SelectabilityType::Edges, false);
}

SelectabilityKit & SelectabilityKit::UnsetFaces()
{
	return SetSelectability(SelectabilityType::Faces, false);
}

SelectabilityKit & SelectabilityKit::UnsetLights()
{
	return SetSelectability(SelectabilityType::Lights, false);
}

SelectabilityKit & SelectabilityKit::UnsetLines()
{
	return SetSelectability(SelectabilityType::Lines, false);
}

SelectabilityKit & SelectabilityKit::UnsetMarkers()
{
	return SetSelectability(SelectabilityType::Markers, false);
}

SelectabilityKit & SelectabilityKit::UnsetVertices()
{
	return SetSelectability(SelectabilityType::Vertices, false);
}

SelectabilityKit & SelectabilityKit::UnsetText()
{
	return SetSelectability(SelectabilityType::Text, false);
}

SelectabilityKit & SelectabilityKit::UnsetGeometry()
{
	return SetSelectability(SelectabilityType::Geometry, false);
}

SelectabilityKit & SelectabilityKit::UnsetEverything()
{
	return SetSelectability(SelectabilityType::Everything, false);
}

SelectabilityKit & SelectabilityKit::SetSelectability(SelectabilityKit::SelectabilityType eType, bool bInFlag)
{
	m_bSelectabilityFlag[(int) eType] = bInFlag;
	return *this;
}

//== Operator ======================================================================================

SelectabilityKit & SelectabilityKit::operator = (SelectabilityKit const & cInKit)
{
	int nIndex = 0;
	for(bool & nFlag : m_bSelectabilityFlag) {
		nFlag = cInKit.GetSelectabilityFlag()[nIndex];
		nIndex++;
	}

	return *this;
}

//== SelectabilityControl ==========================================================================

SelectabilityControl::SelectabilityControl(SegmentKey & cInSegmentKey) :
	m_cInSegmentKey(cInSegmentKey)
{

}

//== Set Selectability Control =====================================================================

SelectabilityControl & SelectabilityControl::SetWindows(bool bInValue)
{
	return SetSelectability("windows", bInValue);
}

SelectabilityControl & SelectabilityControl::SetEdges(bool bInValue)
{
	return SetSelectability("edges", bInValue);
}

SelectabilityControl & SelectabilityControl::SetFaces(bool bInValue)
{
	return SetSelectability("faces", bInValue);
}

SelectabilityControl & SelectabilityControl::SetLights(bool bInValue) 
{
	return SetSelectability("lights", bInValue);
}

SelectabilityControl & SelectabilityControl::SetLines(bool bInValue) 
{
	return SetSelectability("lines", bInValue);
}

SelectabilityControl & SelectabilityControl::SetMarkers(bool bInValue)
{
	return SetSelectability("markers", bInValue);
}

SelectabilityControl & SelectabilityControl::SetVertices(bool bInValue)
{
	return SetSelectability("vertices", bInValue);
}

SelectabilityControl & SelectabilityControl::SetText(bool bInValue)
{
	return SetSelectability("text", bInValue);
}

SelectabilityControl & SelectabilityControl::SetGeometry(bool bInValue)
{
	return SetSelectability("geometry", bInValue);
}

SelectabilityControl & SelectabilityControl::SetEverything(bool bInValue)
{
	return SetSelectability("everything", bInValue);
}

//== Unset Selectability Control ===================================================================

SelectabilityControl & SelectabilityControl::UnsetWindows()
{
	return UnSetSelectability("windows");
}

SelectabilityControl & SelectabilityControl::UnsetEdges()
{
	return UnSetSelectability("edges");
}

SelectabilityControl & SelectabilityControl::UnsetFaces()
{
	return UnSetSelectability("faces");
}

SelectabilityControl & SelectabilityControl::UnsetLights()
{
	return UnSetSelectability("lights");
}

SelectabilityControl & SelectabilityControl::UnsetLines()
{
	return UnSetSelectability("lines");
}

SelectabilityControl & SelectabilityControl::UnsetMarkers() 
{
	return UnSetSelectability("markers");
}

SelectabilityControl & SelectabilityControl::UnsetVertices()
{
	return UnSetSelectability("vertices");
}

SelectabilityControl & SelectabilityControl::UnsetText()
{
	return UnSetSelectability("text");
}

SelectabilityControl & SelectabilityControl::UnsetGeometry() 
{
	return UnSetSelectability("geometry");
}

SelectabilityControl & SelectabilityControl::UnsetEverything()
{
	m_cInSegmentKey.Open();

	HC_UnSet_Selectability();

	m_cInSegmentKey.Close();

	return *this;
}

SelectabilityControl & SelectabilityControl::SetSelectability(CString strInType, bool bInValue)
{
	m_cInSegmentKey.Open();

	CString strList;
	strList.Format(L"%s = %s", strInType, (true == bInValue ? L"on" : L"off"));
	HC_Set_Selectability(H_ASCII_TEXT(strList));

	m_cInSegmentKey.Close();

	return *this;
}

SelectabilityControl & SelectabilityControl::UnSetSelectability(CString strInType)
{
	m_cInSegmentKey.Open();

	HC_UnSet_One_Selectability(H_ASCII_TEXT(strInType));

	m_cInSegmentKey.Close();

	return *this;
}