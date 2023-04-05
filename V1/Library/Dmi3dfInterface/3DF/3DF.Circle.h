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
};

CLOSE_3DF_NAMESPACE