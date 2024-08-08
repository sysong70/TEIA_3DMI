#include "StdAfx.h"

#include "ColorInterpolation.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include <hc.h>
#include <HTools.h>

using namespace H3DF;

//== ColorInterpolationKit class =====================================================================

namespace H3DF
{
	class ColorInterpolationKitImpl : public Impl
	{
	public:
		void Copy(ColorInterpolationKitImpl * pcInThat) 
		{
			m_bDepthRange = pcInThat->m_bDepthRange;
			m_fDepthRangeNear = pcInThat->m_fDepthRangeNear;
			m_fDepthRangeFar = pcInThat->m_fDepthRangeFar;

			m_bFaceDisplacement = pcInThat->m_bFaceDisplacement;
			m_nFaceDisplacementBuckets = pcInThat->m_nFaceDisplacementBuckets;
		}

		bool Equals(ColorInterpolationKitImpl * pcInThat)
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
		int m_nFaceDisplacementBuckets = 8;
	};
}

H3DF::ColorInterpolationKit::ColorInterpolationKit() 
{
	m_pcImpl = new ColorInterpolationKitImpl();
}

H3DF::ColorInterpolationKit::ColorInterpolationKit(ColorInterpolationKit const & cInKit)
{
	m_pcImpl = new ColorInterpolationKitImpl();
	Set(cInKit);
}

void H3DF::ColorInterpolationKit::Set(ColorInterpolationKit const & cInKit)
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	ColorInterpolationKitImpl * pcInThatImpl = (ColorInterpolationKitImpl *)cInKit.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ColorInterpolationKit const & H3DF::ColorInterpolationKit::operator = (ColorInterpolationKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

void H3DF::ColorInterpolationKit::Show(ColorInterpolationKit & cOutKit) const
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	ColorInterpolationKitImpl * pcOutKitImpl = (ColorInterpolationKitImpl *)cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

bool H3DF::ColorInterpolationKit::Empty() const
{
	return true;
}

bool H3DF::ColorInterpolationKit::Equals(ColorInterpolationKit const & cInKit) const
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	ColorInterpolationKitImpl * pcInThatImpl = (ColorInterpolationKitImpl *)cInKit.m_pcImpl;
	return pcImpl->Equals(pcInThatImpl);
}

bool H3DF::ColorInterpolationKit::operator == (ColorInterpolationKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::ColorInterpolationKit::operator != (ColorInterpolationKit const & cInKit) const
{
	return !(*this == cInKit);
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::SetDepthRange(float fInNear, float fInFar)
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDepthRange = true;
	pcImpl->m_fDepthRangeNear = fInNear;
	pcImpl->m_fDepthRangeFar = fInFar;

	return *this;
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::SetFaceDisplacement(int nInBuckets)
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = true;
	pcImpl->m_nFaceDisplacementBuckets = nInBuckets;

	return *this;
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::UnsetDepthRange()
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDepthRange = false;

	return *this;
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::UnsetFaceDisplacement()
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = false;

	return *this;
}

bool H3DF::ColorInterpolationKit::ShowDepthRange(float & fOutNear, float & fOutFar) const
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	fOutNear = pcImpl->m_fDepthRangeNear;
	fOutFar = pcImpl->m_fDepthRangeFar;

	return pcImpl->m_bDepthRange;
}

bool H3DF::ColorInterpolationKit::ShowFaceDisplacement(int & nOutBuckets) const
{
	ColorInterpolationKitImpl * pcImpl = (ColorInterpolationKitImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	nOutBuckets = pcImpl->m_nFaceDisplacementBuckets;

	return pcImpl->m_bFaceDisplacement;
}


//== ColorInterpolationControl class =================================================================

namespace H3DF
{
	class ColorInterpolationControlImpl : public ControlImpl
	{
	public:
		void Copy(ColorInterpolationControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		void SetGeometry(CStringA strName, bool bInState, bool bColor);
		void UnsetGeometry(CStringA strName, bool bColor);
		bool ShowGeometry(CStringA strName, bool & bOutState, bool bColor) const;
	};
}

void H3DF::ColorInterpolationControlImpl::SetGeometry(CStringA strName, bool bInState, bool bColor)
{
	CStringA strOption;
	if(true == bColor) {
		strOption = "color interpolation = ";
	}
	else {
		strOption = "color index interpolation = ";
	}

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		if (false == bInState) {
			strOption += "no ";
		}
		
		strOption += strName;
		HC_Set_Rendering_Options(strOption);
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void H3DF::ColorInterpolationControlImpl::UnsetGeometry(CStringA strName, bool bColor)
{
	CStringA strOption;
	if (true == bColor) {
		strOption = "color interpolation = ";
	}
	else {
		strOption = "color index interpolation = ";
	}

	strOption += strName;

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option(strOption);
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

bool H3DF::ColorInterpolationControlImpl::ShowGeometry(CStringA strName, bool & bOutState, bool bColor) const
{
	bool bResult = false;

	CStringA strColorOption;
	if (true == bColor) {
		strColorOption = "color interpolation = ";
	}
	else {
		strColorOption = "color index interpolation = ";
	}

	strColorOption += strName;

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		CStringA strOption;
		strOption.Format("rendering options = (%s)", strColorOption);

		if (0 < HC_Show_Existence(strOption)) {
			bResult = true;
			CStringA strValue;
			HC_Show_One_Rendering_Option(strColorOption, strValue.GetBuffer(MVO_BUFFER_SIZE));
			strValue.ReleaseBuffer();

			if (0 == strValue.CompareNoCase("on")) {
				bOutState = true;
			}
			else { // "off"
				bOutState = false;
			}
		}
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);

	return bResult;
}

H3DF::ColorInterpolationControl::ColorInterpolationControl() {}

H3DF::ColorInterpolationControl::ColorInterpolationControl(SegmentKey & cInSegmentKey)
{
	ColorInterpolationControlImpl * pcImpl = new ColorInterpolationControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::ColorInterpolationControl::ColorInterpolationControl(ColorInterpolationControl const & cInThat)
{
	m_pcImpl = new ColorInterpolationControlImpl();
	Set(cInThat);
}

void H3DF::ColorInterpolationControl::Set(ColorInterpolationControl const & cInThat)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	ColorInterpolationControlImpl * pcInThatImpl = (ColorInterpolationControlImpl *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::operator = (ColorInterpolationControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetFaceColor(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("faces", bInState, true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetEdgeColor(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("edges", bInState, true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetVertexColor(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("markers", bInState, true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetFaceIndex(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("faces", bInState, false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetEdgeIndex(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("edges", bInState, false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetVertexIndex(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("markers", bInState, false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetFaceColor()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("faces", true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEdgeColor()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("edges", true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetVertexColor()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("markers", true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetFaceIndex()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("faces", false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEdgeIndex()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("edges", false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetVertexIndex()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("markers", false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEverything()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option("color interpolation");
		HC_UnSet_One_Rendering_Option("color index interpolation");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::ColorInterpolationControl::ShowFaceColor(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("faces", bOutState, true);
}

bool H3DF::ColorInterpolationControl::ShowEdgeColor(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("edges", bOutState, true);
}

bool H3DF::ColorInterpolationControl::ShowVertexColor(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("markers", bOutState, true);
}

bool H3DF::ColorInterpolationControl::ShowFaceIndex(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("faces", bOutState, false);
}

bool H3DF::ColorInterpolationControl::ShowEdgeIndex(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("edges", bOutState, false);
}

bool H3DF::ColorInterpolationControl::ShowVertexIndex(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("markers", bOutState, false);
}
