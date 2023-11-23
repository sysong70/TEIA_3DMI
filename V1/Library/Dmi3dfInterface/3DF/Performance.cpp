#include "StdAfx.h"

#include "Performance.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"
#include "./Private/ControlPrivate.h"
#include "../Private/View.Private.h"

#include <hc.h>
#include <HTools.h>

#include <Common_Define.h>

//== PerformanceKit class ==========================================================================

namespace H3DF
{
	class PerformanceKitPrivate : public PrivateImpl
	{
	public:
		PerformanceKitPrivate() { m_eType = H3DF::Type::PerformanceKit; }

		void Copy(PerformanceKitPrivate * pcInThat) {
			m_eInDisplayList = pcInThat->m_eInDisplayList;
		}

		void Default() {
			m_eInDisplayList = Performance::DisplayLists::Segment;
			m_eInModelType = Performance::StaticModel::Attribute;
		}

		Performance::DisplayLists m_eInDisplayList = Performance::DisplayLists::Segment;
		Performance::StaticModel m_eInModelType = Performance::StaticModel::Attribute;
	};
}

using namespace H3DF;

H3DF::PerformanceKit::PerformanceKit()
{
	m_pcImpl = new PerformanceKitPrivate();
}

H3DF::PerformanceKit::PerformanceKit(PerformanceKit const & cInThat)
{
	m_pcImpl = new PerformanceKitPrivate();
	Set(cInThat);
}

void H3DF::PerformanceKit::Set(PerformanceKit const & cInThat)
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	PerformanceKitPrivate * pcInThatImpl = static_cast<PerformanceKitPrivate *>(cInThat.m_pcImpl);
	pcImpl->Copy(pcInThatImpl);
}

PerformanceKit const & H3DF::PerformanceKit::operator = (PerformanceKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

PerformanceKit & H3DF::PerformanceKit::SetDisplayLists(Performance::DisplayLists eInDisplayList)
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	pcImpl->m_eInDisplayList = eInDisplayList;

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::SetStaticModel(Performance::StaticModel eInModelType)
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	pcImpl->m_eInModelType = eInModelType;

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::UnsetDisplayLists()
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	pcImpl->m_eInDisplayList = Performance::DisplayLists::None;

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::UnsetStaticModel()
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	pcImpl->m_eInModelType = Performance::StaticModel::None;

	return *this;
}

bool H3DF::PerformanceKit::ShowDisplayLists(Performance::DisplayLists & eOutDisplayList) const
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); return false; }

	eOutDisplayList = pcImpl->m_eInDisplayList;

	return true;
}

bool H3DF::PerformanceKit::ShowStaticModel(Performance::StaticModel & eOutModelType) const
{
	PerformanceKitPrivate * pcImpl = static_cast<PerformanceKitPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); return false; }

	eOutModelType = pcImpl->m_eInModelType;

	return true;
}

//== PerformanceControl class ======================================================================
class PerformanceControlPrivate : public ControlPrivate
{
public:
	PerformanceControlPrivate() { m_eType = H3DF::Type::PerformanceControl; }

	void Copy(PerformanceControlPrivate * pcInThat) {
		ControlPrivate::Copy(pcInThat);
	}
};

H3DF::PerformanceControl::PerformanceControl(SegmentKey & cInSegmentKey)
{
	PerformanceControlPrivate * pcImpl = new PerformanceControlPrivate();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::PerformanceControl::PerformanceControl(PerformanceControl const & cInThat)
{
	m_pcImpl = new PerformanceControlPrivate();
	Set(cInThat);
}

void H3DF::PerformanceControl::Set(PerformanceControl const & cInThat)
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	PerformanceControlPrivate * pcInThatImpl = static_cast<PerformanceControlPrivate *>(cInThat.m_pcImpl);
	pcImpl->Copy(pcInThatImpl);
}

PerformanceControl & H3DF::PerformanceControl::operator = (PerformanceControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

PerformanceControl & H3DF::PerformanceControl::SetDisplayLists(Performance::DisplayLists eInDisplayList)
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey); {
		switch (eInDisplayList)
		{
			case H3DF::Performance::DisplayLists::None:
				HC_Set_Rendering_Options("no display lists");
				break;

			case H3DF::Performance::DisplayLists::Geometry:
				HC_Set_Rendering_Options("display lists = geometry");
				break;

			case H3DF::Performance::DisplayLists::Segment:
				HC_Set_Rendering_Options("display lists = segment");
				break;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::SetStaticModel(Performance::StaticModel eInModelType)
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey); {
		switch (eInModelType)
		{
			case H3DF::Performance::StaticModel::None:
				HC_Set_Heuristics("static model = off");
				break;

			case H3DF::Performance::StaticModel::Attribute:
				HC_Set_Heuristics("static model = on");
				break;

			case H3DF::Performance::StaticModel::AttributeSpatial:
				HC_Set_Heuristics("static model = (on, condition analysis = view independent)");
				break;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::UnsetDisplayLists()
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option("display lists");
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::UnsetStaticModel()
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Heuristic("static model");
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::PerformanceControl::ShowDisplayLists(Performance::DisplayLists & eOutDisplayList) const
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey); {
		if (HC_Show_Existence("rendering options = display lists")) {
			char chValue[256] = { 0 };
			if (TRUE == HC_PShow_One_Net_Rendering_Option(0, 0, "display lists", chValue)) {
				if (strstr(chValue, "geometry") || strstr(chValue, "on")) {
					eOutDisplayList = Performance::DisplayLists::Geometry;
				}
				else if (nullptr != strstr(chValue, "segment")) {
					eOutDisplayList = Performance::DisplayLists::Segment;
				}
				else {
					eOutDisplayList = Performance::DisplayLists::None;
				}
			}
		}
		else {
			eOutDisplayList = Performance::DisplayLists::None;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return true;
}

bool H3DF::PerformanceControl::ShowStaticModel(Performance::StaticModel & eOutModelType) const
{
	PerformanceControlPrivate * pcImpl = static_cast<PerformanceControlPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cOverrideKey); {
		if (HC_Show_Existence("heuristics = static model")) {
			char chValue[256] = { 0 };
			HC_Show_One_Heuristic("static model", chValue);

			if (strstr(chValue, "view independent")) {
				eOutModelType = Performance::StaticModel::AttributeSpatial;
			}
			else if (strstr(chValue, "on")) {
				eOutModelType = Performance::StaticModel::Attribute;
			}
			else {
				eOutModelType = Performance::StaticModel::None;
			}
		}
		else {
			eOutModelType = Performance::StaticModel::None;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cOverrideKey);

	return true;
/*

	BaseView * pcView = pcImpl->GetBaseView();
	if (nullptr == pcView) {
		assert(false);
		return false;
	}

	HC_Open_Segment_By_Key(pcView->GetModelKey());
	if (HC_Show_Existence("heuristics = static model")) {
		char chValue[256] = { 0 };
		HC_Show_One_Heuristic("static model", chValue);

		if (strstr(chValue, "view independent")) {
			eOutModelType = Performance::StaticModel::AttributeSpatial;
		}
		else if (strstr(chValue, "on")) {
			eOutModelType = Performance::StaticModel::Attribute;
		}
		else {
			eOutModelType = Performance::StaticModel::None;
		}
	}
	else {
		eOutModelType = Performance::StaticModel::None;
	}
	HC_Close_Segment();
*/

	return true;
}
