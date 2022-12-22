#pragma once

#include "Json.h"

#include "3DX.h"
#include "3DXSignal.h"

#include "3DX.ImportBase.h"

// ----- C3D Header -----
#include <templ_array2.h> // 앞쪽에 추가되어야 함.
#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <assembly.h>

#include <topology.h>
#include <topology_faceset.h>
#include <curve.h>
#include <curve3d.h>

#include <mesh_triangle.h>
// ----------------------

#include <unordered_map>
#include <list>
#include <atlcoll.h>

#include <chrono>

#include "Convert/ConvObject.h"
#include "ThreadPool.h"

#include "A3dTracer.h"

class MbModel;
class MbSolid;
class MbFaceShell;
class MbFace;
class MbLoop;
class MbOrientedEdge;
class MbCurveEdge;

class DmiOrientedEdge;

class MbSurface;
class MbCurve;
class MbVertex;
class MbCartPoint;
class MbRect;
class MbVector;

class ConvLoop;
class ConvCoEdge;
class ConvVertex;
class ConvSurface;

using CurveEdgeMap = CAtlMap<DWORD_PTR, c3d::EdgeSPtr>;
using MeshIndexMap = CAtlMap<A3DUns32, A3DUns32>;
using MeshGridLoopVector = std::vector<MbGridLoop>;

OPEN_3DX_NAMESPACE

class C3dImport : public ImportBase
{
public:
	C3dImport(_3DXSignal::Interface * pc3dxInterface);
	~C3dImport();

	// == 명령어 처리 관련 함수 =======================================================================
public:
	bool ExecuteCommand(Json::Object * pcObject);
	bool SendExecuteCommand(Json::Object & pcObject);

private:
	DWORD_PTR m_nId = 0xffffff;
	_3DXSignal::Interface * m_pc3dxInterface = nullptr;

	// == File C3dImport 관련 함수 =====================================================================
protected:
	bool FileImport(CString strFilePathName);
	bool IncrementalFileImport(CString strFilePathName);

	// == 3DX 설정 관련 함수 =========================================================================
protected:
	bool SetDefaultParamsLoadData(A3DRWParamsLoadData & cParamsLoadData);

	// == C3D 변환 관련 함수 =========================================================================
protected:
	bool ConvertAsmModelFile(A3DAsmModelFile * pcAsmModelFile);

	// == Product Occurrences 관련 함수 ==============================================================
	bool ConvertModelFileData(A3DAsmModelFileData & cModelFileData, A3DMiscCascadedAttributes * pcPO);
	bool ConvertProductOccurrence(A3DAsmProductOccurrence * pcProductOccurrence, A3DMiscCascadedAttributes * pcPO,
		c3d::AssemblySPtr & pcParentAssy);

	// Assembly Product Occurence의 파일 경로를 가져옴.
	bool GetProductOccurrenceFilePath(A3DAsmProductOccurrence * pPO, CString & strFilePath, CString & strOriginalFilePath);

	// Layer Filter를 가져오는 함수
	bool GetProductOccurrenceLayerFilter(const A3DAsmProductOccurrenceData & cProductOccurrenceData);

	// Assembly Product Occurence의 위치를 가져오는 함수
	bool GetProductOccurrenceTransformMatrix(A3DAsmProductOccurrenceData const * pcPoData, MbMatrix3D & cTransMatrix);

	bool GetProductOccurrenceExternalData(A3DAsmProductOccurrenceData const * pcPoData, A3DAsmProductOccurrence *& pcExternalData);

	bool GetProductOccurrentPMI(const A3DAsmProductOccurrenceData & sPOccData, A3DMiscCascadedAttributes * pcAttribute);

	bool GetProductOccurrenceChildren(const A3DAsmProductOccurrenceData * pcPoData, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy);

	// == Part 관련 함수 =============================================================================
private:
	double m_dContextScale = 1.0;

protected:
	bool GetProductOccurrencePart(A3DAsmProductOccurrenceData const * pcPoData, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy);

	bool GetPrototypePartRecursive(A3DAsmProductOccurrence * pcPrototypeProductOccurrence, A3DMiscCascadedAttributes * pcAttribute,
		c3d::AssemblySPtr & pcParentAssy, bool bUsedExternalData);

	bool IsPartVisible(A3DAsmPartDefinition * pcPartDefinition, A3DMiscCascadedAttributes * pcPO);

	bool ConvertPart(A3DAsmPartDefinition * pcPartDefinition, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy);

	bool ConvertRiRepresentationItem(int nIndex, A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy);

	void ConvertRiCurve(A3DRiCurve * pcInputRiCurve, A3DMiscCascadedAttributes * pcParentAttr, c3d::AssemblySPtr & pcParentAssy);

	void ConvertRiSet(A3DRiSet * pcInputRiSet, A3DMiscCascadedAttributes * pcParentAttr, c3d::AssemblySPtr & pcParentAssy);

	bool ConvertRiBrepModel(A3DRiRepresentationItem * pcInputRiBrepModel, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy, bool bCopyModelFlag);

	bool ConvertTessBase(A3DRiRepresentationItemData & cRiRepItemData, A3DRiRepresentationItem * pcInputRiBrepModel, A3DMiscCascadedAttributes * pcParentAttr,
		MbMesh *& pcMesh);

	bool ConvertTopoContextScale(const A3DTopoBody * pcBody, double & dTopoContextScale);

	bool ConvertTopoConnex(int nIndex, A3DTopoConnex * pcTopoConnex, A3DMiscCascadedAttributes * pcParentAttr, c3d::SolidsSPtrVector & vpSolidVector);
	bool ConvertTopoConnex(A3DTopoConnex * pcTopoConnex, A3DMiscCascadedAttributes * pcParentAttr, c3d::ShellSPtr & pcFaceShell);
	bool ConvertTopoConnex(A3DTopoConnex * pcTopoConnex, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy, CString & strName, MbMatrix3D & cTransMatrix);

	bool ThreadConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr, c3d::ShellSPtr & pcFaceShell);
	bool ConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr, c3d::ShellSPtr & pcFaceShell);

	bool ConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr, c3d::SolidsSPtrVector & vpSolidVector);
	//bool ConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr, 
	// c3d::AssemblySPtr & pcParentAssy, CString & strName, MbMatrix3D & cTransMatrix);

	bool ConvertMbFace(A3DUns32 nIndex, A3DTopoFace * pcTopoFace, bool bOrientationWithShell, A3DMiscCascadedAttributes * pcParentAttr,
		A3DMiscCascadedAttributesData & cParentAttributesData, COLORREF cParentColor, bool bParentColorDefinedFlag,
		float fParentTransparency, bool bParentTransparencyDefineFlag, c3d::FaceSPtr & pcFace);

	bool ConvertMbFace_V1(A3DUns32 nIndex, A3DTopoFace * pcTopoFace, bool bOrientationWithShell, A3DMiscCascadedAttributes * pcParentAttr,
		A3DMiscCascadedAttributesData & cParentAttributesData, c3d::FaceSPtr & pcFace);

	bool CreateBoundedSurface(c3d::SurfaceSPtr & pcSurface, c3d::LoopSPtr & pcLoop, bool bOrientationWithShell, bool bCheckBoundFlag, c3d::SurfaceSPtr & cBoundedSurface);
	bool CreateBoundedSurface(c3d::SurfaceSPtr & pcSurface, const A3DDomainData & cDomainData, c3d::SurfaceSPtr & cBoundedSurface);
	bool CreateBoundedSurface(c3d::SurfaceSPtr & pcSurface, c3d::PlaneCurveSPtr & pcPlaneCurve, c3d::SurfaceSPtr & cBoundedSurface);

	bool CreateBoundedSurface(MbSurface *& pcSurface, c3d::LoopSPtr & pcLoop, bool bOrientationWithShell, MbSurface *& cBoundedSurface);
	bool CreateBoundedSurface(MbSurface *& pcSurface, const A3DDomainData & cDomainData, MbSurface *& cBoundedSurface);

	bool CreateSurfaceBorderLoop(c3d::SurfaceSPtr & pcSurface, std::list<c3d::LoopSPtr> & vpcLoopList);

	bool ConvertMbLoop(A3DTopoLoop * pcTopoLoop, const MbSurface * pcSurface, bool bOrientationWithShell, c3d::LoopSPtr & pcLoop);
	bool ConvertMbLoop(ConvLoop * pcLoop, c3d::SurfaceSPtr & pcSurface, bool bOrientationWithShell, bool bFirstLoopFlag);

	bool ConvertMbOrientedEdge(A3DTopoCoEdge * pcTopoCoEdge, const MbSurface * pcSurface, DmiOrientedEdge *& pcOrientedEdge);
	bool ConvertMbOrientedEdge(ConvCoEdge * pcConvCoEdge, c3d::SurfaceSPtr & pcSurface);
	static bool ThreadConvertMbOrientedEdge(ConvCoEdge & cConvCoEdge, double dTopoContextScale, MbModel * pcModel);

	bool ConvertRiCoordinateSystem(A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy);

	bool ConvertRiPolyBrepModel(A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
		c3d::AssemblySPtr & pcParentAssy);

	bool GetOrientEdgeSense(MbCurveEdge * pcCurveEdge, MbVertex * pcStartVertex, MbVertex * pcEndVertex, bool bOrientationWithLoopFlag);

	bool CheckNearnessPoints(const MbSurface & surface1, const MbCurve & pCurve1,
		const MbSurface & surface2, const MbCurve & pCurve2,
		double metricAcc, bool checkLimits, double kIrr0 = M_E / 8.0);

	// OrientedEdge의 Open 여부를 확인하고 Open된 경우 Close되도록 Edge를 추가한다.
	void CheckOrientedEdgeOpenAndHealing(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface, double dTolerance);
	bool IsOrientedEdgeAllPlaneLine(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface);
	// OrientedEdge List의 Open 여부를 확인한다.
	bool IsOpenOrientedEdgeList(std::list<const MbOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface, double dTolerance);

	// 주어진 두 점을 이용해서 Line Segment를 Base로하는 Oriented Edge를 생성.
	// 주어진 Surface를 One, Two로 하는 Oriented Edge를 생성함.
	bool CreateLineSegmentOrientedEdge(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList,
		const DmiOrientedEdge * pcCurEdge, const DmiOrientedEdge * pcNextEdge, const MbSurface * pcBaseSurface, double dTolerance,
		const DmiOrientedEdge *& pcLineSegmentOrientedEdge);

	bool CreateLineSegmentOrientedEdge_V1(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList,
		const DmiOrientedEdge * pcCurEdge, const DmiOrientedEdge * pcNextEdge, const MbSurface * pcBaseSurface, double dTolerance,
		const DmiOrientedEdge *& pcLineSegmentOrientedEdge);

	// Oreient Edge가 연결되어 있는지 여부를 확인한다. 연결되어 있지 않은 경우 2개의 포인트를 찾아옴.
	bool CheckConectivity(const DmiOrientedEdge * pcCurEdge, const DmiOrientedEdge * pcNextEdge,
		const MbSurface * pcBaseSurface, double dTolerance, bool & bConnectFlag, const MbCurve ** pcConnectCurve = nullptr);

	bool CheckConectivity_V1(const MbOrientedEdge * pcCurEdge, const MbOrientedEdge * pcNextEdge,
		const MbSurface * pcBaseSurface, double dTolerance, bool & bConnectFlag, const MbCurve ** pcConectiCurve = nullptr);

	// 주어진 Orient Edge에 포함되어 있는 Curve에서 Base Surface와 일치하는 Curve만 선택해서 Bounding Box를 계산함.
	bool CalculateBoundBox(const MbOrientedEdge * pcEdge, const MbSurface * pcBaseSurface, MbRect & cRect);

	// ----- Surface 관련 함수 -----
/*
	bool ConvertSurfBase(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfPlane(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfCylinder(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfCone(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfSphere(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfTorus(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfNurbs(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfExtrusion(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfOffset(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfRuled(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfBlend03(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
*/
// ----- Vertex 관련 함수 -----
	bool ConvertTopoVertex(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex);
	bool ConvertTopoVertex(ConvVertex * pcConvVertex, MbVertex *& pcVertex);

	bool ConvertTopoUniqueVertex(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex);
	bool ConvertTopoUniqueVertex(ConvVertex * pcConvVertex, MbVertex *& pcVertex);

	bool ConvertTopoMultipleVertex(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex);
	bool ConvertTopoMultipleVertex(ConvVertex * pcConvVertex, MbVertex *& pcVertex);

	// ----- PMI 관련 함수 -----
	bool ConvertAnnotationEntity(A3DMkpAnnotationEntity * pcAnnotationEntity, A3DMiscCascadedAttributes * pcParentAttr);

	//== Tessellation Mode 함수 =====================================================================
	bool ConvertTess3d(A3DTess3D * pcTess3d, A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
		A3DTessBaseData * pcTessBaseData, A3DRiCoordinateSystem * pcCoordinateSystem, MbMesh *& pcMesh);

	UINT ConvertTessFaceDataTriangle(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
		MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector);

	UINT ConvertTessFaceDataTriangleFan(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
		MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector);

	UINT ConvertTessFaceDataTriangleStripe(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex, A3DUns32 & nTriColorIndex,
		MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector);

	UINT ConvertTessFaceDataTriangleOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector);

	UINT ConvertTessFaceDataTriangleFanOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector);

	UINT ConvertTessFaceDataTriangleStripeOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
		MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector);

	//----- Utility -----
	void MatchVertexNormal(MeshIndexMap & maNormalIndexMap, A3DUns32 nPointIndex, A3DUns32 & nNormalIndex);

	bool ConvertTriangleVector(MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap,
		A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, c3d::MeshTrianglesVector & cTrianglesVector);

	bool CreateMbGrid(double * pcPoints, double * pcNormals, double dSclae, MeshIndexMap & maPointIndexMap,
		MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector, MbGrid * pcGrid);

	bool CreateMbGridLoop(double * pcPoints, A3DTess3DData & cTess3dData, A3DTessFaceData & cTessFaceData,
		MeshIndexMap & maPointIndexMap, MbGrid * pcGrid);

	//== Attribute 관련 함수 =========================================================================
	bool CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcPO,
		A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData);

	A3DStatus CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
		const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
		A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData);

	bool GetAttributeColor(A3DMiscCascadedAttributesData & cAttributesData, COLORREF & nColor);
	bool GetAttributeTransparency(A3DMiscCascadedAttributesData & cAttrsData, float & fTransparency);
	bool GetAttributeLinePattern(A3DMiscCascadedAttributesData & cAttrsData, int & nLinePattern);

	// == Utility 함수 ==============================================================================
	bool GetTopoFaceFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DTopoFace *& pcTopoFace);
	bool GetSurfBaseFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DSurfBase *& pcSurfBase);
	bool GetSurfBaseFromTopoFace(const A3DTopoFace * pcTopoFace, const A3DSurfBase *& pcSurfBase);

	bool GetMbSurface(const A3DSurfBase * pcSurfBase, c3d::SurfaceSPtr & pcSurface);

	bool FindMbSurface(const A3DSurfBase * pcSurfBase, c3d::SurfaceSPtr & pcSurface);
	static bool FindMbSurface(SurfBaseMap & mpcSurfBaseMap, const A3DSurfBase * pcSurfBase, c3d::SurfaceSPtr & pcSurface);

	bool AddMbSurfaceToMap(const A3DSurfBase * pcSurfBase, c3d::SurfaceSPtr & pcSurface);

	bool FindTopoVertex(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex);
	bool AddTopoVertexToMap(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex);

	// == C3D 관련 Utility 함수 =====================================================================
	bool SetItemName(MbItem * pcItem, CString & strName);
	bool GetMatrix(A3DMiscTransformation * pcLocation, MbMatrix3D & cMatrix);
	bool GetVector3d(const A3DVector3dData & cVector3dData, MbVector3D & cVector);

	bool SetItemStringAttribute(MbItem * pcItem, c3d::string_t strId, c3d::string_t strAttribute);
	bool SetItemIntAttribute(MbItem * pcItem, c3d::string_t strId, int nAttribute);

	bool WriteSolid(const TCHAR * pathName, MbSolid & solid);

private:
	const A3DTopoBrepData * m_pcCurrentTopoBrepData = nullptr;
	const A3DRiBrepModel * m_pcCurRiBrepModel = nullptr;

	SolidMap m_mpcSolidMap;
	MeshMap m_mpcMeshMap;
	ConvSurfaceMap m_mpcConvSurfaceMap;
	SurfBaseMap m_mpcSurfBaseMap;
	ConvCoEdgeMap m_mpcConvCoEdgeMap;
	CurveEdgeMap m_mpcCurveEdgeMap;
	VertexMap m_mpcVertexMap;

	MbModel * m_pcModel;

	bool m_bGlobalDataFlag = false;
	A3DGlobalData m_cGlobalData;

	DWORD m_nRiSetIndex = 0;

	CString m_strCadFileName;

	time_t m_cFileImportStartTime;
	time_t m_cFileImportEndTime;

	DWORD m_nProductId = 1;
	DWORD m_nSolidId = 1;

	//ThreadPool * m_pcThreadPoolpool = nullptr;

	//== Log 관련 함수 ===============================================================================
	void CreateLog(int nId, const WCHAR * pchFilePathName);
	void Log(int nId, LPCWSTR chMessage, ...);
	void LogIncreaseTabIndex(int nId);
	void LogDecreaseTabIndex(int nId);
	CString LogHexStr(DWORD_PTR nValue);
	CString LogBoolStr(bool bValue);

	A3dTracer m_cA3dTracer;
};

CLOSE_3DX_NAMESPACE