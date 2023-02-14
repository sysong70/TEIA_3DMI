#include "StdAfx.h"

#include "3DF.Shell.h"

#include "3DF.MaterialMapping.h"

USING_3DF_NAMESPACE

//== ShellKit Class ================================================================================

size_t ShellKit::GetPointCount() const
{
	if(nullptr == m_pacPointArray) {
		return -0;
	}

	return m_pacPointArray->GetCount();
}
size_t ShellKit::GetFacelistCount() const
{
	if(nullptr == m_panFacelistArray) {
		return -0;
	}

	return m_panFacelistArray->GetCount();
}

size_t ShellKit::GetTristripsCount() const
{
	if(nullptr == m_panTristripsArray) {
		return -0;
	}

	return m_panTristripsArray->GetCount();
}

ShellKit & ShellKit::SetPoints(PointArray const & acInPoints)
{
	m_pacPointArray = &acInPoints;
	return *this;
}
ShellKit & ShellKit::SetNormals(VectorArray const & acInVectors)
{
	m_pacNormalArray = &acInVectors;
	return *this;
}

ShellKit & ShellKit::SetFacelist(IntArray const & acInFacelist)
{
	m_panFacelistArray = &acInFacelist;
	return *this;
}

ShellKit & ShellKit::SetTristrips(IntArray const & acInTristrips)
{
	m_panTristripsArray = &acInTristrips;
	return *this;
}

ShellKit & ShellKit::SetParameters(FloatArray const & aInParameters)
{
	m_paParameterArray = &aInParameters;
	return *this;
}

ShellKit & ShellKit::SetColors(RGBAColorArray const & aInColors)
{
	m_paColorArray = &aInColors;
	return *this;
}

bool ShellKit::ShowPoints(PointArray const *& acOutPoints) const
{
	if(nullptr == m_pacPointArray) {
		return false;
	}

	acOutPoints = m_pacPointArray;
	return true;
}

bool ShellKit::ShowNormals(VectorArray const *& acOutVectors) const
{
	if(nullptr == m_pacNormalArray) {
		return false;
	}

	acOutVectors = m_pacNormalArray;
	return true;
}

bool ShellKit::ShowFacelist(IntArray const *& acOutFacelist) const
{
	if(nullptr == m_panFacelistArray) {
		return false;
	}

	acOutFacelist = m_panFacelistArray;
	return true;
}


bool ShellKit::ShowTristrips(IntArray const *& acOutTristrips) const
{
	if(nullptr == m_panTristripsArray) {
		return false;
	}

	acOutTristrips = m_panTristripsArray;
	return true;
}

bool ShellKit::ShowParameters(FloatArray const *& aOutParameters) const
{
	if(nullptr == m_paParameterArray) {
		return false;
	}

	aOutParameters = m_paParameterArray;
	return true;
}

bool ShellKit::ShowColors(RGBAColorArray const *& aOutColors) const
{
	if(nullptr == m_paColorArray) {
		return false;
	}

	aOutColors = m_paColorArray;
	return true;
}

ShellKit & ShellKit::SetMaterialMapping(MaterialMappingKit const & cInkit)
{
	m_cMaterialMappingKit = cInkit;
	return *this;
}


//== ShellKey Class ================================================================================

ShellKey::ShellKey(HC_KEY nInKey) :
	GeometryKey(nInKey)
{
}

ShellKey::ShellKey(ShellKey const & cInThat) :
	GeometryKey(cInThat)
{
}

ShellKey & ShellKey::operator = (ShellKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}
