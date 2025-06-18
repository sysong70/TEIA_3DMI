#include "StdAfx.h"

#include "TextAttribute.h"
#include "Impl/TextAttribute.Impl.h"

#include <HTools.h>

using namespace H3DF;

H3DF::TextAttributeControl::TextAttributeControl(SegmentKey & cInSegment)
{
	m_pcImpl = std::make_unique<TextAttributeControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<TextAttributeControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	
	pcImpl->m_cOverrideKey = cInSegment;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
}

H3DF::TextAttributeControl::TextAttributeControl(TextAttributeControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::TextAttributeControl::TextAttributeControl(TextAttributeControl && cInThat) noexcept : Control(std::move(cInThat))
{

}

TextAttributeControl & H3DF::TextAttributeControl::operator = (TextAttributeControl && cInThat) noexcept
{
	this->Control::operator = (std::move(cInThat));
	return *this;
}

H3DF::TextAttributeControl::~TextAttributeControl()
{
	auto pcImpl = static_cast<TextAttributeControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

TextAttributeControl & H3DF::TextAttributeControl::operator = (TextAttributeControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

TextAttributeControl & H3DF::TextAttributeControl::SetBackground(bool bInState, CStringA strInName)
{
	auto pcImpl = static_cast<TextAttributeControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	CStringA strName = pcImpl->m_cOverrideKey.Name();

	CStringA strList;
	strList.Format("background=(%s,shape=%s)", (true == bInState) ? "on" : "off", strInName);
		
	HC_Set_Text_Font(strList);

	CStringA strList2;
	HC_Show_Text_Font(strList2.GetBuffer());

	return *this;
}

TextAttributeControl & H3DF::TextAttributeControl::SetBackground(CStringA strInName)
{
	return SetBackground(true, strInName);
}

TextAttributeControl & H3DF::TextAttributeControl::SetBackground(bool bInState)
{
	CStringA strList;
	strList.Format("background=%s", (true == bInState) ? "on" : "off");

	HC_Set_Text_Font(strList);

	return *this;
}

