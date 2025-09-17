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
		VisibilityKitImpl() {
			for (auto & nFlag : m_bVisibilityFlag) {
				nFlag = 0;
			}
		}
		
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<VisibilityKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const VisibilityKitImpl * pcInThat) {

			for (int nIndex = 0; nIndex < (int)H3DF::VisibilityKit::VisibilityType::Count; nIndex++) {
				m_bVisibilityFlag[nIndex] = pcInThat->m_bVisibilityFlag[nIndex];
			}
		}
		
		void SetVisibility(H3DF::VisibilityKit::VisibilityType eType, bool bInFlag);

		int m_bVisibilityFlag[(int)H3DF::VisibilityKit::VisibilityType::Count];
	};
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
	m_pcImpl = std::make_unique<VisibilityKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::VisibilityKit::VisibilityKit(VisibilityKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

VisibilityKit const & H3DF::VisibilityKit::operator = (VisibilityKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

//== Set Visibility Kit ============================================================================

VisibilityKit & H3DF::VisibilityKit::SetWindows(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Windows, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetEdges(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Edges, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetFaces(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Faces, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetLights(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Lights, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetLines(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Lines, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetMarkers(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Markers, bInState);

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::SetVertices(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Vertices, bInState);

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::SetText(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Text, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetGeometry(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Geometry, bInState);

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::SetEverything(bool bInState)
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetVisibility(VisibilityType::Everything, bInState);

	return *this;
}

//== Unset Visibility Kit ==========================================================================

VisibilityKit & H3DF::VisibilityKit::UnsetWindows()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Windows] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetEdges()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Edges] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetFaces()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Faces] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetLights()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Lights] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetLines()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Lines] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetMarkers()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Markers] = VisibilityUnSet;

	return *this;

}

VisibilityKit & H3DF::VisibilityKit::UnsetVertices()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Vertices] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetText()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Text] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetGeometry()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bVisibilityFlag[(int)VisibilityType::Geometry] = VisibilityUnSet;

	return *this;
}

VisibilityKit & H3DF::VisibilityKit::UnsetEverything()
{
	auto pcImpl = static_cast<VisibilityKitImpl *>(m_pcImpl.get());
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
		VisibilityControlImpl() {}

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<VisibilityControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const VisibilityControlImpl * pcInThat) {
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

H3DF::VisibilityControl::VisibilityControl(SegmentKey & cInSegment)
{
	m_pcImpl = std::make_unique<VisibilityControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::VisibilityControl::VisibilityControl(VisibilityControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

VisibilityControl & H3DF::VisibilityControl::operator = (VisibilityControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

//== Set Visibility Control ========================================================================

VisibilityControl & H3DF::VisibilityControl::SetCuttingSections(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("cutting planes", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetWindows(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("windows", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetEdges(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("edges", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetFaces(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("faces", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetLights(bool bInState) 
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("lights", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetLines(bool bInState) 
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("lines", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetMarkers(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("markers", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetVertices(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("vertices", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetText(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("text", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetShadows(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("shadows", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetGeometry(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("geometry", bInState);

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::SetEverything(bool bInState)
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetVisibility("everything", bInState);

	return *this;
}

//== Unset Selectability Control ===================================================================
VisibilityControl & H3DF::VisibilityControl::UnsetCuttingSections()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("cutting planes");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetWindows()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("windows");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetEdges()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("edges");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetFaces()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("faces");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetLights()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("lights");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetLines()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("lines");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetMarkers() 
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("markers");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetVertices()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("vertices");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetText()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("text");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetShadows()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("shadows");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetGeometry() 
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnSetVisibility("geometry");

	return *this;
}

VisibilityControl & H3DF::VisibilityControl::UnsetEverything()
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Visibility();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::VisibilityControl::ShowFaces(bool & bOutState) const
{
	auto pcImpl = static_cast<VisibilityControlImpl *>(m_pcImpl.get());
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