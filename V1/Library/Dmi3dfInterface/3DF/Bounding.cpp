#include "StdAfx.h"

#include "Bounding.h"
#include "./Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "Window.h"

#include <HTools.h>

using namespace H3DF;

//== BoundingKit class =============================================================================

namespace H3DF
{
	class BoundingKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<BoundingKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const BoundingKitImpl * pcInThat)
		{
			m_cCuboid = pcInThat->m_cCuboid;

			m_cSphere = pcInThat->m_cSphere;

			m_bExclude = pcInThat->m_bExclude;
			m_bExcludeValid = pcInThat->m_bExcludeValid;
		}

		SimpleCuboid m_cCuboid;
		SimpleSphere m_cSphere;
		bool m_bExclude = false; bool m_bExcludeValid = false;
	};
}

H3DF::BoundingKit::BoundingKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<BoundingKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::BoundingKit::BoundingKit(BoundingKit const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

BoundingKit & H3DF::BoundingKit::operator = (BoundingKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::BoundingKit::Empty() const
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_cCuboid.IsValid() && false == pcImpl->m_cSphere.IsValid()) {
		return true;
	}

	return false;

}

bool H3DF::BoundingKit::Equals(BoundingKit const & cInKit) const
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInThatImpl = static_cast<const BoundingKitImpl *>(cInKit.GetImpl());
	DEBUG_VALID(pcInThatImpl);

	return pcImpl->m_cCuboid == pcInThatImpl->m_cCuboid;
}

bool H3DF::BoundingKit::operator == (BoundingKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::BoundingKit::operator != (BoundingKit const & cInKit) const
{
	return !Equals(cInKit);
}

BoundingKit & H3DF::BoundingKit::SetVolume(SimpleSphere const & cInSphere)
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cSphere = cInSphere;

	return *this;
}

BoundingKit & H3DF::BoundingKit::SetVolume(SimpleCuboid const & cInCuboid)
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cCuboid = cInCuboid;

	return *this;
}

BoundingKit & H3DF::BoundingKit::SetExclusion(bool bInExclude)
{ 
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bExclude = bInExclude;
	pcImpl->m_bExcludeValid = true;

	return *this;
}

BoundingKit & H3DF::BoundingKit::UnsetVolume()
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cCuboid.Invalidate();
	pcImpl->m_cSphere.Invalidate();

	return *this;
}

BoundingKit & H3DF::BoundingKit::UnsetExclusion()
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bExclude = false;
	pcImpl->m_bExcludeValid = false;

	return *this;
}

BoundingKit & H3DF::BoundingKit::UnsetEverything()
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cCuboid.Invalidate();
	pcImpl->m_cSphere.Invalidate();
	pcImpl->m_bExclude = false;

	return *this;
}

bool H3DF::BoundingKit::ShowVolume(SimpleSphere & cOutSphere, SimpleCuboid & cOutCuboid) const
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (true == Empty()) {
		return false;
	}

	if (false == pcImpl->m_cSphere.IsValid() && false == pcImpl->m_cCuboid.IsValid()) {
		return false;
	}

	cOutSphere = pcImpl->m_cSphere;
	cOutCuboid = pcImpl->m_cCuboid;

	return true;
}

bool H3DF::BoundingKit::ShowExclusion(bool & bOutEexclusion) const
{
	auto pcImpl = static_cast<BoundingKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_bExcludeValid) {
		return false;
	}

	bOutEexclusion = pcImpl->m_bExclude;
	return true;
}

//== BoundingControl class =========================================================================

namespace H3DF
{
	class BoundingControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<BoundingControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const BoundingControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		BaseView * GetBaseView();
	};
}

BaseView * BoundingControlImpl::GetBaseView()
{
	SegmentKeyImpl * pcKeyImpl = static_cast<SegmentKeyImpl *>(m_cOverrideKey.GetImpl());
	DEBUG_VALID(pcKeyImpl);

	if (nullptr == pcKeyImpl->GetWindow()) {
		DEBUG_STOP;
		return nullptr;
	}

	return pcKeyImpl->GetWindow()->GetBaseView();
}

H3DF::BoundingControl::BoundingControl(SegmentKey & cInSegmentKey)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<BoundingControlImpl>();
	DEBUG_VALID(m_pcImpl);
	auto pcImpl = dynamic_cast<BoundingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegmentKey;
}

H3DF::BoundingControl::BoundingControl(BoundingControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

BoundingControl & H3DF::BoundingControl::operator = (BoundingControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

BoundingControl & H3DF::BoundingControl::SetExclusion(bool bInExclusion)
{
	auto pcImpl = static_cast<BoundingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strList;
		if(true == bInExclusion) {
			strList.Format("exclude bounding = on");
		} else {
			strList.Format("exclude bounding = off");
		}

		HC_Set_Heuristics(strList);

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

BoundingControl & H3DF::BoundingControl::UnsetExclusion()
{
	auto pcImpl = static_cast<BoundingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("heuristics = exclude bounding")) {
			HC_UnSet_One_Heuristic("exclude bounding");
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::BoundingControl::ShowExclusion(bool & bOutExclusion) const
{
	auto pcImpl = static_cast<BoundingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("heuristics = exclude bounding")) {
			CStringA strValue;
			HC_Show_One_Heuristic("exclude bounding", strValue.GetBuffer(MVO_BUFFER_SIZE));
			strValue.ReleaseBuffer();

			if (false == strValue.IsEmpty()) {
				bResult = true;

				if ("on" == strValue) {
					bOutExclusion = true;
				}
				else {
					bOutExclusion = false;
				}
			}
			else {
				bResult = false;
			}
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}