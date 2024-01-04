#pragma once

#include "3DF.h"
#include "Key.h"
#include "Control.h"
#include "Definition.h"

namespace H3DF
{
	class API_3DF NamedStyleDefinition : public Definition
	{
	public:
		NamedStyleDefinition();
		NamedStyleDefinition(HC_KEY nInKey);
		NamedStyleDefinition(NamedStyleDefinition const & cInThat);

		void Set(NamedStyleDefinition const & cInThat);
		NamedStyleDefinition & operator = (NamedStyleDefinition const & cInThat);

		SegmentKey GetSource() const;

		PortfolioKey Owner() const;
	};

	class API_3DF StyleKey : public Key
	{
	public:
		StyleKey();
		StyleKey(HC_KEY cInKey);
		StyleKey(StyleKey const & cInThat);

		void Set(StyleKey const & cInThat);
		StyleKey & operator = (StyleKey const & cInThat);
	};

	class API_3DF StyleControl : public Control
	{
	public:
		StyleControl(SegmentKey & cInSegment);
		StyleControl(StyleControl const & cInThat);

		void Set(StyleControl const & cInThat);
		StyleControl & operator = (StyleControl const & cInThat);

		StyleKey PushNamed(CStringA strInStyleName);
		StyleKey PushSegment(SegmentKey const & cInStyleSource);
	};
}
