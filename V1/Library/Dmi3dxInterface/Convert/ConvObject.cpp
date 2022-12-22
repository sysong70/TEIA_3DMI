#include "stdafx.h"
#include "ConvObject.h"

#include "ConvSurface.h"

// == ConvEntityInfo Structure 관련 함수 =============================================================

ConvEntityInfo::ConvEntityInfo(int nCount)
{
	Init(1);
}

bool ConvEntityInfo::Init(int nCount)
{
	nEntityCount = nCount;
	nTargetEntityIndex = 0;

	peEntityType = new A3DEEntityType[nCount];
	if(nullptr == peEntityType) {
		return false;
	}

	pcEntityData = new DWORD_PTR * [nCount];
	if(nullptr == pcEntityData) {
		delete[] peEntityType;
		return false;
	}

	for(int nIndex = 0; nIndex < nCount; nIndex++) {
		peEntityType[nIndex] = kA3DTypeUnknown;
		pcEntityData[nIndex] = nullptr;
	}

	return true;
}

 void ConvEntityInfo::Delete() 
{
	if(nullptr != peEntityType) {
		delete[] peEntityType;
	}

	if(nullptr != pcEntityData) {
		delete[] pcEntityData;
	}

	nEntityCount = 0;
}

 void ConvEntityInfo::ClearData()
 {
	 for(int nIndex = 0; nIndex < nEntityCount; nIndex++) {
		 delete pcEntityData[nIndex];
	 }

	 Delete();
 }

// == ConvObject Class 관련 함수 =====================================================================

ConvObject::ConvObject()
{
}

ConvObject::~ConvObject()
{
}

bool ConvObject::GetConvSurfaceData(const A3DSurfBase * pcSurfBase, double dContextScale, ConvSurfaceMap & mpcConvSurfaceMap, ConvSurface *& pcSurface)
{
	// Map에서 검색
	if(true == mpcConvSurfaceMap.Lookup((DWORD_PTR) pcSurfBase, pcSurface)) {
		return true;
	}

	pcSurface = new ConvSurface(pcSurfBase, dContextScale);
	if(nullptr == pcSurface) {
		return false;
	}

	if(false == pcSurface->IsInit()) {
		return false;
	}

	mpcConvSurfaceMap.SetAt((DWORD_PTR) pcSurfBase, pcSurface);

	return true;
}