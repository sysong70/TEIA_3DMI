#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Math.h"
#include "Math.Matrix.h"

OPEN_3DF_NAMESPACE

class API_3DF Camera
{
public:
	enum class Projection : uint32_t
	{
		Default = 1,
		Perspective,
		Orthographic,
		Stretched,
	};

private:
	Camera() {}
};

class API_3DF CameraKit : public Kit
{
public:
	CameraKit();
	CameraKit(CameraKit const & cInThat);

	void Set(CameraKit const & cInThat);
	CameraKit const & operator=(CameraKit const & cInThat);

	H3DF::Type ObjectType() const { return H3DF::Type::CameraKit; };

	CameraKit & SetUpVector(Vector const & cInUpVector);
	CameraKit & SetPosition(Point const & cInPosition);
 	CameraKit & SetTarget(Point const & cInTarget);
	CameraKit & SetProjection(Camera::Projection eInType, float fInOblique_Y_Skew = 0.0f, float fInOblique_X_Skew = 0.0f);
	CameraKit & SetField(float fInWidth, float fInHeight);
	CameraKit & SetNearLimit(float const fInLimit);

	CameraKit & UnsetUpVector();
	CameraKit & UnsetPosition();
	CameraKit & UnsetTarget();

	bool ShowUpVector(Vector & cOutUpVector) const;
	bool ShowPosition(Point & cOutPosition) const;
	bool ShowTarget(Point & cOutTarget) const;
	bool ShowProjection(Camera::Projection & eOutType) const;
	bool ShowProjection(Camera::Projection & eOutType, float & fOutOblique_Y_Skew, float & fOutOblique_X_Skew) const;
	bool ShowWidth(float & fOutWidth) const;
	bool ShowHeight(float & fOutHeight) const;
	bool ShowField(float & fOutWidth, float & fOutHeight) const;
	bool ShowNearLimit(float & fOutNearLimit) const;
	bool ShowMatrix(MatrixKit & cMatrix) const;
};

CLOSE_3DF_NAMESPACE