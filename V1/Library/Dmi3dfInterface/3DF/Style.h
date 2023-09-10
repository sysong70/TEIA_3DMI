#pragma once

#include "3DF.h"
#include "Key.h"
#include "Control.h"

OPEN_3DF_NAMESPACE

class API_3DF NamedStyleDefinition
{
public:
	NamedStyleDefinition(SegmentKey cInSource);
	NamedStyleDefinition(NamedStyleDefinition const & cInThat);

	NamedStyleDefinition & operator = (NamedStyleDefinition const & cInThat);

	SegmentKey GetSource() const;

	PortfolioKey Owner() const;

private:
	HC_KEY m_nOwnerPortfolioKey = INVALID_KEY;
	HC_KEY m_nSourceSegmentKey = INVALID_KEY;
};

class API_3DF StyleKey : public Key
{
public:
	StyleKey(HC_KEY cInKey = INVALID_KEY);
	StyleKey(StyleKey const & cInThat);

	void Set(StyleKey const & cInThat);
	StyleKey & operator = (StyleKey const & cInThat);
};

class API_3DF StyleControl : public Control
{
public:
	StyleControl(SegmentKey & cInSegment);
	StyleControl(StyleControl const & cInThat);

	StyleControl & operator = (StyleControl const & cInThat);

	//StyleKey PushNamed(CString & strInStyleName);
	StyleKey PushSegment(SegmentKey const & cInStyleSource);
};

CLOSE_3DF_NAMESPACE
