#include "stdafx.h"

#include "ConvSurface.h"

#include "ConvLoop.h"
#include "ConvCurve.h"

#include "../Dmi3dx.h"

#include <model.h>
#include <solid.h>
#include <action_solid.h>
#include "../../C3D/Kernel/Include/instance.h"
#include <space_instance.h>
#include <assisting_item.h>

// Surface
#include <surf_plane.h>
#include <surf_cylinder_surface.h>
#include <surf_spline_surface.h>
#include <surf_curve_bounded_surface.h>
#include <surf_extrusion_surface.h>
#include <surf_elevation_surface.h>
#include <surf_revolution_surface.h>

// 3D curve
#include <curve3d.h>
#include <cur_line_segment3d.h>
#include <cur_line3d.h>
#include <cur_arc3d.h>
#include <cur_nurbs3d.h>
#include <cur_surface_intersection.h>
#include <cur_surface_curve.h>
#include <cur_contour3d.h>
#include <cur_polyline3d.h>
#include <cur_cubic_spline3d.h>
#include <cur_hermit3d.h>

#include "../Surface/DmiSurface.h"
#include "../Surface/DmiPlane.h"
#include "../Surface/DmiCylinderSurface.h"
#include "../Surface/DmiConeSurface.h"
#include "../Surface/DmiSphereSurface.h"
#include "../Surface/DmiTorusSurface.h"
#include "../Surface/DmiRuledSurface.h"
#include "../Surface/DmiElevationSurface.h"
#include "../Surface/DmiRevolutionSurface.h"
#include "../Surface/DmiExtrusionSurface.h"

using namespace c3d;

namespace ConvSurfaceBase
{
	double dTolerance = 1.0e-6;
};

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RETURN(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

ConvSurface::ConvSurface(const A3DSurfBase * pcSurfBase, double dContexScale)
{
	if(nullptr == pcSurfBase) {
		return;
	}

	m_pcSurfBase = pcSurfBase;

	// Surface 정보를 수집해서 저장한다.
	// dContexScale은 AsGetNurb에서 공차를 설정하기 위해서임.
	if(false == ConvSurfaceBase::GetSurfBaseData(pcSurfBase, dContexScale, m_cEntityInfo)) {
		return;
	}

	/*
		if(kA3DTypeSurfOffset == m_eSurfaceType) {
			if(false == ConvSurfaceBase::GetSurfBaseData(((A3DSurfOffsetData *) m_pcSurfBaseData)->m_pBasisSurf, dContexScale,
				m_eRelationEntityType, m_pcRelationEntityData)) {
				return;
			}
		}
	*/
	m_bInitFlag = true;
}

ConvSurface::~ConvSurface()
{
	if(false == m_bInitFlag) {
		return;
	}

	ConvSurfaceBase::ClearSurfBaseData(m_cEntityInfo);
	m_cEntityInfo.Delete();
}

bool ConvSurface::ConvertSurface(double dContextScale)
{
	return ConvertSurface(dContextScale, dContextScale, m_pcSurface);
}

bool ConvSurface::ConvertSurface(double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	m_cEntityInfo.nTargetEntityIndex = 0;
	return ConvSurfaceBase::ConvertSurfBaseData(m_cEntityInfo, dParentScale, dContextScale, pcSurface);
}

// == 1. Surface Data 수집 함수 ======================================================================

// 1.Surface Data 수집
bool ConvSurfaceBase::GetSurfBaseData(const A3DSurfBase * pcSurfBase, double dContextScale, ConvEntityInfo & cEntityInfo)
{
	A3DEEntityType eSurfaceType = kA3DTypeUnknown;
	CHECK_A3D_RETURN(A3DEntityGetType(pcSurfBase, &eSurfaceType));

	double dTolerance = ConvSurfaceBase::dTolerance / dContextScale;

	int nIndex = cEntityInfo.nTargetEntityIndex;
	if(-1 == nIndex) {
		// Entity count는 한개임.
		int nEntityCount = 1;
		switch(eSurfaceType)
		{
			case kA3DTypeSurfExtrusion:
			case kA3DTypeSurfRevolution:
				nEntityCount = 2;
				break;

			case kA3DTypeSurfRuled:
				nEntityCount = 3;
				break;

			case kA3DTypeSurfOffset:
				nEntityCount = 5;
				break;
		}

		cEntityInfo.Init(nEntityCount);
		nIndex = cEntityInfo.nTargetEntityIndex = 0;
	}

	cEntityInfo.peEntityType[nIndex] = eSurfaceType;

	A3DStatus nStatus = A3D_ERROR;
	DWORD_PTR * pcData = nullptr;
	switch(eSurfaceType)
	{
		case kA3DTypeSurfPlane:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfPlaneData();
			A3D_INITIALIZE_DATA(A3DSurfPlaneData, (*((A3DSurfPlaneData *) pcData)));
			nStatus = A3DSurfPlaneGet(pcSurfBase, (A3DSurfPlaneData *) pcData);
			break;

		case kA3DTypeSurfCylinder:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfCylinderData();
			A3D_INITIALIZE_DATA(A3DSurfCylinderData, (*((A3DSurfCylinderData *) pcData)));
			nStatus = A3DSurfCylinderGet(pcSurfBase, (A3DSurfCylinderData *) pcData);
			break;

		case kA3DTypeSurfCone:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfConeData();
			A3D_INITIALIZE_DATA(A3DSurfConeData, (*((A3DSurfConeData *) pcData)));
			nStatus = A3DSurfConeGet(pcSurfBase, (A3DSurfConeData *) pcData);
			break;

		case kA3DTypeSurfSphere:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfSphereData();
			A3D_INITIALIZE_DATA(A3DSurfSphereData, (*((A3DSurfSphereData *) pcData)));
			nStatus = A3DSurfSphereGet(pcSurfBase, (A3DSurfSphereData *) pcData);
			break;

		case kA3DTypeSurfTorus:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfTorusData();
			A3D_INITIALIZE_DATA(A3DSurfTorusData, (*((A3DSurfTorusData *) pcData)));
			nStatus = A3DSurfTorusGet(pcSurfBase, (A3DSurfTorusData *) pcData);
			break;

		case kA3DTypeSurfNurbs:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfNurbsData();
			A3D_INITIALIZE_DATA(A3DSurfNurbsData, (*((A3DSurfNurbsData *) pcData)));
			nStatus = A3DSurfNurbsGet(pcSurfBase, (A3DSurfNurbsData *) pcData);
			break;

		case kA3DTypeSurfOffset:
		{
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfOffsetData();
			A3D_INITIALIZE_DATA(A3DSurfOffsetData, (*((A3DSurfOffsetData *) pcData)));
			nStatus = A3DSurfOffsetGet(pcSurfBase, (A3DSurfOffsetData *) pcData);

			cEntityInfo.nTargetEntityIndex++;
			A3DSurfBase * pcSurfBase = ((A3DSurfOffsetData *) pcData)->m_pBasisSurf;
			if(false == ConvSurfaceBase::GetSurfBaseData(pcSurfBase, dContextScale, cEntityInfo)) {
				nStatus = A3D_ERROR;
			}
		}
		break;

		case kA3DTypeSurfRuled:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfNurbsData();
			A3D_INITIALIZE_DATA(A3DSurfNurbsData, (*((A3DSurfNurbsData *) pcData)));
			nStatus = A3DSurfBaseWithDomainGetAsNurbs(pcSurfBase, nullptr, dTolerance, true, (A3DSurfNurbsData *) pcData);
			cEntityInfo.peEntityType[nIndex] = kA3DTypeSurfNurbs;

			/*
						pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfRuledData();
						A3D_INITIALIZE_DATA(A3DSurfRuledData, (*((A3DSurfRuledData *) pcData)));
						nStatus = A3DSurfRuledGet(pcSurfBase, (A3DSurfRuledData *) pcData);

						cEntityInfo.nTargetEntityIndex++;
						ConvCurveBase::GetCurveData(((A3DSurfRuledData *) pcData)->m_pFirstCurve, dContextScale, cEntityInfo);

						cEntityInfo.nTargetEntityIndex++;
						ConvCurveBase::GetCurveData(((A3DSurfRuledData *) pcData)->m_pSecondCurve, dContextScale, cEntityInfo);
			*/
			break;

		case kA3DTypeSurfExtrusion:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfNurbsData();
			A3D_INITIALIZE_DATA(A3DSurfNurbsData, (*((A3DSurfNurbsData *) pcData)));
			nStatus = A3DSurfBaseWithDomainGetAsNurbs(pcSurfBase, nullptr, dTolerance, true, (A3DSurfNurbsData *) pcData);
			cEntityInfo.peEntityType[nIndex] = kA3DTypeSurfNurbs;
			/*

						pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfExtrusionData();
						A3D_INITIALIZE_DATA(A3DSurfExtrusionData, (*((A3DSurfExtrusionData *) pcData)));
						nStatus = A3DSurfExtrusionGet(pcSurfBase, (A3DSurfExtrusionData *) pcData);

						cEntityInfo.nTargetEntityIndex++;
						ConvCurveBase::GetCurveData(((A3DSurfExtrusionData *) pcData)->m_pCurve, dContextScale, cEntityInfo);
			*/
			break;

		case kA3DTypeSurfBlend03:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfNurbsData();
			A3D_INITIALIZE_DATA(A3DSurfNurbsData, (*((A3DSurfNurbsData *) pcData)));
			nStatus = A3DSurfBaseWithDomainGetAsNurbs(pcSurfBase, nullptr, dTolerance, true, (A3DSurfNurbsData *) pcData);
			cEntityInfo.peEntityType[nIndex] = kA3DTypeSurfNurbs;
			break;

		case kA3DTypeSurfRevolution:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DSurfRevolutionData();
			A3D_INITIALIZE_DATA(A3DSurfRevolutionData, (*((A3DSurfRevolutionData *) pcData)));
			nStatus = A3DSurfRevolutionGet(pcSurfBase, (A3DSurfRevolutionData *) pcData);

			cEntityInfo.nTargetEntityIndex++;
			ConvCurveBase::GetCurveData(((A3DSurfRevolutionData *) pcData)->m_pCurve, dContextScale, cEntityInfo);

			break;
			/*
					case kA3DTypeSurfPipe:
						break;

					case kA3DTypeSurfFromCurves:
						break;
			*/
	}

	if(A3D_SUCCESS != nStatus) {
		CString strType = Dmi3dx::GetA3dEntityTypeString(eSurfaceType).c_str();
		CString strType1 = Dmi3dx::GetA3dEntityTypeString((A3DEEntityType) cEntityInfo.peEntityType[nIndex]).c_str();
		LogManager::Log(2, L"GetSurfBaseData Error: %s, [%s]", strType, strType1);
		return false;
	}

	return true;
}

// 2. Surface Data 삭제
bool ConvSurfaceBase::ClearSurfBaseData(ConvEntityInfo & cEntityInfo)
{
	if(0 == cEntityInfo.nEntityCount) {
		return false;
	}

	switch(cEntityInfo.peEntityType[0])
	{
		case kA3DTypeSurfPlane:
			A3DSurfPlaneGet(nullptr, (A3DSurfPlaneData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfCylinder:
			A3DSurfCylinderGet(nullptr, (A3DSurfCylinderData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfCone:
			A3DSurfConeGet(nullptr, (A3DSurfConeData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfSphere:
			A3DSurfSphereGet(nullptr, (A3DSurfSphereData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfTorus:
			A3DSurfTorusGet(nullptr, (A3DSurfTorusData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfNurbs:
			A3DSurfNurbsGet(nullptr, (A3DSurfNurbsData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfOffset:
			A3DSurfOffsetGet(nullptr, (A3DSurfOffsetData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfRuled:
			A3DSurfRuledGet(nullptr, (A3DSurfRuledData *) cEntityInfo.pcEntityData[0]);
			break;

		case kA3DTypeSurfExtrusion:
			A3DSurfExtrusionGet(nullptr, (A3DSurfExtrusionData *) cEntityInfo.pcEntityData[0]);
			break;

			// NURBS Surface 생성해서 전달했기 때문에, Blend03으로 들어오지 않음.
		case kA3DTypeSurfBlend03:
			break;

		case kA3DTypeSurfRevolution:
			A3DSurfRevolutionGet(nullptr, (A3DSurfRevolutionData *) cEntityInfo.pcEntityData[0]);
			break;
	}

	cEntityInfo.peEntityType[0] = kA3DTypeUnknown;
	cEntityInfo.pcEntityData[0] = nullptr;

	return true;
}

// == 2. Surface 변환 함수 ===========================================================================

// 2. 사전에 수집되어 있는 정보를 이용해서 Surface 생성
bool ConvSurfaceBase::ConvertSurfBase(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	A3DEEntityType eType = kA3DTypeUnknown;
	CHECK_A3D_RETURN(A3DEntityGetType(pcSurfBase, &eType));

	bool bStatus = false;
	bool bSwapUV = false;

	switch(eType)
	{
		case kA3DTypeSurfPlane:
			bStatus = ConvertSurfPlane(pcSurfBase, dParentScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfCylinder:
			bStatus = ConvertSurfCylinder(pcSurfBase, dParentScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfCone:
			bStatus = ConvertSurfCone(pcSurfBase, dParentScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfSphere:
			bStatus = ConvertSurfSphere(pcSurfBase, dParentScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfTorus:
			bStatus = ConvertSurfTorus(pcSurfBase, dParentScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfNurbs:
			bStatus = ConvertSurfNurbs(pcSurfBase, dParentScale, pcSurface);
			break;

		case kA3DTypeSurfExtrusion:
			bStatus = ConvertSurfExtrusion(pcSurfBase, dParentScale, dContextScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfOffset:
			bStatus = ConvertSurfOffset(pcSurfBase, dParentScale, dContextScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfRuled:
			bStatus = ConvertSurfRuled(pcSurfBase, dParentScale, dContextScale, bSwapUV, pcSurface);
			break;

		case kA3DTypeSurfRevolution:
			//bStatus
			break;

		case kA3DTypeSurfBlend03:
			bStatus = ConvertSurfBlend03(pcSurfBase, dContextScale, bSwapUV, pcSurface);
			break;

			// 		case kA3DTypeSurfOffset:
			// 			CHECK_BOOL_RETURN(ConvertSurfOffset(pcSurfBase, pcSurface));
			// 			break;

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
			*/
	}

	if(true == bStatus) {
		MbSurface * pcMbSurface = pcSurface;
		DmiSurface * pcDmiSurface = dynamic_cast<DmiSurface *>(pcMbSurface);
		if(nullptr != pcDmiSurface) {
			pcDmiSurface->SetSwapUV(bSwapUV);
		}
	}
	else {
		CString strType = Dmi3dx::GetA3dEntityTypeString(eType).c_str();
		LogManager::Log(2, L"ConvertSurfBase Error: Type %s", strType);
	}

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfBaseData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	/*
		if(0 == cEntityInfo.nEntityCount) {
			return false;
		}

		int nIndex = cEntityInfo.nTargetEntityIndex;
		bool bStatus = false;

		switch(cEntityInfo.peEntityType[nIndex])
		{
			case kA3DTypeSurfPlane:
				bStatus = ConvertSurfPlaneData(cEntityInfo, dParentScale, pcSurface);
				break;

			case kA3DTypeSurfCylinder:
				bStatus = ConvertSurfCylinderData(cEntityInfo, dParentScale, false, pcSurface);
				break;

			case kA3DTypeSurfCone:
				bStatus = ConvertSurfConeData(cEntityInfo, dParentScale, pcSurface);
				break;

			case kA3DTypeSurfSphere:
				bStatus = ConvertSurfSphereData(cEntityInfo, dParentScale, pcSurface);
				break;

			case kA3DTypeSurfTorus:
				bStatus = ConvertSurfTorusData(cEntityInfo, dParentScale, pcSurface);
				break;

			case kA3DTypeSurfNurbs:
				bStatus = ConvertSurfNurbsData(cEntityInfo, dParentScale, pcSurface);
				break;

			case kA3DTypeSurfOffset:
				bStatus = ConvertSurfOffsetData(cEntityInfo, dParentScale, dContextScale, pcSurface);
				break;

			case kA3DTypeSurfExtrusion:
				bStatus = ConvertSurfExtrusionData(cEntityInfo, dParentScale, dContextScale, pcSurface);
				break;

			case kA3DTypeSurfRuled:
				bStatus = ConvertSurfRuledData(cEntityInfo, dParentScale, dContextScale, pcSurface);
				break;

			case kA3DTypeSurfRevolution:
				bStatus = ConvertSurfRevolutionData(cEntityInfo, dParentScale, dContextScale, pcSurface);
				break;
		}

		if(false == bStatus) {
			CString strType = Dmi3dx::GetA3dEntityTypeString(cEntityInfo.peEntityType[nIndex]).c_str();
			LogManager::Log(2, L"ConvertSurfBaseData Error: %s", strType);
		}

		return bStatus;
	*/
	return false;
}

// 2-1. Plane
bool ConvSurfaceBase::ConvertSurfPlane(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfPlaneData cSurfPlaneData;
	A3D_INITIALIZE_DATA(A3DSurfPlaneData, cSurfPlaneData);
	CHECK_A3D_RETURN(A3DSurfPlaneGet(pcSurfBase, &cSurfPlaneData));

	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cSurfPlaneData.m_sTrsf.m_sOrigin, dParentScale);
	MbVector3D cXAxis = Dmi3dx::GetMbVector3D(cSurfPlaneData.m_sTrsf.m_sXVector);
	MbVector3D cYAxis = Dmi3dx::GetMbVector3D(cSurfPlaneData.m_sTrsf.m_sYVector);

	DmiPlane * pcPlane = new DmiPlane(cOrigin, cXAxis, cYAxis);
	if(nullptr == pcPlane) {
		return false;
	}

	pcPlane->SetUVParameterizationData(cSurfPlaneData.m_sParam, dParentScale, dParentScale);

	// #temp : 함수들이 정리되면 삭제해야 함.
	bSwapUV = (1 == cSurfPlaneData.m_sParam.m_bSwapUV) ? true : false;

	pcSurface = (MbSurface *) pcPlane;

	A3DSurfPlaneGet(nullptr, &cSurfPlaneData);

	return true;
}

// 2-2. Cylinder
bool ConvSurfaceBase::ConvertSurfCylinder(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfCylinderData cSurfCylinderData;
	A3D_INITIALIZE_DATA(A3DSurfCylinderData, cSurfCylinderData);
	CHECK_A3D_RETURN(A3DSurfCylinderGet(pcSurfBase, &cSurfCylinderData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfCylinderData;

	bSwapUV = (1 == cSurfCylinderData.m_sParam.m_bSwapUV) ? true : false;

	bool bStatus = ConvSurfaceBase::ConvertSurfCylinderData(cEntityInfo, dParentScale, bSwapUV, pcSurface);

	A3DSurfCylinderGet(nullptr, &cSurfCylinderData);

	return bStatus;

}

bool ConvSurfaceBase::ConvertSurfCylinderData(ConvEntityInfo & cEntityInfo, double dParentScale, bool bSwapUV, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfCylinderData & cSurfCylinderData = *((A3DSurfCylinderData *) cEntityInfo.pcEntityData[nTargetIndex]);

	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cSurfCylinderData.m_sTrsf.m_sOrigin, dParentScale);
	MbVector3D cXAxis = Dmi3dx::GetMbVector3D(cSurfCylinderData.m_sTrsf.m_sXVector);
	MbVector3D cYAxis = Dmi3dx::GetMbVector3D(cSurfCylinderData.m_sTrsf.m_sYVector);
	const MbPlacement3D cPlacement(cXAxis, cYAxis, cOrigin);

	double dRadius = cSurfCylinderData.m_dRadius * dParentScale;

	double dUCoeff = cSurfCylinderData.m_sParam.m_dUCoeffA;
	double dVCoeff = cSurfCylinderData.m_sParam.m_dVCoeffA;

	// Height는 배율값으로 계산되는 것으로 생각하고 입력.
	DmiCylinderSurface * pcCylinder = new DmiCylinderSurface(cPlacement, dRadius, 1.0);

	if(nullptr == pcCylinder) {
		return false;
	}

	pcCylinder->SetUVCoeffA(dUCoeff / dParentScale, dVCoeff);
	pcSurface = (MbSurface *) pcCylinder;

	return true;
}

// 2-3. Cone
bool ConvSurfaceBase::ConvertSurfCone(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfConeData cSurfConeData;
	A3D_INITIALIZE_DATA(A3DSurfConeData, cSurfConeData);
	CHECK_A3D_RETURN(A3DSurfConeGet(pcSurfBase, &cSurfConeData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfConeData;

	bool bStatus = ConvSurfaceBase::ConvertSurfConeData(cEntityInfo, dParentScale, pcSurface);
	bSwapUV = (1 == cSurfConeData.m_sParam.m_bSwapUV) ? true : false;

	A3DSurfConeGet(nullptr, &cSurfConeData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfConeData(ConvEntityInfo & cEntityInfo, double dParentScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfConeData & cSurfConeData = *((A3DSurfConeData *) cEntityInfo.pcEntityData[nTargetIndex]);

	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cSurfConeData.m_sTrsf.m_sOrigin, dParentScale);
	MbVector3D cXAxis = Dmi3dx::GetMbVector3D(cSurfConeData.m_sTrsf.m_sXVector);
	MbVector3D cYAxis = Dmi3dx::GetMbVector3D(cSurfConeData.m_sTrsf.m_sYVector);

	MbPlacement3D cPlacement(cXAxis, cYAxis, cOrigin);

	double dUCoeff = cSurfConeData.m_sParam.m_dUCoeffA;
	double dVCoeff = cSurfConeData.m_sParam.m_dVCoeffA;
	double dVCoeff1 = cSurfConeData.m_sParam.m_dVCoeffB;

	double dSemiAngle = cSurfConeData.m_dSemiAngle;

	double dEdge = cSurfConeData.m_sParam.m_sUVDomain.m_sMax.m_dY * dParentScale;
	double dRadius = cSurfConeData.m_dRadius * dParentScale;
	//double dRadius = dEdge * sin(dSemiAngle);
	//double dHeight = dEdge * cos(dSemiAngle);
	double dHeight = 1.0;

	if(0.0 > dRadius) {
		cPlacement.Invert();
	}

// 	double dVmin = -dRadius / tan(dSemiAngle);
// 	double dVmax = dEdge * cos(dSemiAngle);

	// Constructor by a local coordinate system, radius, angle and height. 
	DmiConeSurface * pcCone = new DmiConeSurface(cPlacement, dRadius, dSemiAngle, dHeight);

	if(0.0 > dRadius) {
		pcCone->SetNegativeRadius(true);
	}

	if(nullptr == pcCone) {
		return false;
	}

	double dVMin1 = pcCone->GetVMin();
	double dVMax1 = pcCone->GetVMax();

	pcCone->SetUVCoeffA(dUCoeff / dParentScale, dVCoeff);
	pcCone->SetVCoeff1(dVCoeff1 * dParentScale);
	pcSurface = (MbSurface *) pcCone;

	return true;
}

// 2-4. Sphere
bool ConvSurfaceBase::ConvertSurfSphere(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfSphereData cSurfSphereData;
	A3D_INITIALIZE_DATA(A3DSurfSphereData, cSurfSphereData);
	CHECK_A3D_RETURN(A3DSurfSphereGet(pcSurfBase, &cSurfSphereData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfSphereData;

	bool bStatus = ConvSurfaceBase::ConvertSurfSphereData(cEntityInfo, dParentScale, pcSurface);
	bSwapUV = (1 == cSurfSphereData.m_sParam.m_bSwapUV) ? true : false;

	A3DSurfSphereGet(nullptr, &cSurfSphereData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfSphereData(ConvEntityInfo & cEntityInfo, double dParentScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfSphereData & cSurfSphereData = *((A3DSurfSphereData *) cEntityInfo.pcEntityData[nTargetIndex]);

	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cSurfSphereData.m_sTrsf.m_sOrigin, dParentScale);
	MbVector3D cXAxis = Dmi3dx::GetMbVector3D(cSurfSphereData.m_sTrsf.m_sXVector);
	MbVector3D cYAxis = Dmi3dx::GetMbVector3D(cSurfSphereData.m_sTrsf.m_sYVector);
	const MbPlacement3D cPlacement(cXAxis, cYAxis, cOrigin);

	double dRadius = cSurfSphereData.m_dRadius * dParentScale;

	DmiSphereSurface * pcSphere = new DmiSphereSurface(cPlacement, dRadius);

	double dUCoeff = cSurfSphereData.m_sParam.m_dUCoeffA / dParentScale;
	double dVCoeff = cSurfSphereData.m_sParam.m_dVCoeffA / dParentScale;
	pcSphere->SetUVCoeffA(dUCoeff, dVCoeff);

	pcSurface = (MbSurface *) pcSphere;

	return true;
}

// 2-5. Torus
bool ConvSurfaceBase::ConvertSurfTorus(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfTorusData cSurfTorusData;
	A3D_INITIALIZE_DATA(A3DSurfTorusData, cSurfTorusData);
	CHECK_A3D_RETURN(A3DSurfTorusGet(pcSurfBase, &cSurfTorusData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfTorusData;

	bool bStatus = ConvSurfaceBase::ConvertSurfTorusData(cEntityInfo, dParentScale, pcSurface);
	bSwapUV = (1 == cSurfTorusData.m_sParam.m_bSwapUV) ? true : false;

	A3DSurfTorusGet(nullptr, &cSurfTorusData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfTorusData(ConvEntityInfo & cEntityInfo, double dParentScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfTorusData & cSurfTorusData = *((A3DSurfTorusData *) cEntityInfo.pcEntityData[nTargetIndex]);

	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cSurfTorusData.m_sTrsf.m_sOrigin, dParentScale);
	MbVector3D cXAxis = Dmi3dx::GetMbVector3D(cSurfTorusData.m_sTrsf.m_sXVector);
	MbVector3D cYAxis = Dmi3dx::GetMbVector3D(cSurfTorusData.m_sTrsf.m_sYVector);
	const MbPlacement3D cPlacement(cXAxis, cYAxis, cOrigin);

	double dMajorRadius = cSurfTorusData.m_dMajorRadius * dParentScale;
	double dMinorRadius = cSurfTorusData.m_dMinorRadius * dParentScale;

/*
	if(0.0 > dMajorRadius) {
//		dMajorRadius = -dMajorRadius;
		std::swap(dMajorRadius, dMinorRadius);
	}
*/

	// Torus의 경계면 밖으로 Curve가 생성되는 경우 Major Radius를 Minus로 설정하고, 입력되는 좌표값을 보정해서 사용해야 한다.
/*
	if(dMajorRadius < dMinorRadius) {
		double dAngle = M_PI - acos(dMajorRadius / dMinorRadius);

		double dVMin = cSurfTorusData.m_sParam.m_sUVDomain.m_sMin.m_dY * cSurfTorusData.m_sParam.m_dVCoeffA;
		double dVMax = cSurfTorusData.m_sParam.m_sUVDomain.m_sMax.m_dY * cSurfTorusData.m_sParam.m_dVCoeffA;

		double dVMinDegree = dVMin * M_RADDEG;
		double dVMaxDegree = dVMax * M_RADDEG;

		if(M_PI < dVMax) {
			dVMin -= M_PI2;
			dVMax -= M_PI2;
		}
		else if(-M_PI2 > dVMax) {
			dVMin += M_PI2;
			dVMax += M_PI2;
		}

		if(dVMax > dAngle || dVMin < -dAngle) {
			dMajorRadius = -dMajorRadius;
		}
	}
*/

	DmiTorusSurface * pcTorus = new DmiTorusSurface(cPlacement, dMajorRadius, dMinorRadius);

	double dUCoeff = cSurfTorusData.m_sParam.m_dUCoeffA / dParentScale;
	double dVCoeff = cSurfTorusData.m_sParam.m_dVCoeffA / dParentScale;

	pcTorus->SetUVCoeffA(dUCoeff, dVCoeff);
	pcTorus->SetUVCoeffB(cSurfTorusData.m_sParam.m_dUCoeffB, cSurfTorusData.m_sParam.m_dVCoeffB);

	pcSurface = (MbSurface *) pcTorus;

	return true;
}

// 2-6. Nurbs surface
bool ConvSurfaceBase::ConvertSurfNurbs(const A3DSurfBase * pcSurfBase, double dParentScale, SurfaceSPtr & pcSurface)
{
	A3DSurfNurbsData cSurfNurbsData;
	A3D_INITIALIZE_DATA(A3DSurfNurbsData, cSurfNurbsData);
	CHECK_A3D_RETURN(A3DSurfNurbsGet(pcSurfBase, &cSurfNurbsData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfNurbsData;

	bool bStatus = ConvSurfaceBase::ConvertSurfNurbsData(cEntityInfo, dParentScale, pcSurface);

	A3DSurfNurbsGet(nullptr, &cSurfNurbsData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfNurbsData(ConvEntityInfo & cEntityInfo, double dParentScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfNurbsData & cSurfNurbsData = *((A3DSurfNurbsData *) cEntityInfo.pcEntityData[nTargetIndex]);

	A3DUns32 nUDegree = cSurfNurbsData.m_uiUDegree;
	A3DUns32 nVDegree = cSurfNurbsData.m_uiVDegree;

	A3DUns32 nUCtrlSize = cSurfNurbsData.m_uiUCtrlSize;
	A3DUns32 nVCtrlSize = cSurfNurbsData.m_uiVCtrlSize;

	// 2. Control Point & Weight 설정
	Array2<MbCartPoint3D> acCtrlPoints;
	if(false == acCtrlPoints.SetSize(nVCtrlSize, nUCtrlSize)) {
		return false;
	}

	for(A3DUns32 nVCtrlIndex = 0; nVCtrlIndex < nVCtrlSize; nVCtrlIndex++) {
		for(A3DUns32 nUCtrlIndex = 0; nUCtrlIndex < nUCtrlSize; nUCtrlIndex++) {
			A3DUns32 nIndex = (nUCtrlIndex * nVCtrlSize) + nVCtrlIndex;
			MbCartPoint3D cCartPoint = Dmi3dx::GetMbCartPoint3D(cSurfNurbsData.m_pCtrlPts[nIndex], dParentScale);
			acCtrlPoints.Init(nVCtrlIndex, nUCtrlIndex, cCartPoint);
		}
	}

	// 2. Weight 설정
	Array2<double> adWeights;

	// Weight 값이 없는 경우는 1.0으로 채운다.
	if(nullptr != cSurfNurbsData.m_pdWeights) {
		if(false == adWeights.SetSize(nVCtrlSize, nUCtrlSize)) {
			return false;
		}

		for(A3DUns32 nVCtrlIndex = 0; nVCtrlIndex < nVCtrlSize; nVCtrlIndex++) {
			for(A3DUns32 nUCtrlIndex = 0; nUCtrlIndex < nUCtrlSize; nUCtrlIndex++) {
				A3DUns32 nIndex = (nUCtrlIndex * nVCtrlSize) + nVCtrlIndex;
				double dWeight = cSurfNurbsData.m_pdWeights[nIndex];
				adWeights.Init(nVCtrlIndex, nUCtrlIndex, dWeight);
			}
		}
	}

	// U Knot 값 설정
	SArray<double> adUKnots;
	A3DUns32 nUKnotSize = cSurfNurbsData.m_uiUKnotSize; // nUCtrlSize + nUDegree
	for(A3DUns32 nIndex = 0; nIndex < nUKnotSize; nIndex++) {
		adUKnots.Add(cSurfNurbsData.m_pdUKnots[nIndex] * dParentScale); // 스케일을 곱해야 정상적인 값이 나옴.
	}

	// V Knot 값 설정
	SArray<double> adVKnots;
	A3DUns32 nVKnotSize = cSurfNurbsData.m_uiVKnotSize; // nUCtrlSize + nUDegree
	for(A3DUns32 nIndex = 0; nIndex < nVKnotSize; nIndex++) {
		adVKnots.Add(cSurfNurbsData.m_pdVKnots[nIndex] * dParentScale); // 스케일을 곱해야 정상적인 값이 나옴.
	}

	MbSplineSurface * pcSplineSurface = nullptr;

	if(nullptr != cSurfNurbsData.m_pdWeights) {
		pcSplineSurface = new MbSplineSurface(nUDegree + 1, nVDegree + 1, false, false,
			acCtrlPoints, adWeights, adUKnots, adVKnots);
	}
	else {
		pcSplineSurface = new MbSplineSurface(nUDegree + 1, nVDegree + 1, false, false,
			acCtrlPoints, adUKnots, adVKnots);
	}

	pcSurface = (MbSurface *) pcSplineSurface;

	double dUMin = pcSurface->GetUMin();
	double dUMax = pcSurface->GetUMax();

	return true;
}

// 2-7. Extrusion surface
bool ConvSurfaceBase::ConvertSurfExtrusion(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfExtrusionData cSurfExtrusionData;
	A3D_INITIALIZE_DATA(A3DSurfExtrusionData, cSurfExtrusionData);
	CHECK_A3D_RETURN(A3DSurfExtrusionGet(pcSurfBase, &cSurfExtrusionData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfExtrusionData;

	bool bStatus = ConvSurfaceBase::ConvertSurfExtrusionData(cEntityInfo, dParentScale, dContextScale, pcSurface);
	bSwapUV = (1 == cSurfExtrusionData.m_sParam.m_bSwapUV) ? true : false;

	A3DSurfExtrusionGet(nullptr, &cSurfExtrusionData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfExtrusionData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfExtrusionData & cSurfExtrusionData = *((A3DSurfExtrusionData *) cEntityInfo.pcEntityData[nTargetIndex]);

	cEntityInfo.nTargetEntityIndex++;

	double dCoeffA = 1.0, dCoeffB = 1.0;
	ConvCurveBase::GetCurveCoeffcient(cEntityInfo, dCoeffA, dCoeffB);

	SpaceCurveSPtr pcCurve;
	if(false == ConvCurveBase::ConvertSpaceCurveData(cEntityInfo, dParentScale, dContextScale, pcCurve)) {
		return false;
	}

	MbVector3D cExtrusionVector = Dmi3dx::GetMbVector3D(cSurfExtrusionData.m_sDirection);

	DmiExtrusionSurface * pcExtrusionSurface = new DmiExtrusionSurface(*pcCurve, cExtrusionVector, true);
	if(nullptr == pcExtrusionSurface) {
		A3DSurfExtrusionGet(nullptr, &cSurfExtrusionData);
		return false;
	}

	pcExtrusionSurface->SetUCoeff(dCoeffA / dParentScale);

	// 	MbPlacement3D cPlacement;
	// 	Rt3dx::GetPlacement3d(cSurfExtrusionData.m_sTrsf, dParentScale, cPlacement);
	// 	pcExtrusionSurface->Transform(MbMatrix3D(cPlacement));

	pcSurface = (MbSurface *) pcExtrusionSurface;

	return true;
}

// 2-8. Offset surface
bool ConvSurfaceBase::ConvertSurfOffset(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	// 정보 수집
	A3DSurfOffsetData cSurfOffsetData;
	A3D_INITIALIZE_DATA(A3DSurfOffsetData, cSurfOffsetData);
	CHECK_A3D_RETURN(A3DSurfOffsetGet(pcSurfBase, &cSurfOffsetData));

	ConvEntityInfo cEntityInfo(2);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfOffsetData;

	// Offset Base surface의 정보를 가져와 cEntityInfo에 저장한다.
	cEntityInfo.nTargetEntityIndex++;
	if(false == ConvSurfaceBase::GetSurfBaseData(cSurfOffsetData.m_pBasisSurf, dContextScale, cEntityInfo)) {
		ASSERT(false);
		return false;
	}

	bool bStatus = ConvSurfaceBase::ConvertSurfOffsetData(cEntityInfo, dParentScale, dContextScale, pcSurface);
	bSwapUV = (1 == cSurfOffsetData.m_sParam.m_bSwapUV) ? true : false;

	A3DSurfOffsetGet(nullptr, &cSurfOffsetData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfOffsetData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	A3DSurfOffsetData & cSurfOffsetData = *((A3DSurfOffsetData *) cEntityInfo.pcEntityData[0]);

	// Index 1에 Base Surface의 Data가 들어있음.
	cEntityInfo.nTargetEntityIndex++;
	// SurfaceSPtr pcBasisSurface;
	SurfaceSPtr pcBasisSurface;
	if(false == ConvertSurfBaseData(cEntityInfo, dParentScale, dParentScale, pcBasisSurface)) {
		ASSERT(false);
		return false;
	}

	if(nullptr == pcBasisSurface) {
		ASSERT(false);
		return false;
	}

	pcSurface = pcBasisSurface->Offset(cSurfOffsetData.m_dOffset * dParentScale, true);
	if(nullptr == pcSurface) {
		ASSERT(false);
		return false;
	}

	return true;
}

// 2-9. Blend03 surface
bool ConvSurfaceBase::ConvertSurfBlend03(const A3DSurfBase * pcSurfBase, double dContextScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	dTolerance = ConvSurfaceBase::dTolerance / dContextScale;

	A3DSurfNurbsData cSurfNurbsData;
	A3D_INITIALIZE_DATA(A3DSurfNurbsData, cSurfNurbsData);
	CHECK_A3D_RETURN(A3DSurfBaseWithDomainGetAsNurbs(pcSurfBase, nullptr, dTolerance, true, &cSurfNurbsData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfNurbsData;

	bool bStatus = ConvertSurfNurbsData(cEntityInfo, dContextScale, pcSurface);

	A3DSurfNurbsGet(nullptr, &cSurfNurbsData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfBlend03Data(ConvEntityInfo & cEntityInfo, double dParentScale, MbSurface *& pcSurface, MbModel * pcModel)
{
	A3DSurfBlend03Data & cSurfBlend03Data = *((A3DSurfBlend03Data *) cEntityInfo.pcEntityData[0]);

	SArray<MbCartPoint3D> acCenterPointArray;
	SArray<MbCartPoint3D> acRail1PointArray;
	SArray<MbCartPoint3D> acRail2PointArray;

	for(A3DUns32 nIndex = 0; nIndex < cSurfBlend03Data.m_uiNbOfElement; nIndex++) {
		acRail1PointArray.Add(Dmi3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pPositions[3 * nIndex + 0], dParentScale));
		acRail2PointArray.Add(Dmi3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pPositions[3 * nIndex + 1], dParentScale));
		acCenterPointArray.Add(Dmi3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pPositions[3 * nIndex + 2], dParentScale));
	}

	SArray<MbVector3D> acCenterTangentArray;
	SArray<MbVector3D> acRail1TangentArray;
	SArray<MbVector3D> acRail2TangentArray;

	for(A3DUns32 nIndex = 0; nIndex < cSurfBlend03Data.m_uiNbOfElement; nIndex++) {
		acRail1TangentArray.Add(Dmi3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pTangent[3 * nIndex + 0], dParentScale));
		acRail2TangentArray.Add(Dmi3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pTangent[3 * nIndex + 1], dParentScale));
		acCenterTangentArray.Add(Dmi3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pTangent[3 * nIndex + 2], dParentScale));
	}

	/*
		SArray<MbVector3D> acCenterSecondDerivativesArray;
		SArray<MbVector3D> acRail1SecondDerivativesArray;
		SArray<MbVector3D> acRail2SecondDerivativesArray;

		for(A3DUns32 nIndex = 0; nIndex < cSurfBlend03Data.m_uiNbOfElement; nIndex++) {
			acRail1SecondDerivativesArray.Add(Rt3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pSecondDerivatives[3 * nIndex + 0], dParentScale));
			acRail2SecondDerivativesArray.Add(Rt3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pSecondDerivatives[3 * nIndex + 1], dParentScale));
			acCenterSecondDerivativesArray.Add(Rt3dx::GetMbCartPoint3D(cSurfBlend03Data.m_pSecondDerivatives[3 * nIndex + 2], dParentScale));
		}
	*/

	SArray<double> adKnots;

	for(A3DUns32 nIndex = 0; nIndex < cSurfBlend03Data.m_uiNbOfElement; nIndex++) {
		adKnots.Add(cSurfBlend03Data.m_pdParameters[nIndex]);
	}



	MbNurbs3D * pcRail1NurbsCurve = MbNurbs3D::CreateNURBS4(acRail1PointArray, acRail1TangentArray, adKnots, false);
	MbNurbs3D * pcRail2NurbsCurve = MbNurbs3D::CreateNURBS4(acRail2PointArray, acRail2TangentArray, adKnots, false);
	MbNurbs3D * pcCenterNurbsCurve = MbNurbs3D::CreateNURBS4(acCenterPointArray, acCenterTangentArray, adKnots, false);

	MbHermit3D * pcRail1Hermit3D = MbHermit3D::Create(adKnots, acRail1PointArray, acRail1TangentArray, false);
	MbHermit3D * pcRail2Hermit3D = MbHermit3D::Create(adKnots, acRail2PointArray, acRail2TangentArray, false);
	/*
		MbPolyline3D * pcCenterPolyline = new MbPolyline3D(acCenterPointArray, false);
		MbPolyline3D * pcRail1Polyline = new MbPolyline3D(acRail1PointArray, false);
		MbPolyline3D * pcRail2Polyline = new MbPolyline3D(acRail2PointArray, false);

		MbCubicSpline3D * pcCenterSpline = MbCubicSpline3D::Create(acCenterPointArray, acCenterSecondDerivativesArray, false);
		MbCubicSpline3D * pcRail1Spline = MbCubicSpline3D::Create(acRail1PointArray, acRail1SecondDerivativesArray, false);
		MbCubicSpline3D * pcRail2Spline = MbCubicSpline3D::Create(acRail2PointArray, acRail2SecondDerivativesArray, false);

		MbNurbs3D * pcCenteNurbs = pcCenterSpline->NurbsCurve(MbCurveIntoNurbsInfo(*pcCenterSpline, 1, true, true));
		MbNurbs3D * pcRail1Nurbs = pcRail1Spline->NurbsCurve(MbCurveIntoNurbsInfo(*pcRail1Spline, 1, true, true));
		MbNurbs3D * pcRail2Nurbs = pcRail2Spline->NurbsCurve(MbCurveIntoNurbsInfo(*pcRail2Spline, 1, true, true));

		SArray<MbCartPoint3D> acStartPointArray;
		acStartPointArray.Add(acRail1PointArray.front());
		acStartPointArray.Add(acCenterPointArray.front());
		acStartPointArray.Add(acRail2PointArray.front());
		MbPolyline3D * pcStartPolyLine = new MbPolyline3D(acStartPointArray, false);

		SArray<MbCartPoint3D> acEndPointArray;
		acEndPointArray.Add(acRail1PointArray.back());
		acEndPointArray.Add(acCenterPointArray.back());
		acEndPointArray.Add(acRail2PointArray.back());
		MbPolyline3D * pcEndPolyLine = new MbPolyline3D(acEndPointArray, false);

		SInstanceSPtr cStartPolyLineInst(new MbSpaceInstance(*pcStartPolyLine));
		SInstanceSPtr cEndPolyLineInst(new MbSpaceInstance(*pcEndPolyLine));

		pcModel->AddItem(*cStartPolyLineInst);
		pcModel->AddItem(*cEndPolyLineInst);
	*/
	SInstanceSPtr cCenterInst(new MbSpaceInstance(*pcCenterNurbsCurve));
	cCenterInst->SetColor(RGB(255, 0, 0));

	SInstanceSPtr cRail1Inst(new MbSpaceInstance(*pcRail1NurbsCurve));
	cRail1Inst->SetColor(RGB(0, 0, 255));

	SInstanceSPtr cRail2Inst(new MbSpaceInstance(*pcRail2NurbsCurve));

	pcModel->AddItem(*cCenterInst);
	pcModel->AddItem(*cRail1Inst);
	pcModel->AddItem(*cRail2Inst);

	RPArray<MbCurve3D> acArcArray;

	// Arc를 그려보자.

	for(A3DUns32 nIndex = 0; nIndex < cSurfBlend03Data.m_uiNbOfElement; nIndex++) {

		MbVector3D cXaxis = acRail1PointArray[nIndex] - acCenterPointArray[nIndex];
		MbVector3D cYaxis = acRail2PointArray[nIndex] - acCenterPointArray[nIndex];

		cXaxis.Normalize();
		cYaxis.Normalize();

		double dRadius = acCenterPointArray[nIndex].DistanceToPoint(acRail1PointArray[nIndex]);

		MbPlacement3D cPlacement(cXaxis, cYaxis, acCenterPointArray[nIndex]);
		MbArc3D * pcArc = new MbArc3D(cPlacement, dRadius, dRadius, cSurfBlend03Data.m_pdRail2AnglesV[nIndex]);

		double dTmin = pcArc->GetTMin();
		double dTmax = pcArc->GetTMax();

		MbCurveIntoNurbsInfo cNurbsInfo(*pcArc, false, false);
		MbNurbs3D * pcArcNurbs = pcArc->NurbsCurve(cNurbsInfo);
		delete pcArc;

		dTmin = pcArcNurbs->GetTMin();
		dTmax = pcArcNurbs->GetTMax();

		pcArcNurbs->SetLimitParam(0.0, 5.0);

		dTmin = pcArcNurbs->GetTMin();
		dTmax = pcArcNurbs->GetTMax();

		acArcArray.Add(pcArcNurbs);

		SInstanceSPtr cArcInst(new MbSpaceInstance(*pcArcNurbs));
		pcModel->AddItem(*cArcInst);
	}

	DmiElevationSurface * pcElevationSurface = new DmiElevationSurface(acArcArray, true, *pcCenterNurbsCurve, true, false);
	if(nullptr == pcCenterNurbsCurve) {
		return false;
	}

	double dUCoeff = cSurfBlend03Data.m_sParam.m_dUCoeffA / dParentScale;
	double dVCoeff = cSurfBlend03Data.m_sParam.m_dVCoeffA / dParentScale;

	pcElevationSurface->SetUVCoeffA(dUCoeff, dVCoeff);
	pcElevationSurface->SetRail2ParameterV(cSurfBlend03Data.m_dRail2ParameterV);

	pcSurface = pcElevationSurface;

	double dUmin = pcElevationSurface->GetUMin();
	double dUmax = pcElevationSurface->GetUMax();

	double dVmin = pcElevationSurface->GetVMin();
	double dVmax = pcElevationSurface->GetVMax();

	/*

		MbSplineSurface * pcSplineSurface = pcElevationSurface->NurbsSurface(pcElevationSurface->GetUMin(), pcElevationSurface->GetUMax(),
			pcElevationSurface->GetVMin(), pcElevationSurface->GetVMax());

		pcSurface = pcSplineSurface;*/

		/*
			MbControlData3D cCtrlData;
			pcElevationSurface->GetBasisPoints(cCtrlData);

			size_t nTotalCount = cCtrlData.TotalCount();
			size_t nShareCount = cCtrlData.ShareCount();

			int i = 0;

			SInstanceSPtr cInst(new MbSpaceInstance(*pcSplineSurface));
			pcModel->AddItem(*cInst);
		*/

		/*
			MbCubicSpline3D(const SArray<MbCartPoint3D> &points,
				const SArray<MbVector3D   > &seconds,
				const SArray<double       > &params, bool cls);
		*/


	return true;
}

// 2-10. Ruled surface
bool ConvSurfaceBase::ConvertSurfRuled(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfRuledData cSurfRuledData;
	A3D_INITIALIZE_DATA(A3DSurfRuledData, cSurfRuledData);
	CHECK_A3D_RETURN(A3DSurfRuledGet(pcSurfBase, &cSurfRuledData));

	ConvEntityInfo cEntityInfo(3);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfRuledData;

	cEntityInfo.nTargetEntityIndex = 1;
	ConvCurveBase::GetCurveData(cSurfRuledData.m_pFirstCurve, dContextScale, cEntityInfo);

	cEntityInfo.nTargetEntityIndex = 2;
	ConvCurveBase::GetCurveData(cSurfRuledData.m_pSecondCurve, dContextScale, cEntityInfo);

	bool bStatus = ConvSurfaceBase::ConvertSurfRuledData(cEntityInfo, dParentScale, dContextScale, pcSurface);
	bSwapUV = (1 == cSurfRuledData.m_sParam.m_bSwapUV) ? true : false;

	A3DSurfRuledGet(nullptr, &cSurfRuledData);

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfRuledData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfRuledData & cSurfRuledData = *((A3DSurfRuledData *) cEntityInfo.pcEntityData[nTargetIndex]);

	cEntityInfo.nTargetEntityIndex++;
	SpaceCurveSPtr pcFirstCurve;
	if(false == ConvCurveBase::ConvertSpaceCurveData(cEntityInfo, dParentScale, dContextScale, pcFirstCurve)) {
		return false;
	}

	cEntityInfo.nTargetEntityIndex++;
	SpaceCurveSPtr pcSecondCurve;
	if(false == ConvCurveBase::ConvertSpaceCurveData(cEntityInfo, dParentScale, dContextScale, pcSecondCurve)) {
		return false;
	}

	DmiRuledSurface * pcRuledSurface = new DmiRuledSurface(*pcFirstCurve, *pcSecondCurve, true);
	if(nullptr == pcRuledSurface) {
		return false;
	}

	double dUCoeff = cSurfRuledData.m_sParam.m_dUCoeffA / dParentScale;
	double dVCoeff = cSurfRuledData.m_sParam.m_dVCoeffA / dParentScale;

	pcRuledSurface->SetUVCoeffA(dUCoeff, dVCoeff);

	pcSurface = pcRuledSurface;

	/*
		double dUMin = pcRuledSurface->GetUMin();
		double dUMax = pcRuledSurface->GetUMax();

		double dVMin = pcRuledSurface->GetVMin();
		double dVMax = pcRuledSurface->GetVMax();

		SInstanceSPtr pcSpaceInstance1(new MbSpaceInstance(*pcFirstCurve));
		SInstanceSPtr pcSpaceInstance2(new MbSpaceInstance(*pcSecondCurve));

		m_pcModel->AddItem(*pcSpaceInstance1);
		m_pcModel->AddItem(*pcSpaceInstance2);
	*/

	return true;
}

// 2-10. Revolution surface
bool ConvSurfaceBase::ConvertSurfRevolution(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, SurfaceSPtr & pcSurface)
{
	A3DSurfRevolutionData cSurfRevolutionData;
	A3D_INITIALIZE_DATA(A3DSurfRevolutionData, cSurfRevolutionData);
	CHECK_A3D_RETURN(A3DSurfRevolutionGet(pcSurfBase, &cSurfRevolutionData));

	ConvEntityInfo cEntityInfo(2);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cSurfRevolutionData;

	cEntityInfo.nTargetEntityIndex = 1;
	ConvCurveBase::GetCurveData(cSurfRevolutionData.m_pCurve, dParentScale, cEntityInfo);

	bool bStatus = ConvSurfaceBase::ConvertSurfRevolutionData(cEntityInfo, dParentScale, dContextScale, pcSurface);
	bSwapUV = (1 == cSurfRevolutionData.m_sParam.m_bSwapUV) ? true : false;

	return bStatus;
}

bool ConvSurfaceBase::ConvertSurfRevolutionData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, SurfaceSPtr & pcSurface)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DSurfRevolutionData & cSurfRevolutionData = *((A3DSurfRevolutionData *) cEntityInfo.pcEntityData[nTargetIndex]);

	cEntityInfo.nTargetEntityIndex = 1;
	SpaceCurveSPtr pcCurve;
	if(false == ConvCurveBase::ConvertSpaceCurveData(cEntityInfo, dParentScale, dContextScale, pcCurve)) {
		return false;
	}

	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cSurfRevolutionData.m_sOrigin, dParentScale);
	MbVector3D cVector = Dmi3dx::GetMbVector3D(cSurfRevolutionData.m_sDirection);

	DmiRevolutionSurface * pcRevolutionSurface = new DmiRevolutionSurface(*pcCurve, cOrigin, cVector, M_PI2, true);
	if(nullptr == pcRevolutionSurface) {
		return false;
	}

	double dUCoeff = cSurfRevolutionData.m_sParam.m_dUCoeffA;
	double dVCoeff = cSurfRevolutionData.m_sParam.m_dVCoeffA / dParentScale;

	pcRevolutionSurface->SetUVCoeffA(dUCoeff, dVCoeff);

	pcSurface = pcRevolutionSurface;

	return true;
}

// 3-1. 주어진 Plane Curve를 이용해서 MbCurveBoundedSurface를 생성한다.
bool ConvSurfaceBase::CreateBoundedSurface(MbSurface *& pcSurface, PlaneCurveSPtr & pcPlaneCurve, MbSurface * pcBoundedSurface)
{
	MbRect cCurveRect;
	pcPlaneCurve->CalculateGabarit(cCurveRect);
	if(true == cCurveRect.IsEmpty()) {
		ASSERT(false);
		return false;
	}

	// 공차를 주어야 한다. Loop의 Bound Box가 약간이라도 벗어나면 문제가 생김.
	double dTolerance = 1.0e-6;

	bool bCreateBoundedSurface = false;

	double dUMin = pcSurface->GetUMin();
	double dUMax = pcSurface->GetUMax();
	double dVMin = pcSurface->GetVMin();
	double dVMax = pcSurface->GetVMax();

	if(cCurveRect.GetXMin() - dTolerance < dUMin) {
		bCreateBoundedSurface = true;
	}
	else if(cCurveRect.GetXMax() + dTolerance > dUMax) {
		bCreateBoundedSurface = true;
	}
	else if(cCurveRect.GetYMin() - dTolerance < dVMin) {
		bCreateBoundedSurface = true;
	}
	else if(cCurveRect.GetYMax() + dTolerance > dVMax) {
		bCreateBoundedSurface = true;
	}

	// bCreateBoundedSurface = true;

	if(false == bCreateBoundedSurface) {
		return false;
	}

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, cCurveRect);

	return true;
}

// == 3. Utitliy Function ==========================================================================
bool ConvSurfaceBase::GetTargetEntityIndex(ConvEntityInfo & cEntityInfo, int & nTargetIndex)
{
	nTargetIndex = cEntityInfo.nTargetEntityIndex;
	if(nullptr == cEntityInfo.pcEntityData[nTargetIndex]) {
		return false;
	}

	return true;
}