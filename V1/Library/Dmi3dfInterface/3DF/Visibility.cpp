#include "StdAfx.h"

#include "Visibility.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

#define VisibilityOn		0x01
#define VisibilityOff		0x02
#define VisibilityUnSet		0x04

namespace H3DF
{
	class VisibilityKitImpl : public Impl
	{
	public:
		VisibilityKitImpl();

		void Copy(VisibilityKitImpl * pcInThat) {

			for (int nIndex = 0; nIndex < (int)H3DF::VisibilityKit::VisibilityType::Count; nIndex++) {
				m_bVisibilityFlag[nIndex] = pcInThat->m_bVisibilityFlag[nIndex];
			}
		}
		
		void SetVisibility(H3DF::VisibilityKit::VisibilityType eType, bool bInFlag);

		int m_bVisibilityFlag[(int)H3DF::VisibilityKit::VisibilityType::Count];
	};
}

VisibilityKitImpl::VisibilityKitImpl()
{
	for (auto & nFlag : m_bVisibilityFlag) {
		nFlag = 0;
	}
}

void VisibilityKitImpl::SetVisibility(H3DF::VisibilityKit::VisibilityType eType, bool bInFlag)
{
	if (true == bInFlag) {
		m_bVisibilityFlag[(int)eType] = VisibilityOn;
	}
	else {
		m_bVisibilityFlag[(int)eType] = VisibilityOff;
	}
}

H3DF::VisibilityKit::VisibilityKit()
{
	m_pcImpl = new VisibilityKitImpl();
}

H3DF::VisibilityKit::VisibilityKit(VisibilityKit const & cInThat)
{
	m_pcImpl = new VisibilityKitImpl();
	Set(cInThat);
}

void H3DF::VisibilityKit::Set(VisibilityKit const & cInThat)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	VisibilityKitImpl * pcInThatImpl = (VisibilityKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

VisibilityKit const & H3DF::VisibilityKit::operator = (VisibilityKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Set Visibility Kit ============================================================================

VisibilityKit & H3DF::VisibilityKit::SetWindows(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Windows, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetEdges(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Edges, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetFaces(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Faces, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetLights(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Lights, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetLines(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Lines, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetMarkers(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Markers, bInState);

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::SetVertices(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Vertices, bInState);

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::SetText(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Text, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetGeometry(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Geometry, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetEverything(bool bInState)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Everything, bInState);

	return *this;
}

//== Unset Visibility Kit ==========================================================================

VisibilityKit & H3DF::VisibilityKit::UnsetWindows()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Windows] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetEdges()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Edges] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetFaces()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Faces] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetLights()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Lights] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetLines()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Lines] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetMarkers()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Markers] = VisibilityUnSet;

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::UnsetVertices()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Vertices] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetText()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Text] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetGeometry()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Geometry] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetEverything()
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Everything] = VisibilityUnSet;

	return *this;
}

//== Visibility Control ============================================================================

namespace H3DF
{
	class VisibilityControlImpl : public ControlImpl
	{
	public:
		void Copy(VisibilityControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		void SetVisibility(CStringA strInType, bool bInState);
		void UnSetVisibility(CStringA strInType);
	};
}

void H3DF::VisibilityControlImpl::SetVisibility(CStringA strInType, bool bInState)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey);

	CStringA strList;
	strList.Format("%s = %s", strInType, (true == bInState ? "on" : "off"));
	
	HC_Set_Visibility(strList);

	SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void H3DF::VisibilityControlImpl::UnSetVisibility(CStringA strInType)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey);

	HC_UnSet_One_Visibility(strInType);

	SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

H3DF::VisibilityControl::VisibilityControl(SegmentKey & cInSegmentKey)
{
	VisibilityControlImpl * pcImpl = new VisibilityControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::VisibilityControl::VisibilityControl(VisibilityControl const & cInThat)
{
	m_pcImpl = new VisibilityControlImpl();
	Set(cInThat);
}

void H3DF::VisibilityControl::Set(VisibilityControl const & cInThat)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	VisibilityControlImpl * pcInThatImpl = (VisibilityControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

VisibilityControl & H3DF::VisibilityControl::operator = (VisibilityControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Set Visibility Control ========================================================================

VisibilityControl & H3DF::VisibilityControl::SetCuttingSections(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("cutting planes", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetWindows(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("windows", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetEdges(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("edges", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetFaces(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("faces", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetLights(bool bInState) 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("lights", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetLines(bool bInState) 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("lines", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetMarkers(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("markers", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetVertices(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("vertices", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetText(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("text", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetShadows(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("shadows", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetGeometry(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("geometry", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetEverything(bool bInState)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("everything", bInState);

	return *this;
}

//== Unset Selectability Control ===================================================================
VisibilityControl & H3DF::VisibilityControl::UnsetCuttingSections()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("cutting planes");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetWindows()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("windows");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetEdges()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("edges");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetFaces()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("faces");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetLights()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("lights");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetLines()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("lines");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetMarkers() 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("markers");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetVertices()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("vertices");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetText()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("text");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetShadows()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("shadows");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetGeometry() 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("geometry");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetEverything()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Visibility();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::VisibilityControl::ShowFaces(bool & bOutState) const
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
// 		if (FALSE == HC_Show_Existence("visibility")) {
// 			return false;
// 		}

		CStringA strValue;
		HC_Show_Visibility(strValue.GetBuffer());
		//HC_Show_One_Visibility("faces", strValue.GetBuffer());
		strValue.ReleaseBuffer();

		if ("on" == strValue.MakeLower()) {
			bOutState = true;
		}
		else {
			bOutState = false;
		}

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return true;
}