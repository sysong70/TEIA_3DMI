#include "StdAfx.h"

#include "3DF.Geometry.h"

#include "3DF.Math.h"

USING_3DF_NAMESPACE

GeometryKey::GeometryKey(HC_KEY nInKey) :
	Key(nInKey)
{
}

bool GeometryKey::NearPoint(WindowKey const & cInWindow, const MatrixKit & cModelingMatrix, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const
{
	return false;
}

bool GeometryKey::DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const
{
	return false;
}