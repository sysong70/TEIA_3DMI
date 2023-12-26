#include "StdAfx.h"

#include "LineAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include <hc.h>
#include <HTools.h>

//== LineAttributeKit class ========================================================================

OPEN_3DF_NAMESPACE

class LineAttributeKitImpl : public Impl
{
public:
	LineAttributeKitImpl() { m_eType = H3DF::Type::LineAttributeKit; }

	void Copy(LineAttributeKitImpl * pcInThat) {
		m_strPatternName = pcInThat->m_strPatternName;
		m_fWeight = pcInThat->m_fWeight;
	}

	CString m_strPatternName;
	float m_fWeight = -1;
	Line::SizeUnits m_eInUnits = Line::SizeUnits::ScaleFactor;
};

CLOSE_3DF_NAMESPACE

using namespace H3DF;

LineAttributeKit::LineAttributeKit()
{
	m_pcImpl = new LineAttributeKitImpl();
}

LineAttributeKit::LineAttributeKit(LineAttributeKit const & cInThat)
{
	m_pcImpl = new LineAttributeKitImpl();
	Set(cInThat);
}

void LineAttributeKit::Set(LineAttributeKit const & cInThat)
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	LineAttributeKitImpl * pcInThatImpl = (LineAttributeKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

LineAttributeKit const & LineAttributeKit::operator = (LineAttributeKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

void LineAttributeKit::Show(LineAttributeKit & cOutKit) const
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	LineAttributeKitImpl * pcOutKitImpl = (LineAttributeKitImpl *)cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

bool LineAttributeKit::Empty() const
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	return pcImpl->m_strPatternName.IsEmpty() && pcImpl->m_fWeight < 0;
}

bool LineAttributeKit::operator == (LineAttributeKit const & cInThat) const
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	LineAttributeKitImpl * pcInThatImpl = (LineAttributeKitImpl *)cInThat.m_pcImpl;
	return pcImpl->m_strPatternName == pcInThatImpl->m_strPatternName && pcImpl->m_fWeight == pcInThatImpl->m_fWeight;
}

bool LineAttributeKit::operator != (LineAttributeKit const & cInThat) const
{
	return !(*this == cInThat);
}

LineAttributeKit & LineAttributeKit::SetPattern(CString strInPatternName)
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	pcImpl->m_strPatternName = strInPatternName;
	return *this;
}

LineAttributeKit & LineAttributeKit::SetWeight(float fInWeight, Line::SizeUnits eInUnits)
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	pcImpl->m_fWeight = fInWeight;
	return *this;
}

LineAttributeKit & LineAttributeKit::UnsetPattern()
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	pcImpl->m_strPatternName.Empty();
	return *this;
}

LineAttributeKit & LineAttributeKit::UnsetWeight()
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Line::SizeUnits::ScaleFactor;
	return *this;
}

LineAttributeKit & LineAttributeKit::UnsetEverything()
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	pcImpl->m_strPatternName.Empty();
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Line::SizeUnits::ScaleFactor;
	return *this;
}

bool LineAttributeKit::ShowPattern(CString & strOutPatternName) const
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	strOutPatternName = pcImpl->m_strPatternName;
	return !strOutPatternName.IsEmpty();
}

bool LineAttributeKit::ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const
{
	LineAttributeKitImpl * pcImpl = (LineAttributeKitImpl *)m_pcImpl;
	fOutWeight = pcImpl->m_fWeight;
	eOutUnits = pcImpl->m_eInUnits;
	return fOutWeight >= 0;
}

//== LineAttributeControl class ====================================================================

class LineAttributeControlImpl : public ControlImpl
{
public:
	LineAttributeControlImpl() { m_eType = H3DF::Type::LineAttributeControl; }

	void Copy(LineAttributeControlImpl * pcInThat) {
		m_cParentSegmentKey = pcInThat->m_cParentSegmentKey;
	}

	// Parent Segment Key
	SegmentKey m_cParentSegmentKey;
};

LineAttributeControl::LineAttributeControl(SegmentKey & cInSegmentKey)
{
	LineAttributeControlImpl * pcImpl = new LineAttributeControlImpl();
	pcImpl->m_cParentSegmentKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

LineAttributeControl::LineAttributeControl(LineAttributeControl const & cInThat)
{
	m_pcImpl = new LineAttributeControlImpl();
	Set(cInThat);
}

void LineAttributeControl::Set(LineAttributeControl const & cInThat)
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;
	LineAttributeControlImpl * pcInThatImpl = (LineAttributeControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

LineAttributeControl & LineAttributeControl::operator = (LineAttributeControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

LineAttributeControl & LineAttributeControl::SetPattern(CString strInPatternName)
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_Set_Line_Pattern(Utility::ToChar(strInPatternName));
	} SegmentKeyImpl::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

LineAttributeControl & LineAttributeControl::SetWeight(float fInWeight, Line::SizeUnits eInUnits)
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cParentSegmentKey); {
		
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

	} SegmentKeyImpl::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

LineAttributeControl & LineAttributeControl::UnsetPattern()
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_Line_Pattern();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

LineAttributeControl & LineAttributeControl::UnsetWeight()
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_Line_Weight();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

LineAttributeControl & LineAttributeControl::UnsetEverything()
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;
	SegmentKeyImpl::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_Line_Pattern();
		HC_UnSet_Line_Weight();
		HC_UnSet_One_Rendering_Option("geometry options");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cParentSegmentKey);
	return *this;
}

bool LineAttributeControl::ShowPattern(CString & strOutPatternName) const
{
	LineAttributeControlImpl * pcImpl = (LineAttributeControlImpl *)m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cParentSegmentKey); {
		char chBuffer[MVO_BUFFER_SIZE] = "\n";
		HC_Show_Line_Pattern(chBuffer);
		strOutPatternName = chBuffer;
	} SegmentKeyImpl::LocalClose(pcImpl->m_cParentSegmentKey);

	return !strOutPatternName.IsEmpty();
}

bool LineAttributeControl::ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const
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