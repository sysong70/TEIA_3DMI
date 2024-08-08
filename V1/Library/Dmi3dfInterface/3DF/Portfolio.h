#pragma once

#include "Segment.h"
#include "Control.h"

#include <deque>

namespace H3DF
{
	class API_3DF PortfolioKey : public Key
	{
	public:
		PortfolioKey();
		PortfolioKey(HC_KEY nInKey);
		explicit PortfolioKey(Key const & cInThat);
		PortfolioKey(PortfolioKey const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::PortfolioKey;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(PortfolioKey const & cInThat);
		PortfolioKey & operator = (PortfolioKey const & cInThat);

		NamedStyleDefinition DefineNamedStyle(CStringA strInName, SegmentKey const & cInStyleSource);
	};

	class API_3DF PortfolioControl : public Control
	{
	public:
		explicit PortfolioControl(SegmentKey & cInSegmentKey);
		PortfolioControl(PortfolioControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::PortfolioControl;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(PortfolioControl const & cInThat);
		PortfolioControl & operator = (PortfolioControl const & cInThat);

		size_t GetCount() const;

		PortfolioControl & Push(PortfolioKey const & cInPortfolio);
		bool Pop();

		bool ShowTop(PortfolioKey & cOutPortfolio) const;

	private:
		
	};
}