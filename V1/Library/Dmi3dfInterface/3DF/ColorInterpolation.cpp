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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ColorInterpolationKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ColorInterpolationKitImpl * pcInThat) 
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ColorInterpolationKitImpl>();
}

H3DF::ColorInterpolationKit::ColorInterpolationKit(ColorInterpolationKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.m_pcImpl) ? cInKit.m_pcImpl->Clone() : nullptr;
}

ColorInterpolationKit const & H3DF::ColorInterpolationKit::operator = (ColorInterpolationKit const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ColorInterpolationKit::Show(ColorInterpolationKit & cOutKit) const
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	auto pcOutImpl = static_cast<ColorInterpolationKitImpl *>(cOutKit.m_pcImpl.get());

	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return;
	}

	if (nullptr == pcOutImpl) {
		// cOutKit이 Impl을 아직 할당받지 않았다면 새로 할당
		cOutKit.m_pcImpl = std::make_unique<ColorInterpolationKitImpl>();
		pcOutImpl = static_cast<ColorInterpolationKitImpl *>(cOutKit.m_pcImpl.get());
	}

	// 복사 (Copy 함수가 있다면 Copy 활용)
	pcOutImpl->Copy(pcImpl);
}

bool H3DF::ColorInterpolationKit::Empty() const
{
	return true;
}

bool H3DF::ColorInterpolationKit::Equals(ColorInterpolationKit const & cInKit) const
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<ColorInterpolationKitImpl *>(cInKit.m_pcImpl.get());

	if(nullptr == pcImpl || nullptr == pcInThatImpl) {
		DEBUG_STOP;
		return false; // 하나라도 Impl이 없다면 같지 않음
	}

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
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDepthRange = true;
	pcImpl->m_fDepthRangeNear = fInNear;
	pcImpl->m_fDepthRangeFar = fInFar;

	return *this;
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::SetFaceDisplacement(int nInBuckets)
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = true;
	pcImpl->m_nFaceDisplacementBuckets = nInBuckets;

	return *this;
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::UnsetDepthRange()
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDepthRange = false;

	return *this;
}

ColorInterpolationKit & H3DF::ColorInterpolationKit::UnsetFaceDisplacement()
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFaceDisplacement = false;

	return *this;
}

bool H3DF::ColorInterpolationKit::ShowDepthRange(float & fOutNear, float & fOutFar) const
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	fOutNear = pcImpl->m_fDepthRangeNear;
	fOutFar = pcImpl->m_fDepthRangeFar;

	return pcImpl->m_bDepthRange;
}

bool H3DF::ColorInterpolationKit::ShowFaceDisplacement(int & nOutBuckets) const
{
	auto pcImpl = static_cast<ColorInterpolationKitImpl *>(m_pcImpl.get());
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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ColorInterpolationControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}


		void Copy(const ColorInterpolationControlImpl * pcInThat) {
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

	if (false == bInState) {
		strOption = "no ";
	}

	if(true == bColor) {
		strOption += "color interpolation = ";
	}
	else {
		strOption += "color index interpolation = ";
	}

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
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
	bOutState = false;

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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ColorInterpolationControlImpl>();
	auto pcImpl = dynamic_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());

	pcImpl->m_cOverrideKey = cInSegmentKey;
}

H3DF::ColorInterpolationControl::ColorInterpolationControl(ColorInterpolationControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::operator = (ColorInterpolationControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetFaceColor(bool bInState)
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("faces", bInState, true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetEdgeColor(bool bInState)
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("edges", bInState, true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetVertexColor(bool bInState)
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetGeometry("markers", bInState, true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetFaceIndex(bool bInState)
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("faces", bInState, false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetEdgeIndex(bool bInState)
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("edges", bInState, false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::SetVertexIndex(bool bInState)
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->SetGeometry("markers", bInState, false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetFaceColor()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("faces", true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEdgeColor()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("edges", true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetVertexColor()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("markers", true);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetFaceIndex()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("faces", false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEdgeIndex()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("edges", false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetVertexIndex()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	pcImpl->UnsetGeometry("markers", false);

	return *this;
}

ColorInterpolationControl & H3DF::ColorInterpolationControl::UnsetEverything()
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option("color interpolation");
		HC_UnSet_One_Rendering_Option("color index interpolation");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::ColorInterpolationControl::ShowFaceColor(bool & bOutState) const
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("faces", bOutState, true);
}

bool H3DF::ColorInterpolationControl::ShowEdgeColor(bool & bOutState) const
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("edges", bOutState, true);
}

bool H3DF::ColorInterpolationControl::ShowVertexColor(bool & bOutState) const
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("markers", bOutState, true);
}

bool H3DF::ColorInterpolationControl::ShowFaceIndex(bool & bOutState) const
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("faces", bOutState, false);
}

bool H3DF::ColorInterpolationControl::ShowEdgeIndex(bool & bOutState) const
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("edges", bOutState, false);
}

bool H3DF::ColorInterpolationControl::ShowVertexIndex(bool & bOutState) const
{
	auto pcImpl = static_cast<ColorInterpolationControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowGeometry("markers", bOutState, false);
}
