#include "StdAfx.h"

#include "Selectability.h"
#include "Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

//== SelectabilityKitPrivate Class =================================================================
namespace H3DF
{
	class SelectabilityKitImpl : public Impl
	{
	public:
		SelectabilityKitImpl();
		void Empty();

		void Copy(SelectabilityKitImpl * pcInThat);

		void SetSelectability(H3DF::SelectabilityKit::SelectabilityType eType, bool bInFlag);
		void UnsetSelectability(H3DF::SelectabilityKit::SelectabilityType eType);
	
		bool m_bSelectabilityFlag[(int) SelectabilityKit::SelectabilityType::Count];
		bool m_bUnsetSelectabilityFlag[(int) SelectabilityKit::SelectabilityType::Count];
	};
}

H3DF::SelectabilityKitImpl::SelectabilityKitImpl()
{
	Empty();
}

void H3DF::SelectabilityKitImpl::Empty()
{
	for (auto & bSelectability : m_bSelectabilityFlag) {
		bSelectability = true;
	}

	for (auto & bUnsetSelectability : m_bUnsetSelectabilityFlag) {
		bUnsetSelectability = false;
	}
}

void H3DF::SelectabilityKitImpl::Copy(SelectabilityKitImpl * pcInThat)
{
	int nIndex = 0;
	for(auto & bSelectability : m_bSelectabilityFlag) {
		bSelectability = pcInThat->m_bSelectabilityFlag[nIndex];
		nIndex++;
	}

	nIndex = 0;
	for (auto & bUnsetSelectability : m_bUnsetSelectabilityFlag) {
		bUnsetSelectability = pcInThat->m_bUnsetSelectabilityFlag[nIndex];
		nIndex++;
	}
}

void H3DF::SelectabilityKitImpl::SetSelectability(H3DF::SelectabilityKit::SelectabilityType eType, bool bInFlag)
{
	m_bSelectabilityFlag[(int) eType] = bInFlag;
}

void H3DF::SelectabilityKitImpl::UnsetSelectability(H3DF::SelectabilityKit::SelectabilityType eType)
{
	m_bUnsetSelectabilityFlag[(int) eType] = true;
}

//== SelectabilityKit Class ========================================================================
H3DF::SelectabilityKit::SelectabilityKit() 
{
	m_pcImpl = new SelectabilityKitImpl();
}

H3DF::SelectabilityKit::SelectabilityKit(SelectabilityKit const & cInKit)
{
	m_pcImpl = new SelectabilityKitImpl();
	Set(cInKit);
}

SelectabilityKit H3DF::SelectabilityKit::GetDefault()
{
	SelectabilityKit cSelectabilityKit;
	return cSelectabilityKit;
}

void H3DF::SelectabilityKit::Set(SelectabilityKit const & cInKit)
{
	SelectabilityKitImpl * pcImpl = (SelectabilityKitImpl *) m_pcImpl;
	SelectabilityKitImpl * pcInKitImpl = (SelectabilityKitImpl *) cInKit.m_pcImpl;
	pcImpl->Copy(pcInKitImpl);
}

void H3DF::SelectabilityKit::Show(SelectabilityKit & cOutKit) const
{
	SelectabilityKitImpl * pcImpl = (SelectabilityKitImpl *) m_pcImpl;
	SelectabilityKitImpl * pcOutKitImpl = (SelectabilityKitImpl *) cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

SelectabilityKit & H3DF::SelectabilityKit::operator = (SelectabilityKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

bool H3DF::SelectabilityKit::Empty() const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->Empty();
	return true;
}

bool H3DF::SelectabilityKit::Equals(SelectabilityKit const & cInKit) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	SelectabilityKitImpl * pcInKitImpl = static_cast<SelectabilityKitImpl *>(cInKit.m_pcImpl);

	for (int nIndex = 0; nIndex < (int) SelectabilityType::Count; nIndex++) {
		if (pcImpl->m_bSelectabilityFlag[nIndex] != pcInKitImpl->m_bSelectabilityFlag[nIndex]) {
			return false;
		}
	}

	return true;
}

bool H3DF::SelectabilityKit::operator == (SelectabilityKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::SelectabilityKit::operator != (SelectabilityKit const & cInKit) const
{
	return !Equals(cInKit);
}

//== Set Selectability Kit =========================================================================

SelectabilityKit & H3DF::SelectabilityKit::SetWindows(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Windows, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetEdges(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Edges, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetFaces(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Faces, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetLights(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Lights, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetLines(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Lines, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetMarkers(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Markers, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetVertices(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Vertices, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetText(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Text, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetGeometry(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Geometry, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetEverything(bool bInValue)
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);

	for(auto & bSelectability : pcImpl->m_bSelectabilityFlag) {
		bSelectability = bInValue;
	}

	return *this;
}

//== Unset Selectability Kit =======================================================================

SelectabilityKit & H3DF::SelectabilityKit::UnsetWindows()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Windows);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetEdges()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Edges);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetFaces()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Faces);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetLights()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Lights);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetLines()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Lines);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetMarkers()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Markers);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetVertices()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Vertices);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetText()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Text);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetGeometry()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Geometry);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetEverything()
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);

	for (auto & bUnsetSelectability : pcImpl->m_bUnsetSelectabilityFlag) {
		bUnsetSelectability = true;
	}

	return *this;
}

bool H3DF::SelectabilityKit::ShowWindows(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Windows] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowEdges(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Edges] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowFaces(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Faces] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowLights(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Lights] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowLines(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Lines] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowMarkers(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Markers] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowVertices(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Vertices] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowText(Selectability::Value & cOuValue) const
{
	SelectabilityKitImpl * pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Text] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

namespace H3DF
{
	class SelectabilityControlImpl : public ControlImpl
	{
	public:
		SelectabilityControlImpl() = default;

		void Copy(SelectabilityControlImpl * pcInThat) {};

		void SetSelectability(CString strInType, bool bInValue);
		void UnsetSelectability(CString strInType);
	};
}

void H3DF::SelectabilityControlImpl::SetSelectability(CString strInType, bool bInValue)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey);

	CString strList;
	strList.Format(L"%s = %s", strInType, (true == bInValue ? L"on" : L"off"));
	HC_Set_Selectability(Utility::ToChar(strList));

	SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void H3DF::SelectabilityControlImpl::UnsetSelectability(CString strInType)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey);

	HC_UnSet_One_Selectability(Utility::ToChar(strInType));

	SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

//== SelectabilityControl ==========================================================================

H3DF::SelectabilityControl::SelectabilityControl(SegmentKey & cInSegmentKey)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::SelectabilityControl::SelectabilityControl(SelectabilityControl const & cInThat)
{
	m_pcImpl = new SelectabilityControlImpl();
	Set(cInThat);
}

void H3DF::SelectabilityControl::Set(SelectabilityControl const & cInThat)
{
	SelectabilityControlImpl * pcImpl = (SelectabilityControlImpl *) m_pcImpl;
	SelectabilityControlImpl * pcInThatImpl = (SelectabilityControlImpl *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectabilityControl & H3DF::SelectabilityControl::operator = (SelectabilityControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Set Selectability Control =====================================================================

SelectabilityControl & H3DF::SelectabilityControl::SetWindows(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("windows", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetEdges(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("edges", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetFaces(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("faces", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetLights(bool bInValue) 
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("lights", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetLines(bool bInValue) 
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("lines", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetMarkers(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("markers", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetVertices(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("vertices", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetText(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("text", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetGeometry(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("geometry", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetEverything(bool bInValue)
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->SetSelectability("everything", bInValue);
	return *this;
}

//== Unset Selectability Control ===================================================================

SelectabilityControl & H3DF::SelectabilityControl::UnsetWindows()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("windows");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetEdges()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("edges");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetFaces()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("faces");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetLights()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("lights");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetLines()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("lines");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetMarkers() 
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("markers");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetVertices()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("vertices");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetText()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("text");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetGeometry() 
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();
	pcImpl->UnsetSelectability("geometry");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetEverything()
{
	SelectabilityControlImpl * pcImpl = new SelectabilityControlImpl();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);

	HC_UnSet_Selectability();

	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}