#include "StdAfx.h"

#include "Segment.h"

#include "Shell.h"

#include "Impl/KeyImpl.h"

#include "Material.h"

#include <HUtility.h>

using namespace H3DF;

namespace H3DF
{
	class ShellKitImpl : public Impl
	{
	public:
		ShellKitImpl() { m_eType = H3DF::Type::ShellKit; }

		void Copy(ShellKitImpl * pcInThat)
		{
			m_pacPointArray = pcInThat->m_pacPointArray;
			m_pacNormalArray = pcInThat->m_pacNormalArray;
			m_panFacelistArray = pcInThat->m_panFacelistArray;
			m_panTristripsArray = pcInThat->m_panTristripsArray;
			m_paParameterArray = pcInThat->m_paParameterArray;
			m_paColorArray = pcInThat->m_paColorArray;

			m_nPointCount = pcInThat->m_nPointCount;
			m_pcPoints = pcInThat->m_pcPoints;

			m_nNormalCount = pcInThat->m_nNormalCount;

			m_cMaterialMappingKit = pcInThat->m_cMaterialMappingKit;
		}

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

		HShellWrapper * m_pcCreatedShell = nullptr;
	};
}

//== ShellKit Class ================================================================================
H3DF::ShellKit::ShellKit()
{
	ShellKitImpl * pcImpl = new ShellKitImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

H3DF::ShellKit::~ShellKit()
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (nullptr != pcImpl->m_pcCreatedShell) {
		delete pcImpl->m_pcCreatedShell;
		pcImpl->m_pcCreatedShell = nullptr;
	}
}

size_t H3DF::ShellKit::GetPointCount() const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (nullptr != pcImpl->m_pcCreatedShell) {
		return pcImpl->m_pcCreatedShell->m_plen;
	}

	if(nullptr == pcImpl->m_pacPointArray) {
		return -0;
	}

	return pcImpl->m_pacPointArray->size();
}
size_t H3DF::ShellKit::GetFacelistCount() const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(nullptr == pcImpl->m_panFacelistArray) {
		return -0;
	}

	return pcImpl->m_panFacelistArray->size();
}

size_t H3DF::ShellKit::GetTristripsCount() const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(nullptr == pcImpl->m_panTristripsArray) {
		return -0;
	}

	return pcImpl->m_panTristripsArray->size();
}

ShellKit & H3DF::ShellKit::SetPoints(int nInPointCount, Point const * pcInPoints)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nPointCount = nInPointCount;
	pcImpl->m_pcPoints = pcInPoints;
	return *this;
}

ShellKit & H3DF::ShellKit::SetPoints(int nInPointCount, PointArray const & acInPoints)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nPointCount = nInPointCount;
	pcImpl->m_pacPointArray = &acInPoints;
	return *this;
}

ShellKit & H3DF::ShellKit::SetPoints(PointArray const & acInPoints)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pacPointArray = &acInPoints;
	return *this;
}

ShellKit & H3DF::ShellKit::SetNormals(int nInNormalCount, VectorArray const & acInVectors)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nNormalCount = nInNormalCount;
	pcImpl->m_pacNormalArray = &acInVectors;
	return *this;
}

ShellKit & H3DF::ShellKit::SetNormals(VectorArray const & acInVectors)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pacNormalArray = &acInVectors;
	return *this;
}

ShellKit & H3DF::ShellKit::SetFacelist(IntArray const & acInFacelist)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_panFacelistArray = &acInFacelist;
	return *this;
}

ShellKit & H3DF::ShellKit::SetTristrips(IntArray const & acInTristrips)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_panTristripsArray = &acInTristrips;
	return *this;
}

ShellKit & H3DF::ShellKit::SetParameters(FloatArray const & aInParameters)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_paParameterArray = &aInParameters;
	return *this;
}

ShellKit & H3DF::ShellKit::SetColors(RGBAColorArray const & aInColors)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_paColorArray = &aInColors;
	return *this;
}

//== Show 관련 함수 ==================================================================================
bool H3DF::ShellKit::ShowPoints(int & nOutPointCount, Point const *& pcOutPoints) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	nOutPointCount = pcImpl->m_nPointCount;
	pcOutPoints = pcImpl->m_pcPoints;
	return true;
}

bool H3DF::ShellKit::ShowPoints(int & nOutPointCount, PointArray const *& acOutPoints) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (nullptr == pcImpl->m_pacPointArray) {
		return false;
	}

	nOutPointCount = pcImpl->m_nPointCount;
	acOutPoints = pcImpl->m_pacPointArray;
	return true;
}

bool H3DF::ShellKit::ShowPoints(PointArray const *& acOutPoints) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(nullptr == pcImpl->m_pacPointArray) {
		return false;
	}

	acOutPoints = pcImpl->m_pacPointArray;
	return true;
}

bool H3DF::ShellKit::ShowNormals(int & nOutNormalCount, VectorArray const *& acOutVectors) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (nullptr == pcImpl->m_pacNormalArray) {
		return false;
	}
	nOutNormalCount = pcImpl->m_nNormalCount;
	acOutVectors = pcImpl->m_pacNormalArray;
	return true;
}

bool H3DF::ShellKit::ShowNormals(VectorArray const *& acOutVectors) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);


	if (nullptr == pcImpl->m_pacNormalArray) {
		return false;
	}

	acOutVectors = pcImpl->m_pacNormalArray;
	return true;
}

bool H3DF::ShellKit::ShowFacelist(IntArray const *& acOutFacelist) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (nullptr == pcImpl->m_panFacelistArray) {
		return false;
	}

	acOutFacelist = pcImpl->m_panFacelistArray;
	return true;
}


bool H3DF::ShellKit::ShowTristrips(IntArray const *& acOutTristrips) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(nullptr == pcImpl->m_panTristripsArray) {
		return false;
	}

	acOutTristrips = pcImpl->m_panTristripsArray;
	return true;
}

bool H3DF::ShellKit::ShowParameters(FloatArray const *& aOutParameters) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (nullptr == pcImpl->m_paParameterArray) {
		return false;
	}

	aOutParameters = pcImpl->m_paParameterArray;
	return true;
}

bool H3DF::ShellKit::ShowColors(RGBAColorArray const *& aOutColors) const
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(nullptr == pcImpl->m_paColorArray) {
		return false;
	}

	aOutColors = pcImpl->m_paColorArray;
	return true;
}

ShellKit & H3DF::ShellKit::SetMaterialMapping(MaterialMappingKit const & cInkit)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cMaterialMappingKit = cInkit;
	return *this;
}

//== Shell 생성용 함수 ===============================================================================
bool H3DF::ShellKit::CreateShellWrapper()
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	pcImpl->m_pcCreatedShell = new HShellWrapper(INVALID_KEY);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_STOP;
		return false;
	}

	return true;
}

void H3DF::ShellKit::BeginAddFaces(int nFaceCount)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	pcImpl->m_pcCreatedShell->BeginAddFaces(nFaceCount);
}

void H3DF::ShellKit::Regenerate()
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	pcImpl->m_pcCreatedShell->Regenerate(INVALID_KEY, false, true);
}

void H3DF::ShellKit::EndAddFaces()
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	pcImpl->m_pcCreatedShell->EndAddFaces();
}

void H3DF::ShellKit::SetRegionFaces(int nRegionIndex, int nFaceFirstIndex, int nFaceCount)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	if (INVALID_KEY == pcImpl->m_pcCreatedShell->m_Key) {
		DEBUG_RETURN;
	}

	HC_MSet_Region_Faces(pcImpl->m_pcCreatedShell->m_Key, nRegionIndex, nFaceFirstIndex, nFaceCount);
}

void H3DF::ShellKit::SetRegionMaterial(int nInRegionIndex, H3DF::MaterialKit & cInMaterial)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	if (INVALID_KEY == pcImpl->m_pcCreatedShell->m_Key) {
		DEBUG_RETURN;
	}

	HC_Open_Geometry(pcImpl->m_pcCreatedShell->m_Key); {
		HC_Open_Region(nInRegionIndex); {
			cInMaterial.SetMaterial("faces");
		} HC_Close_Region();
	} HC_Close_Geometry();
}

void H3DF::ShellKit::DeleteShellWrapperKey()
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	if (INVALID_KEY == pcImpl->m_pcCreatedShell->m_Key) {
		DEBUG_RETURN;
	}

	HC_Delete_By_Key(pcImpl->m_pcCreatedShell->m_Key);
}

void H3DF::ShellKit::AddNextFaceWithDistinctNormals(Point const * pcInPoints, Vector const * pcInNormals, int const * pnFaceIndex, int const * pnNormalIndex, int nFaceCount)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	pcImpl->m_pcCreatedShell->AddNextFaceWithDistinctNormals((HPoint *)pcInPoints, (HPoint *)pcInNormals, pnFaceIndex, pnNormalIndex, nFaceCount);
}

void H3DF::ShellKit::AddNextFaceWithDistinctNormalsAndTexture(Point const * pcInPoints, Vector const * pcInNormals, Point const * pcInTextureCoord, int const * pnFaceIndex,
	int const * pnNormalIndex, int const * pnTextureIndex, int nFaceCount)
{
	ShellKitImpl * pcImpl = (ShellKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	if (nullptr == pcImpl->m_pcCreatedShell) {
		DEBUG_RETURN;
	}

	pcImpl->m_pcCreatedShell->AddNextFaceWithDistinctNormalsAndTexture((HPoint *)pcInPoints, (HPoint *)pcInNormals, (HPoint *)pcInTextureCoord, pnFaceIndex, pnNormalIndex, pnTextureIndex, nFaceCount);
}

//== ShellKey Class ================================================================================
namespace H3DF {
	class ShellKeyImpl : public KeyImpl
	{
	public:
		ShellKeyImpl() { m_eType = H3DF::Type::ShellKey; }

		void Copy(ShellKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
		}
	};
};

H3DF::ShellKey::ShellKey() : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new ShellKeyImpl();
}

H3DF::ShellKey::ShellKey(HC_KEY nInKey) : GeometryKey(INVALID_KEY)
{
	ShellKeyImpl * pcImpl = new ShellKeyImpl();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

H3DF::ShellKey::ShellKey(Key const & cInKey) : GeometryKey(INVALID_KEY)
{
	ShellKeyImpl * pcImpl = new ShellKeyImpl();
	m_pcImpl = pcImpl;

	if (H3DF::Type::ShellKey != cInKey.ObjectType()) {
		DEBUG_STOP;
		return;
	}

	((KeyImpl *)pcImpl)->Copy((KeyImpl *)(cInKey.GetImpl()));
}

H3DF::ShellKey::ShellKey(ShellKey const & cInThat)
{
	m_pcImpl = new ShellKeyImpl();
	Set(cInThat);
}

void H3DF::ShellKey::Set(ShellKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	ShellKeyImpl * pcImpl = (ShellKeyImpl *)m_pcImpl;
	ShellKeyImpl * pcInThatImpl = (ShellKeyImpl *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

ShellKey & H3DF::ShellKey::operator = (ShellKey const & cInThat)
{
	Set(cInThat);
	return *this;
}
