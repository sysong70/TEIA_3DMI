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
		ColorInterpolationKitImpl() { m_eType = H3DF::Type::ColorInterpolationKit; }

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
		ColorInterpolationControlImpl() { m_eType = H3DF::Type::ColorInterpolationControl; }

		void Copy(ColorInterpolationControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		void SetGeometryColor(CStringA strName, bool bInState);
		void UnsetGeometryColor(CStringA strName);
		bool ShowGeometryColor(CStringA strName, bool & bOutState) const;
	};
}

void H3DF::ColorInterpolationControlImpl::SetGeometryColor(CStringA strName, bool bInState)
{
	CStringA strOption;
	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		if (true == bInState) {
			strOption.Format("color interpolation = %s", strName);
			HC_Set_Rendering_Options(strOption);
		}
		else {
			strOption.Format("color interpolation = no %s", strName);
			HC_Set_Rendering_Options(strOption);
		}
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void H3DF::ColorInterpolationControlImpl::UnsetGeometryColor(CStringA strName)
{
	CStringA strOption;
	strOption.Format("color interpolation = %s", strName);

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option(strOption);
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

bool H3DF::ColorInterpolationControlImpl::ShowGeometryColor(CStringA strName, bool & bOutState) const
{
	bool bResult = false;

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		CStringA strOption;
		strOption.Format("rendering options = (color interpolation = %s)", strName);

		if (0 < HC_Show_Existence(strOption)) {
			bResult = true;
			strOption.Format("color interpolation = %s", strName);
			CStringA strValue;
			HC_Show_One_Rendering_Option(strOption, strValue.GetBuffer(MVO_BUFFER_SIZE));
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
	pcImpl->SetGeometryColor("faces", bInState);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetEdgeColor(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometryColor("edges", bInState);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetVertexColor(bool bInState)
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometryColor("markers", bInState);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetFaceColor()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometryColor("faces");

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEdgeColor()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometryColor("edges");

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetVertexColor()
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometryColor("markers");

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
	return pcImpl->ShowGeometryColor("faces", bOutState);
}

bool H3DF::ColorInterpolationControl::ShowEdgeColor(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometryColor("edges", bOutState);
}

bool H3DF::ColorInterpolationControl::ShowVertexColor(bool & bOutState) const
{
	ColorInterpolationControlImpl * pcImpl = (ColorInterpolationControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometryColor("markers", bOutState);
}
