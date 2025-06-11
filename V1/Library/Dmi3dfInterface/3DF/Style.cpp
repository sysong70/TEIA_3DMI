#include "StdAfx.h"

#include "Style.h"
#include "Segment.h"
#include "Impl/SegmentImpl.h"
#include "Portfolio.h"
#include "Condition.h"

#include "Impl/ControlImpl.h"
#include "Impl/DefinitionImpl.h"

#include "3DF.Utility.h"

#include <hc.h>
#include <Htools.h>

using namespace H3DF;

//== NamedStyleDefinition Function =================================================================

H3DF::NamedStyleDefinition::NamedStyleDefinition()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<DefinitionImpl>();
}

H3DF::NamedStyleDefinition::NamedStyleDefinition(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<DefinitionImpl>();
	auto pcImpl = static_cast<DefinitionImpl *>(m_pcImpl.get());

	pcImpl->SetKeyValue(nInKey);
}

H3DF::NamedStyleDefinition::NamedStyleDefinition(NamedStyleDefinition const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

NamedStyleDefinition & H3DF::NamedStyleDefinition::operator = (NamedStyleDefinition const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

SegmentKey H3DF::NamedStyleDefinition::GetSource() const
{
	auto pcImpl = static_cast<DefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->KeyValue();

	SegmentKey cSegment(pcImpl->KeyValue());
	return cSegment;
}

PortfolioKey H3DF::NamedStyleDefinition::Owner() const
{
	auto pcImpl = static_cast<DefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKey cSegment(pcImpl->KeyValue());

	SegmentKey cOwner = cSegment.Owner();
	HC_KEY nOwner = cOwner.KeyValue();

	PortfolioKey cPortfolioKey(cSegment.Owner());
	return cPortfolioKey;
}

//== StyleKey Function =============================================================================

namespace H3DF
{
	class StyleKeyImpl : public KeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<StyleKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const StyleKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
		}
	};
}

H3DF::StyleKey::StyleKey()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<StyleKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::StyleKey::StyleKey(Key const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<StyleKeyImpl>();
	auto pcImpl = static_cast<StyleKeyImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const KeyImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->KeyImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::StyleKey::StyleKey(StyleKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

StyleKey & H3DF::StyleKey::operator = (StyleKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

CStringA H3DF::StyleKey::Name(bool bIncludePath) const
{
	auto pcImpl = static_cast<KeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	char chName[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Style_Segment(pcImpl->KeyValue(), chName);

	if (false == bIncludePath) {
		HC_Parse_String(chName, "/", -1, chName);
	}

	CStringA strOutName = chName;
	return strOutName;
}

bool H3DF::StyleKey::ShowSource(SegmentKey & cOutSegment) const
{
	auto pcImpl = static_cast<KeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = HC_Show_Style_Segment(pcImpl->KeyValue(), nullptr);

	SegmentKey cSegment(nKey);
	cOutSegment = cSegment;

	return true;
}

bool H3DF::StyleKey::ShowSource(Style::Type & cOutType, SegmentKey & cOutSegment, CStringA & strOutName) const
{
	auto pcImpl = static_cast<KeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = HC_Show_Style_Segment(pcImpl->KeyValue(), strOutName.GetBuffer(MVO_BUFFER_SIZE));
	strOutName.ReleaseBuffer();

	H3DF::Type eType = H3DF::Utility::GetType(pcImpl->KeyValue());

	if (H3DF::Type::SegmentStyle == eType) {
		cOutType = Style::Type::Segment;
	}
	else if (H3DF::Type::NamedStyle == eType) {
		cOutType = Style::Type::Named;
	}
	else {
		DEBUG_STOP;
		cOutType = Style::Type::None;
	}

	SegmentKey cSegment(nKey);
	cOutSegment = cSegment;

	return true;
}

//== StyleControl Function =========================================================================

namespace H3DF
{
	class StyleControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<StyleControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const StyleControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

H3DF::StyleControl::StyleControl(SegmentKey & cInSegment) 
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<StyleControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::StyleControl::StyleControl(StyleControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

StyleControl & H3DF::StyleControl::operator = (StyleControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}


StyleKey H3DF::StyleControl::PushNamed(CStringA strInStyleName)
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Named_Style_Segment(strInStyleName);
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushNamed(CStringA strInStyleName, ConditionalExpression const & cInConditional)
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	CStringA strCondition;
	if (false == cInConditional.ShowCondition(strCondition)) {
		DEBUG_STOP;
	}

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Conditional_Named_Style(strInStyleName, strCondition);
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushSegment(SegmentKey const & cInStyleSource)
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushSegment(SegmentKey const & cInStyleSource, ConditionalExpression const & cInConditional)
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
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

void H3DF::StyleControl::Flush(SegmentKey const & cInStyleSource)
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	CStringA strName = cInStyleSource.Name();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {

		std::vector<CStringA> vStyleStrings;
		int nCount = 0;

		// style 검색
		HC_Begin_Contents_Search(".", "styles");
		{
			HC_Show_Contents_Count(&nCount);

			HC_KEY nKey;
			char chType[MVO_BUFFER_SIZE];
			char chPathName[MVO_BUFFER_SIZE];

			for (int nIndex = 0; nIndex < nCount; nIndex++) {
				HC_Find_Contents(chType, &nKey);
				HC_Show_Style_Segment(nKey, chPathName);

				// 삭제하지 않을 style을 검색
				if (0 != strName.Compare(chPathName)) {
					vStyleStrings.push_back(chPathName);
				}
			}
		}
		HC_End_Contents_Search();

		// 모든 Style을 삭제
		HC_Flush_Contents(".", "styles");

		// Style 재생성
		for (auto strStyle : vStyleStrings) {
			HC_Style_Segment(strStyle);
		}

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

bool H3DF::StyleControl::Show(StyleKeyArray & acOutStyles) const
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	CStringA strStyle;
	int nStyleCount = 0;
	char chType[MVO_BUFFER_SIZE];
	HC_KEY nKey;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Begin_Contents_Search(".", "styles, named styles"); {
			HC_Show_Contents_Count(&nStyleCount);

			for (int nIndex = 0; nIndex < nStyleCount; nIndex++) {
				HC_Find_Contents(chType, &nKey);

				StyleKey cStyle(nKey);
				acOutStyles.push_back(cStyle);
			}

		} HC_End_Contents_Search();
	}
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return !acOutStyles.empty();
}

// ConditionalExpressionArray는 구현 않됨.
bool H3DF::StyleControl::Show(StyleTypeArray & cOutTypes, SegmentKeyArray & cOutSegmentSources, AStringArray & astrOutStyleNames, ConditionalExpressionArray & acOutConditions) const
{
	auto pcImpl = static_cast<StyleControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	CStringA strStyle;
	int nStyleCount = 0;
	char chType[MVO_BUFFER_SIZE];
	HC_KEY nKey;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Begin_Contents_Search(".", "styles, named styles"); {
			HC_Show_Contents_Count(&nStyleCount);

			for (int nIndex = 0; nIndex < nStyleCount; nIndex++) {
				HC_Find_Contents(chType, &nKey);

				SegmentKey cSegment(nKey);
				cOutSegmentSources.push_back(cSegment);

				char chName[MVO_BUFFER_SIZE];
				Style::Type eType;
				if (0 == strcmp("style", chType)) {
					cOutTypes.push_back(Style::Type::Segment);
					HC_Show_Style_Segment(nKey, chName);
					HC_Parse_String(chName, "/", -1, chName);
				}
				else if (0 == strcmp("named style", chType)) {
					cOutTypes.push_back(Style::Type::Named);
					HC_Show_Named_Style_Segment(nKey, chName);
				}
				else {
					DEBUG_STOP;
				}

				cOutTypes.push_back(eType);

				CStringA strName(chName);
				astrOutStyleNames.push_back(strName);

				ConditionalExpression cCondition;
				acOutConditions.push_back(cCondition);
			}

		} HC_End_Contents_Search();
	}
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return !cOutTypes.empty();
}