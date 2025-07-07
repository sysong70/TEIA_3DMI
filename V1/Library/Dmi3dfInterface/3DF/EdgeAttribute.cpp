#include "StdAfx.h"

#include "EdgeAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include <hc.h>
#include <HTools.h>

USING_3DF_NAMESPACE

//== EdgeAttributeKit class ========================================================================

class EdgeAttributeKitImpl : public Impl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<EdgeAttributeKitImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const EdgeAttributeKitImpl * pcInThat) {
		m_strPatternName = pcInThat->m_strPatternName;
		m_fWeight = pcInThat->m_fWeight;
	}

	CString m_strPatternName;
	float m_fWeight = -1;
	Edge::SizeUnits m_eInUnits = Edge::SizeUnits::ScaleFactor;
	float m_fHardAngle = -1;
};

EdgeAttributeKit::EdgeAttributeKit()
{
	m_pcImpl = std::make_unique<EdgeAttributeKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

EdgeAttributeKit::EdgeAttributeKit(EdgeAttributeKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

EdgeAttributeKit const & EdgeAttributeKit::operator = (EdgeAttributeKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void EdgeAttributeKit::Show(EdgeAttributeKit & cOutKit) const
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	auto pcOutKitImpl = static_cast<EdgeAttributeKitImpl *>(cOutKit.m_pcImpl.get());
	if (nullptr == pcImpl || nullptr == pcOutKitImpl) {
		DEBUG_STOP;
		return;
	}

	pcOutKitImpl->Copy(pcImpl);
}

bool EdgeAttributeKit::Empty() const
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	return pcImpl->m_strPatternName.IsEmpty() && pcImpl->m_fWeight < 0 && pcImpl->m_fHardAngle < 0;
}

bool EdgeAttributeKit::operator == (EdgeAttributeKit const & cInThat) const
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<EdgeAttributeKitImpl *>(cInThat.m_pcImpl.get());
	return pcImpl->m_strPatternName == pcInThatImpl->m_strPatternName && pcImpl->m_fWeight == pcInThatImpl->m_fWeight && pcImpl->m_fHardAngle == pcInThatImpl->m_fHardAngle;
}

bool EdgeAttributeKit::operator != (EdgeAttributeKit const & cInThat) const
{
	return !(*this == cInThat);
}

EdgeAttributeKit & EdgeAttributeKit::SetPattern(CString strInPatternName)
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_strPatternName = strInPatternName;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::SetWeight(float fInWeight, Edge::SizeUnits eInUnits)
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_fWeight = fInWeight;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::SetHardAngle(float fInAngle)
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_fHardAngle = fInAngle;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetPattern()
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_strPatternName.Empty();
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetWeight()
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Edge::SizeUnits::ScaleFactor;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetHardAngle()
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_fHardAngle = -1;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetEverything()
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_strPatternName.Empty();
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Edge::SizeUnits::ScaleFactor;
	pcImpl->m_fHardAngle = -1;
	return *this;
}

bool EdgeAttributeKit::ShowPattern(CString & strOutPatternName) const
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	strOutPatternName = pcImpl->m_strPatternName;
	return !strOutPatternName.IsEmpty();
}

bool EdgeAttributeKit::ShowWeight(float & fOutWeight, Edge::SizeUnits & eOutUnits) const
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	fOutWeight = pcImpl->m_fWeight;
	eOutUnits = pcImpl->m_eInUnits;
	return fOutWeight >= 0;
}

bool EdgeAttributeKit::ShowHardAngle(float & fOutAngle) const
{
	auto pcImpl = static_cast<EdgeAttributeKitImpl *>(m_pcImpl.get());
	fOutAngle = pcImpl->m_fHardAngle;
	return fOutAngle >= 0;
}

//== EdgeAttributeControl class ====================================================================

class EdgeAttributeControlImpl : public ControlImpl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<EdgeAttributeControlImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const EdgeAttributeControlImpl * pcInThat) {
		ControlImpl::Copy(pcInThat);
	}
};

H3DF::EdgeAttributeControl::EdgeAttributeControl(SegmentKey & cInSegment)
{
	INIT_IMPL(EdgeAttributeControl);

	IMPL(EdgeAttributeControl);

	impl->m_cOverrideKey = cInSegment;

	H3DF::SegmentKeyImpl::LocalOpen(impl->m_cOverrideKey);
}

H3DF::EdgeAttributeControl::EdgeAttributeControl(EdgeAttributeControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);

	IMPL(EdgeAttributeControl);

	H3DF::SegmentKeyImpl::LocalOpen(impl->m_cOverrideKey);
}

H3DF::EdgeAttributeControl::~EdgeAttributeControl()
{
	IMPL(EdgeAttributeControl);

	H3DF::SegmentKeyImpl::LocalClose(impl->m_cOverrideKey);
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::operator = (EdgeAttributeControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::SetPattern(CString strInPatternName)
{
	auto pcImpl = static_cast<EdgeAttributeControlImpl *>(m_pcImpl.get());

	HC_Set_Edge_Pattern(Utility::ToChar(strInPatternName));

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::SetWeight(float fInWeight, Edge::SizeUnits eInUnits)
{
	auto pcImpl = static_cast<EdgeAttributeControlImpl *>(m_pcImpl.get());

	if (Edge::SizeUnits::ScaleFactor == eInUnits) {
		HC_Set_Edge_Weight(fInWeight);
	}
	else {
		CString strWeight;

		switch (eInUnits)
		{
		case H3DF::Edge::SizeUnits::ObjectSpace:
			strWeight.Format(_T("%f oru"), fInWeight);
			break;

		case H3DF::Edge::SizeUnits::SubscreenRelative:
			strWeight.Format(_T("%f sru"), fInWeight);
			break;

		case H3DF::Edge::SizeUnits::WindowRelative:
			strWeight.Format(_T("%f wru"), fInWeight);
			break;

		case H3DF::Edge::SizeUnits::WorldSpace:
			strWeight.Format(_T("%f wsu"), fInWeight);
			break;

		case H3DF::Edge::SizeUnits::Points:
			strWeight.Format(_T("%f points"), fInWeight);
			break;

		case H3DF::Edge::SizeUnits::Pixels:
			strWeight.Format(_T("%f Pixels"), fInWeight);
			break;
		}

		HC_Set_Variable_Edge_Weight(Utility::ToChar(strWeight));
	}

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::SetHardAngle(float fInAngle)
{
	CString strOption;
	strOption.Format(L"geometry options = (hard edge angle = %f, no invert polycylinders, no fea nodes)", fInAngle);
	HC_Set_Rendering_Options(Utility::ToChar(strOption));

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::UnsetPattern()
{
	HC_UnSet_Edge_Pattern();

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::UnsetWeight()
{
	HC_UnSet_Edge_Weight();

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::UnsetHardAngle()
{
	HC_UnSet_One_Rendering_Option("geometry options");

	return *this;
}

EdgeAttributeControl & H3DF::EdgeAttributeControl::UnsetEverything()
{
	HC_UnSet_Edge_Pattern();
	HC_UnSet_Edge_Weight();
	HC_UnSet_One_Rendering_Option("geometry options");

	return *this;
}

bool H3DF::EdgeAttributeControl::ShowPattern(CString & strOutPatternName) const
{
	char chBuffer[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Edge_Pattern(chBuffer);
	strOutPatternName = chBuffer;

	return !strOutPatternName.IsEmpty();
}

bool H3DF::EdgeAttributeControl::ShowWeight(float & fOutWeight, Edge::SizeUnits & eOutUnits) const
{
	char chBuffer[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Variable_Line_Weight(chBuffer);
	if (0 < strlen(chBuffer)) {
		float fValue = 0;
		char chUnits[32] = "\n";
		sscanf(chBuffer, "%f %s", &fValue, chUnits);
		
		fOutWeight = fValue;

		if (0 == strcmp(chUnits, "oru")) {
			eOutUnits = Edge::SizeUnits::ObjectSpace;
		}
		else if (0 == strcmp(chUnits, "sru")) {
			eOutUnits = Edge::SizeUnits::SubscreenRelative;
		}
		else if (0 == strcmp(chUnits, "wru")) {
			eOutUnits = Edge::SizeUnits::WindowRelative;
		}
		else if (0 == strcmp(chUnits, "wsu")) {
			eOutUnits = Edge::SizeUnits::WorldSpace;
		}
		else if (0 == strcmp(chUnits, "points")) {
			eOutUnits = Edge::SizeUnits::Points;
		}
		else if (0 == strcmp(chUnits, "pixels")) {
			eOutUnits = Edge::SizeUnits::Pixels;
		}
		else {
			assert(false);
			return false;
		}
		return true;
	}
	else {
		HC_Show_Line_Weight(&fOutWeight);
		eOutUnits = Edge::SizeUnits::ScaleFactor;
	}

	return true;
}

bool H3DF::EdgeAttributeControl::ShowHardAngle(float & fOutAngle) const
{
	auto pcImpl = static_cast<EdgeAttributeControlImpl *>(m_pcImpl.get());

	char chBuffer[MVO_BUFFER_SIZE] = "\n";
	HC_Show_One_Rendering_Option("geometry options", chBuffer);
	if (0 < strlen(chBuffer)) {
		sscanf(chBuffer, "hard edge angle = %f", &fOutAngle);
		return true;
	}

	return false;
}

