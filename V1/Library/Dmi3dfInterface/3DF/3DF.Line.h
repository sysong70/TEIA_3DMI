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

	H3DF::Type ObjectType() const { return H3DF::Type::LineKit; };

	unsigned int GetPointCount() const;

	void GetPoints(unsigned int & nOutCount, H3DF::Point pcOutPoints[]) const;
	void SetPoints(unsigned int nInCount, Point const pcInPoints[]);

	void GetRGBColor(H3DF::RGBColor & cOutColor) const;
	void SetRGBColor(H3DF::RGBColor const & cInColor);

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

	H3DF::Type ObjectType() const { return H3DF::Type::LineKey; };

	int GetPointCount() const;

	bool ShowPoints(WorldPointArray & aOutPoints) const;

	bool IsCoincident(const LineKey & cInThat, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2) const;

	bool GetEndPoint(Point & cSP, Point & cEP);
	bool GetMidPoint(Point & cMP);
	bool GetIntersectionPoint(LineKey & cInLine, PointArray & aOutIntersectionPoints);
	bool GetIntersectionPoint(LineKey & cInLine, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2, PointArray & aOutIntersectionPoints);

	//== 계산 함수 ===================================================================================
	bool NearPoint(WindowKey const & cInWindow, const MatrixKit & cModelingMatrix, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const override;
	bool DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const override;

	bool Length(double & dLength) const;

private:
	bool GetIntersectionPoint(const WorldPointArray & aPoints1, const WorldPointArray & aPoints2, PointArray & aOutIntersectionPoints);
};

CLOSE_3DF_NAMESPACE