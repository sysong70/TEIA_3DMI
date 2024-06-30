#include "StdAfx.h"

#include "DrawingAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

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