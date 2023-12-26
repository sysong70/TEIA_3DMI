#include "StdAfx.h"

#include "Camera.h"

#include "Math.h"
#include "Point.h"

#include "Impl/KeyImpl.h"

#include <HTools.h>

USING_3DF_NAMESPACE

class CameraKitImpl : public Impl
{
public:
	void Copy(CameraKitImpl * pcInThat)
	{
		cUpVector = pcInThat->cUpVector;
		bUpVectorFlag = pcInThat->bUpVectorFlag;
		cPosition = pcInThat->cPosition;
		bPositionFlag = pcInThat->bPositionFlag;
		cTarget = pcInThat->cTarget;
		bTargetFlag = pcInThat->bTargetFlag;
	}

	Vector cUpVector; bool bUpVectorFlag = false;
	Point cPosition; bool bPositionFlag = false;
	Point cTarget; bool bTargetFlag = false;
	Camera::Projection eType = Camera::Projection::Default;  bool bTypeFlag = false;
	float fOblique_Y_Skew = 0.0f; bool bOblique_Y_SkewFlag = false;
	float fOblique_X_Skew = 0.0f; bool bOutOblique_X_SkewFlag = false;
	float fWidth = 0.0f; bool bWidthFlag = false;
	float fHeight = 0.0f; bool bHeightFlag = false;
	float fNearLimit = 0.0f; bool bNearLimitFlag = false;
};

CameraKit::CameraKit()
{
	m_pcImpl = new CameraKitImpl();
}

CameraKit::CameraKit(CameraKit const & cInThat)
{
	m_pcImpl = new CameraKitImpl();
	Set(cInThat);
}

void CameraKit::Set(CameraKit const & cInThat)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	CameraKitImpl * pcInThatImpl = (CameraKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

CameraKit const & CameraKit::operator=(CameraKit const & cInThat)
{
	Set(cInThat);
	return *this;
}
//== Set ===========================================================================================
CameraKit & CameraKit::SetUpVector(Vector const & cInUpVector)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->cUpVector = cInUpVector;
	pcImpl->bUpVectorFlag = true;

	return *this;
}

CameraKit & CameraKit::SetPosition(Point const & cInPosition)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->cPosition = cInPosition;
	pcImpl->bPositionFlag = true;

	return *this;
}

CameraKit & CameraKit::SetTarget(Point const & cInTarget)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->cTarget = cInTarget;
	pcImpl->bTargetFlag = true;

	return *this;
}

CameraKit & CameraKit::SetProjection(Camera::Projection eInType, float fInOblique_Y_Skew, float fInOblique_X_Skew)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->eType = eInType;
	pcImpl->bTypeFlag = true;

	pcImpl->fOblique_Y_Skew = fInOblique_Y_Skew;
	pcImpl->bOblique_Y_SkewFlag = true;

	pcImpl->fOblique_X_Skew = fInOblique_X_Skew;
	pcImpl->bOutOblique_X_SkewFlag = true;

	return *this;
}

CameraKit & CameraKit::SetField(float fInWidth, float fInHeight)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->fWidth = fInWidth;
	pcImpl->bWidthFlag = true;

	pcImpl->fHeight = fInHeight;
	pcImpl->bHeightFlag = true;

	return *this;
}

CameraKit & CameraKit::SetNearLimit(float const fInLimit)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->fNearLimit = fInLimit;
	pcImpl->bNearLimitFlag = true;

	return *this;
}



//== Unset =========================================================================================
CameraKit & CameraKit::UnsetUpVector()
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->bUpVectorFlag = false;

	return *this;
}

CameraKit & CameraKit::UnsetPosition()
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->bPositionFlag = false;

	return *this;
}

CameraKit & CameraKit::UnsetTarget()
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->bTargetFlag = false;

	return *this;
}

//== Show ===========================================================================================
bool CameraKit::ShowUpVector(Vector & cOutUpVector) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bUpVectorFlag) {
		return false;
	}

	cOutUpVector = pcImpl->cUpVector;
	return true;
}

bool CameraKit::ShowPosition(Point & cOutPosition) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bPositionFlag) {
		return false;
	}

	cOutPosition = pcImpl->cPosition;
	return true;
}

bool CameraKit::ShowTarget(Point & cOutTarget) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bTargetFlag) {
		return false;
	}

	cOutTarget = pcImpl->cTarget;
	return true;
}

bool CameraKit::ShowProjection(Camera::Projection & eOutType) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bTargetFlag) {
		return false;
	}

	eOutType = pcImpl->eType;
	return true;
}

bool CameraKit::ShowProjection(Camera::Projection & eOutType, float & fOutOblique_Y_Skew, float & fOutOblique_X_Skew) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bTypeFlag) {
		return false;
	}

	if (false == pcImpl->bOblique_Y_SkewFlag) {
		return false;
	}

	if (false == pcImpl->bOutOblique_X_SkewFlag) {
		return false;
	}

	eOutType = pcImpl->eType;
	fOutOblique_Y_Skew = pcImpl->fOblique_Y_Skew;
	fOutOblique_X_Skew = pcImpl->fOblique_X_Skew;

	return true;
}

bool CameraKit::ShowWidth(float & fOutWidth) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bWidthFlag) {
		return false;
	}

	fOutWidth = pcImpl->fWidth;

	return true;
}

bool CameraKit::ShowHeight(float & fOutHeight) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bHeightFlag) {
		return false;
	}

	fOutHeight = pcImpl->fHeight;

	return true;
}

bool CameraKit::ShowField(float & fOutWidth, float & fOutHeight) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bWidthFlag) {
		return false;
	}

	if (false == pcImpl->bHeightFlag) {
		return false;
	}

	fOutWidth = pcImpl->fWidth;
	fOutHeight = pcImpl->fHeight;

	return true;
}

// NearLimit Show
bool CameraKit::ShowNearLimit(float & fOutNearLimit) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bNearLimitFlag) {
		return false;
	}

	fOutNearLimit = pcImpl->fNearLimit;

	return true;
}

// Show Matrix 생성
bool CameraKit::ShowMatrix(MatrixKit & cMatrix) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;

	// Matrix를 생성하기 위해서 필요한 값들이 존재하는지 여부를 확인한다.
	if (false == pcImpl->bUpVectorFlag) {
		return false;
	}
	
	if (false == pcImpl->bPositionFlag) {
		return false;
	}

	if (false == pcImpl->bTargetFlag) {
		return false;
	}

	Point cPosition = pcImpl->cPosition;
	Point cTarget = pcImpl->cTarget;
	Vector cViewNormal = cPosition - cTarget;
	cViewNormal.Normalize();

	Vector cYAxis = pcImpl->cUpVector;
	cYAxis.Normalize();

	Vector cXAxis = cYAxis.Cross(cViewNormal);

	cMatrix[0][0] = cXAxis.x;
	cMatrix[0][1] = cXAxis.y;
	cMatrix[0][2] = cXAxis.z;

	cMatrix[1][0] = cYAxis.x;
	cMatrix[1][1] = cYAxis.y;
	cMatrix[1][2] = cYAxis.z;

	cMatrix[2][0] = cViewNormal.x;
	cMatrix[2][1] = cViewNormal.y;
	cMatrix[2][2] = cViewNormal.z;

 	cMatrix[3][0] = cTarget.x;
 	cMatrix[3][1] = cTarget.y;
 	cMatrix[3][2] = cTarget.z;

	return true;
}
