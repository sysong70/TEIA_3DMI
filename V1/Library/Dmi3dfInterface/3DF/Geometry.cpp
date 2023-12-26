#include "StdAfx.h"

#include "Geometry.h"

#include "Impl/GeometryImpl.h"

#include "Math.h"

#include <HTools.h>

using namespace H3DF;

//== GeometryKey 관련 함수 ===========================================================================

H3DF::GeometryKey::GeometryKey(HC_KEY nInKey) : Key(INVALID_KEY)
{
	if (INVALID_KEY == nInKey) {
		return;
	}

	GeometryKeyImpl * pcImpl = new GeometryKeyImpl();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

H3DF::GeometryKey::~GeometryKey()
{
	int i = 0;
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

//== User Data 관련 함수 ============================================================================

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
 	GeometryKeyImpl::LocalOpen(*this);
	
 	HC_Set_User_Data(nInIndex, pnInData, (long)nInBytes);

 	GeometryKeyImpl::LocalClose(*this);

	return *this;
}

GeometryKey & H3DF::GeometryKey::SetUserData(intptr_t nInIndex, ByteArray const & aInData)
{
	//GeometryKeyPrivate::LocalOpen(*this);

	HC_Open_Geometry(KeyValue());

	HC_Set_User_Data(nInIndex, aInData.data(), (long)aInData.size());

	HC_Close_Geometry();

	//GeometryKeyPrivate::LocalClose(*this);

	return *this;
}

size_t H3DF::GeometryKey::ShowUserDataCount() const
{
	GeometryKeyImpl::LocalOpen(*this);

	size_t nCount = abs(HC_Show_User_Data_Indices(nullptr, 0));

	GeometryKeyImpl::LocalClose(*this);

	return nCount;
}

bool H3DF::GeometryKey::ShowUserData(IntPtrTArray & aOutIndices, ByteArrayArray & aOutData) const
{
	GeometryKeyImpl::LocalOpen(*this);

	long nSize = HC_Show_User_Data_Indices(nullptr, 0);
	nSize = abs(nSize);
	aOutIndices.resize(nSize);

	nSize = HC_Show_User_Data_Indices(aOutIndices.data(), nSize);
	if (0 == nSize) {
		return false;
	}

	aOutData.resize(nSize);

	for (size_t nIndex = 0; nIndex < aOutIndices.size(); ++nIndex) {
		long nBytes = HC_Show_One_User_Data(aOutIndices[nIndex], nullptr, 0);
		nBytes = abs(nBytes);
		aOutData[nIndex].resize(nBytes);

		HC_Show_One_User_Data(aOutIndices[nIndex], aOutData[nIndex].data(), nBytes);
	}

	GeometryKeyImpl::LocalClose(*this);

	return true;
}

bool H3DF::GeometryKey::ShowUserDataIndices(IntPtrTArray & aOutIndices) const
{
	GeometryKeyImpl::LocalOpen(*this);

	long nSize = HC_Show_User_Data_Indices(nullptr, 0);
	nSize = abs(nSize);
	aOutIndices.resize(nSize);

	nSize = HC_Show_User_Data_Indices(aOutIndices.data(), nSize);
	if (0 == nSize) {
		return false;
	}

	GeometryKeyImpl::LocalClose(*this);

	return true;
}

bool H3DF::GeometryKey::ShowUserData(intptr_t nInIndex, ByteArray & aOutData) const
{
	GeometryKeyImpl::LocalOpen(*this);

	long nSize = HC_Show_One_User_Data(nInIndex, nullptr, 0);
	nSize = abs(nSize);

	aOutData.resize(nSize);

	nSize = HC_Show_One_User_Data(nInIndex, aOutData.data(), nSize);
	if (0 == nSize) {
		return false;
	}

	GeometryKeyImpl::LocalClose(*this);

	return true;
}