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

	TDF::Type ObjectType() const { return TDF::Type::LineKit; };

	unsigned int GetPointCount() const;

	void GetPoints(unsigned int & nOutCount, TDF::Point pcOutPoints[]) const;
	void SetPoints(unsigned int nInCount, Point const pcInPoints[]);

	void GetRGBColor(TDF::RGBColor & cOutColor) const;
	void SetRGBColor(TDF::RGBColor const & cInColor);

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

	TDF::Type ObjectType() const { return TDF::Type::LineKey; };

	int GetPointCount() const;
	bool ShowPoints(WorldPointArray & aOutPoints) const;

	bool GetEndPoint(Point & cSP, Point & cEP);
	bool GetMidPoint(Point & cMP);
	bool GetIntersectionPoint(LineKey & cLine, PointArray & aOutIntersectionPoints);

	//== 계산 함수 ===================================================================================
	bool NearPoint(WindowKey const & cInWindow, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const override;
	bool DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const override;
};

CLOSE_3DF_NAMESPACE