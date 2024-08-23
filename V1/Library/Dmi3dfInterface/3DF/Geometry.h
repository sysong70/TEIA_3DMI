#pragma once

#include "3DF.h"
#include "Key.h"
#include "Math.h"

namespace H3DF
{
	class API_3DF GeometryKey : public Key
	{
	public:
		GeometryKey();
		GeometryKey(HC_KEY nInKey);

		H3DF::Type ObjectType() const override { return H3DF::Type::GeometryKey; };

		virtual bool NearPoint(WindowKey const & cInWindow, const MatrixKit & cModelingMatrix, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const;
		virtual bool DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const;

		virtual bool IsCoincident(const LineKey & cInThat) const;

		virtual bool ShowVolume(SimpleCuboid & out_cuboid) const;

		//== User Data 관련 함수 =====================================================================
		GeometryKey & SetUserData(IntPtrTArray const & aInIndices, ByteArrayArray const & aInData);
		GeometryKey & SetUserData(intptr_t nInIndex, size_t nInBytes, BYTE const pnInData[]);
		GeometryKey & SetUserData(intptr_t nInIndex, ByteArray const & aInData);

		// 	SegmentKey & UnsetUserData(intptr_t nInIndex);
		// 	SegmentKey & UnsetUserData(size_t nInCount, intptr_t const pnInIndices[]);
		// 	SegmentKey & UnsetUserData(IntPtrTArray const & pnInIndices);
		// 	SegmentKey & UnsetAllUserData();

		size_t ShowUserDataCount() const;
		bool ShowUserData(IntPtrTArray & aOutIndices, ByteArrayArray & aOutData) const;
		bool ShowUserDataIndices(IntPtrTArray & aOutIndices) const;
		bool ShowUserData(intptr_t nInIndex, ByteArray & aOutData) const;
	};
}
