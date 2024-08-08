#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

#include "Math.h"
#include "Math.Matrix.h"

namespace H3DF
{
	class API_3DF CameraKit : public Kit
	{
	public:
		CameraKit();
		CameraKit(CameraKit const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::CameraKit;
		H3DF::Type ObjectType() const override { return staticType; };

		void Set(CameraKit const & cInThat);
		CameraKit const & operator=(CameraKit const & cInThat);

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

	class API_3DF CameraControl : public Control {
	public:
		explicit CameraControl(SegmentKey & cInSegment);
		CameraControl(CameraControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::CameraControl;
		H3DF::Type ObjectType() const override { return staticType; };

		void Set(CameraControl const & cInThat);
		CameraControl & operator = (CameraControl const & cInThat);

		CameraControl & SetUpVector(Vector const & cInUp);
		CameraControl & SetPosition(Point const & cInPosition);
		CameraControl & SetTarget(Point const & cInTarget);
		CameraControl & SetProjection(Camera::Projection eInType, float fInObliqueXSkew = 0.0f, float fInObliqueYSkew = 0.0f);
		CameraControl & SetField(float fInWidth, float fInHeight);
		CameraControl & SetNearLimit(float fInLimit);

/*
		CameraControl & UnsetEverything();

		bool ShowUpVector(Vector & cOutUpVector) const;
		bool ShowPosition(Point & cOutPosition) const;
		bool ShowTarget(Point & cOutTarget) const;
		bool ShowProjection(Camera::Projection & eOutType) const;
		bool ShowProjection(Camera::Projection & eOutType, float & fOutObliqueXSkew, float & fOutObliqueYSkew) const;
		bool ShowWidth(float & fOutWidth) const;
		bool ShowHeight(float & fOutHeight) const;
		bool ShowField(float & fOutWidth, float & fOutHeight) const;
		bool ShowNearLimit(float & fOutNearLimit) const;
*/

	private:
		CameraControl();
	};

}