#include "stdafx.h"

#include "DmiHpsCreator.h"

#include <Common_Define.h>

#include "DmiModelHandler.h"

#include "DmiHpsUtility.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <model.h>
#include <assembly.h>
#include <solid.h>
#include <mesh.h>
#include "../../C3D/Kernel/Include/instance.h"
#include <space_instance.h>
#include <plane_instance.h>
#include <assisting_item.h>
#include <attr_common_attribute.h>

#include <surf_plane.h>
#include <surf_cone_surface.h>
#include <surf_cylinder_surface.h>
#include <surf_curve_bounded_surface.h>
#include <surf_spline_surface.h>

#include <cur_surface_intersection.h>
#include <cur_hermit.h>
#include <cur_nurbs.h>
#include <cur_nurbs3d.h>
#include <cur_reparam_curve.h>
#include <cur_contour3d.h>
#include <cur_polyline3d.h>
#include <cur_cubic_spline3d.h>

#include <mb_placement3d.h>
#include <mb_vector3d.h>

#include <mesh_primitive.h>
#include <mesh_float_point3d.h>

#include <mb_property.h>

#include <string>
#include <format>

#include "../Dmi3dxInterface/LogManager.h"

using namespace std;
using namespace c3d;
using namespace HPS;

#define LONG_DASH_LN_PAT		"LongDashLinePattern"
#define DOTTED_LN_PAT			"DottedLinePattern"
#define DASHED_LN_PAT			"DashedLinePattern"
#define DASH_DOT_LN_PAT			"DashDotLinePattern"
#define DASH_2DOT_LN_PAT		"Dash2DotLinePattern"

#ifdef _DEBUG
#define _DEBUG_DRAW1_
//#define _DEBUG_DRAW_NO_DRAW_SOLID_
#endif

// Debuging용으로 Face의 No를 User define data로 저장한다.
#define SAVE_FACE_NUMBER_AS_USER_DEFINED_DATA

DmiHpsCreator::DmiHpsCreator()
{
	m_cStepData.SetStepType(ist_SpaceStep);
	m_cStepData.SetStepType(ist_DeviationStep);
	//	m_cStepData.SetSag(0.2);

	double dDeviateSag = Math::deviateSag;

	m_cFormNote.Init(false, true, true);

	m_cPortfolio = HPS::Database::CreatePortfolio();

	// dashed line pattern 생성
	m_cPortfolio.DefineLinePattern(LONG_DASH_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::LongDash));
	m_cPortfolio.DefineLinePattern(DOTTED_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dotted));
	m_cPortfolio.DefineLinePattern(DASHED_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dashed));
	m_cPortfolio.DefineLinePattern(DASH_DOT_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::DashDot));
	m_cPortfolio.DefineLinePattern(DASH_2DOT_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dash2Dot));

	// Create Include library
	m_cIncludeLib = HPS::Database::CreateRootSegment();
	m_cAssyIncludeLib = m_cIncludeLib.Subsegment("assy");
	m_cSolidIncludeLib = m_cIncludeLib.Subsegment("solid");
	m_cSpaceInstanceIncludeLib = m_cIncludeLib.Subsegment("spins"); // Space Instance
	m_cPlaneInstanceIncludeLib = m_cIncludeLib.Subsegment("plins"); // Plane Instance
}

DmiHpsCreator::DmiHpsCreator(DmiModelHandler * pcModelHanler) :
	m_pcModelHanler(pcModelHanler)
{
	m_cStepData.SetStepType(ist_SpaceStep);
	m_cStepData.SetStepType(ist_DeviationStep);
	//	m_cStepData.SetSag(0.2);

	double dDeviateSag = Math::deviateSag;

	m_cFormNote.Init(false, true, true);

// 	m_cPortfolio = HPS::Database::CreatePortfolio();
// 
// 	// dashed line pattern 생성
// 	m_cPortfolio.DefineLinePattern(LONG_DASH_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::LongDash));
// 	m_cPortfolio.DefineLinePattern(DOTTED_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dotted));
// 	m_cPortfolio.DefineLinePattern(DASHED_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dashed));
// 	m_cPortfolio.DefineLinePattern(DASH_DOT_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::DashDot));
// 	m_cPortfolio.DefineLinePattern(DASH_2DOT_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dash2Dot));

	// Create Include library
	m_cIncludeLib = HPS::Database::CreateRootSegment();
	m_cAssyIncludeLib = m_cIncludeLib.Subsegment("assy");
	m_cSolidIncludeLib = m_cIncludeLib.Subsegment("solid");
	m_cSpaceInstanceIncludeLib = m_cIncludeLib.Subsegment("spins"); // Space Instance
	m_cPlaneInstanceIncludeLib = m_cIncludeLib.Subsegment("plins"); // Plane Instance

	m_cCurrentSegmentKey = m_pcModelHanler->GetModel().GetSegmentKey();
}

DmiHpsCreator::~DmiHpsCreator()
{

}

void DmiHpsCreator::SetModelHandler(DmiModelHandler * pcModelHanler)
{
	m_pcModelHanler = pcModelHanler;
	m_cCurrentSegmentKey = m_pcModelHanler->GetModel().GetSegmentKey();
}

// 1. Model
bool DmiHpsCreator::CreateMbModel(MbModel * pcModel)
{
	if(nullptr != pcModel) {
		RETURN_FALSE;
	}

	//CreateLog(0, L"D:\\Temp\\HpsCreator.log");

#ifdef _DEBUG
	// CreateLog(0, L"D:\\Temp\\HpsCreator.log");
#endif

/*
	MbCube cBoundBox;
	pcModel->CalculateGabarit(cBoundBox);
	double dDiagonalDistnace = cBoundBox.GetDiagonal() / 30.0;
*/
	SegmentKey cModelKey = m_pcModelHanler->GetModel().GetSegmentKey();

	double dDiagonalDistnace = 100;

	MbPlacement3D cPlacement;

	for(size_t nIndex = 0; nIndex < pcModel->ItemsCount(); nIndex++) {
		const MbItem * pcItem = pcModel->GetItem(nIndex);

		MbeSpaceType eType = pcItem->IsA();

		if(nullptr != m_pvstrErrMsgVector) {
			m_strParent.clear();
		}

		CreateItem(nIndex, pcItem, cPlacement, cModelKey);
	}

	return true;
}

// 2. Assembly
bool DmiHpsCreator::CreateAssembly(size_t nIndex, const MbAssembly * pcAssy, MbPlacement3D cPlacement, SegmentKey & cParentSegKey)
{
	if(nullptr == pcAssy) {
		return false;
	}

	// Assy Name
	string_t strName;
	GetItemText(pcAssy, L"name", strName);

	std::string strOldParent = m_strParent;
	if(nullptr != m_pvstrErrMsgVector) {
		if(false == strName.empty()) {
			m_strParent = std::format(" / [{}] {}", nIndex, ToSTDstring(strName)) + m_strParent;
		}
		else {
			m_strParent = std::format(" / Assy[{}]", nIndex) + m_strParent;
		}
	}

	// Matrix 처리
// 	MbPlacement3D cPlacement;
// 	pcAssy->GetPlacement(cPlacement);

	HPS::MatrixKit cMatrix;
	GetMatrix(cPlacement, cMatrix);

	auto strAssyId = std::format("assy{}", m_nAssyNoId++);
	SegmentKey cIncludeSegment = m_cAssyIncludeLib.Subsegment(strAssyId.c_str());
	cIncludeSegment.SetModellingMatrix(cMatrix);

	// HPS Key에 Name 설정
	if(false == strName.empty()) {
		char * pchText = nullptr;
		int nTextCount = 0;
		if(true == DmiHps::CStringToChar(strName.c_str(), pchText, nTextCount)) {
			cIncludeSegment.SetUserData(HPS_USER_DATA_NAME_INDEX, nTextCount, (UCHAR *) pchText);
		}
	}

	// HPS Key MbItem 설정
	cIncludeSegment.SetUserData(HPS_USER_DATA_MBITEM_INDEX, sizeof(DWORD_PTR), (UCHAR *) pcAssy);

	cParentSegKey.IncludeSegment(cIncludeSegment);

	for(size_t nIndex = 0; nIndex < pcAssy->ItemsCount(); nIndex++) {
		const MbItem * pcItem = pcAssy->GetItem(nIndex);
		MbPlacement3D cPlacement1;
		CreateItem(nIndex, pcItem, cPlacement1, cIncludeSegment);
	}

	m_strParent = strOldParent;

	return false;
}

// 3. Create Item
bool DmiHpsCreator::CreateItem(const MbItem * pcItem)
{
	return CreateItem(0, pcItem, m_cCurrentPlacement, m_cCurrentSegmentKey);
}

bool DmiHpsCreator::CreateItem(size_t nIndex, const MbItem * pcItem, MbPlacement3D cPlacement, SegmentKey & cParentSegKey)
{
	MbeSpaceType eType = pcItem->IsA();

	switch(eType)
	{
		case st_Solid:
			CreateSolid(nIndex, static_cast<const MbSolid *>(pcItem), cPlacement, cParentSegKey);
			break;

		case st_Instance:
			CreateInstance(nIndex, static_cast<const MbInstance *>(pcItem), cParentSegKey);
			break;

		case st_Assembly:
			CreateAssembly(nIndex, static_cast<const MbAssembly *>(pcItem), cPlacement, cParentSegKey);
			break;

		case st_Mesh:
			CreateMesh(nIndex, static_cast<const MbMesh *>(pcItem), cParentSegKey);
			break;

		case st_SpaceInstance:
			CreateSpaceInstance(nIndex, static_cast<const MbSpaceInstance *>(pcItem), cParentSegKey);
			break;

		case st_AssistedItem:
			CreateAssistedItem(nIndex, static_cast<const MbAssistingItem *>(pcItem), m_dDiagonalDistnace, cParentSegKey);
			break;

		case st_PlaneInstance:
			CreatePlaneInstance(nIndex, static_cast<const MbPlaneInstance *>(pcItem), cParentSegKey);
			break;

		case st_Undefined:
			break;
		case st_SpaceItem:
			break;
		case st_Point3D:
			break;
		case st_FreePoint3D:
			break;
		case st_Curve3D:
			break;
		case st_Line3D:
			break;
		case st_LineSegment3D:
			break;
		case st_Arc3D:
			break;
		case st_Spiral:
			break;
		case st_ConeSpiral:
			break;
		case st_CurveSpiral:
			break;
		case st_CrookedSpiral:
			break;
		case st_PolyCurve3D:
			break;
		case st_Polyline3D:
			break;
		case st_Nurbs3D:
			break;
		case st_Bezier3D:
			break;
		case st_Hermit3D:
			break;
		case st_CubicSpline3D:
			break;
		case st_PlaneCurve:
			break;
		case st_OffsetCurve3D:
			break;
		case st_TrimmedCurve3D:
			break;
		case st_ReparamCurve3D:
			break;
		case st_BridgeCurve3D:
			break;
		case st_CharacterCurve3D:
			break;
		case st_ContourOnSurface:
			break;
		case st_ContourOnPlane:
			break;
		case st_SurfaceCurve:
			break;
		case st_SilhouetteCurve:
			break;
		case st_SurfaceIntersectionCurve:
			break;
		case st_BSpline:
			break;
		case st_Contour3D:
			break;
		case st_CoonsDerivative:
			break;
		case st_FreeCurve3D:
			break;
		case st_Surface:
			break;
		case st_ElementarySurface:
			break;
		case st_Plane:
			break;
		case st_ConeSurface:
			break;
		case st_CylinderSurface:
			break;
		case st_SphereSurface:
			break;
		case st_TorusSurface:
			break;
		case st_SweptSurface:
			break;
		case st_ExtrusionSurface:
			break;
		case st_RevolutionSurface:
			break;
		case st_EvolutionSurface:
			break;
		case st_ExactionSurface:
			break;
		case st_ExpansionSurface:
			break;
		case st_SpiralSurface:
			break;
		case st_RuledSurface:
			break;
		case st_SectorSurface:
			break;
		case st_PolySurface:
			break;
		case st_HermitSurface:
			break;
		case st_SplineSurface:
			break;
		case st_GridSurface:
			break;
		case st_TriBezierSurface:
			break;
		case st_SimplexSurface:
			break;
		case st_OffsetSurface:
			break;
		case st_DeformedSurface:
			break;
		case st_NurbsSurface:
			break;
		case st_CornerSurface:
			break;
		case st_CoverSurface:
			break;
		case st_CoonsPatchSurface:
			break;
		case st_GregoryPatchSurface:
			break;
		case st_LoftedSurface:
			break;
		case st_ElevationSurface:
			break;
		case st_MeshSurface:
			break;
		case st_GregorySurface:
			break;
		case st_SmoothSurface:
			break;
		case st_ChamferSurface:
			break;
		case st_FilletSurface:
			break;
		case st_ChannelSurface:
			break;
		case st_FullFilletSurface:
			break;
		case st_JoinSurface:
			break;
		case st_CurveBoundedSurface:
			break;
		case st_BendedUnbendedSurface:
			break;
		case st_CylindricBendedSurface:
			break;
		case st_CylindricUnbendedSurface:
			break;
		case st_ConicBendedSurface:
			break;
		case st_ConicUnbendedSurface:
			break;
		case st_GregoryRibbonPatchSurface:
			break;
		case st_ExplorationSurface:
			break;
		case st_SectionSurface:
			break;
		case st_SimplexSpline:
			break;
		case st_FreeSurface:
			break;
		case st_Legend:
			break;
		case st_Marker:
			break;
		case st_Thread:
			break;
		case st_Symbol:
			break;
		case st_PointsSymbol:
			break;
		case st_Rough:
			break;
		case st_Leader:
			break;
		case st_Dimension3D:
			break;
		case st_LinearDimension3D:
			break;
		case st_DiameterDimension3D:
			break;
		case st_RadialDimension3D:
			break;
		case st_AngularDimension3D:
			break;
		case st_FreeLegend:
			break;
		case st_Item:
			break;
		case st_PointFrame:
			break;
		case st_WireFrame:
			break;
		case st_Collection:
			break;
		case st_FreeItem:
			break;
	}

	return false;
}

// 3. Instance
bool DmiHpsCreator::CreateInstance(size_t nIndex, const MbInstance * pcInstance, SegmentKey & cParentSegKey)
{
	if(nullptr == pcInstance) {
		return false;
	}

	const MbItem * pcItem = pcInstance->GetItem();
	if(nullptr == pcItem) {
		return false;
	}

	MbPlacement3D cPlacement;
	pcInstance->GetPlacement(cPlacement);

	return CreateItem(nIndex, pcItem, cPlacement, cParentSegKey);
}

// 3-1. Create Item Solid
bool DmiHpsCreator::CreateSolid(size_t nIndex, const MbSolid * pcSolid, MbPlacement3D cPlacement, SegmentKey & cParentSegKey)
{
	if(nullptr != pcSolid && nullptr != m_pcModelHanler) {
		RETURN_FALSE;
	}

	auto cIterator = m_mSolidMap.find(pcSolid);
	if(m_mSolidMap.end() != cIterator) {
		SegmentKey cIncludeSolidSegment = cIterator->second;
		cParentSegKey.IncludeSegment(cIncludeSolidSegment);
		LogManager::Log(L"Solid IncludeSegment");
		return true;
	}

	std::string strOldParent = m_strParent;
	if(nullptr != m_pvstrErrMsgVector) {
		m_strParent = std::format(" / Solid[{}]", nIndex) + m_strParent;
	}

	auto strSolidId = std::format("solid{}", m_nPartNoId++);
	SegmentKey cIncludeSolidSegment = m_cSolidIncludeLib.Subsegment(strSolidId.c_str());

	// Matrix 설정
	HPS::MatrixKit cMatrix;
	GetMatrix(cPlacement, cMatrix);

	if(false == cMatrix.IsIdentity()) {
		cIncludeSolidSegment.SetModellingMatrix(cMatrix);
	}

	// HPS Key에 Name 설정
	string_t strName;
	if(true == GetItemText(pcSolid, L"name", strName)) {
		if(false == strName.empty()) {
			char * pchText = nullptr;
			int nTextCount = 0;
			if(true == DmiHps::CStringToChar(strName.c_str(), pchText, nTextCount)) {
				cIncludeSolidSegment.SetUserData(HPS_USER_DATA_NAME_INDEX, nTextCount, (UCHAR *) pchText);
			}
		}
	}

	// HPS Key MbItem 설정
	cIncludeSolidSegment.SetUserData(HPS_USER_DATA_MBITEM_INDEX, sizeof(DWORD_PTR), (UCHAR *) pcSolid);

	cParentSegKey.IncludeSegment(cIncludeSolidSegment);
	m_mSolidMap.insert(std::make_pair(pcSolid, cIncludeSolidSegment));

	LogManager::Log(L"Create IncludeSegment");

	MbMesh cMesh;
	pcSolid->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

	size_t nGridsCount = cMesh.GridsCount();

	// Color 설정값 가져오기
	bool bSolidColoredFlsg = pcSolid->IsColored();
	COLORREF nSolidColor = pcSolid->GetColor();
	if(true == bSolidColoredFlsg) {
		HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nSolidColor);
		cIncludeSolidSegment.GetMaterialMappingControl().SetFaceColor(cRgbColor);
	}

	// Visual 설정값 가져오기
	bool bSolidVisualedFlag = (nullptr != pcSolid->GetSimpleAttribute(at_Visual)) ? true : false;
	float a, d, sp, sh, fSolidTransparency, e;
	pcSolid->GetVisual(a, d, sp, sh, fSolidTransparency, e);

	if(true == bSolidVisualedFlag) {
		cIncludeSolidSegment.GetMaterialMappingControl().SetFaceAlpha(fSolidTransparency);
	}

	size_t nFacesCount = pcSolid->GetFacesCount();
	ASSERT(nGridsCount == nFacesCount);

	bool bFaceColorizingFlag = false;

	// 수량이 같아야 Color를 입히도록 한다.
	if(nGridsCount == nFacesCount) {
		bFaceColorizingFlag = true;
	}

// 	CString strText;
// 	CString strError;

#ifndef _DEBUG_DRAW_NO_DRAW_SOLID_
	for(size_t nGridIndex = 0; nGridIndex < cMesh.GridsCount(); nGridIndex++)
	{
		const MbGrid * pcGrid = cMesh.GetGrid(nGridIndex);
		ASSERT(pcGrid);

		m_nFaceIndex = nGridIndex;

		if(false == CreateGrid(pcGrid, pcSolid->GetFace(nGridIndex), bSolidColoredFlsg, cIncludeSolidSegment)) {

			// 오류가 난 경우 표시 해당 항목 표시
			const MbFace * pcFace = pcSolid->GetFace(nGridIndex);
			DrawMbFaceEdgeCurve(pcFace);

			if(nullptr != m_pvstrErrMsgVector) {
				size_t nPointCount = pcGrid->PointsCount();
				size_t nTrianglesCount = pcGrid->TrianglesCount();
				size_t nQuadranglesCount = pcGrid->QuadranglesCount();

				std::string strError = std::format("Face[{}] ", nGridIndex) + m_strParent;

				m_pvstrErrMsgVector->push_back(strError);
			}
			continue;
		}
	}

	DrawLoopMbSolid(cMesh, true, cIncludeSolidSegment);
#endif // !_DEBUG_DRAW_NO_DRAW_SOLID_

	m_strParent = strOldParent;

#ifdef _DEBUG_DRAW1_
	DebugDrwingSolid(pcSolid, cIncludeSolidSegment);
#endif

	return true;
}

// 3-1-2. Create Grid (개별 Face를 Shading)
bool DmiHpsCreator::CreateGrid(const MbGrid * pcGrid, const MbFace * pcFace, bool bFaceColorizingFlag, SegmentKey & cParentSegKey)
{
	if(nullptr != pcGrid) {
		RETURN_FALSE;
	}

	size_t nPointCount = pcGrid->PointsCount();
	size_t nNormalCount = pcGrid->NormalsCount();
	size_t nTrianglesCount = pcGrid->TrianglesCount();
	size_t nQuadranglesCount = pcGrid->QuadranglesCount();
	size_t nLoopsCount = pcGrid->LoopsCount();
	size_t nFaceListCount = (pcGrid->TrianglesCount() * 4) + (pcGrid->QuadranglesCount() * 5);

	if(0 == nPointCount || 0 == nFaceListCount) {
		//ASSERT(FALSE);
		return false;
	}

	Point * pcPoints = new Point[nPointCount];
	if(nullptr == pcPoints) {
		//ASSERT(FALSE);
		return false;
	}

	Vector * pcNormals = new Vector[nPointCount];
	if(nullptr == pcNormals) {
		//ASSERT(FALSE);
		REMOVE_ARRAY(pcPoints);
		return false;
	}

	if(0 == nFaceListCount) {
		//ASSERT(FALSE);
		REMOVE_ARRAY(pcPoints);
		REMOVE_ARRAY(pcNormals);
		return false;
	}

	int * pnFaceList = new int[nFaceListCount];
	if(nullptr == pnFaceList) {
		//ASSERT(FALSE);
		REMOVE_ARRAY(pcPoints);
		REMOVE_ARRAY(pcNormals);
		return false;
	}

	for(size_t nIndex = 0; nIndex < nPointCount; nIndex++) {
		MbFloatPoint3D cPoint;
		pcGrid->GetPoint(nIndex, cPoint);
		pcPoints[nIndex] = DmiHps::GetPoint(cPoint);
	}

	size_t nSize = pcGrid->NormalsCount();
	for(size_t nIndex = 0; nIndex < nSize; nIndex++) {
		MbFloatVector3D cNormal;
		pcGrid->GetNormal(nIndex, cNormal);
		pcNormals[nIndex] = DmiHps::GetVector(cNormal);
	}

	size_t nFaceListIndex = 0;

	for(size_t nTriangleIndex = 0; nTriangleIndex < pcGrid->TrianglesCount(); nTriangleIndex++) {
		UINT nPolygonIndex[3];
		if(true == pcGrid->GetTriangleIndex(nTriangleIndex, nPolygonIndex[0], nPolygonIndex[1], nPolygonIndex[2])) {
			pnFaceList[nFaceListIndex++] = 3;
			pnFaceList[nFaceListIndex++] = nPolygonIndex[0];
			pnFaceList[nFaceListIndex++] = nPolygonIndex[1];
			pnFaceList[nFaceListIndex++] = nPolygonIndex[2];
		}
	}

	for(size_t nQuadrangleIndex = 0; nQuadrangleIndex < pcGrid->QuadranglesCount(); nQuadrangleIndex++) {
		UINT nPolygonIndex[4];
		if(true == pcGrid->GetQuadrangleIndex(nQuadrangleIndex, nPolygonIndex[0], nPolygonIndex[1], nPolygonIndex[2], nPolygonIndex[3])) {
			pnFaceList[nFaceListIndex++] = 4;
			pnFaceList[nFaceListIndex++] = nPolygonIndex[0];
			pnFaceList[nFaceListIndex++] = nPolygonIndex[1];
			pnFaceList[nFaceListIndex++] = nPolygonIndex[2];
			pnFaceList[nFaceListIndex++] = nPolygonIndex[3];
		}
	}

	ShellKit cShellKit;
	cShellKit.SetPoints(nPointCount, pcPoints);
	cShellKit.SetVertexNormalsByRange(0, nPointCount, pcNormals);
	cShellKit.SetFacelist(nFaceListCount, pnFaceList);

#ifdef SAVE_FACE_NUMBER_AS_USER_DEFINED_DATA
	CString strText;
	char * pchText = nullptr;
	int nTextCount = 0;

	strText.Format(L"Assy: %d, Part: %d, Face: %d", m_nAssyNoId - 1, m_nPartNoId - 1, m_nFaceIndex);

	if(true == DmiHps::CStringToChar(strText, pchText, nTextCount)) {
		cShellKit.SetUserData(HPS_USER_DATA_NAME_INDEX, nTextCount, (UCHAR *)pchText);
		delete[] pchText;

// 		ByteArray aByteArray;
// 		cShellKit.ShowUserData(0, aByteArray);
// 
// 		CString strText1;
// 		DmiHpsUtility::CharToCString((char *)aByteArray.data(), strText1);
	}
#endif

	// ----- Color 설정 -----
	// Face와 Grid의 갯수가 같은 경우에만 Face color를 설정한다.
	if(true == bFaceColorizingFlag) {
		// Face가 있는 경우 Face에서 Color를 가져온다.
		if(nullptr != pcFace) {
			// Face에 Color가 있는 경우에 Face color 설정
			if(true == pcFace->IsColored()) {
				COLORREF cFaceColor = pcFace->GetColor();
				HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(cFaceColor);
				cShellKit.SetFaceRGBColorsByRange(0, nTrianglesCount + nQuadranglesCount, cRgbColor);
			}
		}
	}
	else {
		if(true == pcGrid->IsColored()) {
			COLORREF cFaceColor = pcGrid->GetColor();
			HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(cFaceColor);
			cShellKit.SetFaceRGBColorsByRange(0, nTrianglesCount + nQuadranglesCount, cRgbColor);
		}
	}

	ShellKey cSolidShellKey = cParentSegKey.InsertShell(cShellKit);

//	cSolidShellKey.

	return true;
}

// 3-1-1. Create Face (Solid Graphic을 생성하다가 문제가 생기는 Face를 다시한번 Create하는 함수)
bool DmiHpsCreator::CreateFace(const MbFace * pcFace, SegmentKey & cParentSegKey)
{
	if(nullptr == pcFace) {
		ASSERT(false);
		return false;
	}

	MbMesh cMesh;
	pcFace->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

	for(size_t nGridIndex = 0; nGridIndex < cMesh.GridsCount(); nGridIndex++) {
		const MbGrid * pcGrid = cMesh.GetGrid(nGridIndex);
		ASSERT(pcGrid);

		if(false == CreateGrid(pcGrid, pcFace, true, cParentSegKey)) {
			ASSERT(false);
			return false;
		}
	}

	return true;
}

// 4. Space Instance
bool DmiHpsCreator::CreateSpaceInstance(size_t nIndex, const MbSpaceInstance * pcSpaceInstance, SegmentKey & cParentSegKey)
{
	if(nullptr == pcSpaceInstance) {
		return false;
	}
	const MbSpaceItem * pcSpaceItem = pcSpaceInstance->GetSpaceItem();
	if(nullptr == pcSpaceItem) {
		return false;
	}

	auto strSpaceInstanceId = std::format("spins{}", m_nSpaceInstanceNoId++);
	SegmentKey cIncludeSpaceInstanceSegment = m_cSpaceInstanceIncludeLib.Subsegment(strSpaceInstanceId.c_str());
	cParentSegKey.IncludeSegment(cIncludeSpaceInstanceSegment);

	// Color 설정값 가져오기
	if(true == pcSpaceInstance->IsColored()) {
		COLORREF nSpaceInstanceColor = pcSpaceInstance->GetColor();
		HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nSpaceInstanceColor);
		cIncludeSpaceInstanceSegment.GetMaterialMappingControl().SetLineColor(cRgbColor);
	}

	// Line Patter 설정값 가져오기, Style 값에 line patter값을 저장해 놓았음.
	SetSegementLinePattern(cIncludeSpaceInstanceSegment, pcSpaceInstance->GetStyle());

	MbeSpaceType eSpaceType = pcSpaceItem->Family();

	switch(pcSpaceItem->Family())
	{
		case st_Curve3D:
			CreateCurve3d(pcSpaceItem, cIncludeSpaceInstanceSegment);
			break;

		case st_Polyline3D:
			CreatePolyline3d(pcSpaceItem, cIncludeSpaceInstanceSegment);
			break;

		case st_Surface:
			CreateSurface(pcSpaceItem, cIncludeSpaceInstanceSegment);
			break;
	}

	return true;
}

bool DmiHpsCreator::CreateSurface(const MbSpaceItem * pcSpaceItem, HPS::SegmentKey & cParentSegKey)
{
	if(st_SplineSurface == pcSpaceItem->IsA()) {
		DebugDrawSplineSurfaceControlPoint(static_cast<const MbSurface &>(*pcSpaceItem), cParentSegKey);
	}

	MbMesh cMesh;
	pcSpaceItem->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

	size_t nGridsCount = cMesh.GridsCount();

	/*
		// Color 설정값 가져오기
		bool bSolidColoredFlsg = pcSolid->IsColored();
		COLORREF nSolidColor = pcSolid->GetColor();
		if(true == bSolidColoredFlsg) {
			HPS::RGBColor cRgbColor = RtHpsUtility::GetRgbColor(nSolidColor);
			cIncludeSolidSegment.GetMaterialMappingControl().SetFaceColor(cRgbColor);
		}

		// Visual 설정값 가져오기
		bool bSolidVisualedFlag = (nullptr != pcSolid->GetSimpleAttribute(at_Visual)) ? true : false;
		float a, d, sp, sh, fSolidTransparency, e;
		pcSolid->GetVisual(a, d, sp, sh, fSolidTransparency, e);

		if(true == bSolidVisualedFlag) {
			cIncludeSolidSegment.GetMaterialMappingControl().SetFaceAlpha(fSolidTransparency);
		}

		size_t nFacesCount = pcSolid->GetFacesCount();
		ASSERT(nGridsCount == nFacesCount);

		bool bFaceColorizingFlag = false;

		// 수량이 같아야 Color를 입히도록 한다.
		if(nGridsCount == nFacesCount) {
			bFaceColorizingFlag = true;
		}*/

	CString strText;
	CString strError;

	for(size_t nGridIndex = 0; nGridIndex < cMesh.GridsCount(); nGridIndex++)
	{
		const MbGrid * pcGrid = cMesh.GetGrid(nGridIndex);
		ASSERT(pcGrid);

		if(false == CreateGrid(pcGrid, nullptr, false, cParentSegKey)) {
			/*
						const MbFace * pcFace = pcSolid->GetFace(nGridIndex);
						DrawMbFaceEdgeCurve(pcFace);

						if(nullptr != m_pvstrErrMsgVector) {
							size_t nPointCount = pcGrid->PointsCount();
							size_t nTrianglesCount = pcGrid->TrianglesCount();
							size_t nQuadranglesCount = pcGrid->QuadranglesCount();

							std::string strError = std::format("[{}] Point: {}, FaceList: {}", nGridIndex,
								nPointCount, nTrianglesCount + nQuadranglesCount, ToSTDstring(strName)) + m_strParent;
							m_pvstrErrMsgVector->push_back(strError);
						}
			*/
			continue;
		}
	}

	return true;
}

// 4-1. Curve 3d
bool DmiHpsCreator::CreateCurve3d(const MbSpaceItem * pcSpaceItem, SegmentKey & cParentSegKey)
{
	const MbCurve3D * pcCurve = static_cast<const MbCurve3D *>(pcSpaceItem);

	if(st_Contour3D == pcCurve->IsA()) {
		return CreateContour3d(pcSpaceItem, cParentSegKey);
	}
	else if(st_Polyline3D == pcCurve->IsA()) {
		return CreatePolyline3d(pcSpaceItem, cParentSegKey);
	}

	MbMesh cMesh;
	pcCurve->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

	if(0 == cMesh.PolygonsCount()) {
		ASSERT(false);
		return false;
	}

	HPS::PointArray acPointArray;

	for(size_t nIndex = 0; nIndex < cMesh.PolygonsCount(); nIndex++) {
		const MbPolygon3D * pcPolygon = cMesh.GetPolygon(nIndex);

		std::vector<MbCartPoint3D> vcPointVector;
		pcPolygon->GetPoints(vcPointVector);

		if(true == vcPointVector.empty()) {
			continue;
		}

		for(const MbCartPoint3D & cPolygonPoint : vcPointVector) {

			HPS::Point cPoint((float) cPolygonPoint.x, (float) cPolygonPoint.y, (float) cPolygonPoint.z);
			acPointArray.push_back(cPoint);
		}

		cParentSegKey.InsertLine(acPointArray);
	}

#ifdef _DEBUG_DRAW2_
	DebugDrawCurveControlPoint(*pcCurve, cParentSegKey);
#endif

	return true;
}

bool DmiHpsCreator::CreateCurve3d(const MbCurve3D * pcCurve, SegmentKey & cParentSegKey)
{
	MbMesh cMesh;
	pcCurve->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

	if(0 == cMesh.PolygonsCount()) {
		ASSERT(false);
		return false;
	}

	HPS::PointArray acPointArray;

	for(size_t nIndex = 0; nIndex < cMesh.PolygonsCount(); nIndex++) {
		const MbPolygon3D * pcPolygon = cMesh.GetPolygon(nIndex);

		std::vector<MbCartPoint3D> vcPointVector;
		pcPolygon->GetPoints(vcPointVector);

		if(true == vcPointVector.empty()) {
			continue;
		}

		for(const MbCartPoint3D & cPolygonPoint : vcPointVector) {

			HPS::Point cPoint((float) cPolygonPoint.x, (float) cPolygonPoint.y, (float) cPolygonPoint.z);
			acPointArray.push_back(cPoint);
		}

		cParentSegKey.InsertLine(acPointArray);
	}

	return true;
}

// 4-1-2. Contour 3d
bool DmiHpsCreator::CreateContour3d(const MbSpaceItem * pcSpaceItem, SegmentKey & cParentSegKey)
{
	const MbContour3D * pcContour = static_cast<const MbContour3D *>(pcSpaceItem);
	if(nullptr == pcContour) {
		return false;
	}

	for(size_t nIndex = 0; nIndex < pcContour->GetSegmentsCount(); nIndex++) {
		const MbCurve3D * pcCurve = pcContour->GetSegment(nIndex);
		CreateCurve3d(pcCurve, cParentSegKey);
	}

	return true;
}

// 4-2. Polyline3d 생성
bool DmiHpsCreator::CreatePolyline3d(const MbSpaceItem * pcSpaceItem, SegmentKey & cParentSegKey)
{
	const MbPolyline3D * pcCurve = static_cast<const MbPolyline3D *>(pcSpaceItem);

	SpacePointsVector vcPointVector;
	pcCurve->GetPointList(vcPointVector);

	HPS::PointArray acPointArray;
	for(const MbCartPoint3D & cPoint : vcPointVector) {
		HPS::Point cHpsPoint((float) cPoint.x, (float) cPoint.y, (float) cPoint.z);
		acPointArray.push_back(cHpsPoint);
	}

	cParentSegKey.InsertLine(acPointArray);

	return true;
}

// 4-3. Assisted Item 생성 (Axis)
bool DmiHpsCreator::CreateAssistedItem(size_t nIndex, const MbAssistingItem * pcAssistingItem, double dAxisSize, SegmentKey & cParentSegKey)
{
	if(nullptr == pcAssistingItem) {
		return false;
	}

	MbPlacement3D cPlacement;
	pcAssistingItem->GetPlacement(cPlacement);

	MbCartPoint3D cOrigin = cPlacement.GetOrigin();
	MbCartPoint3D cXAxisPoint = cOrigin + cPlacement.GetAxisX() * dAxisSize;
	MbCartPoint3D cYAxisPoint = cOrigin + cPlacement.GetAxisY() * dAxisSize;
	MbCartPoint3D cZAxisPoint = cOrigin + cPlacement.GetAxisZ() * dAxisSize;

	SegmentKey cAxisSegment = cParentSegKey.Subsegment();
	cAxisSegment.GetVisibilityControl().SetLines(true);

	cAxisSegment.GetPortfolioControl().Push(m_cPortfolio);
	cAxisSegment.GetLineAttributeControl().SetPattern(LONG_DASH_LN_PAT);

	HPS::Point cP1, cP2;
	// X-Axis
	GetHpsPoint(cOrigin, cP1);
	GetHpsPoint(cXAxisPoint, cP2);
	cAxisSegment.InsertLine(cP1, cP2);

	// Y-Axis
	GetHpsPoint(cYAxisPoint, cP2);
	cAxisSegment.InsertLine(cP1, cP2);

	// Z-Axis
	GetHpsPoint(cZAxisPoint, cP2);
	cAxisSegment.InsertLine(cP1, cP2);

	return true;
}

// 5. Mesh 생성
bool DmiHpsCreator::CreateMesh(size_t nIndex, const MbMesh * pcMesh, SegmentKey & cParentSegKey)
{
	size_t nGridsCount = pcMesh->GridsCount();

	MbeSpaceType eType = pcMesh->GetMeshType();

	SegmentKey cIncludeSolidSegment;
	if(true == m_maMeshMap.Lookup(pcMesh, cIncludeSolidSegment)) {
		cParentSegKey.IncludeSegment(cIncludeSolidSegment);
		LogManager::Log(L"Find Mesh IncludeSegment");
		return true;
	}

	std::string strOldParent = m_strParent;
	if(nullptr != m_pvstrErrMsgVector) {
		m_strParent = std::format(" / Solid[{}] ", nIndex) + m_strParent;
	}

	CString strText;
	CString strError;

	auto strSolidId = std::format("solid{}", m_nPartNoId++);
	cIncludeSolidSegment = m_cSolidIncludeLib.Subsegment(strSolidId.c_str());

	// Color 설정값 가져오기
	bool bSolidColoredFlsg = pcMesh->IsColored();
	COLORREF nSolidColor = pcMesh->GetColor();
	if(true == bSolidColoredFlsg) {
		HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nSolidColor);
		cIncludeSolidSegment.GetMaterialMappingControl().SetFaceColor(cRgbColor);
	}

	// Visual 설정값 가져오기
	bool bSolidVisualedFlag = (nullptr != pcMesh->GetSimpleAttribute(at_Visual)) ? true : false;
	float a, d, sp, sh, fSolidTransparency, e;
	pcMesh->GetVisual(a, d, sp, sh, fSolidTransparency, e);

	if(true == bSolidVisualedFlag) {
		cIncludeSolidSegment.GetMaterialMappingControl().SetFaceAlpha(fSolidTransparency);
	}

	cParentSegKey.IncludeSegment(cIncludeSolidSegment);
	m_maMeshMap.SetAt(pcMesh, cIncludeSolidSegment);

	LogManager::Log(L"Create Mesh IncludeSegment");

	for(size_t nGridIndex = 0; nGridIndex < pcMesh->GridsCount(); nGridIndex++)
	{
		const MbGrid * pcGrid = pcMesh->GetGrid(nGridIndex);
		ASSERT(pcGrid);

		CreateGrid(pcGrid, nullptr, false, cIncludeSolidSegment);
	}

	DrawLoopMbSolid(*(MbMesh *) pcMesh, false, cIncludeSolidSegment);

	m_strParent = strOldParent;

#ifdef _DEBUG_DRAW2_
	DebugDrwingSolid(pcSolid, cIncludeSolidSegment);
#endif
	return true;
}

// 6. Plance Instacne 생성
bool DmiHpsCreator::CreatePlaneInstance(size_t nIndex, const MbPlaneInstance * pcPlaneInstance, SegmentKey & cParentSegKey)
{
	if(nullptr == pcPlaneInstance) {
		return false;
	}
	const MbPlaneItem * pcPlaneItem = pcPlaneInstance->GetPlaneItem();
	if(nullptr == pcPlaneItem) {
		return false;
	}

	auto strPlaneInstanceId = std::format("plins{}", m_nPlaneInstanceNoId++);
	SegmentKey cIncludePlaneInstanceSegment = m_cPlaneInstanceIncludeLib.Subsegment(strPlaneInstanceId.c_str());
	cParentSegKey.IncludeSegment(cIncludePlaneInstanceSegment);

	// Color 설정값 가져오기
	if(true == pcPlaneInstance->IsColored()) {
		COLORREF nSpaceInstanceColor = pcPlaneInstance->GetColor();
		HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nSpaceInstanceColor);
		cIncludePlaneInstanceSegment.GetMaterialMappingControl().SetLineColor(cRgbColor);
	}

	// Line Patter 설정값 가져오기, Style 값에 line patter값을 저장해 놓았음.
	SetSegementLinePattern(cIncludePlaneInstanceSegment, pcPlaneInstance->GetStyle());

	MbePlaneType ePlaneType = pcPlaneItem->Family();

	switch(pcPlaneItem->Family())
	{
		case pt_Curve:
			CreatePlaneCurve(pcPlaneItem, L"", cIncludePlaneInstanceSegment);
			break;
	}

	return true;
}

// 6-1. Plane Curve 생성
bool DmiHpsCreator::CreatePlaneCurve(const MbPlaneItem * pcSpaceItem, CString strCurveName, SegmentKey & cParentSegKey)
{
	const MbCurve * pcCurve = static_cast<const MbCurve *>(pcSpaceItem);
	if(nullptr != pcCurve) {
		return false;
	}

	MbPolygon cPolygon;
	pcCurve->CalculatePolygon(m_cStepData.GetSag(), cPolygon);

	if(0 == cPolygon.Count()) {
		ASSERT(false);
		return false;
	}

	HPS::PointArray acPointArray;

	MbFloatPoint cPoint;

	for(size_t nIndex = 0; nIndex < cPolygon.Count(); nIndex++) {
		cPolygon.GetPoint(nIndex, cPoint);

		HPS::Point cHpsPoint((float) cPoint.x, (float) cPoint.y, 0.0f);
		acPointArray.push_back(cHpsPoint);
	}

	cParentSegKey.InsertLine(acPointArray);

	double dLength = cPolygon.GetLength();

	MbCartPoint3D cSp, cEp;
	cPoint = cPolygon.GetPoint(cPolygon.Count() - 2);
	cSp.Init(cPoint.x, cPoint.y, 0.0);
	cPoint = cPolygon.GetPoint(cPolygon.Count() - 1);
	cEp.Init(cPoint.x, cPoint.y, 0.0);

	double dScale = dLength / 80.0;
	DrawCone(cSp, cEp, 0.3 * dScale, 3 * dScale, cParentSegKey);

	if(false == strCurveName.IsEmpty()) {
		MbCartPoint cMidPoint;
		pcCurve->GetMiddlePoint(cMidPoint);

		HPS::Point cHpsMidPoint;
		cHpsMidPoint.x = (float) cMidPoint.x;
		cHpsMidPoint.y = (float) cMidPoint.y;
		cHpsMidPoint.z = 0.0;

		DrawPlaneText(cHpsMidPoint, strCurveName, cParentSegKey);
	}

	return true;
}

bool DmiHpsCreator::CreatePlaneCurve(const MbCurve & cCurve, bool bSense, CString strCurveName, SegmentKey & cParentSegKey)
{
	MbPolygon cPolygon;
	cCurve.CalculatePolygon(m_cStepData.GetSag(), cPolygon);

	if(0 == cPolygon.Count()) {
		ASSERT(false);
		return false;
	}

	HPS::PointArray acPointArray;

	MbFloatPoint cPoint;

	for(size_t nIndex = 0; nIndex < cPolygon.Count(); nIndex++) {
		cPolygon.GetPoint(nIndex, cPoint);

		HPS::Point cHpsPoint((float) cPoint.x, (float) cPoint.y, 0.0f);
		acPointArray.push_back(cHpsPoint);
	}

	cParentSegKey.InsertLine(acPointArray);

	double dLength = cPolygon.GetLength();

	MbCartPoint3D cSp, cEp;

	if(true == bSense) {
		cPoint = cPolygon.GetPoint(cPolygon.Count() - 2);
		cSp.Init(cPoint.x, cPoint.y, 0.0);
		cPoint = cPolygon.GetPoint(cPolygon.Count() - 1);
		cEp.Init(cPoint.x, cPoint.y, 0.0);
	}
	else {
		cPoint = cPolygon.GetPoint(1);
		cSp.Init(cPoint.x, cPoint.y, 0.0);
		cPoint = cPolygon.GetPoint(0);
		cEp.Init(cPoint.x, cPoint.y, 0.0);
	}

	double dScale = dLength / 80.0;
	DrawCone(cSp, cEp, 0.3 * dScale, 3 * dScale, cParentSegKey);

	if(false == strCurveName.IsEmpty()) {
		MbCartPoint cMidPoint;
		cCurve.GetMiddlePoint(cMidPoint);

		HPS::Point cHpsMidPoint;
		cHpsMidPoint.x = (float) cMidPoint.x;
		cHpsMidPoint.y = (float) cMidPoint.y;
		cHpsMidPoint.z = 0.0;

		DrawPlaneText(cHpsMidPoint, strCurveName, cParentSegKey);
	}

	return true;
}

// == Drawing ======================================================================================
bool DmiHpsCreator::DrawLoopMbSolid(MbMesh & cMesh, bool bAddStartPoint, SegmentKey & cParentSegKey)
{
	SegmentKey cPolygonSegment = cParentSegKey;
	//cPolygonSegment.GetLineAttributeControl().SetWeight(3);
	//cPolygonSegment.GetMaterialMappingControl().Set SetLineColor(RGBColor(0.0, 0.0, 1.0));

	for(size_t nGridIndex = 0; nGridIndex < cMesh.GridsCount(); nGridIndex++) {
		const MbGrid * pcGrid = cMesh.GetGrid(nGridIndex);
		ASSERT(pcGrid);

		size_t nPointCount = pcGrid->PointsCount();
		if(0 == nPointCount) {
			continue;
		}

		for(size_t nLoopIndex = 0; nLoopIndex < pcGrid->LoopsCount(); nLoopIndex++) {
			const MbGridLoop & cGridLoop = pcGrid->GetGridLoop(nLoopIndex);

			HPS::PointArray acPointArray;

			MbFloatPoint3D cPoint;

			for(size_t nGridLoopIndex = 0; nGridLoopIndex < cGridLoop.Count(); nGridLoopIndex++) {
				UINT nPointIndex = cGridLoop.GetIndex(nGridLoopIndex);
				pcGrid->GetPoint(nPointIndex, cPoint);

				HPS::Point cHpsPoint((float) cPoint.x, (float) cPoint.y, (float) cPoint.z);
				acPointArray.push_back(cHpsPoint);
			}

			if(true == bAddStartPoint) {
				acPointArray.push_back(acPointArray.front());
			}

			cPolygonSegment.InsertLine(acPointArray);
		}
	}

	return true;
}

//== Draw Text =====================================================================================
void DmiHpsCreator::DrawPlaneText(HPS::Point cPoint, CString strText, SegmentKey & cParentSegKey)
{
	HPS::UTF8 strUft8Text(strText);

	HPS::TextKit cTextKit;
	cTextKit.SetPosition(cPoint);
	cTextKit.SetFont("verdana");
	cTextKit.SetText(strUft8Text);
	cTextKit.SetColor(HPS::RGBColor(0, 0, 0));
	cTextKit.SetSize(15, HPS::Text::SizeUnits::Points);
	cTextKit.SetAlignment(HPS::Text::Alignment::Center);   // relative to insertion point

	cParentSegKey.InsertText(cTextKit);
	/*

		SegmentKey cTextSegment = cParentSegKey.Subsegment();
		cTextSegment.GetVisibilityControl().SetText(true);


		HPS::TextKey cTextKey = cTextSegment.InsertText(cPoint, static_cast<const char *>(strUft8Text));
		cTextKey.SetColor(RGBColor(1.0, 1.0, 1.0)).SetFont("stroked").SetBold(true).SetSize(5, HPS::Text::SizeUnits::Pixels);
	*/
}

// == Debug Drawing ================================================================================

// 1. Solid Debug Draw
void DmiHpsCreator::DebugDrwingSolid(const MbSolid * pcSolid, SegmentKey & cParentSegKey)
{
	MbVector3D cOffset;

	// Draw Surface
	for(size_t nIndex = 0; nIndex < pcSolid->GetFacesCount(); nIndex++) {
		MbFace * pcFace = pcSolid->GetFace(nIndex);

		DebugDrwingFace(pcFace, cOffset, cParentSegKey);

		const MbSurface & cSurface = pcFace->GetSurface();
		DebugDrawSurface(cSurface, cParentSegKey);

		cOffset.x += 0.3;

		const MbConeSurface & cConeSurface = static_cast<const MbConeSurface &>(pcFace->GetSurface());
	}
}

// 2. Face Debug Draw
void DmiHpsCreator::DebugDrwingFace(const MbFace * pcFace, MbVector3D & cOffset, SegmentKey & cParentSegKey)
{
	const MbSurface * pcFaceSurface = &pcFace->GetSurface();

	if(st_CurveBoundedSurface == pcFaceSurface->IsA()) {
		pcFaceSurface = &pcFaceSurface->GetBasisSurface();
	}

	bool bFaceSense = pcFace->IsSameSense();

	for(size_t nLoopIndex = 0; nLoopIndex < pcFace->GetLoopsCount(); nLoopIndex++) {
		MbLoop * pcLoop = pcFace->GetLoop(nLoopIndex);

		MbRect cRect = pcLoop->GetGabarit();
		double dLoopDiagonalDistance = cRect.GetDiagonal();

		for(size_t nEdgeIndex = 0; nEdgeIndex < pcLoop->GetEdgesCount(); nEdgeIndex++) {
// 			if(2 == nEdgeIndex) {
// 				continue;
// 			}

			MbOrientedEdge * pcOrientedEdge = pcLoop->GetOrientedEdge(nEdgeIndex);
			bool bOrientedEdgeSense = pcOrientedEdge->GetOrientation();

			if(false == bFaceSense) {
				bOrientedEdgeSense = !bOrientedEdgeSense;
			}

			MbCurveEdge & cCurveEdge = pcOrientedEdge->GetCurveEdge();

			CString strText;
			strText.Format(L"%d", nEdgeIndex);

			const MbSurfaceIntersectionCurve & cIntersectionCurve = cCurveEdge.GetIntersectionCurve();
			DebugCreateCurve3d(&cIntersectionCurve, bOrientedEdgeSense, strText, cOffset, cParentSegKey);

			//if(pcFaceSurface == &cIntersectionCurve.GetCurveOneSurface())
			{
				SegmentKey cOneCurveSegKey = cParentSegKey.Subsegment();

				const MbCurve & cCurveOne = cIntersectionCurve.GetCurveOneCurve();
				DebugDrawPlaneCurve(cCurveOne, bOrientedEdgeSense, strText, dLoopDiagonalDistance, RGB(255, 0, 0), cOneCurveSegKey);

				MbVector3D cOneOffset(0.5, 0.5, 0.5);
				DebugDrawLiftPlaneCurve(cCurveOne, &cIntersectionCurve.GetCurveOneSurface(), bOrientedEdgeSense, strText, 
					dLoopDiagonalDistance, RGB(255, 0, 0), cOneOffset, cOneCurveSegKey);

				//DebugDrawNurbsCurveControlPoint(cCurveOne, cOneCurveSegKey);
			}

			//if(pcFaceSurface == &cIntersectionCurve.GetCurveTwoSurface())
			{
				SegmentKey cTwoCurveSegKey = cParentSegKey.Subsegment();

				const MbCurve & cCurveTwo = cIntersectionCurve.GetCurveTwoCurve();
				DebugDrawPlaneCurve(cCurveTwo, bOrientedEdgeSense, strText, dLoopDiagonalDistance, RGB(0, 0, 255), cTwoCurveSegKey);

				MbVector3D cTwoOffset(-0.5, -0.5, -0.5);
				DebugDrawLiftPlaneCurve(cCurveTwo, &cIntersectionCurve.GetCurveTwoSurface(), bOrientedEdgeSense, strText, 
					dLoopDiagonalDistance, RGB(0, 0, 255), cTwoOffset, cTwoCurveSegKey);

				//DebugDrawNurbsCurveControlPoint(cCurveTwo, cTwoCurveSegKey);
			}
		}
	}
}

// 3. Surface Debug Draw
void DmiHpsCreator::DebugDrawSurface(const MbSurface & cSurface, SegmentKey & cParentSegKey)
{
	if(st_CurveBoundedSurface == cSurface.IsA()) {
		const MbCurveBoundedSurface & cCurveBoundedSurface = static_cast<const MbCurveBoundedSurface &>(cSurface);
		DebugDrawSurface(cCurveBoundedSurface.GetBasisSurface(), cParentSegKey);
	}
	else if(st_SplineSurface == cSurface.IsA()) {
		DebugDrawSplineSurfaceControlPoint(cSurface, cParentSegKey);
	}
}

// 3-1. Spline Surface control point Debug Draw
void DmiHpsCreator::DebugDrawSplineSurfaceControlPoint(const MbSurface & cSurface, HPS::SegmentKey & cParentSegKey)
{
	const MbSplineSurface & cSplineSurface = static_cast<const MbSplineSurface &>(cSurface);

	/*
		MbCurve3D * pcUCurve = cSplineSurface.CurveU(0.0, nullptr);
		CreateCurve3d(pcUCurve, cParentSegKey);
		DebugDrawCurveControlPoint(*pcUCurve, cParentSegKey);

		MbCurve3D * pcVCurve = cSplineSurface.CurveU(cSplineSurface.GetVMax(), nullptr);
		CreateCurve3d(pcVCurve, cParentSegKey);
		DebugDrawCurveControlPoint(*pcVCurve, cParentSegKey);

		return;
	*/

	MbControlData3D cCtrlData;
	cSplineSurface.GetBasisPoints(cCtrlData);

	size_t nShareCount = cCtrlData.ShareCount();
	size_t nTotalCount = cCtrlData.TotalCount();

	SegmentKey cDebugPointKey = cParentSegKey.Subsegment("DebugPoint");
	cDebugPointKey.GetVisibilityControl().SetMarkers(true);
	cDebugPointKey.GetMaterialMappingControl().SetMarkerColor(HPS::RGBColor(1.0f, 0.0f, 0.0f));

	HPS::MarkerKit cMakerKit;
	for(size_t nIndex = 0; nIndex < nShareCount; nIndex++) {
		MbCartPoint3D cPoint3d;
		cCtrlData.GetShare(nIndex, cPoint3d);
		cMakerKit.SetPoint(Point((float) cPoint3d.x, (float) cPoint3d.y, (float) cPoint3d.z));
		cDebugPointKey.InsertMarker(cMakerKit);
	}
}
// 4. Debug용 Curve Drawing
bool DmiHpsCreator::DebugCreateCurve3d(const MbCurve3D * pcCurve, bool bSense, CString strCurveName, MbVector3D & cOffset, SegmentKey & cParentSegKey)
{
	/*
		// 오류가 있는 경우에도 MbMesh 함수는 정상적인 Curve의 모양을 출력하는 경우가 있음.
		MbMesh cMesh;
		pcCurve->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

		if(0 == cMesh.PolygonsCount()) {
			ASSERT(false);
			return false;
		}
	*/
	int nStepCount = 200;

	HPS::PointArray acPointArray;

	double dMin = pcCurve->GetTMin();
	double dMax = pcCurve->GetTMax();

	double dStep = (dMax - dMin) / (double) (nStepCount - 1);

	double dParameter = dMin;

	for(int nIndex = 0; nIndex < nStepCount; nIndex++) {
		MbCartPoint3D cCartPoint;
		pcCurve->_PointOn(dParameter, cCartPoint);
		dParameter += dStep;

		cCartPoint += cOffset;

		HPS::Point cPoint((float) cCartPoint.x, (float) cCartPoint.y, (float) cCartPoint.z);
		acPointArray.push_back(cPoint);
	}

	cParentSegKey.InsertLine(acPointArray);

	double dLength = pcCurve->GetLengthEvaluation();

	CString strText;
	strText.Format(L": %f", dLength);
	strCurveName += strText;

	MbCartPoint3D cSp, cEp;
	HPS::Point cPoint;
	if(true == bSense) {
		cPoint = acPointArray[acPointArray.size() - 2];
		cSp.Init(cPoint.x, cPoint.y, cPoint.z);
		cPoint = acPointArray[acPointArray.size() - 1];
		cEp.Init(cPoint.x, cPoint.y, cPoint.z);
	}
	else {
		cPoint = acPointArray[1];
		cSp.Init(cPoint.x, cPoint.y, cPoint.z);
		cPoint = acPointArray[0];
		cEp.Init(cPoint.x, cPoint.y, cPoint.z);
	}

	double dScale = dLength / 80.0;
	DrawCone(cSp, cEp, 0.3 * dScale, 3 * dScale, cParentSegKey);

	if(false == strCurveName.IsEmpty()) {
		HPS::Point cHpsMidPoint = acPointArray[acPointArray.size() / 2];
		DrawPlaneText(cHpsMidPoint, strCurveName, cParentSegKey);
	}
	return true;
}

// 5. Debug용 Plance Curve Drawing
bool DmiHpsCreator::DebugDrawPlaneCurve(const MbCurve & cCurve, bool bSense, CString strCurveName,
	double dLoopDiagonalDistance, COLORREF nColor, SegmentKey & cParentSegKey)
{
	int nStepCount = 200;

	HPS::PointArray acPointArray;

	double dMin = cCurve.GetTMin();
	double dMax = cCurve.GetTMax();

	double dStep = (dMax - dMin) / (double) (nStepCount - 1);

	double dParameter = dMin;

	MbCartPoint cCartPoint1, cCartPoint2;
	cCurve._PointOn(dParameter, cCartPoint1);

	HPS::Point cPoint((float) cCartPoint1.x, (float) cCartPoint1.y, 0.0f);
	acPointArray.push_back(cPoint);

	double dLength = 0.0;

	for(int nIndex = 1; nIndex < nStepCount; nIndex++) {
		dParameter += dStep;

		cCurve._PointOn(dParameter, cCartPoint2);

		dLength += cCartPoint1.DistanceToPoint(cCartPoint2);

		cCartPoint1 = cCartPoint2;

		cPoint.x = (float) cCartPoint2.x;
		cPoint.y = (float) cCartPoint2.y;
		cPoint.z = 0.0f;

		acPointArray.push_back(cPoint);
	}

	cParentSegKey.InsertLine(acPointArray);

	HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nColor);
	cParentSegKey.GetMaterialMappingControl().SetLineColor(cRgbColor);

	MbCartPoint3D cSp, cEp;

	if(true == bSense) {
		cPoint = acPointArray[acPointArray.size() - 2];
		cSp.Init(cPoint.x, cPoint.y, cPoint.z);
		cPoint = acPointArray[acPointArray.size() - 1];
		cEp.Init(cPoint.x, cPoint.y, cPoint.z);
	}
	else {
		cPoint = acPointArray[1];
		cSp.Init(cPoint.x, cPoint.y, cPoint.z);
		cPoint = acPointArray[0];
		cEp.Init(cPoint.x, cPoint.y, cPoint.z);
	}

	double dArrowSize = cCurve.CalculateMetricLength();

	CString strText;
	strText.Format(L": %f, %f", dLength, dArrowSize);
	strCurveName += strText;

	if(dArrowSize < dLoopDiagonalDistance / 5.0) {
		dArrowSize = dLoopDiagonalDistance / 5.0;
	}

	double dScale = dArrowSize / 60.0;
	DrawCone(cSp, cEp, 0.3 * dScale, 3 * dScale, cParentSegKey);

	if(false == strCurveName.IsEmpty()) {
		MbCartPoint cMidPoint;
		cCurve._PointOn(cCurve.GetTMid(), cMidPoint);

		HPS::Point cHpsMidPoint = acPointArray[acPointArray.size() / 2];

		DrawPlaneText(cHpsMidPoint, strCurveName, cParentSegKey);
	}

	return true;
}

// 5-1. Plane Curve를 Point로 List해서 3D Curve를 그리는 함수.
void DmiHpsCreator::DebugDrawLiftPlaneCurve(const MbCurve & cCurve, const MbSurface * pcSurface, bool bSense, CString strCurveName,
	double dLoopDiagonalDistance, COLORREF nColor, MbVector3D cOffset, HPS::SegmentKey & cParentSegKey)
{
	int nStepCount = 200;

	HPS::PointArray acPointArray;

	double dMin = cCurve.GetTMin();
	double dMax = cCurve.GetTMax();

	double dStep = (dMax - dMin) / (double) (nStepCount - 1);

	double dParameter = dMin;

	double dLength = 0.0;

	MbCartPoint cCartPoint;
	cCurve._PointOn(dParameter, cCartPoint);

	MbCartPoint3D cLiftPoint;
	pcSurface->_PointOn(cCartPoint, cLiftPoint);
	cLiftPoint += cOffset;

	HPS::Point cPoint((float) cLiftPoint.x, (float) cLiftPoint.y, (float) cLiftPoint.z);
	acPointArray.push_back(cPoint);

	for(int nIndex = 1; nIndex < nStepCount; nIndex++) {
		dParameter += dStep;

		MbCartPoint cCartPoint1;
		cCurve._PointOn(dParameter, cCartPoint1);

		MbCartPoint3D cLiftPoint1;
		pcSurface->_PointOn(cCartPoint1, cLiftPoint1);
		cLiftPoint1 += cOffset;

		dLength += cLiftPoint.DistanceToPoint(cLiftPoint1);
		cLiftPoint = cLiftPoint1;

		HPS::Point cPoint1((float) cLiftPoint1.x, (float) cLiftPoint1.y, (float) cLiftPoint1.z);
		acPointArray.push_back(cPoint1);
	}

	cParentSegKey.InsertLine(acPointArray);

	CString strText;
	strText.Format(L": %f", dLength);
	strCurveName += strText;

	HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nColor);
	cParentSegKey.GetMaterialMappingControl().SetLineColor(cRgbColor);

	MbCartPoint3D cSp, cEp;
	if(true == bSense) {
		cPoint = acPointArray[acPointArray.size() - 2];
		cSp.Init(cPoint.x, cPoint.y, cPoint.z);
		cPoint = acPointArray[acPointArray.size() - 1];
		cEp.Init(cPoint.x, cPoint.y, cPoint.z);
	}
	else {
		cPoint = acPointArray[1];
		cSp.Init(cPoint.x, cPoint.y, cPoint.z);
		cPoint = acPointArray[0];
		cEp.Init(cPoint.x, cPoint.y, cPoint.z);
	}

	double dArrowSize = dLength;
	if(dArrowSize < dLoopDiagonalDistance / 5.0) {
		dArrowSize = dLoopDiagonalDistance / 5.0;
	}

	double dScale = dArrowSize / 60.0;
	DrawCone(cSp, cEp, 0.3 * dScale, 3 * dScale, cParentSegKey);

	if(false == strCurveName.IsEmpty()) {
		MbCartPoint cMidPoint;
		cCurve._PointOn(cCurve.GetTMid(), cMidPoint);

		HPS::Point cHpsMidPoint = acPointArray[acPointArray.size() / 2];

		DrawPlaneText(cHpsMidPoint, strCurveName, cParentSegKey);
	}
}

// 6. Curve control point Debug Draw
void DmiHpsCreator::DebugDrawCurveControlPoint(const MbCurve3D & cCurve, HPS::SegmentKey & cParentSegKey)
{
	SegmentKey cDebugPointKey = cParentSegKey.Subsegment("DebugPoint");
	cDebugPointKey.GetVisibilityControl().SetMarkers(true);
	cDebugPointKey.GetMaterialMappingControl().SetMarkerColor(HPS::RGBColor(0.0f, 0.0f, 1.0f));

	MbeSpaceType eType = cCurve.IsA();

	if(st_CubicSpline3D == eType) {
		DebugDrawCubicSplineCurveControlPoint(cCurve, cDebugPointKey);
	}
	else if(st_Nurbs3D == eType) {
		DebugDrawNurbs3DCurveControlPoint(cCurve, cDebugPointKey);
	}
}

// 6-1. Cubic spline curve control point Debug Draw
void DmiHpsCreator::DebugDrawCubicSplineCurveControlPoint(const MbCurve3D & cCurve, HPS::SegmentKey & cParentSegKey)
{
	const MbCubicSpline3D & cCubicSpline = static_cast<const MbCubicSpline3D &>(cCurve);

	HPS::MarkerKit cMakerKit;
	for(size_t nIndex = 0; nIndex < cCubicSpline.GetPointListCount(); nIndex++) {
		MbCartPoint3D cPoint3d;
		cCubicSpline.GetPoint(nIndex, cPoint3d);
		cMakerKit.SetPoint(Point((float) cPoint3d.x, (float) cPoint3d.y, (float) cPoint3d.z));
		cParentSegKey.InsertMarker(cMakerKit);
	}
}

// 6-2. NURBS3D curve control point Debug Draw
void DmiHpsCreator::DebugDrawNurbs3DCurveControlPoint(const MbCurve3D & cCurve, HPS::SegmentKey & cParentSegKey)
{
	const MbNurbs3D & cNurbs3d = static_cast<const MbNurbs3D &>(cCurve);

	MbControlData3D cCtrlData;
	cNurbs3d.GetBasisPoints(cCtrlData);

	size_t nShareCount = cCtrlData.ShareCount();
	size_t nTotalCount = cCtrlData.TotalCount();

	HPS::MarkerKit cMakerKit;
	for(size_t nIndex = 0; nIndex < nShareCount; nIndex++) {
		MbCartPoint3D cPoint3d;
		cCtrlData.GetShare(nIndex, cPoint3d);
		cMakerKit.SetPoint(Point((float) cPoint3d.x, (float) cPoint3d.y, (float) cPoint3d.z));
		cParentSegKey.InsertMarker(cMakerKit);
	}
}

// 6-3. NURBS curve control point Debug Draw
void DmiHpsCreator::DebugDrawNurbsCurveControlPoint(const MbCurve & cCurve, HPS::SegmentKey & cParentSegKey)
{
	const MbNurbs * pcNurbs3d = dynamic_cast<const MbNurbs *>(&cCurve);
	if(nullptr == pcNurbs3d) {
		return;
	}

	cParentSegKey.GetVisibilityControl().SetMarkers(true);
	cParentSegKey.GetMaterialMappingControl().SetMarkerColor(HPS::RGBColor(1.0f, 0.0f, 0.0f));

	MbControlData cCtrlData;
	pcNurbs3d->GetBasisPoints(cCtrlData);

	size_t nShareCount = cCtrlData.ShareCount();
	size_t nTotalCount = cCtrlData.TotalCount();

	HPS::MarkerKit cMakerKit;
	for(size_t nIndex = 0; nIndex < nShareCount; nIndex++) {
		MbCartPoint cPoint3d;
		cCtrlData.GetShare(nIndex, cPoint3d);
		cMakerKit.SetPoint(Point((float) cPoint3d.x, (float) cPoint3d.y, 0.0f));
		cParentSegKey.InsertMarker(cMakerKit);
	}
}

void DmiHpsCreator::DrawMbFaceEdgeCurve(const MbFace * pcFace)
{
	// Edge Segement를 Open
	SegmentKey cEdgeSegment = m_pcModelHanler->GetModel().GetSegmentKey().Subsegment();
	cEdgeSegment.GetLineAttributeControl().SetWeight(3);

	size_t nLoopsCount = pcFace->GetLoopsCount();
	for(size_t nLoopIndex = 0; nLoopIndex < nLoopsCount; nLoopIndex++) {
		MbLoop * pcLoop = pcFace->GetLoop(nLoopIndex);
		size_t nEdgeCount = pcLoop->GetEdgesCount();
		for(size_t nEdgeIndex = 0; nEdgeIndex < nEdgeCount; nEdgeIndex++) {
			MbOrientedEdge * pcOrientedEdge = pcLoop->GetOrientedEdge(nEdgeIndex);
			if(nullptr == pcOrientedEdge) {
				continue;
			}

			MbCurveEdge * pcCurveEdge = &pcOrientedEdge->GetCurveEdge();
			if(nullptr == pcCurveEdge) {
				continue;
			}

			bool bExactMeshFlag = false;
			MbMesh cMesh;
			pcCurveEdge->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

			bool bEdgeSense = pcOrientedEdge->GetOrientation();

			HPS::PointArray acPointArray;

			for(size_t nIndex = 0; nIndex < cMesh.PolygonsCount(); nIndex++) {
				const MbPolygon3D * pcPolygon = cMesh.GetPolygon(nIndex);

				std::vector<MbCartPoint3D> vcPointVector;
				pcPolygon->GetPoints(vcPointVector);

				if(true == vcPointVector.empty()) {
					continue;
				}

				for(const MbCartPoint3D & cPolygonPoint : vcPointVector) {

					HPS::Point cPoint((float) cPolygonPoint.x, (float) cPolygonPoint.y, (float) cPolygonPoint.z);
					acPointArray.push_back(cPoint);
				}

				LineKey cLineKey = cEdgeSegment.InsertLine(acPointArray);

				MbCartPoint3D cSp, cEp;

				if(true == bEdgeSense) {
					cSp = vcPointVector[vcPointVector.size() - 2];
					cEp = vcPointVector[vcPointVector.size() - 1];
				}
				else {
					cSp = vcPointVector[1];
					cEp = vcPointVector[0];
				}

				double dScale = 0.1;
				DrawCone(cSp, cEp, 0.3 * dScale, 3 * dScale, cEdgeSegment);
			}
		}
	}
}

void DmiHpsCreator::DrawCone(MbCartPoint3D & cSp, MbCartPoint3D & cEp, double dRadius, double dHeight, SegmentKey & cParentSegKey)
{
	MbVector3D cVec = cSp - cEp;
	cVec.Normalize();
	cSp = cEp + cVec * dHeight;

	Point cPoints[2];
	GetHpsPoint(cSp, cPoints[0]);
	GetHpsPoint(cEp, cPoints[1]);

	float fRadial[2];
	fRadial[0] = (float) dRadius;
	fRadial[1] = 0.0f;

	HPS::CylinderKit cylinderKit;
	cylinderKit.SetPoints(2, cPoints);
	cylinderKit.SetRadii(2, fRadial);
	cylinderKit.SetCaps(HPS::Cylinder::Capping::First);

	cParentSegKey.InsertCylinder(cylinderKit);
}

// == Utility Function =============================================================================

bool DmiHpsCreator::GetItemText(const MbItem * pcItem, c3d::string_t strId, c3d::string_t & strValue)
{
	AttrVector vcAttrVector;
	pcItem->GetCommonAttributes(vcAttrVector, strId, at_StringAttribute);
	if(1 == vcAttrVector.size()) {
		strValue = static_cast<MbStringAttribute *>(vcAttrVector[0])->GetValue();
		return true;
	}

	return false;
}

void DmiHpsCreator::GetMatrix(MbMatrix3D & cMatrix, HPS::MatrixKit & cHpsMatrix)
{
	MbVector3D cAxis = cMatrix.GetAxisX();
	cHpsMatrix.data[0] = (float) cAxis.x, cHpsMatrix.data[1] = (float) cAxis.y, cHpsMatrix.data[2] = (float) cAxis.z;

	cAxis = cMatrix.GetAxisY();
	cHpsMatrix.data[4] = (float) cAxis.x, cHpsMatrix.data[5] = (float) cAxis.y, cHpsMatrix.data[6] = (float) cAxis.z;

	cAxis = cMatrix.GetAxisZ();
	cHpsMatrix.data[8] = (float) cAxis.x, cHpsMatrix.data[9] = (float) cAxis.y, cHpsMatrix.data[10] = (float) cAxis.z;

	MbCartPoint3D cOrigin = cMatrix.GetOrigin();
	cHpsMatrix.data[12] = (float) cOrigin.x, cHpsMatrix.data[13] = (float) cOrigin.y, cHpsMatrix.data[14] = (float) cOrigin.z;
}

void DmiHpsCreator::GetMatrix(MbPlacement3D & cPlacement, HPS::MatrixKit & cHpsMatrix)
{
	MbVector3D cAxis = cPlacement.GetAxisX();
	cHpsMatrix.data[0] = (float) cAxis.x, cHpsMatrix.data[1] = (float) cAxis.y, cHpsMatrix.data[2] = (float) cAxis.z;

	cAxis = cPlacement.GetAxisY();
	cHpsMatrix.data[4] = (float) cAxis.x, cHpsMatrix.data[5] = (float) cAxis.y, cHpsMatrix.data[6] = (float) cAxis.z;

	cAxis = cPlacement.GetAxisZ();
	cHpsMatrix.data[8] = (float) cAxis.x, cHpsMatrix.data[9] = (float) cAxis.y, cHpsMatrix.data[10] = (float) cAxis.z;

	MbCartPoint3D cOrigin = cPlacement.GetOrigin();
	cHpsMatrix.data[12] = (float) cOrigin.x, cHpsMatrix.data[13] = (float) cOrigin.y, cHpsMatrix.data[14] = (float) cOrigin.z;
}

void DmiHpsCreator::SetSegementLinePattern(SegmentKey & cSegKey, int nLinePatter)
{
	char const * pchLinePatternString = nullptr;

	switch((HPS::LinePattern::Default) nLinePatter)
	{
		case HPS::LinePattern::Default::LongDash:
			pchLinePatternString = LONG_DASH_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dotted:
			pchLinePatternString = DOTTED_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dashed:
			pchLinePatternString = DASHED_LN_PAT;
			break;

		case HPS::LinePattern::Default::DashDot:
			pchLinePatternString = DASH_DOT_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dash2Dot:
			pchLinePatternString = DASH_2DOT_LN_PAT;
			break;

		default:
			return;
			break;
	}

	cSegKey.GetPortfolioControl().Push(m_cPortfolio);
	cSegKey.GetLineAttributeControl().SetPattern(pchLinePatternString);
}

void DmiHpsCreator::GetHpsPoint(MbCartPoint3D & cCartPoint, HPS::Point & cPoint)
{
	cPoint.x = (float) cCartPoint.x;
	cPoint.y = (float) cCartPoint.y;
	cPoint.z = (float) cCartPoint.z;
}

void DmiHpsCreator::CreateLog(int nId, const WCHAR * pchFilePathName)
{
	LogManager::SetCurrentId(nId);

	LogManager::SetFilePathName(nId, pchFilePathName);
	LogManager::SetCreateFile(nId, true);
	LogManager::SetWriteLog(nId, true);
	LogManager::ResetTabIndex(nId);

	LogManager::SetWriteTimeLog(nId, true);
	LogManager::Log(nId, L"Create Log");
	LogManager::SetWriteTimeLog(nId, false);
}

//== OLD Version ===================================================================================
bool DmiHpsCreator::CreateSolidThread(size_t nIndex, const MbSolid * pcSolid, SegmentKey & cParentSegKey)
{
	if(nullptr != pcSolid && nullptr != m_pcModelHanler) {
		RETURN_FALSE;
	}

	auto cIterator = m_mSolidMap.find(pcSolid);
	if(m_mSolidMap.end() != cIterator) {
		SegmentKey cIncludeSolidSegment = cIterator->second;
		cParentSegKey.IncludeSegment(cIncludeSolidSegment);
		LogManager::Log(L"Solid IncludeSegment");
		return true;
	}

	std::string strOldParent = m_strParent;
	if(nullptr != m_pvstrErrMsgVector) {
		m_strParent = std::format(" /  Solid[{}]", nIndex) + m_strParent;
	}

	// Assy Name
	string_t strName;
	GetItemText(pcSolid, L"name", strName);

	auto strSolidId = std::format("solid{}", m_nPartNoId++);
	SegmentKey cIncludeSolidSegment = m_cSolidIncludeLib.Subsegment(strSolidId.c_str());

	cParentSegKey.IncludeSegment(cIncludeSolidSegment);
	m_mSolidMap.insert(std::make_pair(pcSolid, cIncludeSolidSegment));

	LogManager::Log(L"Create IncludeSegment");

	/*
		MbMesh cMesh;
		pcSolid->CalculateMesh(m_cStepData, m_cFormNote, cMesh);

		size_t nGridsCount = cMesh.GridsCount();
	*/

	// Color 설정값 가져오기
	bool bSolidColoredFlsg = pcSolid->IsColored();
	COLORREF nSolidColor = pcSolid->GetColor();
	if(true == bSolidColoredFlsg) {
		HPS::RGBColor cRgbColor = DmiHps::GetRgbColor(nSolidColor);
		cIncludeSolidSegment.GetMaterialMappingControl().SetFaceColor(cRgbColor);
	}

	// Visual 설정값 가져오기
	bool bSolidVisualedFlag = (nullptr != pcSolid->GetSimpleAttribute(at_Visual)) ? true : false;
	float a, d, sp, sh, fSolidTransparency, e;
	pcSolid->GetVisual(a, d, sp, sh, fSolidTransparency, e);

	if(true == bSolidVisualedFlag) {
		cIncludeSolidSegment.GetMaterialMappingControl().SetFaceAlpha(fSolidTransparency);
	}

	/*
		size_t nFacesCount = pcSolid->GetFacesCount();
		ASSERT(nGridsCount == nFacesCount);
	*/

	bool bFaceColorizingFlag = true;

	/*
		// 수량이 같아야 Color를 입히도록 한다.
		if(nGridsCount == nFacesCount) {
			bFaceColorizingFlag = true;
		}
	*/

	CString strText;
	CString strError;

	for(int nIndex = 0; nIndex < pcSolid->GetFacesCount(); nIndex++) {
		MbFace * pcFace = pcSolid->GetFace(nIndex);
		MbMesh cMesh;
		pcFace->CalculateMesh(m_cStepData, m_cFormNote, cMesh);
		size_t nCount = cMesh.GridsCount();

		for(size_t nGridIndex = 0; nGridIndex < cMesh.GridsCount(); nGridIndex++)
		{
			const MbGrid * pcGrid = cMesh.GetGrid(nGridIndex);
			ASSERT(pcGrid);

			if(false == CreateGrid(pcGrid, pcSolid->GetFace(nGridIndex), bSolidColoredFlsg, cIncludeSolidSegment)) {
				// 오류가 난 경우 표시 해당 항목 표시
				const MbFace * pcFace = pcSolid->GetFace(nGridIndex);
				DrawMbFaceEdgeCurve(pcFace);

				if(nullptr != m_pvstrErrMsgVector) {
					size_t nPointCount = pcGrid->PointsCount();
					size_t nTrianglesCount = pcGrid->TrianglesCount();
					size_t nQuadranglesCount = pcGrid->QuadranglesCount();

					std::string strError = std::format("[{}] Point: {}, FaceList: {}", nGridIndex,
						nPointCount, nTrianglesCount + nQuadranglesCount, ToSTDstring(strName)) + m_strParent;
					m_pvstrErrMsgVector->push_back(strError);
				}
				continue;
			}
		}
	}

	/*
		for(size_t nGridIndex = 0; nGridIndex < cMesh.GridsCount(); nGridIndex++)
		{
			const MbGrid * pcGrid = cMesh.GetGrid(nGridIndex);
			ASSERT(pcGrid);

			if(false == CreateGrid(pcGrid, pcSolid->GetFace(nGridIndex), bSolidColoredFlsg, cIncludeSolidSegment)) {
				// 오류가 난 경우 표시 해당 항목 표시
				const MbFace * pcFace = pcSolid->GetFace(nGridIndex);
				DrawMbFaceEdgeCurve(pcFace);

				if(nullptr != m_pvstrErrMsgVector) {
					size_t nPointCount = pcGrid->PointsCount();
					size_t nTrianglesCount = pcGrid->TrianglesCount();
					size_t nQuadranglesCount = pcGrid->QuadranglesCount();

					std::string strError = std::format("[{}] Point: {}, FaceList: {}", nGridIndex,
						nPointCount, nTrianglesCount + nQuadranglesCount, ToSTDstring(strName)) + m_strParent;
					m_pvstrErrMsgVector->push_back(strError);
				}
				continue;
			}
		}

		DrawLoopMbSolid(cMesh, cIncludeSolidSegment);
	*/

	m_strParent = strOldParent;

#ifdef _DEBUG_DRAW2_
	// DebugDrwingSolid(pcSolid, cIncludeSolidSegment);
#endif
	return true;
}
