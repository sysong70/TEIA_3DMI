#include "StdAfx.h"

#include "Camera.h"
#include "Impl/CameraImpl.h"

#include "Math.h"
#include "Point.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "Impl/KeyImpl.h"
#include "Impl/ControlImpl.h"

#include <HTools.h>

using namespace H3DF;

H3DF::CameraKit::CameraKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CameraKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CameraKit::CameraKit(CameraKit const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

CameraKit const & H3DF::CameraKit::operator=(CameraKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}
//== Set ===========================================================================================
CameraKit & H3DF::CameraKit::SetUpVector(Vector const & cInUpVector)
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.cUpVector = cInUpVector;
	pcImpl->m_cData.bUpVectorFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetPosition(Point const & cInPosition)
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.cPosition = cInPosition;
	pcImpl->m_cData.bPositionFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetTarget(Point const & cInTarget)
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.cTarget = cInTarget;
	pcImpl->m_cData.bTargetFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetProjection(Camera::Projection eInType, float fInOblique_Y_Skew, float fInOblique_X_Skew)
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.eType = eInType;
	pcImpl->m_cData.bTypeFlag = true;

	pcImpl->m_cData.fOblique_Y_Skew = fInOblique_Y_Skew;
	pcImpl->m_cData.fOblique_X_Skew = fInOblique_X_Skew;

	return *this;
}

CameraKit & H3DF::CameraKit::SetField(float fInWidth, float fInHeight)
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.fWidth = fInWidth;
	pcImpl->m_cData.bWidthFlag = true;

	pcImpl->m_cData.fHeight = fInHeight;
	pcImpl->m_cData.bHeightFlag = true;

	return *this;
}

CameraKit & H3DF::CameraKit::SetNearLimit(float const fInLimit)
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.fNearLimit = fInLimit;
	pcImpl->m_cData.bNearLimitFlag = true;

	return *this;
}



//== Unset =========================================================================================
CameraKit & H3DF::CameraKit::UnsetUpVector()
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.bUpVectorFlag = false;

	return *this;
}

CameraKit & H3DF::CameraKit::UnsetPosition()
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.bPositionFlag = false;

	return *this;
}

CameraKit & H3DF::CameraKit::UnsetTarget()
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	pcImpl->m_cData.bTargetFlag = false;

	return *this;
}

//== Show ===========================================================================================
bool H3DF::CameraKit::ShowUpVector(Vector & cOutUpVector) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bUpVectorFlag) {
		return false;
	}

	cOutUpVector = pcImpl->m_cData.cUpVector;
	return true;
}

bool H3DF::CameraKit::ShowPosition(Point & cOutPosition) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bPositionFlag) {
		return false;
	}

	cOutPosition = pcImpl->m_cData.cPosition;
	return true;
}

bool H3DF::CameraKit::ShowTarget(Point & cOutTarget) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bTargetFlag) {
		return false;
	}

	cOutTarget = pcImpl->m_cData.cTarget;
	return true;
}

bool H3DF::CameraKit::ShowProjection(Camera::Projection & eOutType) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bTargetFlag) {
		return false;
	}

	eOutType = pcImpl->m_cData.eType;
	return true;
}

bool H3DF::CameraKit::ShowProjection(Camera::Projection & eOutType, float & fOutOblique_Y_Skew, float & fOutOblique_X_Skew) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bTypeFlag) {
		return false;
	}

	eOutType = pcImpl->m_cData.eType;
	fOutOblique_Y_Skew = pcImpl->m_cData.fOblique_Y_Skew;
	fOutOblique_X_Skew = pcImpl->m_cData.fOblique_X_Skew;

	return true;
}

bool H3DF::CameraKit::ShowWidth(float & fOutWidth) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bWidthFlag) {
		return false;
	}

	fOutWidth = pcImpl->m_cData.fWidth;

	return true;
}

bool H3DF::CameraKit::ShowHeight(float & fOutHeight) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bHeightFlag) {
		return false;
	}

	fOutHeight = pcImpl->m_cData.fHeight;

	return true;
}

bool H3DF::CameraKit::ShowField(float & fOutWidth, float & fOutHeight) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bWidthFlag) {
		return false;
	}

	if (false == pcImpl->m_cData.bHeightFlag) {
		return false;
	}

	fOutWidth = pcImpl->m_cData.fWidth;
	fOutHeight = pcImpl->m_cData.fHeight;

	return true;
}

// NearLimit Show
bool H3DF::CameraKit::ShowNearLimit(float & fOutNearLimit) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->m_cData.bNearLimitFlag) {
		return false;
	}

	fOutNearLimit = pcImpl->m_cData.fNearLimit;

	return true;
}

// Show Matrix 생성
bool H3DF::CameraKit::ShowMatrix(MatrixKit & cMatrix) const
{
	auto pcImpl = static_cast<CameraKitImpl *>(m_pcImpl.get());

	// Matrix를 생성하기 위해서 필요한 값들이 존재하는지 여부를 확인한다.
	if (false == pcImpl->m_cData.bUpVectorFlag) {
		return false;
	}

	if (false == pcImpl->m_cData.bPositionFlag) {
		return false;
	}

	if (false == pcImpl->m_cData.bTargetFlag) {
		return false;
	}

	Point cPosition = pcImpl->m_cData.cPosition;
	Point cTarget = pcImpl->m_cData.cTarget;
	Vector cViewNormal = cPosition - cTarget;
	cViewNormal.Normalize();

	Vector cYAxis = pcImpl->m_cData.cUpVector;
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

//== CameraControl Class ============================================================================
H3DF::CameraControl::CameraControl(SegmentKey & cInSegment)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CameraControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::CameraControl::CameraControl(CameraControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

CameraControl & H3DF::CameraControl::operator = (CameraControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

CameraControl & H3DF::CameraControl::SetUpVector(Vector const & cInUp)
{
	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Up_Vector(cInUp.x, cInUp.y, cInUp.z);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetPosition(Point const & cInPosition)
{
	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Position(cInPosition.x, cInPosition.y, cInPosition.z);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetTarget(Point const & cInTarget)
{
	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Target(cInTarget.x, cInTarget.y, cInTarget.z);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetProjection(Camera::Projection eInType, float fInObliqueXSkew, float fInObliqueYSkew)
{
	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Field(fInWidth, fInHeight);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

CameraControl & H3DF::CameraControl::SetNearLimit(float fInLimit)
{
	auto pcImpl = static_cast<CameraControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Camera_Near_Limit(fInLimit);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}