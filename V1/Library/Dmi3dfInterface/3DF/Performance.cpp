#include "StdAfx.h"

#include "Performance.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"
#include "../Sprocket/Impl/3DF.View.Impl.h"

#include <hc.h>
#include <HTools.h>

#include <Common_Define.h>

//== PerformanceKit class ==========================================================================

namespace H3DF
{
	class PerformanceKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<PerformanceKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const PerformanceKitImpl * pcInThat) {
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
	m_pcImpl = std::make_unique<PerformanceKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::PerformanceKit::PerformanceKit(PerformanceKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

PerformanceKit const & H3DF::PerformanceKit::operator = (PerformanceKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::SetDisplayLists(Performance::DisplayLists eInDisplayList)
{
	auto pcImpl = static_cast<PerformanceKitImpl *>(m_pcImpl.get());
	pcImpl->m_eInDisplayList = eInDisplayList;

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::SetStaticModel(Performance::StaticModel eInModelType)
{
	auto pcImpl = static_cast<PerformanceKitImpl *>(m_pcImpl.get());
	pcImpl->m_eInModelType = eInModelType;

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::UnsetDisplayLists()
{
	auto pcImpl = static_cast<PerformanceKitImpl *>(m_pcImpl.get());
	pcImpl->m_eInDisplayList = Performance::DisplayLists::None;

	return *this;
}

PerformanceKit & H3DF::PerformanceKit::UnsetStaticModel()
{
	auto pcImpl = static_cast<PerformanceKitImpl *>(m_pcImpl.get());
	pcImpl->m_eInModelType = Performance::StaticModel::None;

	return *this;
}

bool H3DF::PerformanceKit::ShowDisplayLists(Performance::DisplayLists & eOutDisplayList) const
{
	auto pcImpl = static_cast<PerformanceKitImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); return false; }

	eOutDisplayList = pcImpl->m_eInDisplayList;

	return true;
}

bool H3DF::PerformanceKit::ShowStaticModel(Performance::StaticModel & eOutModelType) const
{
	auto pcImpl = static_cast<PerformanceKitImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); return false; }

	eOutModelType = pcImpl->m_eInModelType;

	return true;
}

//== PerformanceControl class ======================================================================
class PerformanceControlImpl : public ControlImpl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<PerformanceControlImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const PerformanceControlImpl * pcInThat) {
		ControlImpl::Copy(pcInThat);
	}
};

H3DF::PerformanceControl::PerformanceControl(SegmentKey & cInSegment)
{
	m_pcImpl = std::make_unique<PerformanceControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::PerformanceControl::PerformanceControl(PerformanceControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

PerformanceControl & H3DF::PerformanceControl::operator = (PerformanceControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::SetDisplayLists(Performance::DisplayLists eInDisplayList)
{
	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
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
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::SetStaticModel(Performance::StaticModel eInModelType)
{
	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
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
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::UnsetDisplayLists()
{
	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option("display lists");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PerformanceControl & H3DF::PerformanceControl::UnsetStaticModel()
{
	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Heuristic("static model");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::PerformanceControl::ShowDisplayLists(Performance::DisplayLists & eOutDisplayList) const
{
	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
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
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return true;
}

bool H3DF::PerformanceControl::ShowStaticModel(Performance::StaticModel & eOutModelType) const
{
	auto pcImpl = static_cast<PerformanceControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
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
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return true;
}
