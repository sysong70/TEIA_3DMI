#pragma once

#ifndef INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>
#endif

class A3dTracer
{
public:
	A3dTracer() {}

	void SetScale(double dScale) { m_dTopoContextScale = dScale; }

	void CreateLog(CString strFilePathName);

	void A3DTopoShellLog(const A3DTopoShell * pcTopoShell);

protected:
	void A3DTopoFaceLog(const A3DTopoFace * pcTopoFace, A3DUns32 nFaceIndex, A3DUns8 nOrientationWithShell);
	void A3DTopoLoopLog(const A3DTopoLoop * pcLoop, A3DUns32 nLoopIndex, A3DUns32 nFaceIndex);
	void A3DTopoCoEdgeLog(const A3DTopoCoEdge * pcTopoCoEdge, A3DUns32 nEdgeIndex, A3DUns32 nFaceIndex);
	void A3DTopoEdgeLog(const A3DTopoEdge * pcTopoEdge, A3DUns32 nFaceIndex);

	// == Surface 관련 함수 =============================================================================
	void A3DSurfBaseLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfPlaneLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfCylinderLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfConeLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfTorusLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfSphereLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfRevolutionLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfExtrusionLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfNurbsLog(const A3DSurfBase * pcSurfBase);

	// == Curve 관련 함수 ===========================================================================
	void A3DCrvBaseLog(const A3DCrvBase * pcCrvBase);
	void A3DCrvCircleLog(const A3DCrvBase * pcCrvBase);
	void A3DCrvNurbsLog(const A3DCrvBase * pcCrvBase);

	// == 기타 요소 관련 함수 =========================================================================
	void A3DDomainDataLog(const A3DDomainData & cData);

	void A3DTopoVertexLog(const A3DTopoVertex * pcTopoVertex);
	void A3DVector3dDataLog(const A3DVector3dData & cVector, CString strPrevText, double dScale);
	void A3DVector3dDataUVLog(const A3DVector3dData & cVector, CString strPrevText, double dScale);
	void A3DVector2dDataLog(const A3DVector2dData & cVector, CString strPrevText, double dScale);
	void A3DUVParameterizationDataLog(const A3DUVParameterizationData & cParam);

	CString GetBSplineSurfaceFormString(A3DEBSplineSurfaceForm eSurfaceForm);
	CString GetKnotTypeString(A3DEKnotType eKnotType);

	// == Log 관련 함수 =============================================================================
	void Log(LPCWSTR chMessage, ...);
	void IncreaseLogTab();
	void DecreaseLogTab();

	CString DblStr(double dValue);
	CString HexStr(DWORD_PTR nValue);
	CString BoolStr(bool bValue);

private:
	double m_dTopoContextScale = 0.0;
	double m_dUSurfCoeff = 1.0;
	double m_dVSurfCoeff = 1.0;

	int m_nOldCurrentId = 0;

};
