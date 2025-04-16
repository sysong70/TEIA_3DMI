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
		BoundingKitImpl() { m_eType = H3DF::Type::BoundingKit; }

		void Copy(BoundingKitImpl * pcInThat)
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
	m_pcImpl = new BoundingKitImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::BoundingKit::BoundingKit(BoundingKit const & cInThat)
{
	m_pcImpl = new BoundingKitImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}

void H3DF::BoundingKit::Set(BoundingKit const & cInThat)
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	BoundingKitImpl * pcInThatImpl = (BoundingKitImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

BoundingKit & H3DF::BoundingKit::operator = (BoundingKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::BoundingKit::Empty() const
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_cCuboid.IsValid() && false == pcImpl->m_cSphere.IsValid()) {
		return true;
	}

	return false;

}

bool H3DF::BoundingKit::Equals(BoundingKit const & cInKit) const
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	BoundingKitImpl * pcInThatImpl = (BoundingKitImpl *)cInKit.m_pcImpl;
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
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cSphere = cInSphere;

	return *this;
}

BoundingKit & H3DF::BoundingKit::SetVolume(SimpleCuboid const & cInCuboid)
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cCuboid = cInCuboid;

	return *this;
}

BoundingKit & H3DF::BoundingKit::SetExclusion(bool bInExclude)
{ 
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bExclude = bInExclude;
	pcImpl->m_bExcludeValid = true;

	return *this;
}

BoundingKit & H3DF::BoundingKit::UnsetVolume()
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cCuboid.Invalidate();
	pcImpl->m_cSphere.Invalidate();

	return *this;
}

BoundingKit & H3DF::BoundingKit::UnsetExclusion()
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bExclude = false;
	pcImpl->m_bExcludeValid = false;

	return *this;
}

BoundingKit & H3DF::BoundingKit::UnsetEverything()
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cCuboid.Invalidate();
	pcImpl->m_cSphere.Invalidate();
	pcImpl->m_bExclude = false;

	return *this;
}

bool H3DF::BoundingKit::ShowVolume(SimpleSphere & cOutSphere, SimpleCuboid & cOutCuboid) const
{
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
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
	BoundingKitImpl * pcImpl = (BoundingKitImpl *)m_pcImpl;
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
		BoundingControlImpl() { m_eType = H3DF::Type::BoundingControl; }

		void Copy(BoundingControlImpl * pcInThat) {
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
	BoundingControlImpl * pcImpl = new BoundingControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::BoundingControl::BoundingControl(BoundingControl const & cInThat)
{
	m_pcImpl = new BoundingControlImpl();
	Set(cInThat);
}

void H3DF::BoundingControl::Set(BoundingControl const & cInThat)
{
	BoundingControlImpl * pcImpl = (BoundingControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	BoundingControlImpl * pcInThatImpl = (BoundingControlImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

BoundingControl & H3DF::BoundingControl::operator = (BoundingControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

BoundingControl & H3DF::BoundingControl::SetExclusion(bool bInExclusion)
{
	BoundingControlImpl * pcImpl = (BoundingControlImpl *) m_pcImpl;
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
	BoundingControlImpl * pcImpl = (BoundingControlImpl *) m_pcImpl;
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
	BoundingControlImpl * pcImpl = (BoundingControlImpl *) m_pcImpl;
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