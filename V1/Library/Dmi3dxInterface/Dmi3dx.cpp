#include "stdafx.h"

#include "Dmi3dx.h"

#include <Common_Define.h>

#include <mb_placement3d.h>

#include <format>

// == Text 관련 함수 =================================================================================

// Char text를 Unicode CString으로 변환하여 돌려줌,
// Windows에서 Unicode 인코딩은 UTF-16을 사용한다. 그러니까 유니코드로 변환은 고정적으로 UTF-16으로 인코딩한다는 것으로 보면 된다. 
// 한국어를 사용하는 윈도우에서는 멀티바이트일때 CP949를 디폴트로 사용한다. 
// 따라서 WideCharToMultiByte 사용시 CodePage 인자로 UTF-8을 넣는게 아니라 CP_ACP를 넣으면 CP949로 인코딩된다.
bool Dmi3dx::CharToCString(char * pchText, CString & strText, UINT nCodePage)
{
	if(nullptr == pchText) {
		RETURN_FALSE;
	}

	size_t nSize = strlen(pchText);
	if(0 != nSize) {
		RETURN_FALSE;
	}

	int nBufferSize = MultiByteToWideChar(nCodePage, 0, pchText, -1, nullptr, 0);
	if(0 != nBufferSize) {
		RETURN_FALSE;
	}

	WCHAR * pchBuffer = new WCHAR[nBufferSize];
	if(nullptr == pchBuffer) {
		RETURN_FALSE;
	}

	int nConvertSize = MultiByteToWideChar(nCodePage, 0, pchText, -1, pchBuffer, nBufferSize);
	if(0 != nConvertSize) {
		RETURN_FALSE;
	}

	strText = pchBuffer;

	REMOVE_ARRAY(pchBuffer);

	return true;
}

bool Dmi3dx::CStringToChar(CString strText, char *& pchText)
{
	int nTextSize = 0;
	return CStringToChar(strText, pchText, nTextSize);
}

// Windows에서 Unicode 인코딩은 UTF-16을 사용한다. 그러니까 유니코드로 변환은 고정적으로 UTF-16으로 인코딩한다는 것으로 보면 된다. 
// 한국어를 사용하는 윈도우에서는 멀티바이트일때 CP949를 디폴트로 사용한다. 
// 따라서 WideCharToMultiByte 사용시 CodePage 인자로 UTF-8을 넣는게 아니라 CP_ACP를 넣으면 CP949로 인코딩된다.
bool Dmi3dx::CStringToChar(CString strText, char *& pchText, int & nTextSize, UINT nCodePage)
{
	if(true == strText.IsEmpty()) {
		return false;
	}

	int nSize = WideCharToMultiByte(CP_ACP, 0, strText, -1, NULL, 0, NULL, NULL);
	pchText = new char[nSize];
	if(nullptr == pchText) {
		assert(FALSE);
		return false;
	}

	nTextSize = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) strText, -1, pchText, nSize, NULL, NULL);

	if(0 == nTextSize) {
		assert(FALSE);
		delete[] pchText;
		return false;
	}

	return true;
}


// 실행 파일 경로 (끝에 '\' 붙어서 나옴)
CString Dmi3dx::GetExecuteDirectory()
{
	TCHAR szBuffer[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Path[_MAX_PATH];
	TCHAR Filename[_MAX_FNAME];
	TCHAR Ext[_MAX_EXT];

	GetModuleFileName(NULL, szBuffer, sizeof(szBuffer)); // get process file name
	_wsplitpath_s(szBuffer, Drive, _MAX_DRIVE, Path, _MAX_PATH, Filename, _MAX_FNAME, Ext, _MAX_EXT); // get drive, path, file, ext name

	CString strFilePath;
	strFilePath.Format(L"%s%s", Drive, Path);

	return strFilePath;
}

// 다중 디렉토리도 생성함.
bool Dmi3dx::CreateFolder(CString strPath)
{
	WCHAR chPathBuffer[MAX_PATH];

	size_t len = wcslen(strPath);

	for(size_t i = 0; i < len; i++)
	{
		chPathBuffer[i] = *(strPath.GetBuffer() + i);
		if(_T('\\') == chPathBuffer[i] || _T('/') == chPathBuffer[i])
		{
			chPathBuffer[i + 1] = NULL;
			if(FALSE == PathFileExists(chPathBuffer))
			{
				if(FALSE == ::CreateDirectory(chPathBuffer, NULL))
				{
					if(GetLastError() != ERROR_ALREADY_EXISTS) {
						return false;
					}
				}
			}
		}
	}

	return true;
}

MbCartPoint Dmi3dx::GetMbCartPoint(A3DVector3dData cVector, double dScale)
{
	MbCartPoint cCartPoint;

	cCartPoint.x = cVector.m_dX * dScale;
	cCartPoint.y = cVector.m_dY * dScale;

	return cCartPoint;
}

MbCartPoint Dmi3dx::GetMbCartPoint(A3DVector3dData cVector)
{
	MbCartPoint cCartPoint;

	cCartPoint.x = cVector.m_dX;
	cCartPoint.y = cVector.m_dY;

	return cCartPoint;
}

MbCartPoint3D Dmi3dx::GetMbCartPoint3D(A3DVector3dData cVector, double dScale)
{
	MbCartPoint3D cCartPoint3D;

	cCartPoint3D.x = cVector.m_dX * dScale;
	cCartPoint3D.y = cVector.m_dY * dScale;
	cCartPoint3D.z = cVector.m_dZ * dScale;

	return cCartPoint3D;
}

MbCartPoint3D Dmi3dx::GetMbCartPoint3D(A3DVector3dData cVector, A3DVector3dData cScaleVector)
{
	MbCartPoint3D cCartPoint3D;

	cCartPoint3D.x = cVector.m_dX * cScaleVector.m_dX;
	cCartPoint3D.y = cVector.m_dY * cScaleVector.m_dY;
	cCartPoint3D.z = cVector.m_dZ * cScaleVector.m_dZ;

	return cCartPoint3D;
}

MbVector3D Dmi3dx::GetMbVector3D(A3DVector3dData cVector)
{
	MbVector3D cVector3D;

	cVector3D.x = cVector.m_dX;
	cVector3D.y = cVector.m_dY;
	cVector3D.z = cVector.m_dZ;

	return cVector3D;
}

MbVector Dmi3dx::GetMbVector(A3DVector3dData cVector)
{
	MbVector cMbVector;

	cMbVector.x = cVector.m_dX;
	cMbVector.y = cVector.m_dY;

	return cMbVector;
}

// 3DX Cartesian Transformation 정보를 C3D Placement3d 데이타로 변경해서 돌려줌.
bool Dmi3dx::GetPlacement3d(const A3DMiscCartesianTransformationData & cTransformationData, double dParentScale, MbPlacement3D & cPlacement)
{
	MbVector3D cXAxis = Dmi3dx::GetMbVector3D(cTransformationData.m_sXVector);
	MbVector3D cYAxis = Dmi3dx::GetMbVector3D(cTransformationData.m_sYVector);
	//MbVector3D cScale = Rt3dxUtility::GetMbVector3D(cTransformationData.m_sScale);
	MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cTransformationData.m_sOrigin, dParentScale);

	cPlacement.InitXY(cOrigin, cXAxis, cYAxis, true);

	return true;
}

// == A3D 관련 함수 ==================================================================================

// 1. A3D Entity의 이름을 돌려준다.
CStringA Dmi3dx::GetA3dEntityTypeString(A3DEEntityType eType)
{
	CStringA strText;

	switch(eType)
	{
		case kA3DTypeUnknown:
			strText = "Unknown";
			break;

		case kA3DTypeCrvNurbs:
			strText = "CrvNurbs";
			break;

		case kA3DTypeCrvCircle:
			strText = "CrvCircle";
			break;

		case kA3DTypeCrvComposite:
			strText = "CrvComposite";
			break;

		case kA3DTypeCrvOnSurf:
			strText = "CrvOnSurf";
			break;

		case kA3DTypeCrvLine:
			strText = "CrvLine";
			break;

		case kA3DTypeCrvOffset:
			strText = "CrvOffset";
			break;

		case kA3DTypeSurfBlend03:
			strText = "SurfBlend03";
			break;

		case kA3DTypeSurfOffset:
			strText = "SurfOffset";
			break;

		case kA3DTypeSurfRuled:
			strText = "SurfRuled";
			break;

		case kA3DTypeSurfRevolution:
			strText = "SurfRevolution";
			break;

		case kA3DTypeSurfExtrusion:
			strText = "SurfExtrusion";
			break;

		case kA3DTypeAsmProductOccurrence:
			strText = "AsmProductOccurrence";
			break;

		case kA3DTypeAsmPartDefinition:
			strText = "AsmPartDefinition";
			break;

		case kA3DTypeAsmPublication:
			strText = "AsmPublication";
			break;

		case kA3DTypeAsmPublicationSet:
			strText = "AsmPublicationSet";
			break;

		case kA3DTypeAsmPublicationLinkedItem:
			strText = "AsmPublicationLinkedItem";
			break;

		case kA3DTypeAsmFilter:
			strText = "AsmFilter";
			break;

		case kA3DTypeAsmConstraint:
			strText = "AsmConstraint";
			break;

		case kA3DTypeMkpView:
			strText = "MkpView";
			break;

		case kA3DTypeMkpMarkup:
			strText = "MkpMarkup";
			break;

		case kA3DTypeMkpLeader:
			strText = "MkpLeader";
			break;

		case kA3DTypeMkpAnnotationItem:
			strText = "MkpAnnotationItem";
			break;

		case kA3DTypeMkpAnnotationSet:
			strText = "MkpAnnotationSet";
			break;

		case kA3DTypeMkpAnnotationReference:
			strText = "MkpAnnotationReference";
			break;

		case kA3DTypeRiRepresentationItem:
			strText = "RiRepresentationItem";
			break;

		case kA3DTypeRiBrepModel:
			strText = "RiBrepModel";
			break;

		case kA3DTypeRiCurve:
			strText = "RiCurve";
			break;

		case kA3DTypeRiDirection:
			strText = "RiDirection";
			break;

		case kA3DTypeRiPlane:
			strText = "RiPlane";
			break;

		case kA3DTypeRiPointSet:
			strText = "RiPointSet";
			break;

		case kA3DTypeRiPolyBrepModel:
			strText = "RiPolyBrepModel";
			break;

		case kA3DTypeRiPolyWire:
			strText = "RiPolyWire";
			break;

		case kA3DTypeRiSet:
			strText = "RiSet";
			break;

		case kA3DTypeRiCoordinateSystem:
			strText = "RiCoordinateSystem";
			break;

		case kA3DTypeRiCoordinateSystemItem:
			strText = "RiCoordinateSystemItem";
			break;

		case kA3DTypeMarkupText: // 10111
			strText = "MarkupText";
			break;

		case kA3DTypeMarkupRichText: // 10112
			strText = "MarkupRichText";
			break;

		case kA3DTypeMarkupCoordinate: // 10113
			strText = "MarkupCoordinate";
			break;

		case kA3DTypeMarkupBalloon: // 10114
			strText = "MarkupBalloon";
			break;

		case kA3DTypeMarkupDatum: // 10115
			strText = "MarkupDatum";
			break;

		case kA3DTypeMarkupGDT: // 10116
			strText = "MarkupGDT";
			break;

		case kA3DTypeMarkupRoughness: // 10117
			strText = "MarkupRoughness";
			break;

		case kA3DTypeMarkupDimension: // 10118
			strText = "MarkupDimension";
			break;

		case kA3DTypeMarkupLocator: // 10119
			strText = "MarkupLocator";
			break;

		case kA3DTypeMarkupMeasurementPoint: // 10120
			strText = "MarkupMeasurementPoint";
			break;

		case kA3DTypeMarkupFastener: // 10121
			strText = "MarkupFastener";
			break;

		case kA3DTypeMarkupSpotWelding: // 10122
			strText = "MarkupSpotWelding";
			break;

		case kA3DTypeMarkupLineWelding: // 10123
			strText = "MarkupLineWelding";
			break;

		default:
			strText.Format("A3dEntityType: {%d}", (int)eType);
			break;
	}

	return strText;
}

// 1-1. A3D Entity의 이름을 돌려준다.
CStringA Dmi3dx::GetA3dEntityTypeString(const A3DEntity * pcInEntity)
{
	if (nullptr == pcInEntity) {
		return "NULL";
	}

	A3DEEntityType eType;
	A3DEntityGetType(pcInEntity, &eType);

	return GetA3dEntityTypeString(eType);
}

