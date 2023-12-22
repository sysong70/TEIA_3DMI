#include "StdAfx.h"

#include "Geometry.h"

#include "Private/KeyPrivate.h"

#include "Math.h"

#include <HTools.h>

using namespace H3DF;

H3DF::GeometryKey::GeometryKey(HC_KEY nInKey) :
	Key(nInKey)
{
}

bool H3DF::GeometryKey::NearPoint(WindowKey const & cInWindow, const MatrixKit & cModelingMatrix, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const
{
	return false;
}

bool H3DF::GeometryKey::DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const
{
	return false;
}

bool H3DF::GeometryKey::IsCoincident(const LineKey & cInThat) const
{
	return false;
}

bool H3DF::GeometryKey::ShowVolume(SimpleCuboid & out_cuboid) const
{
	return false;
}


GeometryKey & H3DF::GeometryKey::SetUserData(IntPtrTArray const & aInIndices, ByteArrayArray const & aInData)
{
	if (aInIndices.size() == aInData.size()) {
		for (size_t nIndex = 0; nIndex < aInIndices.size(); ++nIndex) {
			SetUserData(aInIndices[nIndex], aInData[nIndex]);
		}
	}

	return *this;
}

GeometryKey & H3DF::GeometryKey::SetUserData(intptr_t nInIndex, size_t nInBytes, BYTE const pnInData[])
{
	KeyPrivate::LocalOpen(*this);

	HC_Set_User_Data(nInIndex, pnInData, (long)nInBytes);

	KeyPrivate::LocalClose(*this);

	return *this;
}

GeometryKey & H3DF::GeometryKey::SetUserData(intptr_t nInIndex, ByteArray const & aInData)
{
	KeyPrivate::LocalOpen(*this);

	HC_Set_User_Data(nInIndex, aInData.data(), (long)aInData.size());

	KeyPrivate::LocalClose(*this);

	return *this;
}
