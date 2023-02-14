#pragma once

#include "3DF.Key.h"
#include "3DF.Control.h"

#include <deque>

OPEN_3DF_NAMESPACE

using PortfolioDeque = std::deque<PortfolioKey *>;

class API_3DF PortfolioKey : public Key
{
public:
	PortfolioKey(HC_KEY nInKey = INVALID_KEY);
	PortfolioKey(PortfolioKey const & cInThat);

	void Set(PortfolioKey const & cInThat);
	PortfolioKey & operator = (PortfolioKey const & cInThat);

	NamedStyleDefinition DefineNamedStyle(CString strInName, SegmentKey const & cInStyleSource);
};


class API_3DF PortfolioControl : public Control
{
public:
	virtual ~PortfolioControl();

	size_t GetCount() const;

	PortfolioControl & Push(PortfolioKey const & cInPortfolio);
	bool Pop();

	bool ShowTop(PortfolioKey & cOutPortfolio) const;

private:
	PortfolioDeque * m_pdpcPortfolioDeque = nullptr;
};


CLOSE_3DF_NAMESPACE