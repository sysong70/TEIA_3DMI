#pragma once

#include "3DF.h"

#include "3DF.Kit.h"
#include "3DF.Math.h"

OPEN_3DF_NAMESPACE

class API_3DF CameraKit : public Kit
{
public:
	CameraKit();
	CameraKit(CameraKit const & cInThat);

	void Set(CameraKit const & cInThat);
	CameraKit const & operator=(CameraKit const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::CameraKit; };

	CameraKit & SetUpVector(Vector const & cInUpVector);
	CameraKit & SetPosition(Point const & cInPosition);
 	CameraKit & SetTarget(Point const & cInTarget);
// 	CameraKit & SetProjection(Camera::Projection in_type, float in_oblique_y_skew = 0.0f, float in_oblique_x_skew = 0.0f);
// 	CameraKit & SetField(float in_width, float in_height);
// 	CameraKit & SetNearLimit(float const in_limit);

	CameraKit & UnsetUpVector();
	CameraKit & UnsetPosition();
	CameraKit & UnsetTarget();

	bool ShowUpVector(Vector & cOutUpVector) const;
	bool ShowPosition(Point & cOutPosition) const;
	bool ShowTarget(Point & cOutTarget) const;
/*
	bool ShowProjection(Camera::Projection & out_type) const;
	bool ShowProjection(Camera::Projection & out_type, float & out_oblique_y_skew, float & out_oblique_x_skew) const;
	bool ShowWidth(float & out_width) const;
	bool ShowHeight(float & out_height) const;
	bool ShowField(float & out_width, float & out_height) const;
	bool ShowNearLimit(float & out_near_limit) const;
*/

};

CLOSE_3DF_NAMESPACE