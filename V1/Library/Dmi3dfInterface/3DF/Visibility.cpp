#include "StdAfx.h"

#include "Visibility.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"

#include <HUtility.h>
#include <HTools.h>

USING_3DF_NAMESPACE

VisibilityKit::VisibilityKit() 
{
	for(bool & nFlag : m_bVisibilityFlag) {
		nFlag = false;
	}
}

//== Set Visibility Kit ============================================================================

VisibilityKit & VisibilityKit::SetWindows(bool bInValue)
{
	return SetVisibility(VisibilityType::Windows, true);
}

VisibilityKit & VisibilityKit::SetEdges(bool bInValue)
{
	return SetVisibility(VisibilityType::Edges, true);
}

VisibilityKit & VisibilityKit::SetFaces(bool bInValue)
{
	return SetVisibility(VisibilityType::Edges, true);
}

VisibilityKit & VisibilityKit::SetLights(bool bInValue)
{
	return SetVisibility(VisibilityType::Lights, true);
}

VisibilityKit & VisibilityKit::SetLines(bool bInValue)
{
	return SetVisibility(VisibilityType::Lines, true);
}

VisibilityKit & VisibilityKit::SetMarkers(bool bInValue)
{
	return SetVisibility(VisibilityType::Markers, true);
}

VisibilityKit & VisibilityKit::SetVertices(bool bInValue)
{
	return SetVisibility(VisibilityType::Vertices, true);
}

VisibilityKit & VisibilityKit::SetText(bool bInValue)
{
	return SetVisibility(VisibilityType::Text, true);
}

VisibilityKit & VisibilityKit::SetGeometry(bool bInValue)
{
	return SetVisibility(VisibilityType::Geometry, true);
}

VisibilityKit & VisibilityKit::SetEverything(bool bInValue)
{
	return SetVisibility(VisibilityType::Everything, true);
}

//== Unset Visibility Kit ==========================================================================

VisibilityKit & VisibilityKit::UnsetWindows()
{
	return SetVisibility(VisibilityType::Windows, false);
}

VisibilityKit & VisibilityKit::UnsetEdges()
{
	return SetVisibility(VisibilityType::Edges, false);
}

VisibilityKit & VisibilityKit::UnsetFaces()
{
	return SetVisibility(VisibilityType::Faces, false);
}

VisibilityKit & VisibilityKit::UnsetLights()
{
	return SetVisibility(VisibilityType::Lights, false);
}

VisibilityKit & VisibilityKit::UnsetLines()
{
	return SetVisibility(VisibilityType::Lines, false);
}

VisibilityKit & VisibilityKit::UnsetMarkers()
{
	return SetVisibility(VisibilityType::Markers, false);
}

VisibilityKit & VisibilityKit::UnsetVertices()
{
	return SetVisibility(VisibilityType::Vertices, false);
}

VisibilityKit & VisibilityKit::UnsetText()
{
	return SetVisibility(VisibilityType::Text, false);
}

VisibilityKit & VisibilityKit::UnsetGeometry()
{
	return SetVisibility(VisibilityType::Geometry, false);
}

VisibilityKit & VisibilityKit::UnsetEverything()
{
	return SetVisibility(VisibilityType::Everything, false);
}

VisibilityKit & VisibilityKit::SetVisibility(VisibilityKit::VisibilityType eType, bool bInFlag)
{
	m_bVisibilityFlag[(int) eType] = bInFlag;
	return *this;
}

//== Operator ======================================================================================

VisibilityKit & VisibilityKit::operator = (VisibilityKit const & cInKit)
{
	int nIndex = 0;
	for(bool & nFlag : m_bVisibilityFlag) {
		nFlag = cInKit.GetSelectabilityFlag()[nIndex];
		nIndex++;
	}

	return *this;
}

//== Visibility Control ============================================================================

VisibilityControl::VisibilityControl(SegmentKey & cInSegmentKey) :
	m_cInSegmentKey(cInSegmentKey)
{

}

//== Set Visibility Control ========================================================================

VisibilityControl & VisibilityControl::SetCuttingSections(bool bInValue)
{
	return SetVisibility("cutting planes", bInValue);
}

VisibilityControl & VisibilityControl::SetWindows(bool bInValue)
{
	return SetVisibility("windows", bInValue);
}

VisibilityControl & VisibilityControl::SetEdges(bool bInValue)
{
	return SetVisibility("edges", bInValue);
}

VisibilityControl & VisibilityControl::SetFaces(bool bInValue)
{
	return SetVisibility("faces", bInValue);
}

VisibilityControl & VisibilityControl::SetLights(bool bInValue) 
{
	return SetVisibility("lights", bInValue);
}

VisibilityControl & VisibilityControl::SetLines(bool bInValue) 
{
	return SetVisibility("lines", bInValue);
}

VisibilityControl & VisibilityControl::SetMarkers(bool bInValue)
{
	return SetVisibility("markers", bInValue);
}

VisibilityControl & VisibilityControl::SetVertices(bool bInValue)
{
	return SetVisibility("vertices", bInValue);
}

VisibilityControl & VisibilityControl::SetText(bool bInValue)
{
	return SetVisibility("text", bInValue);
}

VisibilityControl & VisibilityControl::SetGeometry(bool bInValue)
{
	return SetVisibility("geometry", bInValue);
}

VisibilityControl & VisibilityControl::SetEverything(bool bInValue)
{
	return SetVisibility("everything", bInValue);
}

//== Unset Selectability Control ===================================================================
VisibilityControl & VisibilityControl::UnsetCuttingSections()
{
	return UnSetVisibility("cutting planes");
}

VisibilityControl & VisibilityControl::UnsetWindows()
{
	return UnSetVisibility("windows");
}

VisibilityControl & VisibilityControl::UnsetEdges()
{
	return UnSetVisibility("edges");
}

VisibilityControl & VisibilityControl::UnsetFaces()
{
	return UnSetVisibility("faces");
}

VisibilityControl & VisibilityControl::UnsetLights()
{
	return UnSetVisibility("lights");
}

VisibilityControl & VisibilityControl::UnsetLines()
{
	return UnSetVisibility("lines");
}

VisibilityControl & VisibilityControl::UnsetMarkers() 
{
	return UnSetVisibility("markers");
}

VisibilityControl & VisibilityControl::UnsetVertices()
{
	return UnSetVisibility("vertices");
}

VisibilityControl & VisibilityControl::UnsetText()
{
	return UnSetVisibility("text");
}

VisibilityControl & VisibilityControl::UnsetGeometry() 
{
	return UnSetVisibility("geometry");
}

VisibilityControl & VisibilityControl::UnsetEverything()
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	HC_UnSet_Visibility();

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);

	return *this;
}

VisibilityControl & VisibilityControl::SetVisibility(CString strInType, bool bInValue)
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	CString strList;
	strList.Format(L"%s = %s", strInType, (true == bInValue ? L"on" : L"off"));
	HC_Set_Visibility(Utility::ToChar(strList));

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);

	return *this;
}

VisibilityControl & VisibilityControl::UnSetVisibility(CString strInType)
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	HC_UnSet_One_Visibility(Utility::ToChar(strInType));

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);	

	return *this;
}