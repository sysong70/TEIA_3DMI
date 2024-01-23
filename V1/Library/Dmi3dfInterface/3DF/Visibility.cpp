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
	m_eType = H3DF::Type::VisibilityKit;

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

VisibilityKit & H3DF::VisibilityKit::SetWindows(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Windows, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetEdges(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Edges, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetFaces(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Faces, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetLights(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Lights, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetLines(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Lines, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetMarkers(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Markers, bInValue);

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::SetVertices(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Vertices, bInValue);

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::SetText(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Text, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetGeometry(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Geometry, bInValue);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetEverything(bool bInValue)
{
	VisibilityKitImpl * pcImpl = (VisibilityKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Everything, bInValue);

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
		VisibilityControlImpl() { m_eType = H3DF::Type::VisibilityControl; }

		void Copy(VisibilityControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		void SetVisibility(CStringA strInType, bool bInValue);
		void UnSetVisibility(CStringA strInType);
	};
}

void H3DF::VisibilityControlImpl::SetVisibility(CStringA strInType, bool bInValue)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey);

	CStringA strList;
	strList.Format("%s = %s", strInType, (true == bInValue ? "on" : "off"));
	
	HC_Set_Visibility(strList);

	SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void H3DF::VisibilityControlImpl::UnSetVisibility(CStringA strInType)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey);

	HC_UnSet_One_Visibility(strInType);

	SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

VisibilityControl::VisibilityControl(SegmentKey & cInSegmentKey)
{
	VisibilityControlImpl * pcImpl = new VisibilityControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

VisibilityControl::VisibilityControl(VisibilityControl const & cInThat)
{
	m_pcImpl = new VisibilityControlImpl();
	Set(cInThat);
}

void VisibilityControl::Set(VisibilityControl const & cInThat)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	VisibilityControlImpl * pcInThatImpl = (VisibilityControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

VisibilityControl & VisibilityControl::operator = (VisibilityControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Set Visibility Control ========================================================================

VisibilityControl & VisibilityControl::SetCuttingSections(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("cutting planes", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetWindows(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("windows", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetEdges(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("edges", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetFaces(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("faces", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetLights(bool bInValue) 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("lights", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetLines(bool bInValue) 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("lines", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetMarkers(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("markers", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetVertices(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("vertices", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetText(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("text", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetGeometry(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("geometry", bInValue);

	return *this;
}

VisibilityControl & VisibilityControl::SetEverything(bool bInValue)
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("everything", bInValue);

	return *this;
}

//== Unset Selectability Control ===================================================================
VisibilityControl & VisibilityControl::UnsetCuttingSections()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("cutting planes");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetWindows()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("windows");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetEdges()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("edges");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetFaces()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("faces");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetLights()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("lights");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetLines()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("lines");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetMarkers() 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("markers");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetVertices()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("vertices");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetText()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("text");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetGeometry() 
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("geometry");

	return *this;
}

VisibilityControl & VisibilityControl::UnsetEverything()
{
	VisibilityControlImpl * pcImpl = (VisibilityControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);

	HC_UnSet_Visibility();

	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}