#include "StdAfx.h"

#include "Shell.h"

#include "Impl/KeyImpl.h"

#include "Material.h"

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

ShellKit & ShellKit::SetPoints(int nInPointCount, Point const * pcInPoints)
{
	m_nPointCount = nInPointCount;
	m_pcPoints = pcInPoints;
	return *this;
}

ShellKit & ShellKit::SetPoints(int nInPointCount, PointArray const & acInPoints)
{
	m_nPointCount = nInPointCount;
	m_pacPointArray = &acInPoints;
	return *this;
}

ShellKit & ShellKit::SetPoints(PointArray const & acInPoints)
{
	m_pacPointArray = &acInPoints;
	return *this;
}

ShellKit & ShellKit::SetNormals(int nInNormalCount, VectorArray const & acInVectors)
{
	m_nNormalCount = nInNormalCount;
	m_pacNormalArray = &acInVectors;
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

//== Show 관련 함수 ==================================================================================
bool ShellKit::ShowPoints(int & nOutPointCount, Point const *& pcOutPoints) const
{
	nOutPointCount = m_nPointCount;
	pcOutPoints = m_pcPoints;
	return true;
}

bool ShellKit::ShowPoints(int & nOutPointCount, PointArray const *& acOutPoints) const
{
	if (nullptr == m_pacPointArray) {
		return false;
	}

	nOutPointCount = m_nPointCount;
	acOutPoints = m_pacPointArray;
	return true;
}

bool ShellKit::ShowPoints(PointArray const *& acOutPoints) const
{
	if(nullptr == m_pacPointArray) {
		return false;
	}

	acOutPoints = m_pacPointArray;
	return true;
}

bool ShellKit::ShowNormals(int & nOutNormalCount, VectorArray const *& acOutVectors) const
{
	if (nullptr == m_pacNormalArray) {
		return false;
	}
	nOutNormalCount = m_nNormalCount;
	acOutVectors = m_pacNormalArray;
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
namespace H3DF {
	class ShellKeyImpl : public KeyImpl
	{
	public:
		ShellKeyImpl() {m_eType = H3DF::Type::ShellKey; }

		void Copy(ShellKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
		}
	};
};

ShellKey::ShellKey() : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new ShellKeyImpl();
}

ShellKey::ShellKey(HC_KEY nInKey) : GeometryKey(INVALID_KEY)
{
	ShellKeyImpl * pcImpl = new ShellKeyImpl();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

ShellKey::ShellKey(Key const & cInKey) : GeometryKey(INVALID_KEY)
{
	ShellKeyImpl * pcImpl = new ShellKeyImpl();
	m_pcImpl = pcImpl;

	((KeyImpl *)pcImpl)->Copy((KeyImpl *)(cInKey.GetImpl()));

	// 외부에서 들어오는 Key는 ShellKey가 아닐 수 있으므로, ShellKey로 변경한다.
	pcImpl->SetType(H3DF::Type::ShellKey);
}

ShellKey::ShellKey(ShellKey const & cInThat)
{
	m_pcImpl = new ShellKeyImpl();
	Set(cInThat);
}

void ShellKey::Set(ShellKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		return;
	}

	ShellKeyImpl * pcImpl = (ShellKeyImpl *)m_pcImpl;
	ShellKeyImpl * pcInThatImpl = (ShellKeyImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ShellKey & ShellKey::operator = (ShellKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}
