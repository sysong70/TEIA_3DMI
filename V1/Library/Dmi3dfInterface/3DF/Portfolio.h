#pragma once

#include "Segment.h"
#include "Control.h"

#include <deque>

namespace H3DF
{
	class API_3DF PortfolioKey : public SegmentKey
	{
	public:
		PortfolioKey();
		PortfolioKey(CString strInName);
		PortfolioKey(HC_KEY nInKey);
		PortfolioKey(PortfolioKey const & cInThat);

		void Set(PortfolioKey const & cInThat);
		PortfolioKey & operator = (PortfolioKey const & cInThat);

		NamedStyleDefinition DefineNamedStyle(CString strInName, SegmentKey const & cInStyleSource);
	};

	class API_3DF PortfolioControl : public Control
	{
	public:
		explicit PortfolioControl(SegmentKey & cInSegmentKey);
		PortfolioControl(PortfolioControl const & cInThat);

		void Set(PortfolioControl const & cInThat);
		PortfolioControl & operator = (PortfolioControl const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::PortfolioControl; };

		size_t GetCount() const;

		PortfolioControl & Push(PortfolioKey const & cInPortfolio);
		bool Pop();

		bool ShowTop(PortfolioKey & cOutPortfolio) const;

	private:
		
	};
}