#include "StdAfx.h"

#include "VisualEffects.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"

#include <HUtility.h>
#include <HTools.h>

namespace H3DF {
	class VisualEffectsControlPrivate : public PrivateImpl
	{
	public:
		VisualEffectsControlPrivate() { m_eType = H3DF::Type::VisualEffectsControl; }

		void Copy(VisualEffectsControlPrivate * pcInThat) {
			m_cParentSegmentKey = pcInThat->m_cParentSegmentKey;
		}

		// Parent Segment Key
		SegmentKey m_cParentSegmentKey;
	};
}

USING_3DF_NAMESPACE

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
			strOption = L"anti-alias = (screen = off)";
		}

		HC_Set_Rendering_Options(Utility::ToChar(strOption));

	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

VisualEffectsControl & VisualEffectsControl::SetTextAntiAliasing(bool bInState)
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		CString strOption;

		if (true == bInState) {
			strOption = L"anti-alias = (text = on)";
		}
		else {
			strOption = L"anti-alias = (text = off)";
		}

		HC_Set_Rendering_Options(Utility::ToChar(strOption));

	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

VisualEffectsControl & VisualEffectsControl::SetLineAntiAliasing(bool bInState)
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		CString strOption;

		if (true == bInState) {
			strOption = L"anti-alias = (lines = on)";
		}
		else {
			strOption = L"anti-alias = (lines = off)";
		}

		HC_Set_Rendering_Options(Utility::ToChar(strOption));

	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

VisualEffectsControl & VisualEffectsControl::UnsetAntiAliasing()
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		HC_UnSet_One_Rendering_Option("anti-alias");
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return *this;
}

bool VisualEffectsControl::ShowAntiAliasing(bool & bOutState) const
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	bool bResult = false;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("anti-alias", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		else if (NULL != strstr(chValue, "no screen")) {
			bOutState = false;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "screen")) {
			bOutState = true;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
		}

	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return bResult;
}

bool VisualEffectsControl::ShowTextAntiAliasing(bool & bOutState) const
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	bool bResult = false;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("anti-alias = text", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		// chValue값에 no text가 포함되어 있으면 false
		else if (NULL != strstr(chValue, "no text")) {
			bOutState = false;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "text")) {
			bOutState = true;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "screen")) {
			bOutState = true;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return bResult;
}

bool VisualEffectsControl::ShowLineAntiAliasing(bool & bOutState) const
{
	VisualEffectsControlPrivate * pcImpl = (VisualEffectsControlPrivate *)m_pcImpl;

	bool bResult = false;

	SegmentKeyPrivate::LocalOpen(pcImpl->m_cParentSegmentKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("anti-alias", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		// chValue값에 no line이 포함되어 있으면 false
		else if (NULL != strstr(chValue, "no lines")) {
			bOutState = false;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "lines")) {
			bOutState = true;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "screen")) {
			bOutState = true;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
		}
	} SegmentKeyPrivate::LocalClose(pcImpl->m_cParentSegmentKey);

	return bResult;
}