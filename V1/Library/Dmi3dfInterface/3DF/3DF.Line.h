#pragma once

#include "3DF.h"

#include "3DF.Kit.h"
#include "3DF.Geometry.h"

#include "3DF.Color.h"

#include <atlcoll.h>

OPEN_3DF_NAMESPACE

class API_3DF LineKit : public Kit
{
public:
	LineKit();
	LineKit(LineKit const & cInThat);

	void Set(LineKit const & cInThat);
	LineKit const & operator=(LineKit const & cInThat);

	unsigned int GetPointCount() const;

	void GetPoints(unsigned int & nOutCount, _3DF::Point pcOutPoints[]) const;
	void SetPoints(unsigned int nInCount, Point const pcInPoints[]);

	void GetRGBColor(_3DF::RGBColor & cOutColor) const;
	void SetRGBColor(_3DF::RGBColor const & cInColor);

	void GetLinePattern(char out_pattern[PATTERN_BUFFER_SIZE]) const;
};

class API_3DF LineKey : public GeometryKey
{
public:
	LineKey(HC_KEY nInKey = INVALID_KEY);
};

CLOSE_3DF_NAMESPACE