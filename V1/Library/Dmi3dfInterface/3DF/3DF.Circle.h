#pragma once

#include "3DF.h"

#include "3DF.Kit.h"
#include "3DF.Geometry.h"

#include "3DF.Color.h"

#include <atlcoll.h>

OPEN_3DF_NAMESPACE

class API_3DF CircleKit : public Kit
{
public:
	CircleKit();
	CircleKit(CircleKit const & cInThat);

	void Set(CircleKit const & cInThat);
	CircleKit const & operator=(CircleKit const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::CircleKit; };

	CircleKit & SetCenter(Point const & cInCenter);
	CircleKit & SetRadius(double dInRadius);
	CircleKit & SetNormal(Vector const & cInNormal);

	bool ShowCenter(Point & cOutCenter) const;
	bool ShowRadius(float & cOutRadius) const;
	bool ShowNormal(Vector & cOutNormal) const;
};

class API_3DF CircleKey : public GeometryKey
{
public:
	CircleKey();
	explicit CircleKey(Key const & cInKey);
	CircleKey(CircleKey const & cInThat);

	void Set(CircleKey const & cInThat);
	CircleKey & operator=(CircleKey const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::CircleKey; };

	CircleKey & SetCenter(Point const & cInCenter);
	CircleKey & SetRadius(double dInRadius);
	CircleKey & SetNormal(Vector const & cInNormal);
};

CLOSE_3DF_NAMESPACE