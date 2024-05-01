#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Geometry.h"

#include "Color.h"

#include <atlcoll.h>

namespace H3DF
{
	class API_3DF CircleKit : public Kit
	{
	public:
		CircleKit();
		CircleKit(CircleKit const & cInThat);

		void Set(CircleKit const & cInThat);
		CircleKit const & operator=(CircleKit const & cInThat);

		CircleKit & SetCenter(Point const & cInCenter);
		CircleKit & SetRadius(double dInRadius);
		CircleKit & SetXAxis(Vector const & cInAxis);
		CircleKit & SetYAxis(Vector const & cInAxis);

		bool ShowCenter(Point & cOutCenter) const;
		bool ShowRadius(float & cOutRadius) const;
		bool ShowXAxis(Vector & cOutAxis) const;
		bool ShowYAxis(Vector & cOutAxis) const;
		bool ShowNormal(Vector & cOutNormal) const;

		bool ShowPoint(float fInAngle, Point & cOutPoint) const;
	};

	class API_3DF CircleKey : public GeometryKey
	{
	public:
		CircleKey();
		CircleKey(HC_KEY nInKey);
		explicit CircleKey(Key const & cInKey);
		CircleKey(CircleKey const & cInThat);

		void Set(CircleKey const & cInThat);
		CircleKey & operator=(CircleKey const & cInThat);

		CircleKey & SetCenter(Point const & cInCenter);
		CircleKey & SetRadius(double dInRadius);
		CircleKey & SetXAxis(Vector const & cInAxis);
		CircleKey & SetYAxis(Vector const & cInAxis);

		bool ShowCenter(Point & cOutCenter) const;
		bool ShowRadius(float & cOutRadius) const;
		bool ShowXAxis(Vector & cOutAxis) const;
		bool ShowYAxis(Vector & cOutAxis) const;
		bool ShowNormal(Vector & cOutNormal) const;
	};
}