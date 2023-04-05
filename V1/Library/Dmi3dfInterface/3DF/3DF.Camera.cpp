#include "StdAfx.h"

#include "3DF.Camera.h"

#include "3DF.Math.h"
#include "3DF.Point.h"

#include "Private/3DF.KeyPrivate.h"

#include <HTools.h>

USING_3DF_NAMESPACE

class CameraKitPrivate : public PrivateImpl
{
public:
	void Copy(CameraKitPrivate * pcInThat)
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
};

CameraKit::CameraKit()
{
	m_pcImpl = new CameraKitPrivate();
}

CameraKit::CameraKit(CameraKit const & cInThat)
{
	m_pcImpl = new CameraKitPrivate();
	Set(cInThat);
}

void CameraKit::Set(CameraKit const & cInThat)
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	CameraKitPrivate * pcInThatImpl = (CameraKitPrivate *)cInThat.m_pcImpl;
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
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	pcImpl->cUpVector = cInUpVector;
	pcImpl->bUpVectorFlag = true;

	return *this;
}

CameraKit & CameraKit::SetPosition(Point const & cInPosition)
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	pcImpl->cPosition = cInPosition;
	pcImpl->bPositionFlag = true;

	return *this;
}

CameraKit & CameraKit::SetTarget(Point const & cInTarget)
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	pcImpl->cTarget = cInTarget;
	pcImpl->bTargetFlag = true;

	return *this;
}

//== Unset =========================================================================================
CameraKit & CameraKit::UnsetUpVector()
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	pcImpl->bUpVectorFlag = false;

	return *this;
}

CameraKit & CameraKit::UnsetPosition()
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	pcImpl->bPositionFlag = false;

	return *this;
}

CameraKit & CameraKit::UnsetTarget()
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	pcImpl->bTargetFlag = false;

	return *this;
}

//== Show ===========================================================================================
bool CameraKit::ShowUpVector(Vector & cOutUpVector) const
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	if (false == pcImpl->bUpVectorFlag) {
		return false;
	}

	cOutUpVector = pcImpl->cUpVector;
	return true;
}

bool CameraKit::ShowPosition(Point & cOutPosition) const
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	if (false == pcImpl->bPositionFlag) {
		return false;
	}

	cOutPosition = pcImpl->cPosition;
	return true;
}

bool CameraKit::ShowTarget(Point & cOutTarget) const
{
	CameraKitPrivate * pcImpl = (CameraKitPrivate *)m_pcImpl;
	if (false == pcImpl->bTargetFlag) {
		return false;
	}

	cOutTarget = pcImpl->cTarget;
	return true;
}

/*
bool CameraKit::ShowProjection(Camera::Projection & out_type) const
{

}

bool CameraKit::ShowProjection(Camera::Projection & out_type, float & out_oblique_y_skew, float & out_oblique_x_skew) const
{

}

bool CameraKit::ShowWidth(float & out_width) const
{

}

bool CameraKit::ShowHeight(float & out_height) const
{

}

bool CameraKit::ShowField(float & out_width, float & out_height) const
{

}

bool CameraKit::ShowNearLimit(float & out_near_limit) const
{

}
*/
