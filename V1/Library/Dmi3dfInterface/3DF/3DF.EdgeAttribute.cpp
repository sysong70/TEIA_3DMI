#include "StdAfx.h"

#include "3DF.EdgeAttribute.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"

#include <hc.h>
#include <HTools.h>

USING_3DF_NAMESPACE

//== EdgeAttributeKit class ========================================================================

class EdgeAttributeKitPrivate : public PrivateImpl
{
public:
	EdgeAttributeKitPrivate() { m_eType = H3DF::Type::EdgeAttributeKit; }

	void Copy(EdgeAttributeKitPrivate * pcInThat) {
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
	m_pcImpl = new EdgeAttributeKitPrivate();
}

EdgeAttributeKit::EdgeAttributeKit(EdgeAttributeKit const & cInThat)
{
	m_pcImpl = new EdgeAttributeKitPrivate();
	Set(cInThat);
}

void EdgeAttributeKit::Set(EdgeAttributeKit const & cInThat)
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	EdgeAttributeKitPrivate * pcInThatImpl = (EdgeAttributeKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

EdgeAttributeKit const & EdgeAttributeKit::operator = (EdgeAttributeKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

void EdgeAttributeKit::Show(EdgeAttributeKit & cOutKit) const
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	EdgeAttributeKitPrivate * pcOutKitImpl = (EdgeAttributeKitPrivate *)cOutKit.m_pcImpl;
	pcOutKitImpl->Copy(pcImpl);
}

bool EdgeAttributeKit::Empty() const
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	return pcImpl->m_strPatternName.IsEmpty() && pcImpl->m_fWeight < 0 && pcImpl->m_fHardAngle < 0;
}

bool EdgeAttributeKit::operator == (EdgeAttributeKit const & cInThat) const
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	EdgeAttributeKitPrivate * pcInThatImpl = (EdgeAttributeKitPrivate *)cInThat.m_pcImpl;
	return pcImpl->m_strPatternName == pcInThatImpl->m_strPatternName && pcImpl->m_fWeight == pcInThatImpl->m_fWeight && pcImpl->m_fHardAngle == pcInThatImpl->m_fHardAngle;
}

bool EdgeAttributeKit::operator != (EdgeAttributeKit const & cInThat) const
{
	return !(*this == cInThat);
}

EdgeAttributeKit & EdgeAttributeKit::SetPattern(CString strInPatternName)
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_strPatternName = strInPatternName;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::SetWeight(float fInWeight, Edge::SizeUnits eInUnits)
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_fWeight = fInWeight;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::SetHardAngle(float fInAngle)
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_fHardAngle = fInAngle;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetPattern()
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_strPatternName.Empty();
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetWeight()
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Edge::SizeUnits::ScaleFactor;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetHardAngle()
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_fHardAngle = -1;
	return *this;
}

EdgeAttributeKit & EdgeAttributeKit::UnsetEverything()
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	pcImpl->m_strPatternName.Empty();
	pcImpl->m_fWeight = -1;
	pcImpl->m_eInUnits = Edge::SizeUnits::ScaleFactor;
	pcImpl->m_fHardAngle = -1;
	return *this;
}

bool EdgeAttributeKit::ShowPattern(CString & strOutPatternName) const
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	strOutPatternName = pcImpl->m_strPatternName;
	return !strOutPatternName.IsEmpty();
}

bool EdgeAttributeKit::ShowWeight(float & fOutWeight, Edge::SizeUnits & eOutUnits) const
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	fOutWeight = pcImpl->m_fWeight;
	eOutUnits = pcImpl->m_eInUnits;
	return fOutWeight >= 0;
}

bool EdgeAttributeKit::ShowHardAngle(float & fOutAngle) const
{
	EdgeAttributeKitPrivate * pcImpl = (EdgeAttributeKitPrivate *)m_pcImpl;
	fOutAngle = pcImpl->m_fHardAngle;
	return fOutAngle >= 0;
}

//== EdgeAttributeControl class ====================================================================

class EdgeAttributeControlPrivate : public PrivateImpl
{
public:
	EdgeAttributeControlPrivate() { m_eType = H3DF::Type::EdgeAttributeControl; }

	void Copy(EdgeAttributeControlPrivate * pcInThat) {
		m_cParentSegmentKey = pcInThat->m_cParentSegmentKey;
	}

	// Parent Segment Key
	SegmentKey m_cParentSegmentKey;
};

EdgeAttributeControl::EdgeAttributeControl(SegmentKey & cInSegmentKey)
{
	EdgeAttributeControlPrivate * pcImpl = new EdgeAttributeControlPrivate();
	pcImpl->m_cParentSegmentKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

EdgeAttributeControl::EdgeAttributeControl(EdgeAttributeControl const & cInThat)
{
	m_pcImpl = new EdgeAttributeControlPrivate();
	Set(cInThat);
}

void EdgeAttributeControl::Set(EdgeAttributeControl const & cInThat)
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;
	EdgeAttributeControlPrivate * pcInThatImpl = (EdgeAttributeControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

EdgeAttributeControl & EdgeAttributeControl::operator = (EdgeAttributeControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::SetPattern(CString strInPatternName)
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_Set_Edge_Pattern(H_ASCII_TEXT(strInPatternName));
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::SetWeight(float fInWeight, Edge::SizeUnits eInUnits)
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		
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
			
			HC_Set_Variable_Edge_Weight(H_ASCII_TEXT(strWeight));
		}

	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::SetHardAngle(float fInAngle)
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		CString strOption;
		strOption.Format(L"geometry options = (hard edge angle = %f, no invert polycylinders, no fea nodes)", fInAngle);
		HC_Set_Rendering_Options(H_ASCII_TEXT(strOption));
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::UnsetPattern()
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_Edge_Pattern();
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::UnsetWeight()
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_Edge_Weight();
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::UnsetHardAngle()
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_One_Rendering_Option("geometry options");
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

EdgeAttributeControl & EdgeAttributeControl::UnsetEverything()
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;
	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_Edge_Pattern();
		HC_UnSet_Edge_Weight();
		HC_UnSet_One_Rendering_Option("geometry options");
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);
	return *this;
}

bool EdgeAttributeControl::ShowPattern(CString & strOutPatternName) const
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		char chBuffer[MVO_BUFFER_SIZE] = "\n";
		HC_Show_Edge_Pattern(chBuffer);
		strOutPatternName = chBuffer;
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return !strOutPatternName.IsEmpty();
}

bool EdgeAttributeControl::ShowWeight(float & fOutWeight, Edge::SizeUnits & eOutUnits) const
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

bool EdgeAttributeControl::ShowHardAngle(float & fOutAngle) const
{
	EdgeAttributeControlPrivate * pcImpl = (EdgeAttributeControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		char chBuffer[MVO_BUFFER_SIZE] = "\n";
		HC_Show_One_Rendering_Option("geometry options", chBuffer);
		if (0 < strlen(chBuffer)) {
			sscanf(chBuffer, "hard edge angle = %f", &fOutAngle);
			return true;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return false;
}

