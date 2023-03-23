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

	_3DF::Type ObjectType() const { return _3DF::Type::LineKit; };

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
	LineKey();
	explicit LineKey(Key const & cInKey);
	LineKey(LineKey const & cInThat);

	void Set(LineKey const & cInThat);
	LineKey & operator=(LineKey const & cInThat);

	_3DF::Type ObjectType() const { return _3DF::Type::LineKey; };

	int GetPointCount() const;
	bool ShowPoints(WorldPointArray & aOutPoints) const;

	//== 계산 함수 ===================================================================================
	bool NearPoint(WindowKey const & cInWindow, const WorldPoint & cInPoint, WorldPoint & cOutPoint) const override;
	bool DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const override;
};

CLOSE_3DF_NAMESPACE