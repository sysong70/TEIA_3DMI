#pragma once

#include "3DF.h"
#include "3DF.Key.h"
#include "3DF.Math.h"

OPEN_3DF_NAMESPACE

class API_3DF GeometryKey : public Key
{
public:
	GeometryKey(HC_KEY nInKey = INVALID_KEY);

	H3DF::Type ObjectType() const { return H3DF::Type::GeometryKey; };

	virtual bool NearPoint(WindowKey const & cInWindow, const MatrixKit & cModelingMatrix, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const;
	virtual bool DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const;

	virtual bool IsCoincident(const LineKey & cInThat) const;

	virtual bool ShowVolume(SimpleCuboid & out_cuboid) const;
};

CLOSE_3DF_NAMESPACE