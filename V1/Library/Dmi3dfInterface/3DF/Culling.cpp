#include "StdAfx.h"

#include "Culling.h"
#include "./Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include <HTools.h>

using namespace H3DF;

//== BoundingKit class =============================================================================
/*

namespace H3DF
{
	class BoundingKitImpl : public Impl
	{
	public:
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
*/

//== CullingControl class =========================================================================

namespace H3DF
{
	class CullingControlImpl : public ControlImpl
	{
	public:
		void Copy(CullingControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		BaseView * GetBaseView();
	};
}

BaseView * CullingControlImpl::GetBaseView()
{
	SegmentKeyImpl * pcKeyImpl = static_cast<SegmentKeyImpl *>(m_cOverrideKey.GetImpl());
	DEBUG_VALID(pcKeyImpl);

	return pcKeyImpl->GetBaseView();
}

H3DF::CullingControl::CullingControl(SegmentKey & cInSegmentKey)
{
	CullingControlImpl * pcImpl = new CullingControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::CullingControl::CullingControl(CullingControl const & cInThat)
{
	m_pcImpl = new CullingControlImpl();
	Set(cInThat);
}

void H3DF::CullingControl::Set(CullingControl const & cInThat)
{
	CullingControlImpl * pcImpl = (CullingControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	CullingControlImpl * pcInThatImpl = (CullingControlImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

CullingControl & H3DF::CullingControl::operator = (CullingControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

CullingControl & H3DF::CullingControl::SetBackFace(bool bInState)
{
	CullingControlImpl * pcImpl = (CullingControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strList;
		if (true == bInState) {
			strList.Format("backplane cull = on");
		}
		else {
			strList.Format("backplane cull = off");
		}

		HC_Set_Heuristics(strList);

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CullingControl & H3DF::CullingControl::SetFace(Culling::Face eInState)
{
	CullingControlImpl * pcImpl = (CullingControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strList;

		switch (eInState) 
		{
		case Culling::Face::Off:
			strList.Format("no face culling");
			break;

		case Culling::Face::Front:
			strList.Format("face culling front");
			break;

		case Culling::Face::Back:
			strList.Format("face culling back");
			break;
		}

		HC_Set_Heuristics(strList);

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

/*
CullingControl & H3DF::CullingControl::SetExclusion(bool bInExclusion)
{
	CullingControlImpl * pcImpl = (CullingControlImpl *) m_pcImpl;
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

CullingControl & H3DF::CullingControl::UnsetExclusion()
{
	CullingControlImpl * pcImpl = (CullingControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("heuristics = exclude bounding")) {
			HC_UnSet_One_Heuristic("exclude bounding");
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::CullingControl::ShowExclusion(bool & bOutExclusion) const
{
	CullingControlImpl * pcImpl = (CullingControlImpl *) m_pcImpl;
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
}*/