#pragma once

#include <A3DSDKIncludes.h>

namespace Log
{
	void CreateLog(const WCHAR * pchFilePathName);

	void Write(LPCWSTR chMessage, ...);
	void Write(LPCSTR chMessage, ...);

	void Write(int nLogLevel, LPCWSTR chMessage, ...);
	void Write(int nLogLevel, LPCSTR chMessage, ...);

	void IncreaseTabIndex();
	void DecreaseTabIndex();

	void SetLogLevel(int nLogLevel);
	int GetLogLevel();

	void Position(A3DMDPosition * pcPosition);
	void Vector3dData(A3DVector3dData & cData, CStringA strPrompt = "");
	void Vector3dData(int nLogLevel, A3DVector3dData & cData, CStringA strPrompt = "");
	bool GetName(const A3DRootBaseWithGraphics * pcRootBase, CString & strName);
	void View(A3DMkpView * pcView, A3DMkpViewData & cViewData);
	void SurfPlane(A3DSurfPlane * pcInSurfPlane);
	void GraphCamera(A3DGraphCamera * pcInCamera);
	void LinkedItem(A3DMiscMarkupLinkedItemData & cData, A3DMiscEntityReferenceData & cRefData, DWORD_PTR nInAddreass);

	void DimensionData(const A3DMkpMarkup * pcInData);
	void DimensionValue(A3DMDDimensionValue * pcInData, CStringA strPrefix);
	void DimensionValueFormat(A3DMDDimensionValueFormat * pcInData);
	void DimensionExtentionLine(A3DMDDimensionExtentionLine * pcInData);
	void DimensionFunnel(A3DMDDimensionFunnel * pcInData);
	void DimensionExtremityData(A3DMDDimensionExtremityData & cInData);
	void DimensionLine(A3DMDDimensionLine * pcInData);
	void DimensionLineSymbol(A3DMDDimensionLineSymbol * pcInData, CStringA strPrefix);
	void TextProperties(A3DMDTextProperties * pcInData);

	void LeaderData(const A3DMkpMarkup * pcInData);
	void LeaderDefinitionData(A3DMDLeaderDefinition * pcInData);
	void LeaderSymbol(A3DMDLeaderSymbol * pcInData);
	void LeaderStub(A3DMDMarkupLeaderStub * pcInData);

	void TessMarkup(const A3DMkpMarkup * pcInData);

	CString HexStr(DWORD_PTR nValue);
	CStringA HexStrA(DWORD_PTR nValue);
	CString BoolStr(bool bValue);
	CStringA BoolStrA(bool bValue);
	CString DblStr(double dValue);
	CStringA DblStrA(double dValue);
	CString GetVector2dDataString(A3DVector2dData & cData);
	CStringA GetVector2dDataStringA(A3DVector2dData & cData);
	CString GetVector3dDataString(A3DVector3dData & cData);
	CStringA GetVector3dDataStringA(A3DVector3dData & cData);

	CStringA GetDimensionDualDisplayString(EA3DMDDimensionDualDisplay cInType);
	CStringA GetDimensionSymbolTypeString(EA3DMDDimensionSymbolType cInType);
	CStringA GetDimensionTypeString(EA3DMDDimensionType cInType);
	CStringA GetDimensionOrientationString(EA3DMDDimensionOrientation cInType);
	CStringA GetDimensionSymbolShapeString(EA3DMDDimensionSymbolShape eInType);
	CStringA GetDimensionScoreString(EA3DMDDimensionScore eInType);
	CStringA GetDimensionFrameString(EA3DMDDimensionFrame eInType);
	CStringA GetTextPropertiesScoreString(EA3DMDTextPropertiesScore cInType);
	CStringA GetTextPropertiesFormatString(EA3DMDTextPropertiesFormat cInType);
	CStringA GetTextPropertiesJustificationString(EA3DMDTextPropertiesJustification cInType);
	CStringA GetAnchorPointTypeString(EA3DMDAnchorPointType cInType);
	CStringA GetLeaderSymbolTypeString(A3DMDLeaderSymbolType cInType);

	//== Topology Log Functions ====================================================================
	void A3DTopoBrepDataLog(const A3DTopoBrepData * pcInBrepData);
	void A3DTopoConnexLog(A3DTopoConnex * pcInTopoConnex, A3DUns32 nInIndex);
	void A3DTopoShellLog(const A3DTopoShell * pcInTopoShell, A3DUns32 nInIndex);
	void A3DTopoFaceLog(const A3DTopoFace * pcTopoFace, A3DUns32 nInIndex, A3DUns8 nOrientationWithShell);
	void A3DTopoLoopLog(const A3DTopoLoop * pcTopoLoop, A3DUns32 nLoopIndex, A3DUns32 nFaceIndex);
	void A3DTopoCoEdgeLog(const A3DTopoCoEdge * pcTopoCoEdge, A3DUns32 nEdgeIndex);
	void A3DTopoEdgeLog(const A3DTopoEdge * pcTopoEdge);
	void A3DTopoVertexLog(const A3DTopoVertex * pcTopoVertex);

	// == Surface 관련 함수 ==========================================================================
	void A3DSurfBaseLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfPlaneLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfCylinderLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfTorusLog(const A3DSurfBase * pcSurfBase);
	void A3DSurfConeLog(const A3DSurfBase * pcSurfBase);

	// == Curve 관련 함수 ============================================================================
	void A3DCrvBaseLog(const A3DCrvBase * pcCrvBase);
	void A3DCrvCircleLog(const A3DCrvBase * pcCrvBase);
	void A3DCrvNurbsLog(const A3DCrvBase * pcCrvBase);

	// == 기타 요소 관련 함수 =========================================================================
	CString GetKnotTypeString(A3DEKnotType knotType);
	void A3DMiscCartesianTransformationDataLog(A3DMiscCartesianTransformationData & trans);

	void A3DVector3dDataLog(const A3DVector3dData & vector, CString prevText, double scale);
	void A3DVector3dDataUVLog(const A3DVector3dData & vector, CString prevText, double scale, double uSurfCoeff, double vSurfCoeff);
	void A3DVector2dDataLog(const A3DVector2dData & vector, CString prevText, double scale);
	void A3DParameterizationDataLog(const A3DParameterizationData & param);
	void A3DUVParameterizationDataLog(const A3DUVParameterizationData & param);
	void A3DDomainDataLog(const A3DDomainData & domainData);
}
