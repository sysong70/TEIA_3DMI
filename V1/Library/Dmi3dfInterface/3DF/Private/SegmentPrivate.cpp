#include "StdAfx.h"

#include <HTools.h>

#include "SegmentPrivate.h"
#include "../Segment.h"

using namespace H3DF;

void H3DF::SegmentKeyPrivate::Copy(SegmentKeyPrivate * pcInThat)
{
	KeyPrivate::Copy(pcInThat);

	m_bOpen = pcInThat->m_bOpen;
	m_bForcedOpen = pcInThat->m_bForcedOpen;
	m_pcBaseView = pcInThat->m_pcBaseView;
}

//== Segment 관련 함수 ===============================================================================

void H3DF::SegmentKeyPrivate::SetColor(CString strInGeometryName, RGBAColor cInColor)
{
	CString strColorText;
	if (1.0f == cInColor.alpha) {
		strColorText.Format(L"%s = (diffuse = (r=%f g=%f b=%f))", strInGeometryName, cInColor.red, cInColor.green, cInColor.blue);
	}
	else {
		float fTransparency = 1.0f - cInColor.alpha;
		strColorText.Format(L"%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strInGeometryName, cInColor.red, cInColor.green, cInColor.blue, fTransparency, fTransparency, fTransparency);
	}

	HC_Set_Color(Utility::ToChar(strColorText));
}

BaseView * H3DF::SegmentKeyPrivate::GetBaseView() const
{
	return m_pcBaseView;
}

void H3DF::SegmentKeyPrivate::SetBaseView(BaseView * pcInBaseView)
{
	m_pcBaseView = pcInBaseView;
}