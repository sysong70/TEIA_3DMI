#include "StdAfx.h"

#include "Selectability.h"
#include "Private/ControlPrivate.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

//== SelectabilityKitPrivate Class =================================================================
namespace H3DF
{
	class SelectabilityKitPrivate : public PrivateImpl
	{
	public:
		SelectabilityKitPrivate();
		void Empty();

		void Copy(SelectabilityKitPrivate * pcInThat);

		void SetSelectability(H3DF::SelectabilityKit::SelectabilityType eType, bool bInFlag);
		void UnsetSelectability(H3DF::SelectabilityKit::SelectabilityType eType);
	
		bool m_bSelectabilityFlag[(int) SelectabilityKit::SelectabilityType::Count];
		bool m_bUnsetSelectabilityFlag[(int) SelectabilityKit::SelectabilityType::Count];
	};
}

H3DF::SelectabilityKitPrivate::SelectabilityKitPrivate()
{
	Empty();
}

void H3DF::SelectabilityKitPrivate::Empty()
{
	for (auto & bSelectability : m_bSelectabilityFlag) {
		bSelectability = true;
	}

	for (auto & bUnsetSelectability : m_bUnsetSelectabilityFlag) {
		bUnsetSelectability = false;
	}
}

void H3DF::SelectabilityKitPrivate::Copy(SelectabilityKitPrivate * pcInThat)
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

void H3DF::SelectabilityKitPrivate::SetSelectability(H3DF::SelectabilityKit::SelectabilityType eType, bool bInFlag)
{
	m_bSelectabilityFlag[(int) eType] = bInFlag;
}

void H3DF::SelectabilityKitPrivate::UnsetSelectability(H3DF::SelectabilityKit::SelectabilityType eType)
{
	m_bUnsetSelectabilityFlag[(int) eType] = true;
}

//== SelectabilityKit Class ========================================================================
H3DF::SelectabilityKit::SelectabilityKit() 
{
	m_pcImpl = new SelectabilityKitPrivate();
}

H3DF::SelectabilityKit::SelectabilityKit(SelectabilityKit const & cInKit)
{
	m_pcImpl = new SelectabilityKitPrivate();
	Set(cInKit);
}

SelectabilityKit H3DF::SelectabilityKit::GetDefault()
{
	SelectabilityKit cSelectabilityKit;
	return cSelectabilityKit;
}

void H3DF::SelectabilityKit::Set(SelectabilityKit const & cInKit)
{
	SelectabilityKitPrivate * pcImpl = (SelectabilityKitPrivate *) m_pcImpl;
	SelectabilityKitPrivate * pcInKitImpl = (SelectabilityKitPrivate *) cInKit.m_pcImpl;
	pcImpl->Copy(pcInKitImpl);
}

void H3DF::SelectabilityKit::Show(SelectabilityKit & cOutKit) const
{
	SelectabilityKitPrivate * pcImpl = (SelectabilityKitPrivate *) m_pcImpl;
	SelectabilityKitPrivate * pcOutKitImpl = (SelectabilityKitPrivate *) cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

SelectabilityKit & H3DF::SelectabilityKit::operator = (SelectabilityKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

bool H3DF::SelectabilityKit::Empty() const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->Empty();
	return true;
}

bool H3DF::SelectabilityKit::Equals(SelectabilityKit const & cInKit) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	SelectabilityKitPrivate * pcInKitImpl = static_cast<SelectabilityKitPrivate *>(cInKit.m_pcImpl);

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
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Windows, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetEdges(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Edges, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetFaces(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Faces, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetLights(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Lights, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetLines(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Lines, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetMarkers(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Markers, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetVertices(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Vertices, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetText(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Text, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetGeometry(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->SetSelectability(SelectabilityType::Geometry, bInValue);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::SetEverything(bool bInValue)
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);

	for(auto & bSelectability : pcImpl->m_bSelectabilityFlag) {
		bSelectability = bInValue;
	}

	return *this;
}

//== Unset Selectability Kit =======================================================================

SelectabilityKit & H3DF::SelectabilityKit::UnsetWindows()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Windows);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetEdges()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Edges);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetFaces()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Faces);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetLights()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Lights);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetLines()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Lines);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetMarkers()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Markers);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetVertices()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Vertices);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetText()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Text);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetGeometry()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	pcImpl->UnsetSelectability(SelectabilityType::Geometry);
	return *this;
}

SelectabilityKit & H3DF::SelectabilityKit::UnsetEverything()
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);

	for (auto & bUnsetSelectability : pcImpl->m_bUnsetSelectabilityFlag) {
		bUnsetSelectability = true;
	}

	return *this;
}

bool H3DF::SelectabilityKit::ShowWindows(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Windows] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowEdges(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Edges] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowFaces(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Faces] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowLights(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Lights] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowLines(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Lines] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowMarkers(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Markers] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowVertices(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Vertices] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

bool H3DF::SelectabilityKit::ShowText(Selectability::Value & cOuValue) const
{
	SelectabilityKitPrivate * pcImpl = static_cast<SelectabilityKitPrivate *>(m_pcImpl);
	cOuValue = pcImpl->m_bSelectabilityFlag[(int) SelectabilityType::Text] ? Selectability::Value::On : Selectability::Value::Off;
	return true;
}

namespace H3DF
{
	class SelectabilityControlPrivate : public ControlPrivate
	{
	public:
		SelectabilityControlPrivate() = default;

		void Copy(SelectabilityControlPrivate * pcInThat) {};

		void SetSelectability(CString strInType, bool bInValue);
		void UnsetSelectability(CString strInType);
	};
}

void H3DF::SelectabilityControlPrivate::SetSelectability(CString strInType, bool bInValue)
{
	SegmentKeyPrivate::LocalOpen(m_cOverrideKey);

	CString strList;
	strList.Format(L"%s = %s", strInType, (true == bInValue ? L"on" : L"off"));
	HC_Set_Selectability(Utility::ToChar(strList));

	SegmentKeyPrivate::LocalClose(m_cOverrideKey);
}

void H3DF::SelectabilityControlPrivate::UnsetSelectability(CString strInType)
{
	SegmentKeyPrivate::LocalOpen(m_cOverrideKey);

	HC_UnSet_One_Selectability(Utility::ToChar(strInType));

	SegmentKeyPrivate::LocalClose(m_cOverrideKey);
}

//== SelectabilityControl ==========================================================================

H3DF::SelectabilityControl::SelectabilityControl(SegmentKey & cInSegmentKey)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::SelectabilityControl::SelectabilityControl(SelectabilityControl const & cInThat)
{
	m_pcImpl = new SelectabilityControlPrivate();
	Set(cInThat);
}

void H3DF::SelectabilityControl::Set(SelectabilityControl const & cInThat)
{
	SelectabilityControlPrivate * pcImpl = (SelectabilityControlPrivate *) m_pcImpl;
	SelectabilityControlPrivate * pcInThatImpl = (SelectabilityControlPrivate *) cInThat.m_pcImpl;
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
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("windows", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetEdges(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("edges", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetFaces(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("faces", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetLights(bool bInValue) 
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("lights", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetLines(bool bInValue) 
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("lines", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetMarkers(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("markers", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetVertices(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("vertices", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetText(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("text", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetGeometry(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("geometry", bInValue);
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::SetEverything(bool bInValue)
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->SetSelectability("everything", bInValue);
	return *this;
}

//== Unset Selectability Control ===================================================================

SelectabilityControl & H3DF::SelectabilityControl::UnsetWindows()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("windows");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetEdges()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("edges");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetFaces()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("faces");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetLights()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("lights");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetLines()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("lines");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetMarkers() 
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("markers");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetVertices()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("vertices");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetText()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("text");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetGeometry() 
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();
	pcImpl->UnsetSelectability("geometry");
	return *this;
}

SelectabilityControl & H3DF::SelectabilityControl::UnsetEverything()
{
	SelectabilityControlPrivate * pcImpl = new SelectabilityControlPrivate();

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey);

	HC_UnSet_Selectability();

	SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}