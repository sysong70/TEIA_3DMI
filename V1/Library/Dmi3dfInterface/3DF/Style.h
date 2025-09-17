#pragma once

#include "3DF.h"
#include "Key.h"
#include "Control.h"
#include "Definition.h"

#include <Common_Define.h>

namespace H3DF
{
	class API_3DF Style {
	public:
		enum class Type : uint32_t {
			Segment,
			Named,
			None
		};

		enum class AppendMode : uint32_t {
			None,
			And,
			Or
		};

	private:
		Style() {}
	};

	using StyleTypeArray = std::vector<Style::Type>;

	class API_3DF NamedStyleDefinition : public Definition
	{
	public:
		NamedStyleDefinition();
		NamedStyleDefinition(HC_KEY nInKey);
		NamedStyleDefinition(NamedStyleDefinition const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::NamedStyleDefinition;
		H3DF::Type ObjectType() const { return staticType; };

		NamedStyleDefinition & operator = (NamedStyleDefinition const & cInThat);

		SegmentKey GetSource() const;

		PortfolioKey Owner() const;
	};

	class API_3DF StyleKey : public Key
	{
	public:
		StyleKey();
		explicit StyleKey(Key const & cInThat);
		StyleKey(StyleKey const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::StyleKey;
		H3DF::Type ObjectType() const { return staticType; };

		StyleKey & operator = (StyleKey const & cInThat);

		CStringA Name(bool bIncludePath = true) const;

		bool ShowSource(SegmentKey & cOutSegment) const;
		bool ShowSource(Style::Type & cOutType, SegmentKey & cOutSegment, CStringA & strOutName) const;
	};

	class API_3DF StyleControl : public Control
	{
	public:
		StyleControl(SegmentKey & cInSegment);
		StyleControl(StyleControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::StyleControl;
		H3DF::Type ObjectType() const { return staticType; };

		StyleControl & operator = (StyleControl const & cInThat);

		StyleKey PushNamed(CStringA strInStyleName);
		StyleKey PushNamed(CStringA strInStyleName, ConditionalExpression const & cInConditional);

		StyleKey PushSegment(SegmentKey const & cInStyleSource);
		StyleKey PushSegment(SegmentKey const & cInStyleSource, ConditionalExpression const & cInConditional);

		void Flush(SegmentKey const & cInStyleSource);

		bool Show(StyleKeyArray & acOutStyles) const;
		bool Show(StyleTypeArray & cOutTypes, SegmentKeyArray & cOutSegmentSources, AStringArray & astrOutStyleNames, ConditionalExpressionArray & acOutConditions) const;
	};
}
