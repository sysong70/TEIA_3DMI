#include "stdafx.h"

#include "A3dTracer.h"

// CString용 Header
#include <atlstr.h>

#include "LogManager.h"
#include "Dmi3dx.h"

#include <format>

// #define _LOG_NURBS_CURVE_DETAIL_LOG_

using namespace std;

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RETURN(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return; } }

void A3dTracer::A3DTopoShellLog(const A3DTopoShell * pcTopoShell)
{
	//LogManager::SetFileCloseFlag(false);

	A3DTopoShellData cTopoShellData;
	A3D_INITIALIZE_DATA(A3DTopoShellData, cTopoShellData);
	A3DStatus nResult = A3DTopoShellGet(pcTopoShell, &cTopoShellData);
	if(A3D_SUCCESS == nResult) {
		Log(L"A3DTopoShell: %s", HexStr((DWORD_PTR) pcTopoShell));

		Log(L"FaceSize: %d, Closed: %s", cTopoShellData.m_uiFaceSize, BoolStr((bool) cTopoShellData.m_bClosed));

		for(A3DUns32 nIndex = 0; nIndex < cTopoShellData.m_uiFaceSize; nIndex++) {
			A3DTopoFaceLog(cTopoShellData.m_ppFaces[nIndex], nIndex, cTopoShellData.m_pucOrientationWithShell[nIndex]);
		}

		A3DTopoShellGet(nullptr, &cTopoShellData);
	}

	//LogManager::SetFileCloseFlag(true);

	//LogManager::SetCurrentId(m_nOldCurrentId);
}

void A3dTracer::A3DTopoFaceLog(const A3DTopoFace * pcTopoFace, A3DUns32 nFaceIndex, A3DUns8 nOrientationWithShell)
{
	Log(L"A3DTopoFace [%d]: %s, Sense: %s", nFaceIndex, HexStr((DWORD_PTR) pcTopoFace), BoolStr((bool) nOrientationWithShell));

	IncreaseLogTab();
	{
		A3DTopoFaceData cTopoFaceData;
		A3D_INITIALIZE_DATA(A3DTopoFaceData, cTopoFaceData);
		A3DStatus eResult = A3DTopoFaceGet(pcTopoFace, &cTopoFaceData);
		if(A3D_SUCCESS == eResult) {
			A3DSurfBaseLog(cTopoFaceData.m_pSurface);
			Log(L"LoopSize: %d, HasTrimDomain: %s", cTopoFaceData.m_uiLoopSize, BoolStr((bool) cTopoFaceData.m_bHasTrimDomain));
			if(TRUE == cTopoFaceData.m_bHasTrimDomain) {
				A3DDomainDataLog(cTopoFaceData.m_sSurfaceDomain);
			}

			Log(L"OuterLoopIndex: %d", cTopoFaceData.m_uiOuterLoopIndex);

			// Loop Data
			for(A3DUns32 nIndex = 0; nIndex < cTopoFaceData.m_uiLoopSize; nIndex++) {
				A3DTopoLoopLog(cTopoFaceData.m_ppLoops[nIndex], nIndex, nFaceIndex);
			}

			A3DTopoFaceGet(nullptr, &cTopoFaceData);
		}
	}
	DecreaseLogTab();
}

void A3dTracer::A3DTopoLoopLog(const A3DTopoLoop * pcTopoLoop, A3DUns32 nLoopIndex, A3DUns32 nFaceIndex)
{
	Log(L"A3DTopoLoop [%d]: %s", nLoopIndex, HexStr((DWORD_PTR) pcTopoLoop));

	IncreaseLogTab();

	A3DTopoLoopData cTopoLoopData;
	A3D_INITIALIZE_DATA(A3DTopoLoopData, cTopoLoopData);
	A3DStatus eResult = A3DTopoLoopGet(pcTopoLoop, &cTopoLoopData);
	if(A3D_SUCCESS == eResult) {
		Log(L"EdgeSize: %d, OrientationWithSurface: %s", cTopoLoopData.m_uiCoEdgeSize, BoolStr((bool) cTopoLoopData.m_ucOrientationWithSurface));

		for(A3DUns32 nIndex = 0; nIndex < cTopoLoopData.m_uiCoEdgeSize; nIndex++) {
			A3DTopoCoEdgeLog(cTopoLoopData.m_ppCoEdges[nIndex], nIndex, nFaceIndex);
		}

		A3DTopoLoopGet(nullptr, &cTopoLoopData);
	}

	DecreaseLogTab();
}

void A3dTracer::A3DTopoCoEdgeLog(const A3DTopoCoEdge * pcTopoCoEdge, A3DUns32 nEdgeIndex, A3DUns32 nFaceIndex)
{
	Log(L"A3DTopoCoEdge [%d]: %s", nEdgeIndex, HexStr((DWORD_PTR) pcTopoCoEdge));

	IncreaseLogTab();
	{
		A3DTopoCoEdgeData cTopoCoEdgeData;
		A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, cTopoCoEdgeData);
		A3DStatus eResult = A3DTopoCoEdgeGet(pcTopoCoEdge, &cTopoCoEdgeData);
		if(A3D_SUCCESS == eResult) {
			Log(L"OrientationWithLoop: %s, OrientationUVWithLoop: %s", BoolStr((bool) cTopoCoEdgeData.m_ucOrientationWithLoop),
				BoolStr((bool) cTopoCoEdgeData.m_ucOrientationUVWithLoop));

			Log(L"A3DTopoCoEdge Neighbor: %s", HexStr((DWORD_PTR) cTopoCoEdgeData.m_pNeighbor));

			A3DTopoEdgeLog(cTopoCoEdgeData.m_pEdge, nFaceIndex);

			Log(L"A3DCrvBase UV: %s", HexStr((DWORD_PTR) cTopoCoEdgeData.m_pUVCurve));
			IncreaseLogTab();
			{
				A3DCrvBaseLog(cTopoCoEdgeData.m_pUVCurve);
			}
			DecreaseLogTab();

			A3DTopoCoEdgeGet(nullptr, &cTopoCoEdgeData);
		}
	}
	DecreaseLogTab();
}

void A3dTracer::A3DTopoEdgeLog(const A3DTopoEdge * pcTopoEdge, A3DUns32 nFaceIndex)
{
	Log(L"A3DTopoEdge: %s [Face %d]", HexStr((DWORD_PTR) pcTopoEdge), nFaceIndex);

	IncreaseLogTab();

	A3DTopoEdgeData cTopoEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, cTopoEdgeData);
	A3DStatus eResult = A3DTopoEdgeGet(pcTopoEdge, &cTopoEdgeData);
	if(A3D_SUCCESS == eResult) {
		Log(L"3D space curve: %s", HexStr((DWORD_PTR) cTopoEdgeData.m_p3dCurve));
		// 		Log(L"HasTrimDomain: %s, Tolerance: %s", BoolStr((bool) cTopoEdgeData.m_bHasTrimDomain), DblStr(cTopoEdgeData.m_dTolerance));
		// 		Log(L"Interval: %s,%s", DblStr(cTopoEdgeData.m_sInterval.m_dMin), DblStr(cTopoEdgeData.m_sInterval.m_dMax));

		A3DTopoVertexLog(cTopoEdgeData.m_pStartVertex);
		A3DTopoVertexLog(cTopoEdgeData.m_pEndVertex);

		A3DTopoEdgeGet(nullptr, &cTopoEdgeData);
	}

	DecreaseLogTab();
}

// == Surface 관련 함수 =============================================================================
void A3dTracer::A3DSurfBaseLog(const A3DSurfBase * pcSurfBase)
{
	A3DEEntityType eType = kA3DTypeUnknown;
	if(A3D_SUCCESS != A3DEntityGetType(pcSurfBase, &eType)) {
		return;
	}

	switch(eType)
	{
		case kA3DTypeSurfPlane:
			A3DSurfPlaneLog(pcSurfBase);
			break;

		case kA3DTypeSurfCylinder:
			A3DSurfCylinderLog(pcSurfBase);
			break;

		case kA3DTypeSurfCone:
			A3DSurfConeLog(pcSurfBase);
			break;

		case kA3DTypeSurfTorus:
			A3DSurfTorusLog(pcSurfBase);
			break;

		case kA3DTypeSurfSphere:
			A3DSurfSphereLog(pcSurfBase);
			break;

		case kA3DTypeSurfRevolution:
			A3DSurfRevolutionLog(pcSurfBase);
			break;

		case kA3DTypeSurfExtrusion:
			A3DSurfExtrusionLog(pcSurfBase);
			break;

		case kA3DTypeSurfNurbs:
			A3DSurfNurbsLog(pcSurfBase);
			break;

		case kA3DTypeSurfOffset:
			//CHECK_BOOL_RETURN(ConvertSurfOffset(pcSurfBase, pcSurface));
			break;

			/*
					case kA3DTypeSurfNurbs:
						break;

					case kA3DTypeSurfCone:
						break;

					case kA3DTypeSurfCylindrical:
						break;

					case kA3DTypeSurfPipe:
						break;

					case kA3DTypeSurfRuled:
						break;


					case kA3DTypeSurfRevolution:
						break;

					case kA3DTypeSurfExtrusion:
						break;

					case kA3DTypeSurfFromCurves:
						break;

					case kA3DTypeSurfTorus:
						break;*/

		default:
			Log(L"A3DSurfBase: %s, Type %s", HexStr((DWORD_PTR) pcSurfBase), Dmi3dx::GetA3dEntityTypeString(eType));
			break;
	}
}

void A3dTracer::A3DSurfPlaneLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfPlane: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfPlaneData cSurfPlaneData;
	A3D_INITIALIZE_DATA(A3DSurfPlaneData, cSurfPlaneData);
	CHECK_A3D_RETURN(A3DSurfPlaneGet(pcSurfBase, &cSurfPlaneData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfPlaneData.m_sTrsf.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfPlaneData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfPlaneData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);

		Log(L"Behaviour: %d", cSurfPlaneData.m_usStructSize);

		m_dUSurfCoeff = cSurfPlaneData.m_sParam.m_dUCoeffA;
		m_dVSurfCoeff = cSurfPlaneData.m_sParam.m_dVCoeffA;

		A3DUVParameterizationDataLog(cSurfPlaneData.m_sParam);
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfCylinderLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfCylinder: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfCylinderData cSurfCylinderData;
	A3D_INITIALIZE_DATA(A3DSurfPlaneData, cSurfCylinderData);
	CHECK_A3D_RETURN(A3DSurfCylinderGet(pcSurfBase, &cSurfCylinderData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfCylinderData.m_sTrsf.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfCylinderData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfCylinderData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);

		Log(L"Radius: %s", DblStr(cSurfCylinderData.m_dRadius));

		m_dUSurfCoeff = cSurfCylinderData.m_sParam.m_dUCoeffA / m_dTopoContextScale;
		m_dVSurfCoeff = cSurfCylinderData.m_sParam.m_dVCoeffA;

		A3DUVParameterizationDataLog(cSurfCylinderData.m_sParam);
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfConeLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfCone: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfConeData cSurfConeData;
	A3D_INITIALIZE_DATA(A3DSurfConeData, cSurfConeData);
	CHECK_A3D_RETURN(A3DSurfConeGet(pcSurfBase, &cSurfConeData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfConeData.m_sTrsf.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfConeData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfConeData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);

		double dSemiAngle = cSurfConeData.m_dSemiAngle;
		double dEdge = cSurfConeData.m_sParam.m_sUVDomain.m_sMax.m_dY * m_dTopoContextScale;
		double dRadius = cSurfConeData.m_dRadius * m_dTopoContextScale;
		//double dRadius = dEdge * sin(dSemiAngle);
		//double dHeight = dEdge * cos(dSemiAngle);
		double dHeight = 1.0;

		Log(L"Radius: %s, SemiAngle: %s", DblStr(dRadius), DblStr(dSemiAngle));

		double dVmin = -dRadius / tan(dSemiAngle);
		double dVmax = dEdge * cos(dSemiAngle);

		m_dUSurfCoeff = cSurfConeData.m_sParam.m_dUCoeffA / m_dTopoContextScale;
		m_dVSurfCoeff = cSurfConeData.m_sParam.m_dVCoeffA;

		A3DUVParameterizationDataLog(cSurfConeData.m_sParam);
		//Log(L"U,V Coeff: %s, %s", DblStr(dUCoeff), DblStr(dVCoeff));
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfTorusLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfTorus: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfTorusData cSurfTorusData;
	A3D_INITIALIZE_DATA(A3DSurfTorusData, cSurfTorusData);

	CHECK_A3D_RETURN(A3DSurfTorusGet(pcSurfBase, &cSurfTorusData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfTorusData.m_sTrsf.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfTorusData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfTorusData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfTorusData.m_sTrsf.m_sScale, L"Scale: ", 1.0);
		Log(L"Behaviour: %d", cSurfTorusData.m_sTrsf.m_ucBehaviour);

		double dMajorRadius = cSurfTorusData.m_dMajorRadius * m_dTopoContextScale;
		double dMinorRadius = cSurfTorusData.m_dMinorRadius * m_dTopoContextScale;

		Log(L"Major Radius: %s, Minor Radius: %s", DblStr(dMajorRadius), DblStr(dMinorRadius));

		m_dUSurfCoeff = cSurfTorusData.m_sParam.m_dUCoeffA / m_dTopoContextScale;
		m_dVSurfCoeff = cSurfTorusData.m_sParam.m_dVCoeffA / m_dTopoContextScale;

		A3DUVParameterizationDataLog(cSurfTorusData.m_sParam);
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfSphereLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfSphere: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfSphereData cSurfSphereData;
	A3D_INITIALIZE_DATA(A3DSurfSphereData, cSurfSphereData);

	CHECK_A3D_RETURN(A3DSurfSphereGet(pcSurfBase, &cSurfSphereData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfSphereData.m_sTrsf.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfSphereData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfSphereData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);

		double dRadius = cSurfSphereData.m_dRadius * m_dTopoContextScale;

		Log(L"Radius: %s", DblStr(dRadius));

		m_dUSurfCoeff = cSurfSphereData.m_sParam.m_dUCoeffA / m_dTopoContextScale;
		m_dVSurfCoeff = cSurfSphereData.m_sParam.m_dVCoeffA / m_dTopoContextScale;

		A3DUVParameterizationDataLog(cSurfSphereData.m_sParam);
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfRevolutionLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfRevolution: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfRevolutionData cSurfRevolutionData;
	A3D_INITIALIZE_DATA(A3DSurfRevolutionData, cSurfRevolutionData);
	CHECK_A3D_RETURN(A3DSurfRevolutionGet(pcSurfBase, &cSurfRevolutionData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfRevolutionData.m_sTrsf.m_sOrigin, L"Trsf Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfRevolutionData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfRevolutionData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);

		A3DVector3dDataLog(cSurfRevolutionData.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfRevolutionData.m_sDirection, L"Direction: ", 1);

		m_dUSurfCoeff = cSurfRevolutionData.m_sParam.m_dUCoeffA;
		m_dVSurfCoeff = cSurfRevolutionData.m_sParam.m_dVCoeffA / m_dTopoContextScale;

		A3DUVParameterizationDataLog(cSurfRevolutionData.m_sParam);
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfExtrusionLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfExtrusion: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfExtrusionData cSurfExtrusionData;
	A3D_INITIALIZE_DATA(A3DSurfExtrusionData, cSurfExtrusionData);
	CHECK_A3D_RETURN(A3DSurfExtrusionGet(pcSurfBase, &cSurfExtrusionData));

	IncreaseLogTab();
	{
		A3DVector3dDataLog(cSurfExtrusionData.m_sTrsf.m_sOrigin, L"Trsf Origin: ", m_dTopoContextScale);
		A3DVector3dDataLog(cSurfExtrusionData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		A3DVector3dDataLog(cSurfExtrusionData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);

		A3DVector3dDataLog(cSurfExtrusionData.m_sDirection, L"Direction: ", 1);

		m_dUSurfCoeff = cSurfExtrusionData.m_sParam.m_dUCoeffA / m_dTopoContextScale;
		m_dVSurfCoeff = cSurfExtrusionData.m_sParam.m_dVCoeffA / m_dTopoContextScale;

		A3DUVParameterizationDataLog(cSurfExtrusionData.m_sParam);
	}
	DecreaseLogTab();
}

void A3dTracer::A3DSurfNurbsLog(const A3DSurfBase * pcSurfBase)
{
	Log(L"A3DSurfNurbs: %s", HexStr((DWORD_PTR) pcSurfBase));

	A3DSurfNurbsData cSurfNurbsData;
	A3D_INITIALIZE_DATA(A3DSurfNurbsData, cSurfNurbsData);

	CHECK_A3D_RETURN(A3DSurfNurbsGet(pcSurfBase, &cSurfNurbsData));

	/*
		A3DEKnotType								m_eKnotType;		/ *!< Knot type. * /
		A3DEBSplineSurfaceForm					m_eSurfaceForm;	/ *!< Surface form. * /
		A3DUns32										m_uiUDegree;		/ *!< Degree in U direction. * /
		A3DUns32										m_uiVDegree;		/ *!< Degree in V direction. * /
		A3DUns32										m_uiUKnotSize;		/ *!< Knots number in U direction. * /
		A3DUns32										m_uiVKnotSize;		/ *!< Knots number in V direction. * /
		A3DUns32										m_uiUCtrlSize;		/ *!< Control points number in U direction. * /
		A3DUns32										m_uiVCtrlSize;		/ *!< Control points number in V direction. * /
		A3DDouble * m_pdUKnots;			/ *!< Knots in U direction. * /
		A3DDouble * m_pdVKnots;			/ *!< Knots in V direction. * /
		A3DDouble * m_pdWeights;		/ *!< Weights if surface is rational. * /
		A3DVector3dData * m_pCtrlPts;			/ *!< Control points. * /
	*/

	IncreaseLogTab();
	{
		Log(L"KnotType: %s, CurveForm: %s", GetKnotTypeString(cSurfNurbsData.m_eKnotType), GetBSplineSurfaceFormString(cSurfNurbsData.m_eSurfaceForm));
		Log(L"U Degree: %d, U Knot Size: %d, U Ctrl Size: %d", cSurfNurbsData.m_uiUDegree, cSurfNurbsData.m_uiUKnotSize, cSurfNurbsData.m_uiUCtrlSize);
		Log(L"V Degree: %d, V Knot Size: %d, V Ctrl Size: %d", cSurfNurbsData.m_uiVDegree, cSurfNurbsData.m_uiVKnotSize, cSurfNurbsData.m_uiVCtrlSize);
		// 
		// 		A3DVector3dDataLog(cSurfSphereData.m_sTrsf.m_sOrigin, L"Origin: ", m_dTopoContextScale);
		// 		A3DVector3dDataLog(cSurfSphereData.m_sTrsf.m_sXVector, L"X-Axis: ", 1.0);
		// 		A3DVector3dDataLog(cSurfSphereData.m_sTrsf.m_sYVector, L"Y-Axis: ", 1.0);
		// 
		// 		double dRadius = cSurfSphereData.m_dRadius * m_dTopoContextScale;
		// 
		// 		Log(L"Radius: %s", DblStr(dRadius));
		// 
		// 		m_dUSurfCoeff = cSurfSphereData.m_sParam.m_dUCoeffA / m_dTopoContextScale;
		// 		m_dVSurfCoeff = cSurfSphereData.m_sParam.m_dVCoeffA / m_dTopoContextScale;
		// 
		// 		A3DUVParameterizationDataLog(cSurfNurbsData.m_sParam);
	}
	DecreaseLogTab();

	A3DSurfNurbsGet(nullptr, &cSurfNurbsData);
}

// == Curve 관련 함수 ===============================================================================
void A3dTracer::A3DCrvBaseLog(const A3DCrvBase * pcCrvBase)
{
	A3DEEntityType eType = kA3DTypeUnknown;
	if(A3D_SUCCESS != A3DEntityGetType(pcCrvBase, &eType)) {
		return;
	}

	switch(eType)
	{
		// 		case kA3DTypeCrvLine:
		// 			CHECK_BOOL_RETURN(ConvertCrvLine(pcCrvBase, pcCurve, pcSurface));
		// 			break;
		// 
		case kA3DTypeCrvCircle:
			A3DCrvCircleLog(pcCrvBase);
			break;

		case kA3DTypeCrvNurbs:
			A3DCrvNurbsLog(pcCrvBase);
			break;
	}
}

void A3dTracer::A3DCrvCircleLog(const A3DCrvBase * pcCrvBase)
{
	Log(L"A3DCrvNurbs");

	A3DCrvCircleData cCrvCircleData;
	A3D_INITIALIZE_DATA(A3DCrvCircleData, cCrvCircleData);
	CHECK_A3D_RETURN(A3DCrvCircleGet(pcCrvBase, &cCrvCircleData));

	IncreaseLogTab();
	{
		//ptrdiff_t nDegree = cCrvNurbsData.m_uiDegree;
	}
	DecreaseLogTab();
}

void A3dTracer::A3DCrvNurbsLog(const A3DCrvBase * pcCrvBase)
{
	Log(L"A3DCrvNurbs");

	A3DCrvNurbsData cCrvNurbsData;
	A3D_INITIALIZE_DATA(A3DCrvNurbsData, cCrvNurbsData);

	CHECK_A3D_RETURN(A3DCrvNurbsGet(pcCrvBase, &cCrvNurbsData));

	IncreaseLogTab();
	{
		ptrdiff_t nDegree = cCrvNurbsData.m_uiDegree;
		A3DUns32 nCtrlSize = cCrvNurbsData.m_uiCtrlSize;

		CString strKnotType = GetKnotTypeString(cCrvNurbsData.m_eKnotType);

		CString strCurveForm;
		switch(cCrvNurbsData.m_eCurveForm)
		{
			case kA3DBSplineCurveFormUnspecified:
				strCurveForm = L"Unspecified";
				break;

			case kA3DBSplineCurveFormPolyline:
				strCurveForm = L"Polyline";
				break;

			case kA3DBSplineCurveFormCircularArc:
				strCurveForm = L"CircularArc";
				break;

			case kA3DBSplineCurveFormEllipticArc:
				strCurveForm = L"EllipticArc";
				break;

			case kA3DBSplineCurveFormParabolicArc:
				strCurveForm = L"ParabolicArc";
				break;

			case kA3DBSplineCurveFormHyperbolicArc:
				strCurveForm = L"HyperbolicArc";
				break;
		}

		Log(L"KnotType: %s, CurveForm: %s", strKnotType, strCurveForm);
		Log(L"Degree: %d, Control Point Count: %d", nDegree, nCtrlSize);

		IncreaseLogTab();
		{
#ifdef _LOG_NURBS_CURVE_DETAIL_LOG_
			for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiCtrlSize; nIndex++) {
				CString strText;
				strText.Format(L"Po [%d]: ", nIndex);
				A3DVector3dDataUVLog(cCrvNurbsData.m_pCtrlPts[nIndex], strText, m_dTopoContextScale);
			}

			for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiKnotSize; nIndex++) {
				Log(L"Knot [%d]: %s", nIndex, DblStr(cCrvNurbsData.m_pdKnots[nIndex]));
			}

			for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiWeightSize; nIndex++) {
				Log(L"Weight [%d]: %s", nIndex, DblStr(cCrvNurbsData.m_pdWeights[nIndex]));
			}
#else
			CString strText;
			A3DUns32 nIndex = 0;
			strText.Format(L"Po [%d]: ", nIndex);
			A3DVector3dDataUVLog(cCrvNurbsData.m_pCtrlPts[nIndex], strText, m_dTopoContextScale);

			nIndex = cCrvNurbsData.m_uiCtrlSize - 1;
			strText.Format(L"Po [%d]: ", nIndex);
			A3DVector3dDataUVLog(cCrvNurbsData.m_pCtrlPts[nIndex], strText, m_dTopoContextScale);
#endif
		}
		DecreaseLogTab();
	}
	DecreaseLogTab();
}

// == 기타 요소 관련 함수 =============================================================================
void A3dTracer::A3DDomainDataLog(const A3DDomainData & cData)
{
	Log(L"A3DDomainData");

	IncreaseLogTab();

	Log(L"UMin / UMax: %s,%s (%s,%s)", DblStr(cData.m_sMin.m_dX * m_dTopoContextScale), DblStr(cData.m_sMax.m_dX * m_dTopoContextScale),
		DblStr(cData.m_sMin.m_dX), DblStr(cData.m_sMax.m_dX));
	Log(L"VMin / VMax: %s,%s", DblStr(cData.m_sMin.m_dY * m_dVSurfCoeff * m_dTopoContextScale), DblStr(cData.m_sMax.m_dY * m_dVSurfCoeff * m_dTopoContextScale));

	DecreaseLogTab();
}

void A3dTracer::A3DTopoVertexLog(const A3DTopoVertex * pcTopoVertex)
{
	A3DEEntityType eType = kA3DTypeUnknown;
	A3DEntityGetType(pcTopoVertex, &eType);

	double x = 0.0, y = 0.0, z = 0.0;

	CString strVertexType = L"Vertex Error";

	switch(eType)
	{
		case kA3DTypeTopoUniqueVertex:
		{
			A3DTopoUniqueVertexData cTopoUniqueVertexData;
			A3D_INITIALIZE_DATA(A3DTopoUniqueVertexData, cTopoUniqueVertexData);
			if(A3D_SUCCESS == A3DTopoUniqueVertexGet(pcTopoVertex, &cTopoUniqueVertexData)) {
				x = cTopoUniqueVertexData.m_sPoint.m_dX * m_dTopoContextScale;
				y = cTopoUniqueVertexData.m_sPoint.m_dY * m_dTopoContextScale;
				z = cTopoUniqueVertexData.m_sPoint.m_dZ * m_dTopoContextScale;

				A3DTopoUniqueVertexGet(nullptr, &cTopoUniqueVertexData);

				strVertexType = L"UniqueVertex";
			}
		}
		break;

		case kA3DTypeTopoMultipleVertex:
		{
			A3DTopoMultipleVertexData cMultipleVertexData;
			A3D_INITIALIZE_DATA(A3DTopoMultipleVertexData, cMultipleVertexData);
			if(A3D_SUCCESS == A3DTopoMultipleVertexGet(pcTopoVertex, &cMultipleVertexData)) {
				if(0 != cMultipleVertexData.m_uiSize) {
					x = cMultipleVertexData.m_pPts[0].m_dX * m_dTopoContextScale;
					y = cMultipleVertexData.m_pPts[0].m_dY * m_dTopoContextScale;
					z = cMultipleVertexData.m_pPts[0].m_dZ * m_dTopoContextScale;
				}

				A3DTopoMultipleVertexGet(nullptr, &cMultipleVertexData);

				strVertexType.Format(L"MultipleVertex [%d]", cMultipleVertexData.m_uiSize);
			}
		}
		break;
	}

	Log(L"%s: %s, %s,%s,%s", strVertexType, HexStr((DWORD_PTR) pcTopoVertex), DblStr(x), DblStr(y), DblStr(z));
}

void A3dTracer::A3DVector3dDataLog(const A3DVector3dData & cVector, CString strPrevText, double dScale)
{
	CString strNumberText;
	strNumberText.Format(L"%s, %s, %s", DblStr(cVector.m_dX * dScale), DblStr(cVector.m_dY * dScale),
		DblStr(cVector.m_dZ * dScale));

	if(true == strPrevText.IsEmpty()) {
		Log(strNumberText);
	}
	else {
		Log(L"%s%s", strPrevText, strNumberText);
	}
}

void A3dTracer::A3DVector3dDataUVLog(const A3DVector3dData & cVector, CString strPrevText, double dScale)
{
	CString strNumberText;
	strNumberText.Format(L"%s, %s (%s, %s)", DblStr(cVector.m_dX * dScale * m_dUSurfCoeff), DblStr(cVector.m_dY * dScale * m_dVSurfCoeff),
		DblStr(cVector.m_dX), DblStr(cVector.m_dY));

	if(true == strPrevText.IsEmpty()) {
		Log(strNumberText);
	}
	else {
		Log(L"%s%s", strPrevText, strNumberText);
	}
}

void A3dTracer::A3DVector2dDataLog(const A3DVector2dData & cVector, CString strPrevText, double dScale)
{
	CString strNumberText;
	strNumberText.Format(L"%s, %s", DblStr(cVector.m_dX * dScale), DblStr(cVector.m_dY * dScale));

	if(true == strPrevText.IsEmpty()) {
		Log(strNumberText);
	}
	else {
		Log(L"%s%s", strPrevText, strNumberText);
	}
}

void A3dTracer::A3DUVParameterizationDataLog(const A3DUVParameterizationData & cParam)
{
	Log(L"SwapUV: %s", BoolStr(cParam.m_bSwapUV));
	Log(L"UA,UB Coeff: %s, %s", DblStr(cParam.m_dUCoeffA), DblStr(cParam.m_dUCoeffB));
	Log(L"VA,VB Coeff: %s, %s", DblStr(cParam.m_dVCoeffA), DblStr(cParam.m_dVCoeffB));

	A3DVector2dDataLog(cParam.m_sUVDomain.m_sMax, L"UVDomain Max: ", 1.0);
	A3DVector2dDataLog(cParam.m_sUVDomain.m_sMin, L"UVDomain Min: ", 1.0);
}

CString A3dTracer::GetKnotTypeString(A3DEKnotType eKnotType)
{
	CString strKnotTypeText;
	switch(eKnotType)
	{
		case kA3DKnotTypeUniformKnots:
			strKnotTypeText = L"Uniform";
			break;

		case kA3DKnotTypeUnspecified:
			strKnotTypeText = L"Unspecified";
			break;

		case kA3DKnotTypeQuasiUniformKnots:
			strKnotTypeText = L"Quasi-uniform";
			break;

		case kA3DKnotTypePieceWiseBezierKnots:
			strKnotTypeText = L"PieceWiseBezier"; // Extrema with multiplicities of degree + 1, internal is degree.
			break;
	}

	return strKnotTypeText;
}

CString A3dTracer::GetBSplineSurfaceFormString(A3DEBSplineSurfaceForm eSurfaceForm)
{
	CString strSurfaceForm;

	switch(eSurfaceForm)
	{
		case kA3DBSplineSurfaceFormPlane:
			strSurfaceForm = L"Planar surface";
			break;

		case kA3DBSplineSurfaceFormCylindrical:
			strSurfaceForm = L"Cylindrical surface";
			break;

		case kA3DBSplineSurfaceFormConical:
			strSurfaceForm = L"Conical surface";
			break;

		case kA3DBSplineSurfaceFormSpherical:
			strSurfaceForm = L"Spherical surface";
			break;

		case kA3DBSplineSurfaceFormRevolution:
			strSurfaceForm = L"Surface of revolution";
			break;

		case kA3DBSplineSurfaceFormRuled:
			strSurfaceForm = L"Ruled surface";
			break;

		case kA3DBSplineSurfaceFormGeneralizedCone:
			strSurfaceForm = L"Cone";
			break;

		case kA3DBSplineSurfaceFormQuadric:
			strSurfaceForm = L"Quadric surface";
			break;

		case kA3DBSplineSurfaceFormLinearExtrusion:
			strSurfaceForm = L"Surface of extrusion";
			break;

		case kA3DBSplineSurfaceFormUnspecified:
			strSurfaceForm = L"Unspecified";
			break;

		case kA3DBSplineSurfaceFormPolynomial:
			strSurfaceForm = L"Polynomial surface";
			break;
	}

	return strSurfaceForm;
}

// == Log 관련 함수 =================================================================================
void A3dTracer::CreateLog(CString strFilePathName)
{
	m_nOldCurrentId = LogManager::GetCurrentId();
	LogManager::SetCurrentId(3);
	LogManager::ResetTabIndex();
	LogManager::SetFilePathName(strFilePathName);
	LogManager::SetCreateFile(true);
	LogManager::SetWriteLog(true);

	LogManager::SetWriteTimeLog(true);
	Log(strFilePathName);
	LogManager::SetWriteTimeLog(false);
}

void A3dTracer::Log(LPCWSTR chMessage, ...)
{
	if(false == LogManager::IsWriteLog()) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(strBuffer);
}

void A3dTracer::IncreaseLogTab()
{
	LogManager::IncreaseTabIndex();
}

void A3dTracer::DecreaseLogTab()
{
	LogManager::DecreaseTabIndex();
}

CString A3dTracer::DblStr(double dValue)
{
	CString strText;
	strText.Format(L"%.8f", dValue);
	return strText;
}

CString A3dTracer::HexStr(DWORD_PTR nValue)
{
	CString strText;

	if(0 == nValue) {
		strText = L"NULL";
	}
	else {
		strText.Format(L"0x%llx", nValue);
	}
	return strText;
}

CString A3dTracer::BoolStr(bool bValue)
{
	if(true == bValue) {
		return L"true";
	}

	return L"false";
}


