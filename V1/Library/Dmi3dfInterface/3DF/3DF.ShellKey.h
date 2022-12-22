#pragma once

#include "3DF.Key.h"

#include "3DF.Math.h"

#include "3DF.MaterialMappingControl.h"

OPEN_3DF_NAMESPACE

class API_3DF ShellKit
{
public:
	ShellKit() {};
	virtual ~ShellKit() {};

	size_t GetPointCount() const;
	size_t GetFacelistCount() const;
	size_t GetTristripsCount() const;

	ShellKit & SetPoints(PointArray const & acInPoints);
	ShellKit & SetNormals(VectorArray const & acInVectors);
	ShellKit & SetFacelist(IntArray const & acInFacelist);
	ShellKit & SetTristrips(IntArray const & acInTristrips);

	bool ShowPoints(PointArray & acOutPoints) const;
	bool ShowNormals(VectorArray & acOutVectors) const;
	bool ShowFacelist(IntArray & acOutFacelist) const;
	bool ShowTristrips(IntArray & acOutTristrips) const;

	ShellKit & SetMaterialMapping(MaterialMappingKit const & cInkit);

	//ShellKit & operator = (ShellKit const & cOther);

private:
	PointArray const * m_pacPointArray = nullptr;
	VectorArray const * m_pacNormalArray = nullptr;
	IntArray const * m_panFacelistArray = nullptr;
	IntArray const * m_panTristripsArray = nullptr;

	MaterialMappingKit m_cMaterialMappingKit;
};

class API_3DF ShellKey : public Key
{
public:
	ShellKey() {};
	ShellKey(HC_KEY nKey);
	ShellKey(ShellKey const & cInThat);

	ShellKey & operator = (ShellKey const & cOther);
};

CLOSE_3DF_NAMESPACE