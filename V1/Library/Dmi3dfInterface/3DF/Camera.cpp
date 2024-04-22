#include "StdAfx.h"

#include "Camera.h"

#include "Math.h"
#include "Point.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "Impl/KeyImpl.h"
#include "Impl/ControlImpl.h"

#include <HTools.h>

using namespace H3DF;

namespace H3DF
{
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

		Point cPosition; bool bPositionFlag = false;
		Point cTarget; bool bTargetFlag = false;
		Vector cUpVector; bool bUpVectorFlag = false;
		float fWidth = 0.0f; bool bWidthFlag = false;
		float fHeight = 0.0f; bool bHeightFlag = false;
		float fNearLimit = 0.0f; bool bNearLimitFlag = false;
		Camera::Projection eType = Camera::Projection::Default;  bool bTypeFlag = false;
		float fOblique_Y_Skew = 0.0f;
		float fOblique_X_Skew = 0.0f;
	};
}

H3DF::CameraKit::CameraKit()
{
	m_pcImpl = new CameraKitImpl();
}

H3DF::CameraKit::CameraKit(CameraKit const & cInThat)
{
	m_pcImpl = new CameraKitImpl();
	Set(cInThat);
}

void H3DF::CameraKit::Set(CameraKit const & cInThat)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	CameraKitImpl * pcInThatImpl = (CameraKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

CameraKit const & H3DF::CameraKit::operator=(CameraKit const & cInThat)
{
	Set(cInThat);
	return *this;
}
//== Set ===========================================================================================
CameraKit & H3DF::CameraKit::SetUpVector(Vector const & cInUpVector)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->cUpVector = cInUpVector;
	pcImpl->bUpVectorFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetPosition(Point const & cInPosition)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->cPosition = cInPosition;
	pcImpl->bPositionFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetTarget(Point const & cInTarget)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->cTarget = cInTarget;
	pcImpl->bTargetFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetProjection(Camera::Projection eInType, float fInOblique_Y_Skew, float fInOblique_X_Skew)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->eType = eInType;
	pcImpl->bTypeFlag = true;

	pcImpl->fOblique_Y_Skew = fInOblique_Y_Skew;
	pcImpl->fOblique_X_Skew = fInOblique_X_Skew;

	return *this;
}

CameraKit & H3DF::CameraKit::SetField(float fInWidth, float fInHeight)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->fWidth = fInWidth;
	pcImpl->bWidthFlag = true;

	pcImpl->fHeight = fInHeight;
	pcImpl->bHeightFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetNearLimit(float const fInLimit)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->fNearLimit = fInLimit;
	pcImpl->bNearLimitFlag = true;

	return *this;
}



//== Unset =========================================================================================
CameraKit & H3DF::CameraKit::UnsetUpVector()
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->bUpVectorFlag = false;

	return *this;
}

CameraKit & H3DF::CameraKit::UnsetPosition()
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->bPositionFlag = false;

	return *this;
}

CameraKit & H3DF::CameraKit::UnsetTarget()
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	pcImpl->bTargetFlag = false;

	return *this;
}

//== Show ===========================================================================================
bool H3DF::CameraKit::ShowUpVector(Vector & cOutUpVector) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bUpVectorFlag) {
		return false;
	}

	cOutUpVector = pcImpl->cUpVector;
	return true;
}

bool H3DF::CameraKit::ShowPosition(Point & cOutPosition) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bPositionFlag) {
		return false;
	}

	cOutPosition = pcImpl->cPosition;
	return true;
}

bool H3DF::CameraKit::ShowTarget(Point & cOutTarget) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bTargetFlag) {
		return false;
	}

	cOutTarget = pcImpl->cTarget;
	return true;
}

bool H3DF::CameraKit::ShowProjection(Camera::Projection & eOutType) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bTargetFlag) {
		return false;
	}

	eOutType = pcImpl->eType;
	return true;
}

bool H3DF::CameraKit::ShowProjection(Camera::Projection & eOutType, float & fOutOblique_Y_Skew, float & fOutOblique_X_Skew) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bTypeFlag) {
		return false;
	}

	eOutType = pcImpl->eType;
	fOutOblique_Y_Skew = pcImpl->fOblique_Y_Skew;
	fOutOblique_X_Skew = pcImpl->fOblique_X_Skew;

	return true;
}

bool H3DF::CameraKit::ShowWidth(float & fOutWidth) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bWidthFlag) {
		return false;
	}

	fOutWidth = pcImpl->fWidth;

	return true;
}

bool H3DF::CameraKit::ShowHeight(float & fOutHeight) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bHeightFlag) {
		return false;
	}

	fOutHeight = pcImpl->fHeight;

	return true;
}

bool H3DF::CameraKit::ShowField(float & fOutWidth, float & fOutHeight) const
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
bool H3DF::CameraKit::ShowNearLimit(float & fOutNearLimit) const
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)m_pcImpl;
	if (false == pcImpl->bNearLimitFlag) {
		return false;
	}

	fOutNearLimit = pcImpl->fNearLimit;

	return true;
}

// Show Matrix 생성
bool H3DF::CameraKit::ShowMatrix(MatrixKit & cMatrix) const
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

//
namespace H3DF
{
	class CameraControlImpl : public ControlImpl
	{
	public:
		CameraControlImpl() { m_eType = H3DF::Type::CameraControl; }

		void Copy(CameraControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

//== CameraControl Class ============================================================================
H3DF::CameraControl::CameraControl(SegmentKey & cInSegmentKey)
{
	CameraControlImpl * pcImpl = new CameraControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::CameraControl::CameraControl(CameraControl const & cInThat)
{
	m_pcImpl = new CameraControlImpl();
	Set(cInThat);
}

void H3DF::CameraControl::Set(CameraControl const & cInThat)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	CameraControlImpl * pcInThatImpl = (CameraControlImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

CameraControl & H3DF::CameraControl::operator = (CameraControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

CameraControl & H3DF::CameraControl::SetUpVector(Vector const & cInUp)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Up_Vector(cInUp.x, cInUp.y, cInUp.z);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}


CameraControl & H3DF::CameraControl::SetPosition(Point const & cInPosition)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Position(cInPosition.x, cInPosition.y, cInPosition.z);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetTarget(Point const & cInTarget)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Target(cInTarget.x, cInTarget.y, cInTarget.z);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetProjection(Camera::Projection eInType, float fInObliqueXSkew, float fInObliqueYSkew)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strProjectionType;
		if (Camera::Projection::Orthographic == eInType) {
			strProjectionType = "orthographic";
		}
		else if (Camera::Projection::Perspective == eInType) {
			strProjectionType = "perspective";
		}
		else if (Camera::Projection::Stretched == eInType) {
			strProjectionType = "stretched";
		}

		if (0.0f != fInObliqueXSkew || 0.0f != fInObliqueYSkew) {
			strProjectionType.Format("oblique %s = (%f, %f)", strProjectionType, fInObliqueXSkew, fInObliqueYSkew);
		}

		HC_Set_Camera_Projection(strProjectionType);

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetField(float fInWidth, float fInHeight)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Field(fInWidth, fInHeight);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetNearLimit(float fInLimit)
{
	CameraControlImpl * pcImpl = (CameraControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Near_Limit(fInLimit);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}