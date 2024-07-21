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
			m_fDepthRangeNear = pcInThat->m_fDepthRangeNear;
			m_fDepthRangeFar = pcInThat->m_fDepthRangeFar;

			m_bFaceDisplacement = pcInThat->m_bFaceDisplacement;
			m_nFaceDisplacementBuckets = pcInThat->m_nFaceDisplacementBuckets;
		}

		bool Equals(DrawingAttributeKitImpl * pcInThat)
		{
			if (m_bDepthRange != pcInThat->m_bDepthRange) return false;
			if (m_fDepthRangeNear != pcInThat->m_fDepthRangeNear) return false;
			if (m_fDepthRangeFar != pcInThat->m_fDepthRangeFar) return false;

			if (m_bFaceDisplacement != pcInThat->m_bFaceDisplacement) return false;
			if (m_nFaceDisplacementBuckets != pcInThat->m_nFaceDisplacementBuckets) return false;

			return true;
		}

		bool m_bDepthRange = false;
		float m_fDepthRangeNear = 0.0f, m_fDepthRangeFar = 1.0f;

		bool m_bFaceDisplacement = false;
		float m_nFaceDisplacementBuckets = 8;
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
	pcImpl->m_fDepthRangeNear = fInNear;
	pcImpl->m_fDepthRangeFar = fInFar;

	return *this;
}

DrawingAttributeKit & H3DF::DrawingAttributeKit::SetFaceDisplacement(int nInBuckets)
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = true;
	pcImpl->m_nFaceDisplacementBuckets = nInBuckets;

	return *this;
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

bool H3DF::DrawingAttributeKit::ShowDepthRange(float & fOutNear, float & fOutFar) const
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	fOutNear = pcImpl->m_fDepthRangeNear;
	fOutFar = pcImpl->m_fDepthRangeFar;

	return pcImpl->m_bDepthRange;
}

bool H3DF::DrawingAttributeKit::ShowFaceDisplacement(int & nOutBuckets) const
{
	DrawingAttributeKitImpl * pcImpl = (DrawingAttributeKitImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	nOutBuckets = pcImpl->m_nFaceDisplacementBuckets;

	return pcImpl->m_bFaceDisplacement;
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

H3DF::DrawingAttributeControl::DrawingAttributeControl() {}

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

DrawingAttributeControl & H3DF::DrawingAttributeControl::SetFaceDisplacement(int nInBuckets)
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strOption;
		strOption.Format("face displacement = %d", nInBuckets);
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
			HC_Set_Heuristics("no quick moves");
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

DrawingAttributeControl & H3DF::DrawingAttributeControl::UnsetDepthRange()
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("rendering options = depth range")) {
			HC_UnSet_One_Rendering_Option("depth range");
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

DrawingAttributeControl & H3DF::DrawingAttributeControl::UnsetFaceDisplacement()
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("rendering options = face displacement")) {
			HC_UnSet_One_Rendering_Option("face displacement");
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

DrawingAttributeControl & H3DF::DrawingAttributeControl::UnsetOverlay()
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("heuristic = quick moves")) {
			HC_UnSet_One_Heuristic("quick moves");
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::DrawingAttributeControl::ShowDepthRange(float & fOutX, float & fOutY) const
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("rendering options = depth range")) {
			bResult = true;
			CStringA strValue;
			HC_Show_One_Rendering_Option("depth range", strValue.GetBuffer(MVO_BUFFER_SIZE));
			strValue.ReleaseBuffer();

			sscanf(strValue, "%f,%f", &fOutX, &fOutY);
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}

bool H3DF::DrawingAttributeControl::ShowFaceDisplacement(int & nOutBuckets) const
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("rendering options = face displacement")) {
			bResult = true;
			CStringA strValue;
			HC_Show_One_Rendering_Option("face displacement", strValue.GetBuffer(MVO_BUFFER_SIZE));
			strValue.ReleaseBuffer();

			sscanf(strValue, "%d", &nOutBuckets);
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}

bool H3DF::DrawingAttributeControl::ShowOverlay(Drawing::Overlay & eOutOverlay) const
{
	DrawingAttributeControlImpl * pcImpl = (DrawingAttributeControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		if (0 < HC_Show_Existence("heuristics = quick moves")) {
			CStringA strValue;
			HC_Show_One_Heuristic("quick moves", strValue.GetBuffer(MVO_BUFFER_SIZE));
			strValue.ReleaseBuffer();

			if (false == strValue.IsEmpty()) {
				bResult = true;

				if ("on" == strValue) {
					eOutOverlay = Drawing::Overlay::Default;
				}
				else if ("spriting" == strValue) {
					eOutOverlay = Drawing::Overlay::WithZValues;
				}
				else if ("inplace" == strValue) {
					eOutOverlay = Drawing::Overlay::InPlace;
				}
				else {
					eOutOverlay = Drawing::Overlay::None;
					DEBUG_STOP;
				}
			}
			else {
				bResult = false;
			}
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}