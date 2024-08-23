#include "StdAfx.h"

#include "3DX.Log.h"

#include "Dmi3dx.h"
#include "3DX.Base.h"

#include <WStr.h>

#include <A3DSDKIncludes.h>

#ifdef _DEBUG
#	define USED_LOG_MANAGER
#endif

#	define USED_LOG_MANAGER

#ifdef USED_LOG_MANAGER

void Log::CreateLog(int nId, const WCHAR * pchFilePathName)
{
	LogManager::SetCurrentId(nId);

	LogManager::CreateLog(nId, pchFilePathName);
	LogManager::SetFileCloseFlag(true);
	LogManager::SetWriteLog(nId, true);
	LogManager::ResetTabIndex(nId);
}

void Log::Write(int nId, LPCWSTR chMessage, ...)
{
	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, strBuffer);
}

void Log::Write(int nId, LPCSTR chMessage, ...)
{
	va_list cArgList;
	va_start(cArgList, chMessage);

	CStringA strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	CString strText(strBuffer);
	LogManager::GetInstance()->WriteLog(nId, strText);
}

void Log::IncreaseTabIndex(int nId)
{
	LogManager::IncreaseTabIndex(nId);
}

void Log::DecreaseTabIndex(int nId)
{
	LogManager::DecreaseTabIndex(nId);
}

void Log::CreateLog(const WCHAR * pchFilePathName)
{
	CreateLog(2, pchFilePathName);
}

void Log::Write(LPCWSTR chMessage, ...)
{
	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(2, strBuffer);
}

void Log::Write(LPCSTR chMessage, ...)
{
	va_list cArgList;
	va_start(cArgList, chMessage);

	CStringA strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	CString strText(strBuffer);
	LogManager::GetInstance()->WriteLog(2, strText);
}

void Log::IncreaseTabIndex()
{
	IncreaseTabIndex(2);
}

void Log::DecreaseTabIndex()
{
	DecreaseTabIndex(2);
}
\
void Log::Position(A3DMDPosition * pcInData)
{
	if (nullptr == pcInData) {
		Write("Position: NULL");
		return;
	}

	A3DEEntityType eType = kA3DTypeUnknown;
	A3DEntityGetType(pcInData, &eType);

	switch (eType)
	{
		case kA3DTypeMDPosition2D: {
			Write("Position 2D: %s", HexStrA((DWORD_PTR)pcInData));

			A3DMDPosition2DData sData;
			A3D_INITIALIZE_DATA(A3DMDPosition2DData, sData);
			A3DMDPosition2DGet(pcInData, &sData);

			IncreaseTabIndex(); {
				Write("Position: %s, Offset: %f", GetVector2dDataStringA(sData.m_sPosition), sData.m_dOffset);
				SurfPlane(sData.m_pPlane);
			} DecreaseTabIndex();

			A3DMDPosition2DGet(nullptr, &sData);
		} break;

		case kA3DTypeMDPosition3D: {
			Write("Position 3D: %s", HexStrA((DWORD_PTR)pcInData));

			A3DMDPosition3DData sData;
			A3D_INITIALIZE_DATA(A3DMDPosition3DData, sData);
			A3DMDPosition3DGet(pcInData, &sData);

			IncreaseTabIndex(); {
				Write("Position: %s", GetVector3dDataStringA(sData.m_sPosition));
			} DecreaseTabIndex();

			A3DMDPosition3DGet(nullptr, &sData);
		} break;

		case kA3DTypeMDPositionReference: {
			Write("Position Reference: %s", HexStrA((DWORD_PTR)pcInData));

			A3DMDPositionReferenceData sData;
			A3D_INITIALIZE_DATA(A3DMDPositionReferenceData, sData);
			A3DMDPositionReferenceGet(pcInData, &sData);

			IncreaseTabIndex(); {
				Write("LinkedItem: %s", HexStrA((DWORD_PTR)sData.m_psLinkedItem));
				Write("AttachType: %s", GetAnchorPointTypeString(sData.m_eAttachType));
				Write("OffsetToReference: %s", GetVector2dDataStringA(sData.m_sOffsetToReference));
			} DecreaseTabIndex();

			A3DMDPositionReferenceGet(nullptr, &sData);
		} break;
	}
}

void Log::Vector3dData(A3DVector3dData & cData, CStringA strPrompt)
{
	if (true == strPrompt.IsEmpty()) {
		Write(2, "VectorData: %f, %f, %f", cData.m_dX, cData.m_dY, cData.m_dZ);
	}
	else {
		Write(2, "%s: %f, %f, %f", strPrompt, cData.m_dX, cData.m_dY, cData.m_dZ);
	}
}

bool Log::GetName(const A3DRootBaseWithGraphics * pcRootBase, CString & strName)
{
	A3DRootBaseData sRootBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sRootBaseData);

	A3DStatus eStstus = A3DRootBaseGet(pcRootBase, &sRootBaseData);
	if (A3D_SUCCESS != eStstus) {
		return false;
	}

	bool bStatus = false;
	if (nullptr != sRootBaseData.m_pcName) {
		H3DX::CharToCString(sRootBaseData.m_pcName, strName, CP_UTF8);
	}

	A3DRootBaseGet(nullptr, &sRootBaseData);

	return bStatus;
}

void Log::View(A3DMkpView * pcView, A3DMkpViewData & cViewData)
{
	CString strName;
	GetName(pcView, strName);

	Write(2, L"MkpView: %s, '%s'", HexStr((DWORD_PTR)pcView), strName);

	IncreaseTabIndex(2); {
		SurfPlane(cViewData.m_pPlane);

		// SceneDisplayParameters 정보 수집
		if (nullptr != cViewData.m_pSceneDisplayParameters) {
			Write(2, "Scene Display Parameters Information");

			IncreaseTabIndex(2); {
				A3DGraphSceneDisplayParametersData sData;
				A3D_INITIALIZE_DATA(A3DGraphSceneDisplayParametersData, sData);
				if (A3D_SUCCESS == A3DGraphSceneDisplayParametersGet(cViewData.m_pSceneDisplayParameters, &sData)) {
					Write(2, "IsActive: %s", BoolStrA(sData.m_bIsActive));
					GraphCamera(sData.m_pCamera);
					Write(2, "LightSize: %d", sData.m_uiLightSize);
					Write(2, "PlaneSize: %d", sData.m_uiPlaneSize);
					for (A3DUns32 nIndex = 0; nIndex < sData.m_uiPlaneSize; nIndex++) {
						SurfPlane(sData.m_ppClippingPlanes[nIndex]);
					}
					Write(2, "HasRotationCenter: %s", BoolStrA(sData.m_bHasRotationCenter));
					Vector3dData(sData.m_sRotationCenter, "RotationCenter");
					Write(2, "BackgroundStyleIndex: %d", sData.m_uiBackgroundStyleIndex);
					Write(2, "DefaultStyleIndex: %d", sData.m_uiDefaultStyleIndex);
					Write(2, "DefaultPerTypeIndexSize: %d", sData.m_uiDefaultPerTypeIndexSize);
				}

				A3DGraphSceneDisplayParametersGet(nullptr, &sData);
			} DecreaseTabIndex(2);
		}

		if (A3D_TRUE == cViewData.m_bIsAnnotationView) {
			Write(2, "Annotation View Type");
		}

		if (A3D_TRUE == cViewData.m_bIsDefaultView) {
			Write(2, "Default View Type");
		}

		Write(2, "Linked Items Size: %d", cViewData.m_uiLinkedItemsSize);

		Write(2, "Display Filter Size: %d", cViewData.m_uiDisplayFilterSize);

	} DecreaseTabIndex(2);
}

void Log::SurfPlane(A3DSurfPlane * pcInData)
{
	if (nullptr == pcInData) {
		Write(2, "SurfPlane: NULL");
		return;
	}

	Write(2, "SurfPlane: %s", HexStrA((DWORD_PTR)pcInData));

	A3DSurfPlaneData cData;
	A3D_INITIALIZE_DATA(A3DSurfPlaneData, cData);

	if (A3D_SUCCESS == A3DSurfPlaneGet(pcInData, &cData)) {
		IncreaseTabIndex(2); {
			Write(2, "Origin: %f, %f, %f", cData.m_sTrsf.m_sOrigin.m_dX, cData.m_sTrsf.m_sOrigin.m_dY, cData.m_sTrsf.m_sOrigin.m_dZ);
			Write(2, "X Axis: %f, %f, %f", cData.m_sTrsf.m_sXVector.m_dX, cData.m_sTrsf.m_sXVector.m_dY, cData.m_sTrsf.m_sXVector.m_dZ);
			Write(2, "Y Axis: %f, %f, %f", cData.m_sTrsf.m_sYVector.m_dX, cData.m_sTrsf.m_sYVector.m_dY, cData.m_sTrsf.m_sYVector.m_dZ);

			CStringA strBehaviourInfo;
			if (kA3DTransformationIdentity & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Identity ";
			}
			if (kA3DTransformationTranslate & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Translate ";
			}
			if (kA3DTransformationRotate & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Rotate ";
			}
			if (kA3DTransformationMirror & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Mirror ";
			}
			if (kA3DTransformationScale & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Scale ";
			}
			if (kA3DTransformationNonUniformScale & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "NonUniformScale ";
			}

			Write(2, "Scale: %f, Behaviour: %s", cData.m_sTrsf.m_sScale, strBehaviourInfo);

			// Write(2, "Min Domain: %f, %f", sSurfPlaneData.m_sParam.m_sUVDomain.m_sMin.m_dX, sSurfPlaneData.m_sParam.m_sUVDomain.m_sMin.m_dY);
			// Write(2, "Max Domain: %f, %f", sSurfPlaneData.m_sParam.m_sUVDomain.m_sMax.m_dX, sSurfPlaneData.m_sParam.m_sUVDomain.m_sMax.m_dY);

			Write(2, L"SwapUV: %s", BoolStr(cData.m_sParam.m_bSwapUV));

			// Write(2, "CoeffA: %f, %f", sSurfPlaneData.m_sParam.m_dUCoeffA, sSurfPlaneData.m_sParam.m_dVCoeffA);
			// Write(2, "CoeffB: %f, %f", sSurfPlaneData.m_sParam.m_dUCoeffB, sSurfPlaneData.m_sParam.m_dVCoeffB);
		} DecreaseTabIndex(2);

		A3DSurfPlaneGet(nullptr, &cData);
	}
}


void Log::GraphCamera(A3DGraphCamera * pcInCamera)
{
	if (nullptr == pcInCamera) {
		return;
	}

	Write(2, "Graph Camera Information: %s", HexStrA((DWORD_PTR)pcInCamera));

	IncreaseTabIndex(2);
	A3DGraphCameraData sCameraData;
	A3D_INITIALIZE_DATA(A3DGraphCameraData, sCameraData);

	if (A3D_SUCCESS == A3DGraphCameraGet(pcInCamera, &sCameraData)) {
		IncreaseTabIndex(2); {
			Write(2, "Orthographic: %s", BoolStrA(sCameraData.m_bOrthographic));
			Vector3dData(sCameraData.m_sLocation, "Location");
			Vector3dData(sCameraData.m_sLookAt, "LookAt");
			Vector3dData(sCameraData.m_sUp, "Up");
			Write(2, "Fovy: %f, %f", sCameraData.m_dXFovy, sCameraData.m_dYFovy);
			Write(2, "AspectRatio: %f", sCameraData.m_dAspectRatio);
			Write(2, "ZNear: %f, ZFar: %f, ZoomFactor: %f", sCameraData.m_dZNear, sCameraData.m_dZFar, sCameraData.m_dZoomFactor);
		} DecreaseTabIndex(2);

		A3DGraphCameraGet(nullptr, &sCameraData);
	}

	DecreaseTabIndex(2);
}

void Log::LinkedItem(A3DMiscMarkupLinkedItemData & cInData, A3DMiscEntityReferenceData & cRefData, DWORD_PTR nInAddreass)
{
	Write(2, "MarkupLinkedItem Information: %s", HexStrA(nInAddreass));

	IncreaseTabIndex(2); {
		Write(2, "MarkupShowControl: %s", BoolStrA(cInData.m_bMarkupShowControl));
		Write(2, "MarkupDeleteControl: %s", BoolStrA(cInData.m_bMarkupDeleteControl));
		Write(2, "LeaderShowControl: %s", BoolStrA(cInData.m_bLeaderShowControl));
		Write(2, "LeaderDeleteControl: %s", BoolStrA(cInData.m_bLeaderDeleteControl));
		Write(2, "TargetProductOccurrence: %s", HexStrA((DWORD_PTR)cInData.m_pTargetProductOccurrence));

		CString strName;
		GetName(cInData.m_pReference, strName);

		Write(2, "Reference: %s, Type: %s, '%s'", HexStrA((DWORD_PTR)cInData.m_pReference), Dmi3dx::GetA3dEntityTypeString(cInData.m_pReference), CStringA(strName));

		Write(2, "RefData Referenced entity: %s, Type: %s", HexStrA((DWORD_PTR)cRefData.m_pEntity), Dmi3dx::GetA3dEntityTypeString(cRefData.m_pEntity));
		Write(2, "RefData CoordinateSystem: %s", HexStrA((DWORD_PTR)cRefData.m_pCoordinateSystem), Dmi3dx::GetA3dEntityTypeString(cInData.m_pReference));
	} DecreaseTabIndex(2);
}

//== Dimension Data 관련 함수 ========================================================================
void Log::DimensionData(const A3DMkpMarkup * pcInData)
{
	A3DMarkupDimensionData cInData;
	A3D_INITIALIZE_DATA(A3DMarkupDimensionData, cInData);
	A3DMarkupDimensionGet(pcInData, &cInData);

	Write(2, "Dimension Data: %s", HexStrA((DWORD_PTR)pcInData));

	IncreaseTabIndex(2); {
		Write(2, "Type: %s, Value: %f, Symbol: %s, AdditionnalSymbol: %s", GetDimensionTypeString(cInData.m_eType), cInData.m_dValue,
			GetDimensionSymbolTypeString(cInData.m_eSymbol), GetDimensionSymbolTypeString(cInData.m_eAdditionnalSymbol));

		CStringA strPosition;
		switch (cInData.m_usPosition) {
			case 0: strPosition = "Auto"; break;
			case 1: strPosition = "Inside"; break;
			case 2:	strPosition = "Outside"; break;
		}

		Write(2, "Suffixe: '%s', Prefixe: '%s', Orientation: %s, Position: %s", cInData.m_pcSuffixe, cInData.m_pcPrefixe,
			GetDimensionOrientationString(cInData.m_eOrientation), strPosition);

		DimensionValue(cInData.m_pMainValue, "Main");

		Write(2, "DualDisplay: %s", GetDimensionDualDisplayString(cInData.m_eDualDisplay));

		DimensionValue(cInData.m_pDualValue, "Dual");

		DimensionExtentionLine(cInData.m_pExtentionLine);

		DimensionLine(cInData.m_pLine);

		Write("Orientation: '%s', Offset: %s", GetDimensionOrientationString(cInData.m_eOrientation), GetVector3dDataStringA(cInData.m_sOffSet));

		CStringA strScore;

		switch (cInData.m_usScore) {
			case 0: strScore = "Not to score"; break;
			case 1: strScore = "Only the value"; break;
			case 2: strScore = "All dimension texts"; break;
		}

		CStringA strScoreGroup;
		switch (cInData.m_usScoreGroup)
		{
			case 0: strScoreGroup = "Main value"; break;
			case 1: strScoreGroup = "Dual value"; break;
			case 2: strScoreGroup = "Both values"; break;
		}
		
		Write("Scoring: %s, Scoring covering: %s, Scoring element: %s", GetDimensionScoreString(cInData.m_eScore), strScore, strScoreGroup);
		
		CStringA strFrame;
		switch (cInData.m_usFrame)
		{
			case 0: strFrame = "Value"; break;
			case 1: strFrame = "Value + Tolerance"; break;
			case 2: strFrame = "Value + Tolerance + Texts"; break;
		}

		CStringA strFrameGroup;
		switch (cInData.m_usFrameGroup)
		{
			case 0: strFrameGroup = "Main value"; break;
			case 1: strFrameGroup = "Dual value"; break;
			case 2: strFrameGroup = "Both values"; break;
		}

		Write("Frame: %s, Frame covering: %s, Frame element: %s", GetDimensionFrameString(cInData.m_eFrame), strFrame, strFrameGroup);

		TextProperties(cInData.m_pTextProperties);

		Write(L"Chamfer dimension markup second value: %f", cInData.m_dChamferDimSecondValue);

	} DecreaseTabIndex(2);

	A3DMarkupDimensionGet(nullptr, &cInData);
}

void Log::DimensionValue(A3DMDDimensionValue * pcInData, CStringA strPrefix)
{
	if (nullptr == pcInData) {
		Write(2, "%s DimensionValueData: NULL", strPrefix);
		return;
	}

	A3DMDDimensionValueData cData;
	A3D_INITIALIZE_DATA(A3DMDDimensionValueData, cData);
	A3DMDDimensionValueGet(pcInData, &cData);

	Write(2, "%s DimensionValueData Information: %s", strPrefix, HexStrA((DWORD_PTR)pcInData));

	IncreaseTabIndex(2); {
		CStringA strType;
		switch (cData.m_iType) {
			case 0:
				strType = "true value is used";
				break;
			case 1:
				strType = "fake value is used, original value is numerical";
				break;
			case 2:
				strType = "fake value is used, original value is alpha numerical";
				break;
		}

		CStringA strFormat;
		if (A3D_TRUE == cData.m_bFormat) {
			strFormat = "Decimal";
		}
		else {
			strFormat = "Fractional";
		}

		Write(2, "Value type: %s, Format: %s, Accuracy: %f, Tolerance accuracy: %f", strType, strFormat, cData.m_dAccuracy, cData.m_dToleranceAccuracy);
		Write(2, "FakeValue: '%s', Superior numerical tolerance: %f, Inferior numerical tolerance: %f", cData.m_pcFakeValue, cData.m_dTolNumSup, cData.m_dTolNumInf);
		Write(2, "Superior alpha numerical tolerance: '%s', Inferior alpha numerical tolerance: '%s'", cData.m_pcTolTxtSup, cData.m_pcTolTxtInf);
		Write(2, "Text diplayed Before value: '%s', After value: '%s', Above value: '%s', Below value: '%s'", cData.m_pcBeforeText, cData.m_pcAfterText, cData.m_pcAboveText, cData.m_pcBelowText);
		Write(2, "Last separator to use: %d, Option of semantic data: %d, Value of delta if the option MIN/MAX is activated: %f", cData.m_usLastSeparDefinedNum, cData.m_iOption, cData.m_dDeltaForMinMax);

		DimensionValueFormat(cData.m_pDimValueFormat);

		if (nullptr != cData.m_pToleranceFormat) {
			Write(2, "ToleranceFormat: %s", HexStrA((DWORD_PTR)cData.m_pToleranceFormat));
		}
		else {
			Write(2, "ToleranceFormat: NULL");
		}

		DimensionValue(cData.m_pChamferDimSecondValue, "ChamferDim Second Value");

	} DecreaseTabIndex(2);

	A3DMDDimensionValueGet(nullptr, &cData);
}

void Log::DimensionValueFormat(A3DMDDimensionValueFormat * pcInData)
{
	if (nullptr == pcInData) {
		Write("Dimension Value Format: NULL");
		return;
	}

	Write("Dimension Value Format: %s", HexStrA((DWORD_PTR)pcInData));

	A3DMDDimensionValueFormatData cData;
	A3D_INITIALIZE_DATA(A3DMDDimensionValueFormatData, cData);
	A3DMDDimensionValueFormatGet(pcInData, &cData);

	IncreaseTabIndex(); {

		CStringA strType = (cData.m_iType == 1) ? "Length" : "Angle";
		CStringA strUnit = "Unknown";
		switch (cData.m_iUnit) {
			case 1:
				strUnit = "mm";
				break;
			case 2:
				strUnit = "inch";
				break;
			case 3:
				strUnit = "radian";
				break;
			case 4:
				strUnit = "degree";
				break;
			case 5:
				strUnit = "grade";
				break;
		}

		Write("Name: '%s', Type: '%s', Unit: '%s', Global Multiplying Factor: %f", cData.m_pcName, strType, strUnit, cData.m_dGlobFact);

		CStringA strNulFac_1 = (cData.m_iNulFac_1 == 1) ? "No display" : "Display";
		CStringA strNulFac_2 = (cData.m_iNulFac_2 == 1) ? "No display" : "Display";
		CStringA strExise = (cData.m_iExise == 1) ? "No display" : "Display";
		Write("%s of Null Terms, %s of leading zeros in last factor, %s of separator for thousands, Separator characters: %d, %c", strNulFac_1, strNulFac_2, strExise, cData.m_iSep1000, cData.m_iSep1000);

		CStringA strFinZer = (cData.m_iFinZer == 1) ? "No display" : "Display";
		Write("%s of trailing zeros", strFinZer);

		Write("Term Multiplying Factor on term 1: %f, term 2: %f, term 3: %f", cData.m_dFact_1, cData.m_dFact_2, cData.m_dFact_3);
		Write("Term vertical positioning offset on term 1: %f, term 2: %f, term 3: %f", cData.m_dValPos_1, cData.m_dValPos_2, cData.m_dValPos_3);
		Write("Term unit suffix, 16 char string on term 1: '%s', term 2: '%s', term 3: '%s'", cData.m_pcSepar_1, cData.m_pcSepar_2, cData.m_pcSepar_3);
		Write("Term unit height ratio on term 1: %f, term 2: %f, term 3: %f", cData.m_dSepScl_1, cData.m_dSepScl_2, cData.m_dSepScl_3);
		Write("Term unit vertical positioning offset on term 1: %f, term 2: %f, term 3: %f", cData.m_dSepPos_1, cData.m_dSepPos_2, cData.m_dSepPos_3);
		Write("Ratio to the character height RestX: %f, RestY: %f, NulOther: %d, ", cData.m_dRestX, cData.m_dRestY, cData.m_iNulOther);
		Write("Offset between fractional rest numerator and denominator: % f", cData.m_dOperY);
		Write("Fractional Rest: %d, Fraction type: %d, Position of last term unit: %d", cData.m_iSepNum, cData.m_iTypFrac, cData.m_iSepDen);
		Write("Unit height: %f, Number of terms in the value: %d", cData.m_dResScl, cData.m_iFact);

	} DecreaseTabIndex();

	A3DMDDimensionValueFormatGet(nullptr, &cData);
}

void Log::DimensionExtentionLine(A3DMDDimensionExtentionLine * pcInData)
{
	if (nullptr == pcInData) {
		Write("Dimension Extention Line: NULL");
		return;
	}

	Write("Dimension Extention Line: %s", HexStrA((DWORD_PTR)pcInData));

	A3DMDDimensionExtentionLineData cData;
	A3D_INITIALIZE_DATA(A3DMDDimensionExtentionLineData, cData);
	A3DMDDimensionExtentionLineGet(pcInData, &cData);

	IncreaseTabIndex(); {
		DimensionFunnel(cData.m_pFunnel);

		DimensionExtremityData(cData.m_sExtremity1);
		DimensionExtremityData(cData.m_sExtremity2);

		Write("Line Slant: %f, Style: %d, Thickness: %f", cData.m_dLineSlant, cData.m_usStyle, cData.m_fThickness);

	} DecreaseTabIndex();

	A3DMDDimensionExtentionLineGet(nullptr, &cData);
}

void Log::DimensionFunnel(A3DMDDimensionFunnel * pcInData)
{
	if (nullptr == pcInData) {
		Write("Dimension Funnel: NULL");
		return;
	}

	Write("Dimension Funnel: %s", HexStrA((DWORD_PTR)pcInData));

	A3DMDDimensionFunnelData cData;
	A3D_INITIALIZE_DATA(A3DMDDimensionFunnelData, cData);
	A3DMDDimensionFunnelGet(pcInData, &cData);

	IncreaseTabIndex(); {

		CStringA strIndex;
		switch (cData.m_usIndex)
		{
			case 0:
				strIndex = "First extension line only";
				break;
			case 1:
				strIndex = "Second extension line only";
				break;
			case 2:
				strIndex = "Both extension lines";
				break;
		}

		CStringA strMode = (cData.m_bMode == A3D_TRUE) ? "Outside" : "Insize";

		Write("Funnel covering: %s, Mode: %s, Angle: %f, Height: %f, Width: %f", strIndex, strMode, cData.m_dAngle, cData.m_dHeight, cData.m_dWidth);

	} DecreaseTabIndex();

	A3DMDDimensionFunnelGet(nullptr, &cData);
}

void Log::DimensionExtremityData(A3DMDDimensionExtremityData & cInData)
{
	Write("Dimension Extremity Data");

	IncreaseTabIndex(); {
		Write("Show: %s, Blanking: %f, Overrun: %f", BoolStrA(cInData.m_bShow), cInData.m_dBlanking, cInData.m_dOverrun);
	} DecreaseTabIndex();
}

void Log::DimensionLine(A3DMDDimensionLine * pcInData)
{
	if (nullptr == pcInData) {
		Write("Dimension Line: NULL");
		return;
	}

	Write("Dimension Line: %s", HexStrA((DWORD_PTR)pcInData));

	A3DMDDimensionLineData cData;
	A3D_INITIALIZE_DATA(A3DMDDimensionLineData, cData);
	A3DMDDimensionLineGet(pcInData, &cData);

	IncreaseTabIndex(); {
		DimensionLineSymbol(cData.m_pSymbol1, "First");
		DimensionLineSymbol(cData.m_pSymbol2, "Second");
		DimensionLineSymbol(cData.m_pLeaderSymbol, "Leader");

	} DecreaseTabIndex();

	A3DMDDimensionLineGet(nullptr, &cData);

/*
	typedef struct
	{
		A3DUns16									m_usStructSize;			// Reserved: will be initialized by \ref A3D_INITIALIZE_DATA.
		EA3DMDDimensionLineGraphicalRepresentation	m_eGraphRepresentation; / *!< Graphical representation of the dimension line. * /
		A3DMDDimensionSecondPart * m_pSecondPart;			/ *!< Second part description. * /
		A3DUns16									m_usStyle;				/ *!< Line style Index. * /
		A3DFloat									m_fThickness;			/ *!< Thickness. * /
		A3DUns16									m_usReversal;			/ *!< Reversal. * /
		A3DMDDimensionForeshortened * m_psForeshortened;		/ *!< Optionnal foreshortened description. * /
		A3DDouble									m_dLeaderAngle;			/ *!< Leader angle. * /
		EA3DMDDimensionLineExtensionType			m_eExtension;			/ *!< Extension type. * /
		A3DRiCurve * m_pExplicitCurve;		/ *!< Explicit cure, in case of curvilinear dimension. * /
	} A3DMDDimensionLineData;
*/
}

void Log::DimensionLineSymbol(A3DMDDimensionLineSymbol * pcInData, CStringA strPrefix)
{
	if (nullptr == pcInData) {
		Write("%s Dimension Line Symbol: NULL", strPrefix);
		return;
	}

	Write("%s Dimension Line Symbol: %s", strPrefix, HexStrA((DWORD_PTR)pcInData));

	A3DMDDimensionLineSymbolData cData;
	A3D_INITIALIZE_DATA(A3DMDDimensionLineSymbolData, cData);
	A3DMDDimensionLineSymbolGet(pcInData, &cData);

	IncreaseTabIndex(); {
		Write("Shape: %s, Color Index:%d, Thickness: %f, Arrow length: %f, Arrow angle: %f", GetDimensionSymbolShapeString(cData.m_eShape), cData.m_iLineColor,
			cData.m_fThickness, cData.m_fArrowLength, cData.m_fArrowAngle);
	} DecreaseTabIndex();

	A3DMDDimensionLineSymbolGet(nullptr, &cData);
}

void Log::TextProperties(A3DMDTextProperties * pcInData)
{
	if (nullptr == pcInData) {
		Write("Text Properties: NULL");
		return;
	}

	Write("Text Properties: %s", HexStrA((DWORD_PTR)pcInData));

	A3DMDTextPropertiesData cData;
	A3D_INITIALIZE_DATA(A3DMDTextPropertiesData, cData);
	A3DMDTextPropertiesGet(pcInData, &cData);

	IncreaseTabIndex(); {
		Write("Font: '%s', Angle: %f, Slant: %s, Thickness: %d, Size: %d", cData.m_pcFont, cData.m_dAngle, BoolStrA(cData.m_bSlant), cData.m_ucThickness, cData.m_ucSize);
		Write("LineStyle: %d, Height: %f, Width: %f, Spacing: %f, Char fixed ratio mode: %d", cData.m_usLineStyle, cData.m_dCharHeight, cData.m_dCharWidth, cData.m_dCharSpacing, cData.m_iCharFixedRatioMode);
		Write("Global text scoring: '%s', Text format: '%s', Text justification: '%s'", GetTextPropertiesScoreString(cData.m_eGlobalTextScoring), GetTextPropertiesFormatString(cData.m_eFormat),
			GetTextPropertiesJustificationString(cData.m_eJustification));

	} DecreaseTabIndex();

	A3DMDTextPropertiesGet(nullptr, &cData);
}

//== Leader Data 관련 함수 ===========================================================================
void Log::LeaderData(const A3DMkpMarkup * pcInData)
{
	if (nullptr == pcInData) {
		Write("Leader Data: NULL");
		return;
	}

	A3DMkpMarkupData sMarkupData;
	A3D_INITIALIZE_DATA(A3DMkpMarkupData, sMarkupData);
	A3DMkpMarkupGet(pcInData, &sMarkupData);

	Write("Leader Data: %s, Count: %d", HexStrA((DWORD_PTR)pcInData), sMarkupData.m_uiLeadersSize);

	IncreaseTabIndex(); {
		for (A3DUns32 nIndex = 0; nIndex < sMarkupData.m_uiLeadersSize; nIndex++) {
			A3DMkpLeader * psLeader = sMarkupData.m_ppLeaders[nIndex];
			if (nullptr == psLeader) {
				continue;
			}

			Write("Leader No.%d", nIndex);
			IncreaseTabIndex(); {
				LeaderDefinitionData(psLeader);
			} DecreaseTabIndex();
		}
	} DecreaseTabIndex();

	A3DMkpMarkupGet(nullptr, &sMarkupData);
}

void Log::LeaderDefinitionData(A3DMDLeaderDefinition * pcInData)
{
	if (nullptr == pcInData) {
		Write("Leader Definition Data: NULL");
		return;
	}

	Write("Leader Definition Data: %s", HexStrA((DWORD_PTR) pcInData));

	A3DMDLeaderDefinitionData cData;
	A3D_INITIALIZE_DATA(A3DMDLeaderDefinitionData, cData);
	A3DMDLeaderDefinitionGet(pcInData, &cData);

	IncreaseTabIndex(); {
		Write("Leader Positions Count: %d", cData.m_uiNumberOfPathLeaderPositions);

		for (A3DUns32 nIndex = 0; nIndex < cData.m_uiNumberOfPathLeaderPositions; nIndex++) {
			A3DMDPosition * pcPosition = cData.m_ppsPathLeaderPositions[nIndex];
			if (nullptr == pcPosition) {
				continue;
			}

			Write("Leader Position No.%d", nIndex);
			IncreaseTabIndex(); {
				Position(pcPosition);
			} DecreaseTabIndex();

			Write("Number of gap in following list: %d", cData.m_uiNbGapsElements);
			IncreaseTabIndex(); {
				for (A3DUns32 nGapIndex = 0; nGapIndex < cData.m_uiNbGapsElements; nGapIndex++) {
					A3DDouble dGap = cData.m_pdGapList[nGapIndex];
					Write("Gap No.%d: %f", nGapIndex, dGap);
				}
			} DecreaseTabIndex();

			LeaderSymbol(cData.m_pHeadSymbol);
			Write("Tail Stymole: %s", GetLeaderSymbolTypeString(cData.m_eTailSymbol));

			Write("Next Leader: %s", HexStrA((DWORD_PTR)cData.m_pNextLeader));
			if (nullptr != cData.m_pNextLeader) {
				IncreaseTabIndex(); {
					LeaderDefinitionData(cData.m_pNextLeader);
				} DecreaseTabIndex();
			}

			Write("AnchorFrame: %d, AnchorPoint: %d", cData.m_uAnchorFrame, cData.m_uAnchorPoint);

			LeaderStub(cData.m_pStub);
		}
	} DecreaseTabIndex();

	A3DMDLeaderDefinitionGet(nullptr, &cData);
}

void Log::LeaderSymbol(A3DMDLeaderSymbol * pcInData)
{
	if (nullptr == pcInData) {
		Write("Leader Symbol: NULL");
		return;
	}

	Write("Leader Symbol: %s", HexStrA((DWORD_PTR)pcInData));

	A3DMDLeaderSymbolData cData;
	A3D_INITIALIZE_DATA(A3DMDLeaderSymbolData, cData);
	A3DMDLeaderSymbolGet(pcInData, &cData);

	IncreaseTabIndex(); {
		Write("Head Symbol: %s, Length: %f, Additional Parameter: %f", GetLeaderSymbolTypeString(cData.m_eHeadSymbol), cData.m_dLength, cData.m_dAdditionalParameter);
	} DecreaseTabIndex();

	A3DMDLeaderSymbolGet(nullptr, &cData);
}

void Log::LeaderStub(A3DMDMarkupLeaderStub * pcInData)
{
	if (nullptr == pcInData) {
		Write("Leader Stub: NULL");
		return;
	}

	A3DMDMarkupLeaderStubData cData;
	A3D_INITIALIZE_DATA(A3DMDMarkupLeaderStubData, cData);
	A3DMDMarkupLeaderStubGet(pcInData, &cData);

	Write("Leader Stub: %s, Count: %d", HexStrA((DWORD_PTR)pcInData), cData.m_uiValuesAndAnchorTypesSize);

	IncreaseTabIndex(); {
		for (A3DUns32 nIndex = 0; nIndex < cData.m_uiValuesAndAnchorTypesSize; nIndex++) {
			Write("No.%d, Length: %f, AnchorTypes: %s", nIndex, cData.m_pdValues[nIndex], GetAnchorPointTypeString((EA3DMDAnchorPointType)cData.m_piAnchorTypes[nIndex]));
		}
	} DecreaseTabIndex();

	A3DMDMarkupLeaderStubGet(nullptr, &cData);
}

//== Tessellation Markup 관련 함수 ===================================================================
void Log::TessMarkup(const A3DMkpMarkup * pcInData)
{
	if (nullptr == pcInData) {
		Write("TessMarkup Data: NULL");
		return;
	}

	A3DMkpMarkupData sMarkupData;
	A3D_INITIALIZE_DATA(A3DMkpMarkupData, sMarkupData);
	A3DMkpMarkupGet(pcInData, &sMarkupData);

	if (nullptr == sMarkupData.m_pTessellation) {
		Write("TessMarkup Data: NULL");
	}

	A3DTessBaseData sTessBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sTessBaseData);
	A3DTessBaseGet(sMarkupData.m_pTessellation, &sTessBaseData);

	A3DTessMarkupData sTessMarkupData;
	A3D_INITIALIZE_DATA(A3DTessMarkupData, sTessMarkupData);
	A3DTessMarkupGet(sMarkupData.m_pTessellation, &sTessMarkupData);

	Write("TessMarkup Data: %s", HexStrA((DWORD_PTR)sMarkupData.m_pTessellation));

	IncreaseTabIndex(); {
		Write("Coord Count: %d, IsCalculated: %s", sTessBaseData.m_uiCoordSize / 3, BoolStrA(sTessBaseData.m_bIsCalculated));
		IncreaseTabIndex(); {
			for (A3DUns32 nIndex = 0; nIndex < sTessBaseData.m_uiCoordSize / 3; nIndex++) {
				Write("%d. %f, %f, %f", nIndex, sTessBaseData.m_pdCoords[nIndex * 3], sTessBaseData.m_pdCoords[nIndex * 3 + 1], sTessBaseData.m_pdCoords[nIndex * 3 + 2]);
			}
		} DecreaseTabIndex();

		Write("Text Count: %d", sTessMarkupData.m_uiTextsSize);
		IncreaseTabIndex(); {
			for (A3DUns32 nIndex = 0; nIndex < sTessMarkupData.m_uiTextsSize; nIndex++) {
				Write(L"%d. %s", nIndex, WStr::ToUtf16(sTessMarkupData.m_ppcTexts[nIndex]));
			}
		} DecreaseTabIndex();


/*
		A3DUns32 m_uiTextsSize;		/ *!< The size of \ref m_ppcTexts. * /
		A3DUTF8Char ** m_ppcTexts;	/ *!< Texts used in tessellation. See explanations in \ref a3d_tessmarkup. * /
		A3DUTF8Char * m_pcLabel;		/ *!< Markup label. * /
		A3DInt8 m_cBehaviour;		/ *!< Flags for tessellation. See \ref a3d_tessmarkupdef. * /
*/

	} DecreaseTabIndex();

	A3DMkpMarkupGet( nullptr, &sMarkupData);
}



//== String 관련 함수 ================================================================================
CString Log::HexStr(DWORD_PTR nValue)
{
	if (0 == nValue) {
		return L"NULL";
	}

	return LogManager::HexStr(nValue);
}

CStringA Log::HexStrA(DWORD_PTR nValue)
{
	if (0 == nValue) {
		return "NULL";
	}

	CStringA strValue(LogManager::HexStr(nValue));
	return strValue;
}

CString Log::BoolStr(bool bValue)
{
	return LogManager::BoolStr(bValue);
}

CStringA Log::BoolStrA(bool bValue)
{
	CStringA strValue(LogManager::BoolStr(bValue));
	return strValue;
}

CString Log::GetVector2dDataString(A3DVector2dData & cData)
{
	CString strData;
	strData.Format(L"%f, %f", cData.m_dX, cData.m_dY);

	return strData;
}

CStringA Log::GetVector2dDataStringA(A3DVector2dData & cData)
{
	CStringA strData;
	strData.Format("%f, %f", cData.m_dX, cData.m_dY);

	return strData;
}

CString Log::GetVector3dDataString(A3DVector3dData & cData)
{
	CString strData;
	strData.Format(L"%f, %f, %f", cData.m_dX, cData.m_dY, cData.m_dZ);

	return strData;
}

CStringA Log::GetVector3dDataStringA(A3DVector3dData & cData)
{
	CStringA strData;
	strData.Format("%f, %f, %f", cData.m_dX, cData.m_dY, cData.m_dZ);

	return strData;
}

CStringA Log::GetDimensionSymbolShapeString(EA3DMDDimensionSymbolShape eInType)
{
	switch (eInType)
	{
		case KEA3DDimensionSymbolNone: // 0, No symbol.
			return "None";
		case KEA3DDimensionSymbolOpenArrow: // 1, Open arrow.
			return "Open arrow";
		case KEA3DDimensionSymbolClosedArrow: // 2, Closed arrow.
			return "Closed arrow";
		case KEA3DDimensionSymbolFilledArrow: // 3, Filled arrow.
			return "Filled arrow";
		case KEA3DDimensionSymbolSymArrow: // 4, Symetric arrow.
			return "Symetric arrow";
		case KEA3DDimensionSymbolSlash: // 5, Slash.
			return "Slash";
		case KEA3DDimensionSymbolCircle: // 6, Circle.
			return "Circle";
		case KEA3DDimensionSymbolFilledCircle: // 7, Filled circle.
			return "Filled circle";
		case KEA3DDimensionSymbolScoredCircle: // 8, Scored circle.
			return "Scored circle";
		case KEA3DDimensionSymbolCircledCross: // 9, Circled cross.
			return "Circled cross";
		case KEA3DDimensionSymbolTriangle: // 10, Triangle.
			return "Triangle";
		case KEA3DDimensionSymbolFilledTriangle: // 11, Filled Triangle.
			return "Filled Triangle";
		case KEA3DDimensionSymbolCross: // 12, Cross.
			return "Cross";
		case KEA3DDimensionSymbolXCross: // 13, X cross.
			return "X cross";
		case KEA3DDimensionSymbolDoubleArrow: // 14, Double arrow.
			return "Double arrow";
		case KEA3DDimensionSymbolSquare: // 15, Box. \version 7.1
			return "Box";
		case KEA3DDimensionSymbolFilledSquare: // 16, Filled box. \version 7.1
			return "Filled box";
		case KEA3DDimensionSymbolWave: // 17, Wave. \version 7.1
			return "Wave";
	}

	return "Unknown";
}

CStringA Log::GetDimensionScoreString(EA3DMDDimensionScore eInType)
{
	switch (eInType)
	{
		case KEA3DMDDimensionScoreNone: // 0, No Score.
			return "None";
		case KEA3DMDDimensionUnderScored: // 1, Underscored.
			return "Underscored";
		case KEA3DMDDimensionScored: // 2, Scored.
			return "Scored";
		case KEA3DMDDimensionOverScored: // 3, overscored.
			return "overscored";
	}

	return "Unknown";
}

CStringA Log::GetDimensionFrameString(EA3DMDDimensionFrame eInType)
{
	switch (eInType)
	{
		case KEA3DMDDimensionFrameNone: // 0, No frame.
			return "None";
		case KEA3DMDDimensionFrameCircle: // 1, Circle frame.
			return "Circle";
		case KEA3DMDDimensionFrameScoredCircle: // 2, Scored circle frame.
			return "Scored circle";
		case KEA3DMDDimensionFrameDiamondShaped: // 3, Diamond frame.
			return "Diamond";
		case KEA3DMDDimensionFrameSquare: // 4, Square frame.
			return "Square";
		case KEA3DMDDimensionFrameRectangle: // 5, Rectangle frame.
			return "Rectangle";
		case KEA3DMDDimensionFrameOblong: // 6, Oblong frame.
			return "Oblong";
		case KEA3DMDDimensionFrameRightFlag: // 7, Right flag frame.
			return "Right flag";
		case KEA3DMDDimensionFrameRightTriangle: // 8, Triangle frame.
			return "Triangle";
	}

	return "Unknown";
}

CStringA Log::GetDimensionDualDisplayString(EA3DMDDimensionDualDisplay cInType)
{
	switch (cInType)
	{
		case KEA3DMDDimensionDualDisplayNone: // 0, No.
			return "None";
		case KEA3DMDDimensionDualDisplayBelow: // 1, Display below.
			return "Below";
		case KEA3DMDDimensionDualDisplayFractional: // 2, fractional display.
			return "Fractional";
		case KEA3DMDDimensionDualDisplaySideBySide: // 3,  Side by side display.
			return "Side by side";
		case KEA3DMDDimensionDualDisplayOnLeft: // 4, Dual value display before main value.
			return "On Left";
		case KEA3DMDDimensionDualDisplayOnRight: // 5,Dual value display after main value.
			return "On Right";
		case KEA3DMDDimensionDualDisplayAbove: // 6,	Dual value display above main value.
			return "Above";
		case KEA3DMDDimensionDualDisplayOnly: // 7, Display only dual value, main value is hidden.
			return "Only";
	}

	return "Unknown";
}


CStringA Log::GetDimensionTypeString(EA3DMDDimensionType cInType)
{
	CStringA strType = "Unknown";

	switch (cInType)
	{
		case KEA3DMDDimensionTypeDistance: // 0
			strType = "Distance";
			break;

		case KEA3DMDDimensionTypeDistanceOffset: // 1
			strType = "Distance Offset";
			break;

		case KEA3DMDDimensionTypeLength: // 2
			strType = "Length";
			break;

		case KEA3DMDDimensionTypeLengthCurvilinear: // 3
			strType = "Length Curvilinear";
			break;

		case KEA3DMDDimensionTypeAngle: // 4
			strType = "Angle";
			break;

		case KEA3DMDDimensionTypeRadius: // 5
			strType = "Radius";
			break;

		case KEA3DMDDimensionTypeRadiusTangent: // 6
			strType = "Radius Tangent";
			break;

		case KEA3DMDDimensionTypeRadiusCylinder: // 7
			strType = "Radius Cylinder";
			break;

		case KEA3DMDDimensionTypeRadiusEdge: // 8
			strType = "Radius Edge";
			break;

		case KEA3DMDDimensionTypeDiameter: // 9
			strType = "Diameter";
			break;

		case KEA3DMDDimensionTypeDiameterTangent: // 10
			strType = "Diameter Tangent";
			break;

		case KEA3DMDDimensionTypeDiameterCylinder: // 11
			strType = "Diameter Cylinder";
			break;

		case KEA3DMDDimensionTypeDiameterEdge: // 12
			strType = "Diameter Edge";
			break;

		case KEA3DMDDimensionTypeDiameterCone: // 13
			strType = "Diameter Cone";
			break;

		case KEA3DMDDimensionTypeChamfer: // 14
			strType = "Chamfer";
			break;

		case KEA3DMDDimensionTypeSlope: // 15
			strType = "Slope";
			break;
	}

	return strType;
}

CStringA Log::GetDimensionSymbolTypeString(EA3DMDDimensionSymbolType cInType)
{
	CStringA strType = "Unknown";

	switch (cInType)
	{
		case KEA3DDimensionSymbolTypeNONE: // 0,  None
			strType = "None";
			break;

		case KEA3DDimensionSymbolTypePROJTOLZONE: // 1,  Projection tolerance zone
			strType = "Projection tolerance zone";
			break;

		case KEA3DDimensionSymbolTypeMAXMATERIAL: // 2,  Max material
			strType = "Max material";
			break;

		case KEA3DDimensionSymbolTypeLEASTMATERIAL: // 3,  Least material
			strType = "Least material";
			break;

		case KEA3DDimensionSymbolTypeFREESTATE: // 4,  Freestate
			strType = "Freestate";
			break;

		case KEA3DDimensionSymbolTypeOHM: // 5,  Ohm
			strType = "Ohm";
			break;

		case KEA3DDimensionSymbolTypeCENTERLINE: // 6,  Center line
			strType = "Center line";
			break;

		case KEA3DDimensionSymbolTypeDEPTH: // 7,  Depth
			strType = "Depth";
			break;

		case KEA3DDimensionSymbolTypeCOUNTERBORE: // 8,  Counter bore
			strType = "Counter bore";
			break;

		case KEA3DDimensionSymbolTypeCOUNTERSUNK: // 9,  Counter sunk
			strType = "Counter sunk";
			break;

		case KEA3DDimensionSymbolTypeCIRCULARRUNOUT: // 10,  Circular runout
			strType = "Circular runout";
			break;

		case KEA3DDimensionSymbolTypeSURFPROFILE: // 11,  Surface profile
			strType = "Surface profile";
			break;

		case KEA3DDimensionSymbolTypeLINEPROFILE: // 12,  Line profile
			strType = "Line profile";
			break;

		case KEA3DDimensionSymbolTypeFLATNESS: // 13,  Flatness
			strType = "Flatness";
			break;

		case KEA3DDimensionSymbolTypeSTRAIGHT: // 14,  Straight
			strType = "Straight";
			break;

		case KEA3DDimensionSymbolTypeTOTALRUNOUT: // 15,  Total runout
			strType = "Total runout";
			break;

		case KEA3DDimensionSymbolTypeSYMMETRY: // 16,  Symmetry
			strType = "Symmetry";
			break;

		case KEA3DDimensionSymbolTypePERPENDICULAR: // 17,  Perpendicular
			strType = "Perpendicular";
			break;

		case KEA3DDimensionSymbolTypePARALLEL: // 18,  Parallel
			strType = "Parallel";
			break;

		case KEA3DDimensionSymbolTypeCYLINDRIC: // 19,  Cylindric
			strType = "Cylindric";
			break;

		case KEA3DDimensionSymbolTypeCONCENTRIC: // 20,  Concentric
			strType = "Concentric";
			break;

		case KEA3DDimensionSymbolTypeCIRCULAR: // 21,  Circular
			strType = "Circular";
			break;

		case KEA3DDimensionSymbolTypeANGULAR: // 22,  Angular
			strType = "Angular";
			break;

		case KEA3DDimensionSymbolTypeMICRO: // 23,  Micro
			strType = "Micro";
			break;

		case KEA3DDimensionSymbolTypeDEGREE: // 24,  Degree
			strType = "Degree";
			break;

		case KEA3DDimensionSymbolTypePLUSMINUS: // 25,  Plus - minus
			strType = "Plus - minus";
			break;

		case KEA3DDimensionSymbolTypePOSITION: // 26,  Position
			strType = "Position";
			break;

		case KEA3DDimensionSymbolTypeDIAMETER: // 27,  Diameter
			strType = "Diameter";
			break;

		case KEA3DDimensionSymbolTypeENVELOPE: // 28,  Envelope
			strType = "Envelope";
			break;

		case KEA3DDimensionSymbolTypeARROW: // 29,  Arrow
			strType = "Arrow";
			break;

		case KEA3DDimensionSymbolTypeNOACTUALSIZE: // 30,  No actual size
			strType = "No actual size";
			break;

		case KEA3DDimensionSymbolTypeTANGENTPLANE: // 31,  Tangent plane
			strType = "Tangent plane";
			break;

		case KEA3DDimensionSymbolTypeLOWEROREQUAL: // 32,  Lower or equal
			strType = "Lower or equal";
			break;

		case KEA3DDimensionSymbolTypeGREATEROREQUAL: // 33,  Greater or equal
			strType = "Greater or equal";
			break;

		case KEA3DDimensionSymbolTypeTHREADPREFIX: // 34,  Thread prefix
			strType = "Thread prefix";
			break;

		case KEA3DDimensionSymbolTypeSLOPE: // 35,  Slope
			strType = "Slope";
			break;

		case KEA3DDimensionSymbolTypeCONICALTAPER: // 36,  Conical taper
			strType = "Conical taper";
			break;

		case KEA3DDimensionSymbolTypeUPTRIANGLE: // 37,  Up triangle
			strType = "Up triangle";
			break;

		case KEA3DDimensionSymbolTypeSQUARE: // 38,  Square
			strType = "Square";
			break;

		case KEA3DDimensionSymbolTypeST: // 39,  Statistical
			strType = "Statistical";
			break;

		case KEA3DDimensionSymbolTypeSDIAMETER: // 40,  Spherical Diameter
			strType = "Spherical Diameter";
			break;

		case KEA3DDimensionSymbolTypeRADIUS: // 41,  Radius
			strType = "Radius";
			break;

		case KEA3DDimensionSymbolTypeSRADIUS: // 42,  Spherical Radius
			strType = "Spherical Radius";
			break;

		case KEA3DDimensionSymbolTypeCRADIUS: // 43,  Controlled Radius
			strType = "Controlled Radius";
			break;

		case KEA3DDimensionSymbolTypeUNEQUALLY: // 44,  Unequally
			strType = "Unequally";
			break;

		case KEA3DDimensionSymbolTypeCF: // 45,  Continuous Feature
			strType = "Continuous Feature";
			break;

			//a symbol can use to precise the dimension value,
			//It can be placed before or after the gloval dimension value,
			//the following value, precise that the symbol is to be placed after the dim value
		case KEA3DDimensionSymbolTypeSET_AFTER: // 1000  Set after
			strType = "Set after";
			break;
	}

	return strType;
}

CStringA Log::GetDimensionOrientationString(EA3DMDDimensionOrientation cInType)
{
	switch (cInType)
	{
		case KEA3DMDDimensionOrientationScreenHorizontal: // 0, Dimension along horizontal line of the screen.
			return "Screen Horizontal";
		case KEA3DMDDimensionOrientationScreenVertical: // 1, Dimension along vertical line of the screen.
			return "Screen Vertical";
		case KEA3DMDDimensionOrientationScreenAngle: // 2, Uses m_dAttachAngle of \ref A3DMarkupDefinitionData.
			return "Screen Angle";
		case KEA3DMDDimensionOrientationViewHorizontal: // 3, Dimension along horizontal line of the view.
			return "View Horizontal";
		case KEA3DMDDimensionOrientationViewVertical: // 4, Dimension along vertical line of the view.
			return "View Vertical";
		case KEA3DMDDimensionOrientationViewAngle: // 5, Uses m_dAttachAngle of \ref A3DMarkupDefinitionData.
			return "View Angle";
		case KEA3DMDDimensionOrientationParallel: // 6, Dimension is parallel to the dimension line.
			return "Parallel";
		case KEA3DMDDimensionOrientationPerpendicular: // 7, Dimension is perpendicular to the dimension line.
			return "Perpendicular";
		case KEA3DMDDimensionOrientationAngle: // 8, Uses m_dAttachAngle of \ref A3DMarkupDefinitionData.
			return "Angle";
	}

	return "Unknown";
}

CStringA Log::GetTextPropertiesScoreString(EA3DMDTextPropertiesScore cInType)
{
	switch (cInType)
	{
		case KEA3DMDTextPropertiesScoreNone: // 0, Text is not scored.
			return "None";
		case KEA3DMDTextPropertiesUnderScored: // 1, Text is underscored: draw a horizontal line below the text.
			return "UnderScored";
		case KEA3DMDTextPropertiesScored: // 2, Text is scored: draw a horizontal line through the text.
			return "Scored";
		case KEA3DMDTextPropertiesOverScored: // 3, Text is overscored: draw a horizontal line above the text.
			return "OverScored";
	}

	return "Unknown";
}

CStringA Log::GetTextPropertiesFormatString(EA3DMDTextPropertiesFormat cInType)
{
	switch (cInType)
	{
		case KEA3DMDTextPropertiesFormatNormal: // 0, Classical text.
			return "Normal";
		case KEA3DMDTextPropertiesFormatUnderLine: // 1, Text positioning under the classical text.
			return "UnderLine";
		case KEA3DMDTextPropertiesFormatOverLine: // 2, Text positioning over the classical text.
			return "OverLine";
		case KEA3DMDTextPropertiesFormatExposant: // 3, Text positioning as an exposant.
			return "Exposant";
		case KEA3DMDTextPropertiesFormatindice: // 4, Text positioning as an indice.
			return "Indice";
	}

	return "Unknown";

}

CStringA Log::GetTextPropertiesJustificationString(EA3DMDTextPropertiesJustification cInType)
{
	switch (cInType)
	{
		case KEA3DMDTextPropertiesJustificationLeft: // 0, Justifies the text on the left side.
			return "Left";
		case KEA3DMDTextPropertiesJustificationCenter: // 1, Centers the text.
			return "Center";
		case KEA3DMDTextPropertiesJustificationRight: // 2, Justifies the text on the right side.
			return "Right";
	}

	return "Unknown";
}

CStringA Log::GetAnchorPointTypeString(EA3DMDAnchorPointType cInType)
{
	switch (cInType)
	{
		case KEA3DMDAnchorIgnored: // -1, Ignored.
			return "Ignored";
		case KEA3DMDAnchorTop_left: // 0, Top left point.
			return "Top left";
		case KEA3DMDAnchorTop_center: // 1, Top center point.
			return "Top center";
		case KEA3DMDAnchorTop_right: // 2, Top right point.
			return "Top right";
		case KEA3DMDAnchorMiddle_left: // 10, Middle left point.
			return "Middle left";
		case KEA3DMDAnchorMiddle_center: // 11, Middle center point.
			return "Middle center";
		case KEA3DMDAnchorMiddle_right: // 12, Middle right point.
			return "Middle right";
		case KEA3DMDAnchorMiddle_auto: // 13, Closest point to the middle of the frame.
			return "Middle auto";
		case KEA3DMDAnchorBottom_left: // 20, Bottom left point.
			return "Bottom left";
		case KEA3DMDAnchorBottom_center: // 21, Bottom center point.
			return "Bottom center";
		case KEA3DMDAnchorBottom_right: // 22, Bottom right point.
			return "Bottom right";
		case KEA3DMDAnchorBottom_auto: // 23, Closest point to the bottom of the frame.
			return "Bottom auto";
		case KEA3DMDAnchorAutomatic: // 24, Closest point to the frame.
			return "Automatic";
	}

	return "Unknown";
}

CStringA Log::GetLeaderSymbolTypeString(A3DMDLeaderSymbolType cInType)
{
	switch (cInType)
	{
		case KA3DMDLeaderSymbolNotUsed: // 0, Unused symbol.
			return "Not Used";
		case KA3DMDLeaderSymbolCross: // 1, Cross.
			return "Cross";
		case KA3DMDLeaderSymbolPlus: // 2, Plus sign.
			return "Plus";
		case KA3DMDLeaderSymbolConcentric: // 3, Concentric sign.
			return "Concentric";
		case KA3DMDLeaderSymbolCoincident: // 4, Coincident.
			return "Coincident";
		case KA3DMDLeaderSymbolFullCircle: // 5, Full circle.
			return "Full Circle";
		case KA3DMDLeaderSymbolFullSquare: // 6, Full square.
			return "Full Square";
		case KA3DMDLeaderSymbolStar: // 7, Star.
			return "Star";
		case KA3DMDLeaderSymbolDot: // 8, Dot.
			return "Dot";
		case KA3DMDLeaderSymbolSmallDot: // 9, Small dot.
			return "Small Dot";
		case KA3DMDLeaderSymbolMisc1: // 10, Misc1.
			return "Misc1";
		case KA3DMDLeaderSymbolMisc2: // 11, Misc2.
			return "Misc2";
		case KA3DMDLeaderSymbolFullCircle2: // 12, Full circle2.
			return "Full Circle2";
		case KA3DMDLeaderSymbolFullSquare2: // 13, Full square2.
			return "Full Square2";
		case KA3DMDLeaderSymbolOpenArrow: // 14, Open arrow.
			return "Open Arrow";
		case KA3DMDLeaderSymbolUnfilledArrow: // 15, Transparent arrow.
			return "Transparent Arrow";
		case KA3DMDLeaderSymbolBlankedArrow: // 16, Blanked arrow.
			return "Blanked Arrow";
		case KA3DMDLeaderSymbolFilledArrow: // 17, Filled arrow.
			return "Filled Arrow";
		case KA3DMDLeaderSymbolUnfilledCircle: // 18, Transparent circle.
			return "Transparent Circle";
		case KA3DMDLeaderSymbolBlankedCircle: // 19, Opaque circle.
			return "Opaque Circle";
		case KA3DMDLeaderSymbolFilledCircle: // 20, Filled circle.
			return "Filled Circle";
		case KA3DMDLeaderSymbolCrossedCircle: // 21, Crossed circle.
			return "Crossed Circle";
		case KA3DMDLeaderSymbolBlankedSquare: // 22, Opaque square.
			return "Opaque Square";
		case KA3DMDLeaderSymbolFilledSquare: // 23, Filled square.
			return "Filled Square";
		case KA3DMDLeaderSymbolBlankedTriangle: // 24, Opaque triangle.
			return "Opaque Triangle";
		case KA3DMDLeaderSymbolFilledTriangle: // 25, Filled triangle.
			return "Filled Triangle";
		case KA3DMDLeaderSymbolManipulatorSquare: // 26, Manipulator square.
			return "Manipulator Square";
		case KA3DMDLeaderSymbolMamipulatorDiamond: // 27, Mamipulator diamond.
			return "Mamipulator Diamond";
		case KA3DMDLeaderSymbolManipulatorCircle: // 28, Manipulator circle.
			return "Manipulator Circle";
		case KA3DMDLeaderSymbolManipulatorTriangle: // 29, Manipulator triangle.
			return "Manipulator Triangle";
		case KA3DMDLeaderSymbolDoubleOpenArrow: // 30, Double open arrow.
			return "Double Open Arrow";
		case KA3DMDLeaderSymbolWave: // 31, Wave sign.
			return "Wave";
		case KA3DMDLeaderSymbolSegment: // 32, Segment.
			return "Segment";
		case KA3DMDLeaderSymbolDoubleFilledArrow: // 33, Double filled arrow.
			return "Double Filled Arrow";
		case KA3DMDLeaderSymbolDoubleClosedArrow: // 34, Double closed arrow.
			return "Double Closed Arrow";
		case KA3DMDLeaderSymbolHalfOpenArrowUp: // 35, Half open arrow up.
			return "Half Open Arrow Up";
		case KA3DMDLeaderSymbolHalfOpenArrowDown: // 36, Half open arrow down.
			return "Half Open Arrow Down";
		case KA3DMDLeaderSymbolHalfFilledArrowUp: // 37, Half filled arrow up.
			return "Half Filled Arrow Up";
		case KA3DMDLeaderSymbolHalfFilledArrowDown: // 38, Half filled arrow down.
			return "Half Filled Arrow Down";
		case KA3DMDLeaderSymbolSlash: // 39, Slash.
			return "Slash";
		case KA3DMDLeaderSymbolDoubleBlankedArrow: // 40, Double blanked arrow.
			return "Double Blanked Arrow";
		case KA3DMDLeaderSymbolIntegral: // 41, Integral.
			return "Integral";
		case KA3DMDLeaderSymbolZoneGlobalAllAround: // 50, Global All Around.
			return "Global All Around";
		case KA3DMDLeaderSymbolZonePartialAllAround: // 51, Partial All Around.
			return "Partial All Around";
		case KA3DMDLeaderSymbolZoneGlobalAllAboutWithHorizontalAxisIndicator: // 52, Zone global all about with horizontal axis indicator.
			return "Zone Global All About With Horizontal Axis Indicator";
		case KA3DMDLeaderSymbolZoneGlobalAllAboutWithVerticalAxisIndicator: // 53, Zone global all about with vertical axis indicator.
			return "Zone Global All About With Vertical Axis Indicator";
		case KA3DMDLeaderSymbolZonePartialAllAboutWithHorizontalAxisIndicator: // 54, Zone partial all about with horizontal axis indicator.
			return "Zone Partial All About With Horizontal Axis Indicator";
		case KA3DMDLeaderSymbolZonePartialAllAboutWithVerticalAxisIndicator: // 55, Zone partial all about with vertical axis indicator.
			return "Zone Partial All About With Vertical Axis Indicator";
		case KA3DMDLeaderSymbolZoneGlobalAllOver: // 56, Zone global all over.
			return "Zone Global All Over";
		case KA3DMDLeaderSymbolZonePartialAllOver: // 57, Zone partial all over.
			return "Zone Partial All Over";
	}

	return "Unknown";
}

#else
void Log::CreateLog(int nId, const WCHAR * pchFilePathName) {}
void Log::Write(int nId, LPCWSTR chMessage, ...) {}
void Log::Write(int nId, LPCSTR chMessage, ...) {}
void Log::IncreaseTabIndex(int nId) {}
void Log::DecreaseTabIndex(int nId) {}

void Log::CreateLog(const WCHAR * pchFilePathName) {}
void Log::Write(LPCWSTR chMessage, ...) {}
void Log::Write(LPCSTR chMessage, ...) {}
void Log::IncreaseTabIndex() {}
void Log::DecreaseTabIndex() {}

void Log::Position(A3DMDPosition * pcPosition);
void Log::Vector3dData(A3DVector3dData & cData, CStringA strPrompt) {}
bool Log::GetName(const A3DRootBaseWithGraphics * pcRootBase, CString & strName) { return false; }
void Log::View(A3DMkpView * pcView, A3DMkpViewData & cViewData) {}
void Log::SurfPlane(A3DSurfPlane * pcInSurfPlane) {}
void Log::GraphCamera(A3DGraphCamera * pcInCamera) {}
void Log::LinkedItem(A3DMiscMarkupLinkedItemData & cData, A3DMiscEntityReferenceData & cRefData, DWORD_PTR nInAddreass) {}

void Log::DimensionData(const A3DMkpMarkup * pcInData) {}
void Log::DimensionValue(A3DMDDimensionValue * pcInData, CStringA strPrefix) {}
void Log::DimensionValueFormat(A3DMDDimensionValueFormat * pcInData) {}
void Log::DimensionExtentionLine(A3DMDDimensionExtentionLine * pcInData) {}
void Log::DimensionFunnel(A3DMDDimensionFunnel * pcInData) {}
void Log::DimensionExtremityData(A3DMDDimensionExtremityData & cInData) {}
void Log::DimensionLine(A3DMDDimensionLine * pcInData) {}
void Log::DimensionLineSymbol(A3DMDDimensionLineSymbol * pcInData, CStringA strPrefix) {}
void Log::TextProperties(A3DMDTextProperties * pcInData) {}

void Log::LeaderData(const A3DMkpMarkup * pcInData) {}
void Log::LeaderDefinitionData(A3DMDLeaderDefinition * pcInData) {}
void Log::LeaderSymbol(A3DMDLeaderSymbol * pcInData) {}
void Log::LeaderStub(A3DMDMarkupLeaderStub * pcInData) {}

void Log::TessMarkup(const A3DMkpMarkup * pcInData);

CString Log::HexStr(DWORD_PTR nValue) { return L""; }
CStringA Log::HexStrA(DWORD_PTR nValue) { return ""; }
CString Log::BoolStr(bool bValue) { return L""; }
CStringA Log::BoolStrA(bool bValue) { return ""; }
CString Log::GetVector2dDataString(A3DVector2dData & cData) { return L""; }
CStringA Log::GetVector2dDataStringA(A3DVector2dData & cData) { return ""; }
CString Log::GetVector3dDataString(A3DVector3dData & cData) { return L""; }
CStringA Log::GetVector3dDataStringA(A3DVector3dData & cData) { return ""; }

CStringA Log::GetDimensionTypeString(EA3DMDDimensionType cInType) { return ""; }
CStringA Log::GetDimensionSymbolTypeString(EA3DMDDimensionSymbolType cInType) { return ""; }
CStringA Log::GetDimensionOrientationString(EA3DMDDimensionOrientation cInType) { return ""; }
CStringA Log::GetDimensionDualDisplayString(EA3DMDDimensionDualDisplay cInType) { return ""; }
CStringA Log::GetDimensionSymbolShapeString(EA3DMDDimensionSymbolShape eInType) { return ""; }
CStringA Log::GetDimensionScoreString(EA3DMDDimensionScore eInType) { return ""; }
CStringA Log::GetDimensionFrameString(EA3DMDDimensionFrame eInType) { return ""; }
CStringA Log::GetTextPropertiesScoreString(EA3DMDTextPropertiesScore cInType) { return ""; }
CStringA Log::GetTextPropertiesFormatString(EA3DMDTextPropertiesFormat cInType) { return ""; }
CStringA Log::GetTextPropertiesJustificationString(EA3DMDTextPropertiesJustification cInType) { return ""; }
CStringA Log::GetAnchorPointTypeString(EA3DMDAnchorPointType cInType) { return ""; }
CStringA Log::GetLeaderSymbolTypeString(A3DMDLeaderSymbolType cInType) { return ""; }
#endif