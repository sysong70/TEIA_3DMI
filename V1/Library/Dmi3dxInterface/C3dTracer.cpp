#include "stdafx.h"

#include "C3dTracer.h"

// CString용 Header
#include <atlstr.h>

// 꼭 추가되어야 Compile 됨.
#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <topology.h>
#include <model.h>
#include <assembly.h>
#include <solid.h>

#include <surf_plane.h>
#include <surf_cone_surface.h>
#include <surf_cylinder_surface.h>
#include <surf_curve_bounded_surface.h>
#include <surf_sphere_surface.h>
#include <surf_torus_surface.h>
#include <surf_spline_surface.h>
#include <surf_revolution_surface.h>
#include <surf_extrusion_surface.h>

#include <cur_arc.h>
#include <cur_surface_intersection.h>
#include <cur_hermit.h>
#include <cur_nurbs.h>
#include <cur_reparam_curve.h>

#include <mb_placement3d.h>
#include <mb_placement.h>
#include <mb_vector3d.h>

#include <format>

#include "LogManager.h"

//#define _LOG_NURBS_CURVE_DETAIL_LOG_

using namespace std;

namespace C3dTracer
{
	CString DblStr(double dValue, int nDigit = 8);
	CString HexStr(DWORD_PTR nValue);
	CString BoolStr(bool bValue);
	CString GetCartPointString(const MbCartPoint & cPoint);
	CString GetCartPoint3DString(const MbCartPoint3D & cPoint);
	CString GetVectorString(const MbVector & cVector);
	CString GetVector3DString(const MbVector3D & cVector);
	// 주어진 Face의 Index String을 가져온다.
	CString GetFaceIndexString(const MbFace * pcFace);
	// 주어진 Surface를 Face vector에서 검색해서 Face의 Index String을 가져온다.
	CString GetFaceIndexOfSurfaceString(const MbSurface & cSurface);

	std::vector<const MbFace *> m_vpcFaceVector;
};

// == Topology 관련 정보 분석 ========================================================================
void C3dTracer::CreateLog(WCHAR * pchFilePathName)
{
	LogManager::SetCurrentId(0);
	LogManager::SetFilePathName(pchFilePathName);
	LogManager::SetCreateFile(true);
	LogManager::SetWriteLog(true);
	LogManager::SetWriteTimeLog(false);
}

// Model 관련 정보 분석
void C3dTracer::ModelInformation(const MbModel & cModel, WCHAR * pchFilePathName, WCHAR * pchCadFileName)
{
	LogManager::SetFilePathName(pchFilePathName);
	LogManager::SetCreateFile(true);
	LogManager::SetWriteLog(true);
	LogManager::SetWriteTimeLog(true);
	LogManager::Log(L"CAD File: %s, MbModel: %s", pchCadFileName, HexStr((DWORD_PTR) &cModel));
	LogManager::SetWriteTimeLog(false);

	auto cIterator = cModel.CBegin();

	while(cIterator != cModel.CEnd()) {
		const MbItem * pcItem = *cIterator;
		++cIterator;

		ItemInformation(pcItem);
	}
}

void C3dTracer::AssemblyInformation(const MbAssembly & cAssembly)
{
	LogManager::Log(L"MbAssembly: %s", HexStr((DWORD_PTR) &cAssembly));

	LogManager::IncreaseTabIndex();

	size_t nItemsCount = cAssembly.ItemsCount();
	LogManager::Log(L"Items Count: %d", nItemsCount);

	for(size_t nItemsIndex = 0; nItemsIndex < nItemsCount; nItemsIndex++) {
		const MbItem * pcItem = cAssembly.GetItem(nItemsIndex);
		ItemInformation(pcItem);
	}

	LogManager::DecreaseTabIndex();
}

// Solid 관련 정보 분석
void C3dTracer::SolidInformation(const MbSolid & cSolid, const WCHAR * pchFilePathName, WCHAR * pchCadFileName)
{
	int nOldCurrentId = LogManager::GetCurrentId();
	LogManager::SetCurrentId(5);

	if(nullptr != pchFilePathName) {
		LogManager::SetFilePathName(pchFilePathName);
		LogManager::SetCreateFile(true);
		LogManager::SetWriteLog(true);
		LogManager::SetWriteTimeLog(true);
	}

	LogManager::ResetTabIndex();
	// 	LogManager::SetFilePathName(L"Z:\\SolidInfo.log");
	// 	LogManager::SetCreateFile(true);
	// 	LogManager::SetWriteLog(true);
	//	LogManager::SetWriteTimeLog(true);
	if(nullptr == pchCadFileName) {
		LogManager::Log(L"MbSolid: %s", HexStr((DWORD_PTR) &cSolid));
	}
	else {
		LogManager::Log(L"CAD File: %s, MbSolid: %s", pchCadFileName, HexStr((DWORD_PTR) &cSolid));
	}


	if(nullptr != pchFilePathName) {
		LogManager::SetWriteTimeLog(false);
	}

	size_t nFacesCount = cSolid.GetFacesCount();
	LogManager::Log(L"Faces Count: %d", nFacesCount);

	m_vpcFaceVector.clear();
	for(size_t nFaceIndex = 0; nFaceIndex < cSolid.GetFacesCount(); nFaceIndex++) {
		const MbFace * pcFace = cSolid.GetFace(nFaceIndex);
		m_vpcFaceVector.push_back(pcFace);
	}

	for(size_t nFaceIndex = 0; nFaceIndex < cSolid.GetFacesCount(); nFaceIndex++) {
		const MbFace * pcFace = cSolid.GetFace(nFaceIndex);
		bool bSameSense = pcFace->IsSameSense();
		LogManager::Log(L"MbFace [%d]: %s, SameSense: %s", nFaceIndex, GetFaceIndexString(pcFace), BoolStr(bSameSense));

		LogManager::IncreaseTabIndex();
		{
			SpaceItemInformation(pcFace->GetSurface());

			size_t nLoopsCount = pcFace->GetLoopsCount();
			for(size_t nLoopIndex = 0; nLoopIndex < nLoopsCount; nLoopIndex++) {
				MbLoop * pcLoop = pcFace->GetLoop(nLoopIndex);

				LogManager::Log(L"MbLoop [%d]: %s, Count: %d", nLoopIndex, HexStr((DWORD_PTR) pcLoop), nLoopsCount);

				LogManager::IncreaseTabIndex();
				{
					size_t nEdgeCount = pcLoop->GetEdgesCount();
					for(size_t nEdgeIndex = 0; nEdgeIndex < nEdgeCount; nEdgeIndex++) {
						MbOrientedEdge * pcOrientedEdge = pcLoop->GetOrientedEdge(nEdgeIndex);
						OrientedEdgeInformation(pcOrientedEdge, nEdgeIndex, pcFace->GetSurface());
					}
				}
				LogManager::DecreaseTabIndex();
			}
		}
		LogManager::DecreaseTabIndex();
	}
}

void C3dTracer::SolidInformation(const MbSolid & cSolid)
{
	size_t nFacesCount = cSolid.GetFacesCount();
	LogManager::Log(L"MbSolid: % s / Faces Count: %d", HexStr((DWORD_PTR) &cSolid), nFacesCount);

	m_vpcFaceVector.clear();
	for(size_t nFaceIndex = 0; nFaceIndex < cSolid.GetFacesCount(); nFaceIndex++) {
		const MbFace * pcFace = cSolid.GetFace(nFaceIndex);
		m_vpcFaceVector.push_back(pcFace);
	}

	for(size_t nFaceIndex = 0; nFaceIndex < cSolid.GetFacesCount(); nFaceIndex++) {
		const MbFace * pcFace = cSolid.GetFace(nFaceIndex);
		bool bSameSense = pcFace->IsSameSense();
		LogManager::Log(L"MbFace [%d]: %s, SameSense: %s", nFaceIndex, GetFaceIndexString(pcFace), BoolStr(bSameSense));

		LogManager::IncreaseTabIndex();
		{
			SpaceItemInformation(pcFace->GetSurface());

			size_t nLoopsCount = pcFace->GetLoopsCount();
			for(size_t nLoopIndex = 0; nLoopIndex < nLoopsCount; nLoopIndex++) {
				MbLoop * pcLoop = pcFace->GetLoop(nLoopIndex);

				LogManager::Log(L"MbLoop [%d]: %s, Count: %d", nLoopIndex, HexStr((DWORD_PTR) pcLoop), nLoopsCount);

				LogManager::IncreaseTabIndex();
				{
					size_t nEdgeCount = pcLoop->GetEdgesCount();
					for(size_t nEdgeIndex = 0; nEdgeIndex < nEdgeCount; nEdgeIndex++) {
						MbOrientedEdge * pcOrientedEdge = pcLoop->GetOrientedEdge(nEdgeIndex);
						OrientedEdgeInformation(pcOrientedEdge, nEdgeIndex, pcFace->GetSurface());
					}
				}
				LogManager::DecreaseTabIndex();
			}
		}
		LogManager::DecreaseTabIndex();
	}

	LogManager::DecreaseTabIndex();
}

void C3dTracer::FaceInformation(const MbFace * pcFace, const WCHAR * pchFilePathName, WCHAR * pchCadFileName)
{
	int nOldCurrnetId = LogManager::GetCurrentId();
	LogManager::SetCurrentId(4);

	if(nullptr != pchFilePathName) {
		LogManager::SetFilePathName(pchFilePathName);
		LogManager::SetCreateFile(true);
		LogManager::SetWriteLog(true);
		LogManager::SetWriteTimeLog(true);

	}

	LogManager::ResetTabIndex();
	// 	LogManager::SetFilePathName(L"Z:\\SolidInfo.log");
	// 	LogManager::SetCreateFile(true);
	// 	LogManager::SetWriteLog(true);
	//	LogManager::SetWriteTimeLog(true);
	if(nullptr == pchCadFileName) {
		LogManager::Log(L"MbFace: %s", HexStr((DWORD_PTR) pcFace));
	}
	else {
		LogManager::Log(L"CAD File: %s, MbFace: %s", pchCadFileName, HexStr((DWORD_PTR) pcFace));
	}

	if(nullptr != pchFilePathName) {
		LogManager::SetWriteTimeLog(false);
	}

	bool bSameSense = pcFace->IsSameSense();
	LogManager::Log(L"MbFace: SameSense: %s", BoolStr(bSameSense));

	LogManager::IncreaseTabIndex();
	{
		SpaceItemInformation(pcFace->GetSurface());

		size_t nLoopsCount = pcFace->GetLoopsCount();
		for(size_t nLoopIndex = 0; nLoopIndex < nLoopsCount; nLoopIndex++) {
			MbLoop * pcLoop = pcFace->GetLoop(nLoopIndex);

			LogManager::Log(L"MbLoop [%d]: %s, Count: %d", nLoopIndex, HexStr((DWORD_PTR) pcLoop), nLoopsCount);

			LogManager::IncreaseTabIndex();
			{
				size_t nEdgeCount = pcLoop->GetEdgesCount();
				for(size_t nEdgeIndex = 0; nEdgeIndex < nEdgeCount; nEdgeIndex++) {
					MbOrientedEdge * pcOrientedEdge = pcLoop->GetOrientedEdge(nEdgeIndex);
					OrientedEdgeInformation(pcOrientedEdge, nEdgeIndex, pcFace->GetSurface());
				}
			}
			LogManager::DecreaseTabIndex();
		}
	}
	LogManager::DecreaseTabIndex();

	LogManager::SetCurrentId(nOldCurrnetId);
}

// Edge 관련 정보 분석
void C3dTracer::OrientedEdgeInformation(MbOrientedEdge * pcOrientedEdge, size_t nEdgeIndex, const MbSurface & cSurface)
{
	bool bOrientation = pcOrientedEdge->GetOrientation();
	LogManager::Log(L"MbOrientedEdge [%d]: %s, Orientation: %s", nEdgeIndex, HexStr((DWORD_PTR) pcOrientedEdge), BoolStr(bOrientation));

	LogManager::IncreaseTabIndex();
	{
		MbCartPoint3D cBegPoint, cEndPoint, cMidPoint;
		pcOrientedEdge->GetBegPoint(cBegPoint);
		pcOrientedEdge->GetEndPoint(cEndPoint);
		pcOrientedEdge->Point(0.0, cBegPoint);
		pcOrientedEdge->Point(0.5, cMidPoint);
		pcOrientedEdge->Point(1.0, cEndPoint);
		//LogManager::Log(L"Point: %s / %s / %s", GetCartPoint3DString(cBegPoint), GetCartPoint3DString(cMidPoint), GetCartPoint3DString(cEndPoint));
		LogManager::Log(L"Point: %s / %s", GetCartPoint3DString(cBegPoint), GetCartPoint3DString(cEndPoint));

		VertexInformation(pcOrientedEdge->GetBegVertex());
		VertexInformation(pcOrientedEdge->GetEndVertex());

		// 	LogManager::DecreaseTabIndex();
		// 	return;

		MbCurveEdge * pcCurveEdge = &pcOrientedEdge->GetCurveEdge();
		bool bSameSense = pcCurveEdge->IsSameSense();
		LogManager::Log(L"MbCurveEdge: %s, SameSense: %s", HexStr((DWORD_PTR) pcCurveEdge), BoolStr(bSameSense));

		LogManager::IncreaseTabIndex();
		{
			MbCartPoint3D cBegPoint, cEndPoint;
			pcCurveEdge->GetBegPoint(cBegPoint);
			pcCurveEdge->GetEndPoint(cEndPoint);

			// Curve Edge Vertex
// 			VertexInformation(pcCurveEdge->GetBegVertex());
// 			VertexInformation(pcCurveEdge->GetEndVertex());

			const MbFace * pcFacePlus = pcCurveEdge->GetFacePlus();
			const MbFace * pcFaceMinus = pcCurveEdge->GetFaceMinus();

			LogManager::Log(L"FacePlus: %s, FaceMinus: %s", GetFaceIndexString(pcFacePlus), GetFaceIndexString(pcFaceMinus));

			MbeSpaceType eSpaceType = pcCurveEdge->GetCurve().IsA();

			const MbSurfaceIntersectionCurve * pcInterCurve = (MbSurfaceIntersectionCurve *) &pcCurveEdge->GetCurve();

			CString strBuildType;
			switch(pcInterCurve->GetBuildType())
			{
				case cbt_Boundary:
					strBuildType = L"Boundary";
					break;

				case cbt_Ordinary:
					strBuildType = L"Ordinary";
					break;

				case cbt_Specific:
					strBuildType = L"Specific";
					break;

				case cbt_Tolerant:
					strBuildType = L"Tolerant";
					break;
			}

			CString strGlueType;
			switch(pcInterCurve->GetGlueType())
			{
				case cgt_Unknown:
					strGlueType = L"Unknown";
					break;

				case cgt_Pole:
					strGlueType = L"Pole";
					break;

				case cgt_Edge:
					strGlueType = L"Edge";
					break;

				case cgt_Stitch:
					strGlueType = L"Stitch";
					break;

				case cgt_Split:
					strGlueType = L"Split";
					break;
			}

			LogManager::Log(L"MbSurfaceIntersectionCurve: %s, BuildType: %s, GlueType: %s", HexStr((DWORD_PTR) pcInterCurve), strBuildType, strGlueType);

			LogManager::IncreaseTabIndex();
			{
				/*
								MbCartPoint3D cLimitPoint[2];
								pcInterCurve->GetLimitPoint(1, cLimitPoint[0]);
								pcInterCurve->GetLimitPoint(2, cLimitPoint[1]);
								LogManager::Log(L"LimitPoint: %s ~ %s", GetCartPoint3DString(cLimitPoint[0]), GetCartPoint3DString(cLimitPoint[1]));

								LogManager::Log(L"SpaceCurve: %s", HexStr((DWORD_PTR) pcInterCurve->GetSpaceCurve()));
								MbeSpaceType eSpaceCurveType = pcInterCurve->GetSpaceCurve()->IsA();
				*/
				//pcInterCurve->GetSpaceCurve();

				//LogManager::Log(L"MetricLength: %s", DblStr(pcInterCurve->GetMetricLength()));

/*
				bool bSameOneFlag = (0.0 > ((C3dTracerSurfaceCurve *) &pcInterCurve->GetCurveOne())->GetOriginLengthEvaluation()) ? false : true;
				bool bSameTwoFlag = (0.0 > ((C3dTracerSurfaceCurve *) &pcInterCurve->GetCurveTwo())->GetOriginLengthEvaluation()) ? false : true;

				LogManager::Log(L"SameOne: %s, SameTwo: %s", BoolStr(bSameOneFlag), BoolStr(bSameTwoFlag));
*/
// Surface Type
				MbeSpaceType eSpaceType1 = pcInterCurve->GetSurfaceOne()->IsA();
				MbeSpaceType eSpaceType2 = pcInterCurve->GetSurfaceTwo()->IsA();

				const MbCurve & cCurve1 = pcInterCurve->GetCurveOneCurve();
				const MbCurve & cCurve2 = pcInterCurve->GetCurveTwoCurve();

				CString strComment;

				LogManager::Log(L"Curve One");
				LogManager::IncreaseTabIndex();
				{
					strComment.Format(L"Base: %s, %s, %s", GetSpaceItemNameString(*pcInterCurve->GetSurfaceOne()).c_str(),
						HexStr((DWORD_PTR) pcInterCurve->GetSurfaceOne()), GetFaceIndexOfSurfaceString(*pcInterCurve->GetSurfaceOne()));
					LogManager::SetComment(strComment);
					PlaneItemInformation(cCurve1, *pcInterCurve->GetSurfaceOne());
				}
				LogManager::DecreaseTabIndex();

				LogManager::Log(L"Curve Two");
				LogManager::IncreaseTabIndex();
				{
					strComment.Format(L"Base: %s, %s, %s", GetSpaceItemNameString(*pcInterCurve->GetSurfaceTwo()).c_str(),
						HexStr((DWORD_PTR) pcInterCurve->GetSurfaceTwo()), GetFaceIndexOfSurfaceString(*pcInterCurve->GetSurfaceTwo()));
					LogManager::SetComment(strComment);
					PlaneItemInformation(cCurve2, *pcInterCurve->GetSurfaceTwo());
				}
				LogManager::DecreaseTabIndex();
			}
			LogManager::DecreaseTabIndex();
		}
		LogManager::DecreaseTabIndex();
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::VertexInformation(const MbVertex & cVertex)
{
	const MbCartPoint3D & cCartPoint = cVertex.GetCartPoint();
	LogManager::Log(L"MbVertex: %s, %s", HexStr((DWORD_PTR) &cVertex), GetCartPoint3DString(cCartPoint));
}

void C3dTracer::ItemInformation(const MbItem * pcItem)
{
	MbeSpaceType cSpaceType = pcItem->IsA();

	switch(cSpaceType)
	{
		case st_Assembly:
			AssemblyInformation(*(const MbAssembly *) pcItem);
			break;

		case st_Solid:
			SolidInformation(*(const MbSolid *) pcItem);
			break;

		default:
			break;
	}
}

// == Geometry 관련 정보 분석 ========================================================================
void C3dTracer::SpaceItemInformation(const MbSpaceItem & cSpaceItem)
{
	MbeSpaceType eSpaceType = cSpaceItem.IsA();

	switch(eSpaceType)
	{
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
			PlaneInformation((const MbPlane &) cSpaceItem);
			break;
		case st_ConeSurface:
			ConeSurfaceInformation((const MbConeSurface &) cSpaceItem);
			break;
		case st_CylinderSurface:
			CylinderSurfaceInformation((const MbCylinderSurface &) cSpaceItem);
			break;
		case st_SphereSurface:
			SphereSurfaceInformation((const MbSphereSurface &) cSpaceItem);
			break;
		case st_TorusSurface:
			TorusSurfaceInformation((const MbTorusSurface &) cSpaceItem);
			break;
		case st_SweptSurface:
			break;
		case st_ExtrusionSurface:
			ExtrusionSurfaceInformation((const MbExtrusionSurface &) cSpaceItem);
			break;
		case st_RevolutionSurface:
			RevolutionSurfaceInformation((const MbRevolutionSurface &) cSpaceItem);
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
			SplineSurfaceInformation((const MbSplineSurface &) cSpaceItem);
			break;
		case st_GridSurface:
			break;
		case st_TriBezierSurface:
			break;
		case st_OffsetSurface:
			break;
		case st_DeformedSurface:
			break;
		case st_NurbsSurface:
			LogManager::Log(L"MbNurbsSurface: %s", HexStr((DWORD_PTR) &cSpaceItem));
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
			CurveBoundedSurfaceInformation((const MbCurveBoundedSurface &) cSpaceItem);
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
		case st_AssistedItem:
			break;
		case st_PointFrame:
			break;
		case st_WireFrame:
			break;
		case st_Solid:
			break;
		case st_Instance:
			break;
		case st_Assembly:
			break;
		case st_Mesh:
			break;
		case st_SpaceInstance:
			break;
		case st_PlaneInstance:
			break;
		case st_Collection:
			break;
		case st_FreeItem:
			break;
		default:
			break;
	}
}

void C3dTracer::PlaneItemInformation(const MbPlaneItem & cPlaneItem, const MbSurface & cSurface)
{
	MbePlaneType cPlaneType = cPlaneItem.IsA();

	switch(cPlaneType)
	{
		case pt_Undefined:
			LogManager::Log(L"pt_Undefined: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_PlaneItem:
			LogManager::Log(L"pt_PlaneItem: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Curve:
			LogManager::Log(L"pt_Curve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Line:
			LogManager::Log(L"pt_Line: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_LineSegment:
			LineSegmentInformation((const MbLineSegment &) cPlaneItem, cSurface);
			break;
		case pt_Arc:
			ArcInformation((const MbArc &) cPlaneItem);
			break;
		case pt_Cosinusoid:
			LogManager::Log(L"pt_Cosinusoid: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_PolyCurve:
			LogManager::Log(L"pt_PolyCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Polyline:
			LogManager::Log(L"pt_Polyline: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Bezier:
			LogManager::Log(L"pt_Bezier: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Hermit:
			HermitInformation((const MbHermit &) cPlaneItem);
			break;
		case pt_Nurbs:
			NurbsInformation((const MbNurbs &) cPlaneItem, cSurface);
			break;
		case pt_CubicSpline:
			LogManager::Log(L"pt_CubicSpline: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_TrimmedCurve:
			LogManager::Log(L"pt_TrimmedCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_OffsetCurve:
			LogManager::Log(L"pt_OffsetCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_ReparamCurve:
		{
			LogManager::Log(L"MbReparamCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));

			LogManager::IncreaseTabIndex();

			const MbReparamCurve & cReparamCurve = (MbReparamCurve &) cPlaneItem;
			LogManager::Log(L"TMin / TMax: %s / %s", DblStr(cReparamCurve.GetTMin()), DblStr(cReparamCurve.GetTMax()));
			PlaneItemInformation(cReparamCurve.GetBasisCurve(), cSurface);

			LogManager::DecreaseTabIndex();
		}
		break;

		case pt_PointCurve:
			LogManager::Log(L"pt_PointCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_CharacterCurve:
			LogManager::Log(L"pt_CharacterCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_ProjCurve:
			LogManager::Log(L"pt_ProjCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_SweptImageCurve:
			LogManager::Log(L"pt_SweptImageCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_TransformedCurve:
			LogManager::Log(L"pt_TransformedCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_ConeBendedCurve:
			LogManager::Log(L"pt_ConeBendedCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_ConeUnbendedCurve:
			LogManager::Log(L"pt_ConeUnbendedCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Contour:
		{
			LogManager::Log(L"MbContour: %s", HexStr((DWORD_PTR) &cPlaneItem));
			//const MbContour & cContour = (MbContour &) cPlaneItem;
		}
		break;

		case pt_ContourWithBreaks:
			LogManager::Log(L"pt_ContourWithBreaks: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_FreeCurve:
			LogManager::Log(L"pt_FreeCurve: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Multiline:
			LogManager::Log(L"pt_Multiline: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_Region:
			LogManager::Log(L"pt_Region: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		case pt_FreeItem:
			LogManager::Log(L"pt_FreeItem: %s", HexStr((DWORD_PTR) &cPlaneItem));
			break;
		default:
			break;
	}

	LogManager::SetComment(L"");
}

// == Surface 관련 정보 분석 =========================================================================
void C3dTracer::PlaneInformation(const MbPlane & cPlane)
{
	LogManager::Log(L"MbPlane: %s, %s", HexStr((DWORD_PTR) &cPlane), GetFaceIndexOfSurfaceString(cPlane));

	LogManager::IncreaseTabIndex();
	{
		Placement3DInformation(cPlane.GetPlacement());
		SurfaceDomainInformation((MbSurface *) &cPlane);
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::ConeSurfaceInformation(const MbConeSurface & cConeSurface)
{
	LogManager::Log(L"MbConeSurface: %s, %s", HexStr((DWORD_PTR) &cConeSurface), GetFaceIndexOfSurfaceString(cConeSurface));

	LogManager::IncreaseTabIndex();
	{
		Placement3DInformation(cConeSurface.GetPlacement());

		LogManager::Log(L"Radius: %s", DblStr(cConeSurface.GetRadius()));
		LogManager::Log(L"Height / RealHeight: %s / %s", DblStr(cConeSurface.GetHeight()), DblStr(cConeSurface.GetRealHeight()));
		LogManager::Log(L"Angle: %s, %s", DblStr(cConeSurface.GetAngle()), DblStr(cConeSurface.GetAngle() * 180.0 / M_PI));
		SurfaceDomainInformation((MbSurface *) &cConeSurface);
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::CylinderSurfaceInformation(const MbCylinderSurface & cCylinderSurface)
{
	LogManager::Log(L"MbCylinderSurface: %s, %s", HexStr((DWORD_PTR) &cCylinderSurface), GetFaceIndexOfSurfaceString(cCylinderSurface));

	LogManager::IncreaseTabIndex();
	{
		Placement3DInformation(cCylinderSurface.GetPlacement());

		LogManager::Log(L"Radius: %s", DblStr(cCylinderSurface.GetRadius()));
		LogManager::Log(L"Height / RealHeight: %s / %s", DblStr(cCylinderSurface.GetHeight()), DblStr(cCylinderSurface.GetRealHeight()));
		SurfaceDomainInformation((MbSurface *) &cCylinderSurface);
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::SphereSurfaceInformation(const MbSphereSurface & cSphereSurface)
{
	LogManager::Log(L"MbSphereSurface: %s, %s", HexStr((DWORD_PTR) &cSphereSurface), GetFaceIndexOfSurfaceString(cSphereSurface));

	LogManager::IncreaseTabIndex();
	{
		LogManager::IncreaseTabIndex();
		{
			Placement3DInformation(cSphereSurface.GetPlacement());

			LogManager::Log(L"Radius: %s", DblStr(cSphereSurface.GetRadius()));
		}
		LogManager::DecreaseTabIndex();
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::TorusSurfaceInformation(const MbTorusSurface & cTorusSurface)
{
	LogManager::Log(L"MbTorusSurface: %s, %s", HexStr((DWORD_PTR) &cTorusSurface), GetFaceIndexOfSurfaceString(cTorusSurface));

	LogManager::IncreaseTabIndex();
	{
		Placement3DInformation(cTorusSurface.GetPlacement());

		LogManager::Log(L"Major Radius: %s, Minor Radius: %s", DblStr(cTorusSurface.GetMajorRadius()), DblStr(cTorusSurface.GetMinorRadius()));
		LogManager::Log(L"U Close: %s, V Close: %s", BoolStr(cTorusSurface.IsUClosed()), BoolStr(cTorusSurface.IsVClosed()));

		SurfaceDomainInformation((MbSurface *) &cTorusSurface);
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::RevolutionSurfaceInformation(const MbRevolutionSurface & cRevolutionSurface)
{
	LogManager::Log(L"MbRevolutionSurface: %s, %s", HexStr((DWORD_PTR) &cRevolutionSurface), GetFaceIndexOfSurfaceString(cRevolutionSurface));

	LogManager::IncreaseTabIndex();
	{
		Placement3DInformation(cRevolutionSurface.GetPlacement());

		// 		LogManager::Log(L"Major Radius: %s, Minor Radius: %s", DblStr(cRevolutionSurface.GetMajorRadius()), DblStr(cRevolutionSurface.GetMinorRadius()));
		// 		LogManager::Log(L"U Close: %s, V Close: %s", BoolStr(cRevolutionSurface.IsUClosed()), BoolStr(cRevolutionSurface.IsVClosed()));

		SurfaceDomainInformation((MbSurface *) &cRevolutionSurface);
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::ExtrusionSurfaceInformation(const MbExtrusionSurface & cExtrusionSurface)
{
	LogManager::Log(L"MbExtrusionSurface: %s, %s", HexStr((DWORD_PTR) &cExtrusionSurface), GetFaceIndexOfSurfaceString(cExtrusionSurface));

	LogManager::IncreaseTabIndex();
	{
		//SpaceItemInformation(cExtrusionSurface.GetCurve());
		LogManager::Log(L"Basis Curve: %s", GetSpaceItemNameString(cExtrusionSurface.GetCurve()).c_str());
		LogManager::Log(L"TMin / TMax: %s / %s", DblStr(cExtrusionSurface.GetCurve().GetTMin()), DblStr(cExtrusionSurface.GetCurve().GetTMax()));

		//Placement3DInformation(cExtrusionSurface.GetPlacement());

		// 		LogManager::Log(L"Major Radius: %s, Minor Radius: %s", DblStr(cRevolutionSurface.GetMajorRadius()), DblStr(cRevolutionSurface.GetMinorRadius()));
		// 		LogManager::Log(L"U Close: %s, V Close: %s", BoolStr(cRevolutionSurface.IsUClosed()), BoolStr(cRevolutionSurface.IsVClosed()));

		SurfaceDomainInformation((MbSurface *) &cExtrusionSurface);
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::SplineSurfaceInformation(const MbSplineSurface & cSplineSurface)
{
	LogManager::Log(L"MbSplineSurface: %s, %s", HexStr((DWORD_PTR) &cSplineSurface), GetFaceIndexOfSurfaceString(cSplineSurface));

	LogManager::IncreaseTabIndex();
	{
		SurfaceDomainInformation((MbSurface *) &cSplineSurface);
		LogManager::Log(L"U Degree: %d, U Knot size: %d, U Ctrl size: %d", cSplineSurface.GetUDegree(), cSplineSurface.GetKnotsCount(true), cSplineSurface.GetPointsUCount());
		LogManager::Log(L"V Degree: %d, V Knot size: %d, V Ctrl size: %d", cSplineSurface.GetVDegree(), cSplineSurface.GetKnotsCount(false), cSplineSurface.GetPointsVCount());
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::CurveBoundedSurfaceInformation(const MbCurveBoundedSurface & cCurveBoundedSurface)
{
	LogManager::Log(L"MbCurveBoundedSurface: %s, %s", HexStr((DWORD_PTR) &cCurveBoundedSurface), GetFaceIndexOfSurfaceString(cCurveBoundedSurface));

	LogManager::IncreaseTabIndex();
	{
		SurfaceDomainInformation((MbSurface *) &cCurveBoundedSurface);

		LogManager::IncreaseTabIndex();
		{
			SpaceItemInformation(cCurveBoundedSurface.GetBasisSurface());
		}
		LogManager::DecreaseTabIndex();
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::SurfaceDomainInformation(MbSurface * pcSurface)
{
	LogManager::Log(L"UMin / UMax: %s /%s", DblStr(pcSurface->GetUMin(), 32), DblStr(pcSurface->GetUMax(), 32));
	LogManager::Log(L"VMin / VMax: %s /%s", DblStr(pcSurface->GetVMin(), 32), DblStr(pcSurface->GetVMax(), 32));
}

// == Curve 2D 관련 정보 분석 ========================================================================
void C3dTracer::LineSegmentInformation(const MbLineSegment & cLineSegment, const MbSurface & cSurface)
{
	LogManager::Log(L"MbLineSegment: %s", HexStr((DWORD_PTR) &cLineSegment));

	LogManager::IncreaseTabIndex();
	{
		MbCartPoint cP1, cP2, cP3;
		cLineSegment.GetStartPoint(cP1);
		cLineSegment.GetMiddlePoint(cP2);
		cLineSegment.GetEndPoint(cP3);

		//LogManager::Log(L"TMin / TMax: %s / %s", DblStr(cLineSegment.GetTMin()), DblStr(cLineSegment.GetTMax()));
		LogManager::Log(L"2D: [%s ~ %s]", GetCartPointString(cP1), GetCartPointString(cP3));

		MbCartPoint3D cSurfPo1, cSurfPo2, cSurfPo3;

		cSurface._PointOn(cP1, cSurfPo1);
		cSurface._PointOn(cP2, cSurfPo2);
		cSurface._PointOn(cP3, cSurfPo3);

		//LogManager::Log(L"3D: [%s / %s / %s]", GetCartPoint3DString(cSurfPo1), GetCartPoint3DString(cSurfPo2), GetCartPoint3DString(cSurfPo3));
		LogManager::Log(L"3D: [%s / %s]", GetCartPoint3DString(cSurfPo1), GetCartPoint3DString(cSurfPo3));
	}
	LogManager::DecreaseTabIndex();
}

void C3dTracer::ArcInformation(const MbArc & cArc)
{
	LogManager::Log(L"MbArc: %s", HexStr((DWORD_PTR) &cArc));

	LogManager::IncreaseTabIndex();

	LogManager::Log(L"TMin / TMax: %s / %s", DblStr(cArc.GetTMin()), DblStr(cArc.GetTMax()));

	PlacementInformation(cArc.GetPlacement());
	LogManager::Log(L"Radius: %s / X Radius: %s / Y Radius: %s", DblStr(cArc.GetRadius()), DblStr(cArc.GetRadiusA()), DblStr(cArc.GetRadiusB()));
	LogManager::Log(L"Trim1: %s / Trim2: %s", DblStr(cArc.GetTrim1() * 180 / M_PI), DblStr(cArc.GetTrim2() * 180 / M_PI));
	LogManager::Log(L"sense: %s, circle: %s, closed: %s", BoolStr(cArc.GetSense()), BoolStr(cArc.IsCircle()), BoolStr(cArc.IsClosed()));

	MbCartPoint cP1, cP2;
	cArc.GetStartPoint(cP1);
	cArc.GetEndPoint(cP2);

	LogManager::Log(L"Start Point: %s", GetCartPointString(cP1));
	LogManager::Log(L"End Point: %s", GetCartPointString(cP2));

	LogManager::DecreaseTabIndex();
}

void C3dTracer::HermitInformation(const MbHermit & cHermit)
{
	LogManager::Log(L"MbHermit: %s, Count: %d", HexStr((DWORD_PTR) &cHermit), cHermit.GetCount());

	LogManager::IncreaseTabIndex();

	LogManager::Log(L"TMin / TMax: %s / %s", DblStr(cHermit.GetTMin()), DblStr(cHermit.GetTMax()));

	SArray<MbVector> acVectors;
	cHermit.GetVectorList(acVectors);

	int nIndex = 0;
	for(const MbVector & cVector : acVectors) {
		//LogManager::Log(L"Vector [%d]: %s", nIndex++, GetVectorString(cVector));
	}

	nIndex = 0;
	SArray<double> adPrams;
	cHermit.GetTList(adPrams);
	for(const double & dPram : adPrams) {
		//LogManager::Log(L"Param [%d]: %s", nIndex++, DblStr(dPram));
	}

	MbCartPoint cP1, cP2;
	cHermit.GetStartPoint(cP1);
	cHermit.GetEndPoint(cP2);

	LogManager::Log(L"Start Point: %s", GetCartPointString(cP1));
	LogManager::Log(L"End Point: %s", GetCartPointString(cP2));

	LogManager::DecreaseTabIndex();
}

void C3dTracer::NurbsInformation(const MbNurbs & cNurbs, const MbSurface & cSurface)
{
	LogManager::Log(L"MbNurbs: %s", HexStr((DWORD_PTR) &cNurbs));

	LogManager::IncreaseTabIndex();
	{
		LogManager::Log(L"TMin / TMax: %s / %s", DblStr(cNurbs.GetTMin()), DblStr(cNurbs.GetTMax()));

		LogManager::Log(L"Degree: %d, Count: %d", cNurbs.GetDegree(), cNurbs.GetPointsCount());

#ifdef _LOG_NURBS_CURVE_DETAIL_LOG_
		for(size_t nIndex = 0; nIndex < cNurbs.GetPointsCount(); nIndex++) {
			MbCartPoint cPoint;
			MbCartPoint3D cSurfPo;
			cNurbs.GetPoint(nIndex, cPoint);
			cSurface._PointOn(cPoint, cSurfPo);

			LogManager::Log(L"Point %d: %s, [%s]", nIndex, GetCartPointString(cPoint), GetCartPoint3DString(cSurfPo));
		}

		for(size_t nIndex = 0; nIndex < cNurbs.GetKnotsCount(); nIndex++) {
			LogManager::Log(L"Knot %d: %s", nIndex, DblStr(cNurbs.GetKnot(nIndex)));
		}

		for(size_t nIndex = 0; nIndex < cNurbs.GetWeightsCount(); nIndex++) {
			LogManager::Log(L"Weight %d: %s", nIndex, DblStr(cNurbs.GetWeight(nIndex)));
		}
#else
		MbCartPoint cP1, cP2, cMidPo;
		cNurbs.GetStartPoint(cP1);
		cNurbs.GetEndPoint(cP2);
		cNurbs._PointOn(cNurbs.GetTMid(), cMidPo);

		MbCartPoint3D cSurfPo1, cSurfPo2, cSurfMidPo;
		cSurface._PointOn(cP1, cSurfPo1);
		cSurface._PointOn(cP2, cSurfPo2);
		cSurface._PointOn(cMidPo, cSurfMidPo);

		LogManager::Log(L"Start Point: %s, [%s]", GetCartPointString(cP1), GetCartPoint3DString(cSurfPo1));
		//LogManager::Log(L"Mid Point: %s, [%s]", GetCartPointString(cMidPo), GetCartPoint3DString(cSurfMidPo));
		LogManager::Log(L"End Point: %s, [%s]", GetCartPointString(cP2), GetCartPoint3DString(cSurfPo2));
#endif
	}
	LogManager::DecreaseTabIndex();
}

// == 기타 요소 관련 정보 분석 =====================================================================
void C3dTracer::Placement3DInformation(const MbPlacement3D & cPlacement)
{
	LogManager::Log(L"MbPlacement3D");

	LogManager::IncreaseTabIndex();

	LogManager::Log(L"Origin: %s", GetCartPoint3DString(cPlacement.GetOrigin()));
	LogManager::Log(L"X-Axis: %s", GetVector3DString(cPlacement.GetAxisX()));
	LogManager::Log(L"Y-Axis: %s", GetVector3DString(cPlacement.GetAxisY()));
	LogManager::Log(L"Z-Axis: %s", GetVector3DString(cPlacement.GetAxisZ()));

	LogManager::DecreaseTabIndex();
}

void C3dTracer::PlacementInformation(const MbPlacement & cPlacement)
{
	LogManager::Log(L"MbPlacement");

	LogManager::IncreaseTabIndex();

	LogManager::Log(L"Origin: %s", GetCartPointString(cPlacement.GetOrigin()));
	LogManager::Log(L"X-Axis: %s", GetVectorString(cPlacement.GetAxisX()));
	LogManager::Log(L"Y-Axis: %s", GetVectorString(cPlacement.GetAxisY()));

	LogManager::DecreaseTabIndex();
}

void C3dTracer::Matrix3dInformation(const MbMatrix3D & cMatrix)
{
	if(true == cMatrix.IsSingle()) {
		return;
	}

	LogManager::Log(L"MbMatrix3D");

	LogManager::IncreaseTabIndex();

	LogManager::Log(L"X-Axis: %s", GetVector3DString(cMatrix.GetAxisX()));
	LogManager::Log(L"Y-Axis: %s", GetVector3DString(cMatrix.GetAxisY()));
	LogManager::Log(L"Z-Axis: %s", GetVector3DString(cMatrix.GetAxisZ()));
	LogManager::Log(L"Origin: %s", GetCartPoint3DString(cMatrix.GetOrigin()));

	LogManager::DecreaseTabIndex();
}

// == Utility Function =============================================================================

// 주어진 Face의 Index String을 가져온다.
CString C3dTracer::GetFaceIndexString(const MbFace * pcFace)
{
	CString strText;

	auto cIterator = std::find(m_vpcFaceVector.begin(), m_vpcFaceVector.end(), pcFace);

	if(cIterator != m_vpcFaceVector.end()) {
		int nIndex = (int) (cIterator - m_vpcFaceVector.begin());
		strText.Format(L"Face %d", nIndex);
	}

	return strText;
}

// 주어진 Surface를 Face vector에서 검색해서 Face의 Index String을 가져온다.
CString C3dTracer::GetFaceIndexOfSurfaceString(const MbSurface & cSurface)
{
	CString strText;

	size_t nFaceIndex = 0;
	for(const MbFace * pcFace : m_vpcFaceVector) {
		const MbSurface & cFaceSurface = pcFace->GetSurface();

		if(&cFaceSurface == &cSurface) {
			strText.Format(L"Face %d", (int) nFaceIndex);
			return strText;
		}
		else if(st_CurveBoundedSurface == cFaceSurface.IsA()) {
			const MbCurveBoundedSurface & cCurveBoundedSurface = (MbCurveBoundedSurface &) cFaceSurface;
			const MbSurface & cBasisSurface = cCurveBoundedSurface.GetBasisSurface();

			if(&cBasisSurface == &cSurface) {
				strText.Format(L"Face %d", (int) nFaceIndex);
				return strText;
			}
		}

		nFaceIndex++;
	}

	return L"";
}

CString C3dTracer::DblStr(double dValue, int nDigit)
{
	CString strText, strFormat;
	strFormat.Format(L"%%.%df", nDigit);
	strText.Format(strFormat, dValue);
	return strText;
}

CString C3dTracer::HexStr(DWORD_PTR nValue)
{
	CString strText;
	strText.Format(L"0x%lx", (int) nValue);
	return strText;
}

CString C3dTracer::BoolStr(bool bValue)
{
	if(true == bValue) {
		return L"true";
	}

	return L"false";
}

CString C3dTracer::GetCartPointString(const MbCartPoint & cPoint)
{
	CStringW strText;
	strText.Format(L"%s,%s", DblStr(cPoint.x), DblStr(cPoint.y));
	return strText;
}

CString C3dTracer::GetCartPoint3DString(const MbCartPoint3D & cPoint)
{
	CStringW strText;
	strText.Format(L"%s,%s,%s", DblStr(cPoint.x), DblStr(cPoint.y), DblStr(cPoint.z));
	return strText;
}

CString C3dTracer::GetVectorString(const MbVector & cVector)
{
	CString strText;
	strText.Format(L"%s,%s", DblStr(cVector.x), DblStr(cVector.y));
	return strText;
}

CString C3dTracer::GetVector3DString(const MbVector3D & cVector)
{
	CString strText;
	strText.Format(L"%s,%s,%s", DblStr(cVector.x), DblStr(cVector.y), DblStr(cVector.z));
	return strText;
}

// Space Item의 이름을 가져온다.
wstring C3dTracer::GetSpaceItemNameString(const MbSpaceItem & eSpaceItem)
{
	MbeSpaceType eSpaceType = eSpaceItem.IsA();
	wstring strText;

	switch(eSpaceType)
	{
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
			strText = L"Line3D";
			break;
		case st_LineSegment3D:
			strText = L"LineSegment3D";
			break;
		case st_Arc3D:
			strText = L"Arc3D";
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
			strText = L"Plane";
			break;
		case st_ConeSurface:
			strText = L"ConeSurface";
			break;
		case st_CylinderSurface:
			strText = L"CylinderSurface";
			break;
		case st_SphereSurface:
			strText = L"SphereSurface";
			break;
		case st_TorusSurface:
			strText = L"TorusSurface";
			break;
		case st_SweptSurface:
			break;
		case st_ExtrusionSurface:
			strText = L"ExtrusionSurface";
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
			strText = L"RuledSurface";
			break;
		case st_SectorSurface:
			strText = L"SectorSurface";
			break;
		case st_PolySurface:
			strText = L"PolySurface";
			break;
		case st_HermitSurface:
			strText = L"HermitSurface";
			break;
		case st_SplineSurface:
			strText = L"SplineSurface";
			break;
		case st_GridSurface:
			break;
		case st_TriBezierSurface:
			break;
		case st_OffsetSurface:
			strText = L"OffsetSurface";
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
			strText = L"ElevationSurface";
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
		case st_AssistedItem:
			break;
		case st_PointFrame:
			break;
		case st_WireFrame:
			break;
		case st_Solid:
			break;
		case st_Instance:
			break;
		case st_Assembly:
			break;
		case st_Mesh:
			break;
		case st_SpaceInstance:
			break;
		case st_PlaneInstance:
			break;
		case st_Collection:
			break;
		case st_FreeItem:
			break;
	}

	if(true == strText.empty()) {
		wprintf(strText.c_str(), L"SpaceItem Type: %d", (int) eSpaceType);
		//strText = std::format(L"SpaceItem Type: {}", (int) eSpaceType);
	}

	return strText;
}

void C3dTracer::PointOnInformation(WCHAR * pchClassName, double t, MbCartPoint & cPoint)
{
	LogManager::Log(L"%s / Para: %s, Point3d: %s", pchClassName, DblStr(t), GetCartPointString(cPoint));
}

void C3dTracer::PointOnInformation(WCHAR * pchClassName, double t, MbCartPoint3D & cPoint)
{
	LogManager::Log(L"%s / Para: %s, Point3d: %s", pchClassName, DblStr(t), GetCartPoint3DString(cPoint));
}