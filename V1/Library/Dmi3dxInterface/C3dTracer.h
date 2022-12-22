#pragma once

class MbItem;

class MbSpaceItem;
class MbPlaneItem;

class MbAssembly;
class MbModel;
class MbSolid;

class MbFace;

class MbVertex;

class MbSurface;
class MbPlane;
class MbConeSurface;
class MbCylinderSurface;
class MbCurveBoundedSurface;
class MbSphereSurface;
class MbTorusSurface;
class MbRevolutionSurface;
class MbSplineSurface;
class MbExtrusionSurface;

class MbOrientedEdge;
class MbCurve;
class MbLineSegment;
class MbArc;
class MbNurbs;
class MbHermit;

class MbCartPoint;
class MbCartPoint3D;

class MbPlacement3D;
class MbPlacement;

class MbMatrix3D;

namespace C3dTracer
{
	void CreateLog(WCHAR * pchFilePathName);

	void ModelInformation(const MbModel & cModel, WCHAR * pchFilePathName, WCHAR * pchCadFileName);
	void AssemblyInformation(const MbAssembly & cAssembly);
	void SolidInformation(const MbSolid & cSolid, const WCHAR * pchFilePathName, WCHAR * pchCadFileName);
	void SolidInformation(const MbSolid & cSolid);
	void FaceInformation(const MbFace * pcFace, const WCHAR * pchFilePathName, WCHAR * pchCadFileName);
	void OrientedEdgeInformation(MbOrientedEdge * pcOrientedEdge, size_t nEdgeIndex, const MbSurface & cSurface);

	void VertexInformation(const MbVertex & cVertex);

	void ItemInformation(const MbItem * pcItem);

	void SpaceItemInformation(const MbSpaceItem & cSpaceItem);
	void PlaneItemInformation(const MbPlaneItem & cPlaneItem, const MbSurface & cSurface);

	// == Surface 관련 정보 분석 =====================================================================
	void PlaneInformation(const MbPlane & cPlane);
	void ConeSurfaceInformation(const MbConeSurface & cConeSurface);
	void CylinderSurfaceInformation(const MbCylinderSurface & cCylinderSurface);
	void SphereSurfaceInformation(const MbSphereSurface & cSphereSurface);
	void TorusSurfaceInformation(const MbTorusSurface & cTorusSurface);
	void RevolutionSurfaceInformation(const MbRevolutionSurface & cRevolutionSurface);
	void ExtrusionSurfaceInformation(const MbExtrusionSurface & cExtrusionSurface);
	void SplineSurfaceInformation(const MbSplineSurface & cSplineSurface);
	void CurveBoundedSurfaceInformation(const MbCurveBoundedSurface & cCurveBoundedSurface);
	void SurfaceDomainInformation(MbSurface * pcSurface);

	// == Curve 2D 관련 정보 분석 ====================================================================
	void LineSegmentInformation(const MbLineSegment & cLineSegment, const MbSurface & cSurface);
	void ArcInformation(const MbArc & cArc);
	void HermitInformation(const MbHermit & cHermit);
	void NurbsInformation(const MbNurbs & cNurbs, const MbSurface & cSurface);

	// == 기타 요소 관련 정보 분석 =====================================================================
	void Placement3DInformation(const MbPlacement3D & cPlacement);
	void PlacementInformation(const MbPlacement & cPlacement);

	void Matrix3dInformation(const MbMatrix3D & cMatrix);

	void PointOnInformation(WCHAR * pchClassName, double t, MbCartPoint & cPoint);
	void PointOnInformation(WCHAR * pchClassName, double t, MbCartPoint3D & cPoint);

	std::wstring GetSpaceItemNameString(const MbSpaceItem & eSpaceItem);
};
