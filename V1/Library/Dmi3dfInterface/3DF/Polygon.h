#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Geometry.h"

#include "Math.h"

#include "Color.h"


#include <atlcoll.h>

OPEN_3DF_NAMESPACE

class API_3DF PolygonKit : public Kit
{
public:
	PolygonKit();
	PolygonKit(PolygonKit const & cInThat);

	static const H3DF::Type staticType = H3DF::Type::PolygonKit;
	H3DF::Type ObjectType() const { return staticType; };

	PolygonKit const & operator = (PolygonKit const & cInThat);

	unsigned int GetPointCount() const;
	void GetPoints(unsigned int & nOutCount, H3DF::Point * pcOutPoints) const;
	// Replace the points on this PolygonKey with the specified points.
	PolygonKit & SetPoints(size_t nInCount, H3DF::Point const cInPoints[]);

	void GetRGBColor(H3DF::RGBColor & cOutColor) const;
	void SetRGBColor(H3DF::RGBColor const & cInColor);
};

class API_3DF PolygonKey : public GeometryKey
{
public:
	PolygonKey();
	PolygonKey(HC_KEY nInKey);
	
	static const H3DF::Type staticType = H3DF::Type::PolygonKey;
	H3DF::Type ObjectType() const { return staticType; };
};

CLOSE_3DF_NAMESPACE