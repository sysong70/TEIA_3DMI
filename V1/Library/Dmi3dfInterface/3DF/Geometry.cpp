#include "StdAfx.h"

#include "Geometry.h"

#include "Math.h"

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

bool GeometryKey::IsCoincident(const LineKey & cInThat) const
{
	return false;
}

bool GeometryKey::ShowVolume(SimpleCuboid & out_cuboid) const
{
	return false;
}