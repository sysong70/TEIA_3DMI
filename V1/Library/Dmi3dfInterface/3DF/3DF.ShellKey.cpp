#include "StdAfx.h"

#include "3DF.ShellKey.h"

#include "3DF.MaterialMappingControl.h"

USING_3DF_NAMESPACE

//== ShellKit Class ================================================================================

size_t ShellKit::GetPointCount() const
{
	if(nullptr == m_pacPointArray) {
		return -0;
	}

	return m_pacPointArray->size();
}
size_t ShellKit::GetFacelistCount() const
{
	if(nullptr == m_panFacelistArray) {
		return -0;
	}

	return m_panFacelistArray->size();
}

size_t ShellKit::GetTristripsCount() const
{
	if(nullptr == m_panTristripsArray) {
		return -0;
	}

	return m_panTristripsArray->size();
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

bool ShellKit::ShowPoints(PointArray & acOutPoints) const
{
	if(nullptr == m_pacPointArray) {
		return false;
	}

	acOutPoints = *m_pacPointArray;
	return true;
}

bool ShellKit::ShowNormals(VectorArray & acOutVectors) const
{
	if(nullptr == m_pacNormalArray) {
		return false;
	}

	acOutVectors = *m_pacNormalArray;
	return true;
}

bool ShellKit::ShowFacelist(IntArray & acOutFacelist) const
{
	if(nullptr == m_panFacelistArray) {
		return false;
	}

	acOutFacelist = *m_panFacelistArray;
	return true;
}

bool ShellKit::ShowTristrips(IntArray & acOutTristrips) const
{
	if(nullptr == m_panTristripsArray) {
		return false;
	}

	acOutTristrips = *m_panTristripsArray;
	return true;
}

ShellKit & ShellKit::SetMaterialMapping(MaterialMappingKit const & cInkit)
{
	m_cMaterialMappingKit = cInkit;
	return *this;
}

/*
ShellKit & ShellKit::operator = (ShellKit const & cOther)
{
	m_cMaterialMappingKit = cOther.GetM
	m_nKey = cOther.KeyValue();
	return *this;
}
*/

//== ShellKey Class ================================================================================

ShellKey::ShellKey(HC_KEY nKey)
{
	m_nKey = nKey;
}

ShellKey::ShellKey(ShellKey const & cInThat)
{
	m_nKey = cInThat.KeyValue();
}

ShellKey & ShellKey::operator = (ShellKey const & cOther)
{
	m_nKey = cOther.KeyValue();
	return *this;
}
