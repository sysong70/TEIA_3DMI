#include "StdAfx.h"

#include "Style.h"
#include "Segment.h"
#include "Impl/SegmentImpl.h"
#include "Portfolio.h"
#include "Condition.h"

#include "Impl/ControlImpl.h"
#include "Impl/DefinitionImpl.h"

#include <hc.h>
#include <Htools.h>

using namespace H3DF;

//== NamedStyleDefinition Function =================================================================

H3DF::NamedStyleDefinition::NamedStyleDefinition()
{
}

H3DF::NamedStyleDefinition::NamedStyleDefinition(HC_KEY nInKey) : Definition(nInKey)
{
}

H3DF::NamedStyleDefinition::NamedStyleDefinition(NamedStyleDefinition const & cInThat)
{
	Set(cInThat);
}

void H3DF::NamedStyleDefinition::Set(NamedStyleDefinition const & cInThat)
{
	auto * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	auto * pcInThatImpl = dynamic_cast<DefinitionImpl *>(cInThat.m_pcImpl);
	DEBUG_VALID(pcInThatImpl);

	pcImpl->SetKeyValue(pcInThatImpl->KeyValue());
}

NamedStyleDefinition & H3DF::NamedStyleDefinition::operator = (NamedStyleDefinition const & cInThat)
{
	auto * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	auto * pcInThatImpl = dynamic_cast<DefinitionImpl *>(cInThat.m_pcImpl);
	DEBUG_VALID(pcInThatImpl);

	pcImpl->SetKeyValue(pcInThatImpl->KeyValue());

	return *this;
}

SegmentKey H3DF::NamedStyleDefinition::GetSource() const
{
	auto * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->KeyValue();

	SegmentKey cSegment(pcImpl->KeyValue());
	return cSegment;
}

PortfolioKey H3DF::NamedStyleDefinition::Owner() const
{
	auto * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKey cSegment(pcImpl->KeyValue());

	SegmentKey cOwner = cSegment.Owner();
	HC_KEY nOwner = cOwner.KeyValue();

	PortfolioKey cPortfolioKey(cSegment.Owner());
	return cPortfolioKey;
}

//== StyleKey Function =============================================================================

H3DF::StyleKey::StyleKey() : Key()
{
}

H3DF::StyleKey::StyleKey(HC_KEY nInKey) : Key(nInKey)
{
}

H3DF::StyleKey::StyleKey(StyleKey const & cInThat) : Key(cInThat)
{
}

void H3DF::StyleKey::Set(StyleKey const & cInThat)
{
	Key::Set(cInThat);
}

StyleKey & H3DF::StyleKey::operator = (StyleKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== StyleControl Function =========================================================================

class StyleControlImpl : public ControlImpl
{
public:
	StyleControlImpl() { m_eType = H3DF::Type::StyleControl; }

	void Copy(StyleControlImpl * pcInThat) {
		ControlImpl::Copy(pcInThat);
	}
};

H3DF::StyleControl::StyleControl(SegmentKey & cInSegment) 
{
	StyleControlImpl * pcImpl = new StyleControlImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
	m_pcImpl = pcImpl;
}

H3DF::StyleControl::StyleControl(StyleControl const & cInThat)
{
	m_pcImpl = new StyleControlImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}

void H3DF::StyleControl::Set(StyleControl const & cInThat)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *)m_pcImpl;
	StyleControlImpl * pcInThatImpl = (StyleControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

StyleControl & H3DF::StyleControl::operator = (StyleControl const & cInThat)
{
	Set(cInThat);
	return *this;
}


StyleKey H3DF::StyleControl::PushNamed(CStringA strInStyleName)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Style_Segment(strInStyleName);
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushNamed(CStringA strInStyleName, ConditionalExpression const & cInConditional)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	CStringA strCondition;
	if (false == cInConditional.ShowCondition(strCondition)) {
		DEBUG_STOP;
	}

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Conditional_Style(strInStyleName, strCondition);
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushSegment(SegmentKey const & cInStyleSource)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *)m_pcImpl;
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushSegment(SegmentKey const & cInStyleSource, ConditionalExpression const & cInConditional)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *)m_pcImpl;
	if (nullptr == pcImpl) { assert(false); }

	CStringA strCondition;
	if (false == cInConditional.ShowCondition(strCondition)) {
		DEBUG_STOP;
	}

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Conditional_Style_By_Key(cInStyleSource.KeyValue(), strCondition);
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

bool H3DF::StyleControl::Show(StyleKeyArray & acOutStyles) const
{
	StyleControlImpl * pcImpl = dynamic_cast<StyleControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	CStringA strStyle;
	int nStyleCount = 0;
	char chType[MVO_BUFFER_SIZE];
	HC_KEY nSegmentKey;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Begin_Contents_Search(".", "styles"); {
			HC_Show_Contents_Count(&nStyleCount);

			for (int nIndex = 0; nIndex < nStyleCount; nIndex++) {
				HC_Find_Contents(chType, &nSegmentKey);

				StyleKey cStyle(nSegmentKey);
				acOutStyles.push_back(cStyle);
			}

		} HC_End_Contents_Search();
	}
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return !acOutStyles.empty();
}

bool H3DF::StyleControl::Show(StyleTypeArray & cOutTypes, SegmentKeyArray & cOutSegmentSources, AStringArray & astrOutStyleNames, ConditionalExpressionArray & acOutConditions) const
{
	StyleControlImpl * pcImpl = dynamic_cast<StyleControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	CStringA strStyle;
	int nStyleCount = 0;
	char chType[MVO_BUFFER_SIZE];
	HC_KEY nKey;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Begin_Contents_Search(".", "styles"); {
			HC_Show_Contents_Count(&nStyleCount);

			for (int nIndex = 0; nIndex < nStyleCount; nIndex++) {
				HC_Find_Contents(chType, &nKey);

				Style::Type eType = Style::Type::Segment;
				cOutTypes.push_back(eType);

				SegmentKey cSegment(nKey);
				cOutSegmentSources.push_back(cSegment);

				CStringA strStyleName;
				HC_Show_Style_Segment(nKey, strStyleName.GetBuffer(MVO_BUFFER_SIZE));
				astrOutStyleNames.push_back(strStyleName);

				ConditionalExpression cCondition;
				acOutConditions.push_back(cCondition);
			}

		} HC_End_Contents_Search();
	}
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return !cOutTypes.empty();
}

bool H3DF::StyleControl::ShowAllSegment(StyleKeyArray & acOutStyles) const
{
	DEBUG_STOP;
	return false;
}