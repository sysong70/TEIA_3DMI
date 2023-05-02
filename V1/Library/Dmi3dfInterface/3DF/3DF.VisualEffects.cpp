#include "StdAfx.h"

#include "3DF.VisualEffects.h"

#include "3DF.Segment.h"
#include "./Private/3DF.SegmentPrivate.h"

#include <HUtility.h>
#include <HTools.h>

USING_3DF_NAMESPACE

class VisualEffectsControlPrivate : public PrivateImpl
{
public:
	VisualEffectsControlPrivate() { m_eType = TDF::Type::VisualEffectsControl; }

	void Copy(VisualEffectsControlPrivate * pcInThat) {
		m_cParentSegmentKey = pcInThat->m_cParentSegmentKey;
	}

	// Parent Segment Key
	SegmentKey m_cParentSegmentKey;
};

VisualEffectsControl::VisualEffectsControl(SegmentKey & cInSegmentKey)
{
	VisualEffectsControlPrivate * pcImpl = new VisualEffectsControlPrivate();
	pcImpl->m_cParentSegmentKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

VisualEffectsControl::VisualEffectsControl(VisualEffectsControl const & cInThat)
{
	m_pcImpl = new VisualEffectsControlPrivate();
	Set(cInThat);
}

void VisualEffectsControl::Set(VisualEffectsControl const & cInThat)
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;
	VisualEffectsControlPrivate * pcInThatImpl = (VisualEffectsControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

VisualEffectsControl & VisualEffectsControl::operator = (VisualEffectsControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

// Manipulates the state of anti - aliasing(text and screen).
VisualEffectsControl & VisualEffectsControl::SetAntiAliasing(bool bInState)
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		CString strOption;

		if (true == bInState) {
			strOption = L"anti-alias = (screen = on)";
		}
		else {
			strOption = L"anti-alias=off";
		}

		HC_Set_Rendering_Options(H_ASCII_TEXT(strOption));

	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}