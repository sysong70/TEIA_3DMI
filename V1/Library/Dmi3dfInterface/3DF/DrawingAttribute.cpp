#include "StdAfx.h"

#include "DrawingAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include <hc.h>
#include <HTools.h>

using namespace H3DF;

//== DrawingAttributeKit class =====================================================================

namespace H3DF
{
	class DrawingAttributeKitImpl : public Impl
	{
	public:
		DrawingAttributeKitImpl() { m_eType = H3DF::Type::DrawingAttributeKit; }

		void Copy(DrawingAttributeKitImpl * pcInThat) 
		{
			m_bDepthRange = pcInThat->m_bDepthRange;
			m_fInNear = pcInThat->m_fInNear;
			m_fInFar = pcInThat->m_fInFar;
		}

		bool Equals(DrawingAttributeKitImpl * pcInThat)
		{
			if (m_bDepthRange != pcInThat->m_bDepthRange) return false;
			if (m_fInNear != pcInThat->m_fInNear) return false;
			if (m_fInFar != pcInThat->m_fInFar) return false;

			return true;
		}

		bool m_bDepthRange = false;
		float m_fInNear = 0.0f, m_fInFar = 1.0f;

		bool m_bFaceDisplacement = false;
		int m_nBuckets = 8;
	};
}

H3DF::DrawingAttributeKit::DrawingAttributeKit() 
{
	m_pcImpl = new DrawingAttributeKitImpl();
}

H3DF::DrawingAttributeKit::DrawingAttributeKit(DrawingAttributeKit const & cInKit)
{
	m_pcImpl = new DrawingAttributeKitImpl();
	Set(cInKit);
}

void H3DF::DrawingAttributeKit::Set(DrawingAttributeKit const & cInKit)
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DrawingAttributeKitImpl * pcInThatImpl = (DrawingAttributeKitImpl *)cInKit.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

DrawingAttributeKit const & H3DF::DrawingAttributeKit::operator = (DrawingAttributeKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

void H3DF::DrawingAttributeKit::Show(DrawingAttributeKit & cOutKit) const
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DrawingAttributeKitImpl * pcOutKitImpl = (DrawingAttributeKitImpl *)cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

bool H3DF::DrawingAttributeKit::Empty() const
{
	return true;
}

bool H3DF::DrawingAttributeKit::Equals(DrawingAttributeKit const & cInKit) const
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DrawingAttributeKitImpl * pcInThatImpl = (DrawingAttributeKitImpl *)cInKit.m_pcImpl;
	return pcImpl->Equals(pcInThatImpl);
}

bool H3DF::DrawingAttributeKit::operator == (DrawingAttributeKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::DrawingAttributeKit::operator != (DrawingAttributeKit const & cInKit) const
{
	return !(*this == cInKit);
}

DrawingAttributeKit & H3DF::DrawingAttributeKit::SetDepthRange(float fInNear, float fInFar)
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDepthRange = true;
	pcImpl->m_fInNear = fInNear;
	pcImpl->m_fInFar = fInFar;

	return *this;
}

DrawingAttributeKit & H3DF::DrawingAttributeKit::SetFaceDisplacement(bool bInState, int bInBuckets)
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = bInState;
	pcImpl->m_nBuckets = bInBuckets;

	return *this;
}

DrawingAttributeKit & H3DF::DrawingAttributeKit::SetFaceDisplacement(int bInBuckets)
{
	return SetFaceDisplacement(true, bInBuckets);
}


DrawingAttributeKit & H3DF::DrawingAttributeKit::UnsetDepthRange()
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDepthRange = false;

	return *this;
}

DrawingAttributeKit & H3DF::DrawingAttributeKit::UnsetFaceDisplacement()
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = false;

	return *this;
}

//== DrawingAttributeControl class =================================================================

namespace H3DF
{
	class DrawingAttributeControlImpl : public ControlImpl
	{
	public:
		DrawingAttributeControlImpl() { m_eType = H3DF::Type::DrawingAttributeControl; }

		void Copy(DrawingAttributeControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}
H3DF::DrawingAttributeControl::DrawingAttributeControl(SegmentKey & cInSegmentKey)
{
	DrawingAttributeControlImpl * pcImpl = new DrawingAttributeControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::DrawingAttributeControl::DrawingAttributeControl(DrawingAttributeControl const & cInThat)
{
	m_pcImpl = new DrawingAttributeControlImpl();
	Set(cInThat);
}

void H3DF::DrawingAttributeControl::Set(DrawingAttributeControl const & cInThat)
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DrawingAttributeControlImpl * pcInThatImpl = (DrawingAttributeControlImpl *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

DrawingAttributeControl & H3DF::DrawingAttributeControl::operator = (DrawingAttributeControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

DrawingAttributeControl & H3DF::DrawingAttributeControl::SetDepthRange(float fInNear, float fInFar)
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strOption;
		strOption.Format("depth range = (%f, %f)", fInNear, fInFar);
		HC_Set_Rendering_Options(strOption);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

DrawingAttributeControl & H3DF::DrawingAttributeControl::SetOverlay(Drawing::Overlay eInOverlay)
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {

		switch (eInOverlay)
		{
		case Drawing::Overlay::None:
			HC_Set_Heuristics("quick moves = off");
			break;

		case Drawing::Overlay::Default:
			HC_Set_Heuristics("quick moves = on");
			break;

		case Drawing::Overlay::WithZValues:
			HC_Set_Heuristics("quick moves = spriting");
			break;

		case Drawing::Overlay::InPlace:
			HC_Set_Heuristics("quick moves = inplace");
			break;

		default:
			DEBUG_STOP;
			break;
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}