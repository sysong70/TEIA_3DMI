#pragma once

#include <A3DSDKIncludes.h>

#ifdef _DEBUG
#	define USED_LOG_MANAGER
#endif

namespace Log
{
#ifdef USED_LOG_MANAGER
	void CreateLog(int nId, const WCHAR * pchFilePathName);
	void Write(int nId, LPCWSTR chMessage, ...);
	void Write(int nId, LPCSTR chMessage, ...);
	void IncreaseTabIndex(int nId);
	void DecreaseTabIndex(int nId);

	void CreateLog(const WCHAR * pchFilePathName);
	void Write(LPCWSTR chMessage, ...);
	void Write(LPCSTR chMessage, ...);
	void IncreaseTabIndex();
	void DecreaseTabIndex();

	void Position(A3DMDPosition * pcPosition);
	void Vector3dData(A3DVector3dData & cData, CStringA strPrompt = "");
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

#else
	void CreateLog(int nId, const WCHAR * pchFilePathName) {}
	void Log(int nId, LPCWSTR chMessage, ...) {}
	void Log(int nId, LPCSTR chMessage, ...) {}
	void IncreaseTabIndex(int nId) {}
	void DecreaseTabIndex(int nId) {}

	void CreateLog(const WCHAR * pchFilePathName) {}
	void Write(LPCWSTR chMessage, ...) {}
	void Write(LPCSTR chMessage, ...) {}
	void IncreaseTabIndex() {}
	void DecreaseTabIndex() {}

	void Position(A3DMDPosition * pcPosition);
	void Vector3dData(A3DVector3dData & cData, CStringA strPrompt = "") {}
	bool GetName(const A3DRootBaseWithGraphics * pcRootBase, CString & strName) { return false; }
	void View(A3DMkpView * pcView, A3DMkpViewData & cViewData) {}
	void SurfPlane(A3DSurfPlane * pcInSurfPlane) {}
	void GraphCamera(A3DGraphCamera * pcInCamera) {}
	void LinkedItem(A3DMiscMarkupLinkedItemData & cData, A3DMiscEntityReferenceData & cRefData, DWORD_PTR nInAddreass) {}

	void DimensionData(A3DMarkupDimensionData & cData, DWORD_PTR nInAddreass) {}
	void DimensionValue(A3DMDDimensionValue * pcInData, CStringA strPrefix) {}
	void DimensionValueFormat(A3DMDDimensionValueFormat * pcInData) {}
	void DimensionExtentionLine(A3DMDDimensionExtentionLine * pcInData) {}
	void DimensionFunnel(A3DMDDimensionFunnel * pcInData) {}
	void DimensionExtremityData(A3DMDDimensionExtremityData & cInData) {}
	void DimensionLine(A3DMDDimensionLine * pcInData) {}
	void DimensionLineSymbol(A3DMDDimensionLineSymbol * pcInData, CStringA strPrefix) {}
	void TextProperties(A3DMDTextProperties * pcInData) {}

	void LeaderData(const A3DMkpMarkup * pcInData) {}
	void LeaderDefinitionData(A3DMDLeaderDefinition * pcInData) {}
	void LeaderSymbol(A3DMDLeaderSymbol * pcInData) {}
	void LeaderStub(A3DMDMarkupLeaderStub * pcInData) {}

	void TessMarkup(const A3DMkpMarkup * pcInData);

	CString HexStr(DWORD_PTR nValue) { return L""; }
	CStringA HexStrA(DWORD_PTR nValue) { return ""; }
	CString BoolStr(bool bValue) { return L""; }
	CStringA BoolStrA(bool bValue) { return ""; }
	CString GetVector2dDataString(A3DVector2dData & cData) { return L""; }
	CStringA GetVector2dDataStringA(A3DVector2dData & cData) { return ""; }
	CString GetVector3dDataString(A3DVector3dData & cData) { return L""; }
	CStringA GetVector3dDataStringA(A3DVector3dData & cData) { return ""; }

	CStringA GetDimensionTypeString(EA3DMDDimensionType cInType) { return ""; }
	CStringA GetDimensionSymbolTypeString(EA3DMDDimensionSymbolType cInType) { return ""; }
	CStringA GetDimensionOrientationString(EA3DMDDimensionOrientation cInType) { return ""; }
	CStringA GetDimensionDualDisplayString(EA3DMDDimensionDualDisplay cInType) { return ""; }
	CStringA GetDimensionSymbolShapeString(EA3DMDDimensionSymbolShape eInType) { return ""; }
	CStringA GetDimensionScoreString(EA3DMDDimensionScore eInType) { return ""; }
	CStringA GetDimensionFrameString(EA3DMDDimensionFrame eInType) { return ""; }
	CStringA GetTextPropertiesScoreString(EA3DMDTextPropertiesScore cInType) { return ""; }
	CStringA GetTextPropertiesFormatString(EA3DMDTextPropertiesFormat cInType) { return ""; }
	CStringA GetTextPropertiesJustificationString(EA3DMDTextPropertiesJustification cInType) { return ""; }
	CStringA GetAnchorPointTypeString(EA3DMDAnchorPointType cInType) { return ""; }
	CStringA GetLeaderSymbolTypeString(A3DMDLeaderSymbolType cInType) { return ""; }
#endif
}
