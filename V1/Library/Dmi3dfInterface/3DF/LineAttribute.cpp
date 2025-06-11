#include "StdAfx.h"

#include "LineAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include <hc.h>
#include <HTools.h>

//== LineAttributeKit class ========================================================================

namespace H3DF
{
	class LineAttributeKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<LineAttributeKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}
		void Copy(const LineAttributeKitImpl * pcInThat) {
			m_strPatternName = pcInThat->m_strPatternName;
			m_fWeight = pcInThat->m_fWeight;
		}

		CString m_strPatternName;
		float m_fWeight = -1;
		Line::SizeUnits m_eInUnits = Line::SizeUnits::ScaleFactor;
	};
}

using namespace H3DF;

H3DF::LineAttributeKit::LineAttributeKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<LineAttributeKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::LineAttributeKit::LineAttributeKit(LineAttributeKit const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

LineAttributeKit const & H3DF::LineAttributeKit::operator = (LineAttributeKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::LineAttributeKit::Show(LineAttributeKit & cOutKit) const
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	auto pcOutKitImpl = static_cast<LineAttributeKitImpl *>(cOutKit.GetImpl());

	pcOutKitImpl->Copy(pcImpl);
}

bool H3DF::LineAttributeKit::Empty() const
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	return pcImpl->m_strPatternName.IsEmpty() && pcImpl->m_fWeight < 0;
}

bool H3DF::LineAttributeKit::operator == (LineAttributeKit const & cInThat) const
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<LineAttributeKitImpl *>(cInThat.m_pcImpl.get());

	return pcImpl->m_strPatternName == pcInThatImpl->m_strPatternName && pcImpl->m_fWeight == pcInThatImpl->m_fWeight;
}

bool H3DF::LineAttributeKit::operator != (LineAttributeKit const & cInThat) const
{
	return !(*this == cInThat);
}

LineAttributeKit & H3DF::LineAttributeKit::SetPattern(CString strInPatternName)
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_strPatternName = strInPatternName;
	return *this;
}

LineAttributeKit & H3DF::LineAttributeKit::SetWeight(float fInWeight, Line::SizeUnits eInUnits)
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_fWeight = fInWeight;
	return *this;
}

LineAttributeKit & H3DF::LineAttributeKit::UnsetPattern()
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_strPatternName.Empty();
	return *this;
}

LineAttributeKit & H3DF::LineAttributeKit::UnsetWeight()
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Line::SizeUnits::ScaleFactor;
	return *this;
}

LineAttributeKit & H3DF::LineAttributeKit::UnsetEverything()
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	pcImpl->m_strPatternName.Empty();
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Line::SizeUnits::ScaleFactor;
	return *this;
}

bool H3DF::LineAttributeKit::ShowPattern(CString & strOutPatternName) const
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	strOutPatternName = pcImpl->m_strPatternName;
	return !strOutPatternName.IsEmpty();
}

bool H3DF::LineAttributeKit::ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const
{
	auto pcImpl = static_cast<LineAttributeKitImpl *>(m_pcImpl.get());
	fOutWeight = pcImpl->m_fWeight;
	eOutUnits = pcImpl->m_eInUnits;
	return fOutWeight >= 0;
}

//== LineAttributeControl class ====================================================================

namespace H3DF
{
	class LineAttributeControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<LineAttributeControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const LineAttributeControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

H3DF::LineAttributeControl::LineAttributeControl(SegmentKey & cInSegment)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<LineAttributeControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::LineAttributeControl::LineAttributeControl(LineAttributeControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

LineAttributeControl & H3DF::LineAttributeControl::operator = (LineAttributeControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

LineAttributeControl & H3DF::LineAttributeControl::SetPattern(CStringA strInPatternName)
{
	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Line_Pattern(strInPatternName);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

LineAttributeControl & H3DF::LineAttributeControl::SetWeight(float fInWeight, Line::SizeUnits eInUnits)
{
	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		
		if (Line::SizeUnits::ScaleFactor == eInUnits) {
			HC_Set_Line_Weight(fInWeight);
		}
		else {
			CString strWeight;

			switch (eInUnits)
			{
				case H3DF::Line::SizeUnits::ObjectSpace:
					strWeight.Format(_T("%f oru"), fInWeight);
					break;

				case H3DF::Line::SizeUnits::SubscreenRelative:
					strWeight.Format(_T("%f sru"), fInWeight);
					break;

				case H3DF::Line::SizeUnits::WindowRelative:
					strWeight.Format(_T("%f wru"), fInWeight);
					break;

				case H3DF::Line::SizeUnits::WorldSpace:
					strWeight.Format(_T("%f wsu"), fInWeight);
					break;

				case H3DF::Line::SizeUnits::Points:
					strWeight.Format(_T("%f points"), fInWeight);
					break;

				case H3DF::Line::SizeUnits::Pixels:
					strWeight.Format(_T("%f pixels"), fInWeight);
					break;
			}
			
			HC_Set_Variable_Line_Weight(Utility::ToChar(strWeight));
		}

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

LineAttributeControl & H3DF::LineAttributeControl::UnsetPattern()
{
	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Line_Pattern();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

LineAttributeControl & H3DF::LineAttributeControl::UnsetWeight()
{
	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Line_Weight();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

LineAttributeControl & H3DF::LineAttributeControl::UnsetEverything()
{
	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());
	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Line_Pattern();
		HC_UnSet_Line_Weight();
		HC_UnSet_One_Rendering_Option("geometry options");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::LineAttributeControl::ShowPattern(CStringA & strOutPatternName) const
{
	auto pcImpl = static_cast<LineAttributeControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		char chBuffer[MVO_BUFFER_SIZE] = "\n";
		HC_Show_Line_Pattern(chBuffer);
		strOutPatternName = chBuffer;
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return !strOutPatternName.IsEmpty();
}

bool H3DF::LineAttributeControl::ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const
{
	char chBuffer[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Variable_Line_Weight(chBuffer);

	if (0 < strlen(chBuffer)) {
		float fValue = 0;
		char chUnits[32] = "\n";
		sscanf(chBuffer, "%f %s", &fValue, chUnits);
		
		fOutWeight = fValue;

		if (0 == strcmp(chUnits, "oru")) {
			eOutUnits = Line::SizeUnits::ObjectSpace;
		}
		else if (0 == strcmp(chUnits, "sru")) {
			eOutUnits = Line::SizeUnits::SubscreenRelative;
		}
		else if (0 == strcmp(chUnits, "wru")) {
			eOutUnits = Line::SizeUnits::WindowRelative;
		}
		else if (0 == strcmp(chUnits, "wsu")) {
			eOutUnits = Line::SizeUnits::WorldSpace;
		}
		else if (0 == strcmp(chUnits, "points")) {
			eOutUnits = Line::SizeUnits::Points;
		}
		else if (0 == strcmp(chUnits, "pixels")) {
			eOutUnits = Line::SizeUnits::Pixels;
		}
		else {
			assert(false);
			return false;
		}
		return true;
	}
	else {
		HC_Show_Line_Weight(&fOutWeight);
		eOutUnits = Line::SizeUnits::ScaleFactor;
	}

	return true;
}