#pragma once

#include "3DF.Geometry.h"

#include "3DF.Math.h"

#include "3DF.MaterialMapping.h"

OPEN_3DF_NAMESPACE

class API_3DF ShellKit
{
public:
	ShellKit() {};
	virtual ~ShellKit() {};

	size_t GetPointCount() const;
	size_t GetFacelistCount() const;
	size_t GetTristripsCount() const;

	ShellKit & SetPoints(int nInPointCount, Point const * pcInPoints);
	ShellKit & SetPoints(int nInPointCount, PointArray const & acInPoints);
	ShellKit & SetPoints(PointArray const & acInPoints);

	ShellKit & SetNormals(int nInNormalCount, VectorArray const & acInVectors);
	ShellKit & SetNormals(VectorArray const & acInVectors);

	ShellKit & SetFacelist(IntArray const & acInFacelist);
	ShellKit & SetTristrips(IntArray const & acInTristrips);
	ShellKit & SetParameters(FloatArray const & aInParameters);
	ShellKit & SetColors(RGBAColorArray const & aInColors);

	bool ShowPoints(int & nOutPointCount, Point const *& pcOutPoints) const;
	bool ShowPoints(int & nOutPointCount, PointArray const *& acOutPoints) const;
	bool ShowPoints(PointArray const *& acOutPoints) const;

	bool ShowNormals(int & nOutNormalCount, VectorArray const *& acOutVectors) const;
	bool ShowNormals(VectorArray const *& acOutVectors) const;

	bool ShowFacelist(IntArray const *& acOutFacelist) const;
	bool ShowTristrips(IntArray const *& acOutTristrips) const;
	bool ShowParameters(FloatArray const *& aOutParameters) const;
	bool ShowColors(RGBAColorArray const *& aOutColors) const;

	ShellKit & SetMaterialMapping(MaterialMappingKit const & cInkit);

private:
	PointArray const * m_pacPointArray = nullptr;
	VectorArray const * m_pacNormalArray = nullptr;
	IntArray const * m_panFacelistArray = nullptr;
	IntArray const * m_panTristripsArray = nullptr;
	FloatArray const * m_paParameterArray = nullptr;
	RGBAColorArray const * m_paColorArray = nullptr;

	int m_nPointCount = 0;
	Point const * m_pcPoints = nullptr;

	int m_nNormalCount = 0;

	MaterialMappingKit m_cMaterialMappingKit;
};

class API_3DF ShellKey : public GeometryKey
{
public:
	ShellKey(HC_KEY nInKey = INVALID_KEY);
	ShellKey(ShellKey const & cInThat);

	ShellKey & operator = (ShellKey const & cInThat);
};

CLOSE_3DF_NAMESPACE