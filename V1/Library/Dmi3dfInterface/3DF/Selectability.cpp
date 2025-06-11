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

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectabilityKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectabilityKitImpl * pcInThat);

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

void H3DF::SelectabilityKitImpl::Copy(const SelectabilityKitImpl * pcInThat)
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<SelectabilityKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SelectabilityKit::SelectabilityKit(SelectabilityKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.GetImpl()) ? cInKit.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

SelectabilityKit H3DF::SelectabilityKit::GetDefault()
{
	SelectabilityKit cSelectabilityKit;
	return cSelectabilityKit;
}

void H3DF::SelectabilityKit::Show(SelectabilityKit & cOutKit) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	auto pcOutKitImpl = static_cast<SelectabilityKitImpl *>(cOutKit.m_pcImpl.get());
	pcOutKitImpl->Copy(pcImpl);
}

SelectabilityKit & H3DF::SelectabilityKit::operator = (SelectabilityKit const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::SelectabilityKit::Empty() const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->Empty();
	return true;
}

bool H3DF::SelectabilityKit::Equals(SelectabilityKit const & cInKit) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	auto pcInKitImpl = static_cast<SelectabilityKitImpl *>(cInKit.m_pcImpl.get());

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
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Windows, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetEdges(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Edges, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetFaces(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Faces, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetLights(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Lights, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetLines(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Lines, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetMarkers(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Markers, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetVertices(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Vertices, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetText(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Text, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetGeometry(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability(SelectabilityType::Geometry, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetEverything(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());

	for(auto & bSelectability : pcImpl->m_bSelectabilityFlag) {
		bSelectability = bInValue;
	}

	return *this;
}

//== Unset Selectability Kit =======================================================================

SelectabilityKit & H3DF::SelectabilityKit::UnsetWindows()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Windows);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetEdges()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Edges);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetFaces()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Faces);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetLights()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Lights);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetLines()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Lines);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetMarkers()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Markers);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetVertices()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Vertices);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetText()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Text);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetGeometry()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability(SelectabilityType::Geometry);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetEverything()
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());

	for (auto & bUnsetSelectability : pcImpl->m_bUnsetSelectabilityFlag) {
		bUnsetSelectability = true;
	}

	return *this;
}

bool H3DF::SelectabilityKit::ShowWindows(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Windows] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowEdges(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Edges] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowFaces(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Faces] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowLights(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Lights] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowLines(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Lines] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowMarkers(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Markers] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowVertices(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Vertices] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowText(Selectability::Value & cOuValue) const
{
	auto pcImpl = static_cast<SelectabilityKitImpl *>(m_pcImpl.get());
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Text] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

namespace H3DF
{
	class SelectabilityControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectabilityControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectabilityControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		};

		void SetSelectability(CString strInType, bool bInValue);
		void UnsetSelectability(CString strInType);
	};
}

void H3DF::SelectabilityControlImpl::SetSelectability(CString strInType, bool bInValue)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		CString strList;
		strList.Format(L"%s = %s", strInType, (true == bInValue ? L"on" : L"off"));
		HC_Set_Selectability(Utility::ToChar(strList));
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void H3DF::SelectabilityControlImpl::UnsetSelectability(CString strInType)
{
	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_UnSet_One_Selectability(Utility::ToChar(strInType));
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

//== SelectabilityControl ==========================================================================

H3DF::SelectabilityControl::SelectabilityControl(SegmentKey & cInSegment)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<SelectabilityControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::SelectabilityControl::SelectabilityControl(SelectabilityControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

SelectabilityControl & H3DF::SelectabilityControl::operator = (SelectabilityControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

//== Set Selectability Control =====================================================================

SelectabilityControl & H3DF::SelectabilityControl::SetWindows(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("windows", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetEdges(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("edges", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetFaces(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("faces", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetLights(bool bInValue) 
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("lights", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetLines(bool bInValue) 
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("lines", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetMarkers(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("markers", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetVertices(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("vertices", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetText(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("text", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetGeometry(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("geometry", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetEverything(bool bInValue)
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->SetSelectability("everything", bInValue);
	return *this;
}

//== Unset Selectability Control ===================================================================

SelectabilityControl & H3DF::SelectabilityControl::UnsetWindows()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("windows");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetEdges()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("edges");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetFaces()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("faces");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetLights()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("lights");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetLines()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("lines");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetMarkers() 
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("markers");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetVertices()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("vertices");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetText()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("text");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetGeometry() 
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());
	pcImpl->UnsetSelectability("geometry");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetEverything()
{
	auto pcImpl = static_cast<SelectabilityControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);

	HC_UnSet_Selectability();

	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}