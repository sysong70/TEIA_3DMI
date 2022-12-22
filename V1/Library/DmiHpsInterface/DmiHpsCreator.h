#pragma once

#include <mb_data.h>

#include <unordered_map>
#include <atlcoll.h>

#include <mb_placement3d.h>

class DmiModelHandler;

class MbModel;
class MbSolid;
class MbFace;

class MbSurface;
class MbCurve3D;
class MbCurve;

class MbMesh;
class MbGrid;

class MbAssembly;
class MbInstance;
class MbItem;
class MbSpaceInstance;
class MbSpaceItem;
class MbPlaneInstance;
class MbPlaneItem;

class MbMatrix3D;
class MbPlacement3D;

class MbAssistingItem;

using AssyMap = std::unordered_map<const MbItem *, HPS::SegmentKey>;
using SolidMap = std::unordered_map<const MbSolid *, HPS::SegmentKey>;
using MeshMap = CAtlMap<const MbMesh *, HPS::SegmentKey>;

class DmiHpsCreator
{
public:
	DmiHpsCreator();
	DmiHpsCreator(DmiModelHandler * pcModelHanler);
	~DmiHpsCreator();

	bool CreateMbModel(MbModel * pcModel);
	bool CreateItem(const MbItem * pcItem);

	void SetModelHandler(DmiModelHandler * pcModelHanler);

protected:
	bool CreateAssembly(size_t nIndex, const MbAssembly * pcAssy, MbPlacement3D cPlacement, HPS::SegmentKey & cParentSegKey);
	bool CreateItem(size_t nIndex, const MbItem * pcItem, MbPlacement3D cPlacement, HPS::SegmentKey & cParentSegKey);
	bool CreateSolid(size_t nIndex, const MbSolid * pcSolid, MbPlacement3D cPlacement, HPS::SegmentKey & cParentSegKey);
	bool CreateSolidThread(size_t nIndex, const MbSolid * pcSolid, HPS::SegmentKey & cParentSegKey);
	bool CreateGrid(const MbGrid * pcGrid, const MbFace * pcFace, bool bFaceColorizingFlag, HPS::SegmentKey & cParentSegKey);
	bool CreateFace(const MbFace * pcFace, HPS::SegmentKey & cParentSegKey);

	bool CreateInstance(size_t nIndex, const MbInstance * pcInstance, HPS::SegmentKey & cParentSegKey);
	bool CreateSpaceInstance(size_t nIndex, const MbSpaceInstance * pcSpaceInstance, HPS::SegmentKey & cParentSegKey);

	bool CreatePlaneInstance(size_t nIndex, const MbPlaneInstance * pcPlaneInstance, HPS::SegmentKey & cParentSegKey);
	bool CreatePlaneCurve(const MbPlaneItem * pcSpaceItem, CString strCurveName, HPS::SegmentKey & cParentSegKey);
	bool CreatePlaneCurve(const MbCurve & cCurve, bool bSense, CString strCurveName, HPS::SegmentKey & cParentSegKey);

	bool CreateSurface(const MbSpaceItem * pcSpaceItem, HPS::SegmentKey & cParentSegKey);

	// ----- Curve 3d -----
	bool CreateCurve3d(const MbSpaceItem * pcSpaceItem, HPS::SegmentKey & cParentSegKey);
	bool CreateCurve3d(const MbCurve3D * pcCurve, HPS::SegmentKey & cParentSegKey);
	bool CreatePolyline3d(const MbSpaceItem * pcSpaceItem, HPS::SegmentKey & cParentSegKey);
	bool CreateContour3d(const MbSpaceItem * pcSpaceItem, HPS::SegmentKey & cParentSegKey);

	bool CreateAssistedItem(size_t nIndex, const MbAssistingItem * pcAssistingItem, double dAxisSize, HPS::SegmentKey & cParentSegKey);

	bool CreateMesh(size_t nIndex, const MbMesh * pcMesh, HPS::SegmentKey & cParentSegKey);

protected:
	// == Drawing Function =========================================================================
	bool DrawLoopMbSolid(MbMesh & cMesh, bool bAddStartPoint, HPS::SegmentKey & cParentSegKey);

	//== Draw Text =================================================================================
	void DrawPlaneText(HPS::Point cPoint, CString strText, HPS::SegmentKey & cParentSegKey);

	// == Debug Drawing ============================================================================
protected:
	void DebugDrwingSolid(const MbSolid * pcSolid, HPS::SegmentKey & cParentSegKey);

	void DebugDrawSurface(const MbSurface & cSurface, HPS::SegmentKey & cParentSegKey);
	void DebugDrawSplineSurfaceControlPoint(const MbSurface & cSurface, HPS::SegmentKey & cParentSegKey);

	void DebugDrwingFace(const MbFace * pcFace, MbVector3D & cOffset, HPS::SegmentKey & cParentSegKey);

	bool DebugCreateCurve3d(const MbCurve3D * pcCurve, bool bSense, CString strCurveName, MbVector3D & cOffset, HPS::SegmentKey & cParentSegKey);

	bool DebugDrawPlaneCurve(const MbCurve & cCurve, bool bSense, CString strCurveName, double dLoopDiagonalDistance, COLORREF nColor, HPS::SegmentKey & cParentSegKey);
	void DebugDrawLiftPlaneCurve(const MbCurve & cCurve, const MbSurface * pcSurface, bool bSense, CString strCurveName,
		double dLoopDiagonalDistance, COLORREF nColor, MbVector3D cOffset, HPS::SegmentKey & cParentSegKey);

	void DebugDrawCurveControlPoint(const MbCurve3D & cCurve, HPS::SegmentKey & cParentSegKey);
	void DebugDrawCubicSplineCurveControlPoint(const MbCurve3D & cCurve, HPS::SegmentKey & cParentSegKey);
	void DebugDrawNurbs3DCurveControlPoint(const MbCurve3D & cCurve, HPS::SegmentKey & cParentSegKey);
	void DebugDrawNurbsCurveControlPoint(const MbCurve & cCurve, HPS::SegmentKey & cParentSegKey);

	void DrawMbFaceEdgeCurve(const MbFace * pcFace);
	void DrawCone(MbCartPoint3D & cSp, MbCartPoint3D & cEp, double dRadius, double dHeight, HPS::SegmentKey & cParentSegKey);

	// == Utility Function =========================================================================
	bool GetItemText(const MbItem * pcItem, c3d::string_t strId, c3d::string_t & strValue);
	void GetMatrix(MbMatrix3D & cMatrix, HPS::MatrixKit & cHpsMatrix);
	void GetMatrix(MbPlacement3D & cPlacement, HPS::MatrixKit & cHpsMatrix);
	void SetSegementLinePattern(HPS::SegmentKey & cSegKey, int nLinePatter);
	void GetHpsPoint(MbCartPoint3D & cCartPoint, HPS::Point & cPoint);

	void CreateLog(int nId, const WCHAR * pchFilePathName);

private:
	DmiModelHandler * m_pcModelHanler;

	MbStepData m_cStepData;
	MbFormNote m_cFormNote;

	HPS::PortfolioKey m_cPortfolio;
	HPS::SegmentKey m_cIncludeLib;
	HPS::SegmentKey m_cAssyIncludeLib;
	HPS::SegmentKey m_cSolidIncludeLib;
	HPS::SegmentKey m_cSpaceInstanceIncludeLib;
	HPS::SegmentKey m_cPlaneInstanceIncludeLib;

	AssyMap m_mAssyMap;
	SolidMap m_mSolidMap;
	MeshMap m_maMeshMap;

	ULONGLONG m_nAssyNoId = 0;
	ULONGLONG m_nPartNoId = 0;
	ULONGLONG m_nSpaceInstanceNoId = 0;
	ULONGLONG m_nPlaneInstanceNoId = 0;

	ULONGLONG m_nFaceIndex = 0;

	double m_dDiagonalDistnace = 100.0;

	HPS::SegmentKey m_cCurrentSegmentKey;
	MbPlacement3D m_cCurrentPlacement;
	size_t m_nChildIndex;
	size_t m_nChildCount;

	// ----- Error Log용 변수 -----
public:
	void SetErrorMessageVector(std::vector<std::string> * pvstrErrMsgVector) {
		m_pvstrErrMsgVector = pvstrErrMsgVector;
	}

private:
	std::string m_strParent;
	std::vector<std::string> * m_pvstrErrMsgVector = nullptr;
};

