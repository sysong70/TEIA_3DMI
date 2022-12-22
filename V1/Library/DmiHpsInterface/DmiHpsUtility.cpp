#include "stdafx.h"
#include "DmiHpsUtility.h"


HPS::Point DmiHps::GetPoint(MbCartPoint3D & cOther)
{
	HPS::Point cPoint;

	cPoint.x = (float)cOther.x;
	cPoint.y = (float)cOther.y;
	cPoint.z = (float)cOther.z;

	return cPoint;
}

HPS::Point DmiHps::GetPoint(MbFloatPoint3D & cOther)
{
	HPS::Point cPoint;

	cPoint.x = cOther.x;
	cPoint.y = cOther.y;
	cPoint.z = cOther.z;

	return cPoint;
}

HPS::Vector DmiHps::GetVector(MbFloatVector3D & cOther)
{
	HPS::Vector cVector;

	cVector.x = cOther.x;
	cVector.y = cOther.y;
	cVector.z = cOther.z;

	return cVector;
}

HPS::RGBColor DmiHps::GetRgbColor(COLORREF cColor)
{
	HPS::RGBColor cRgbColor;

	cRgbColor.red = GetRValue(cColor) / 255.0f;
	cRgbColor.green = GetGValue(cColor) / 255.0f;
	cRgbColor.blue = GetBValue(cColor) / 255.0f;

	return cRgbColor;
}

// Char text를 Unicode CString으로 변환하여 돌려줌,
// Windows에서 Unicode 인코딩은 UTF-16을 사용한다. 그러니까 유니코드로 변환은 고정적으로 UTF-16으로 인코딩한다는 것으로 보면 된다. 
// 한국어를 사용하는 윈도우에서는 멀티바이트일때 CP949를 디폴트로 사용한다. 
// 따라서 WideCharToMultiByte 사용시 CodePage 인자로 UTF-8을 넣는게 아니라 CP_ACP를 넣으면 CP949로 인코딩된다.
bool DmiHps::CharToCString(char * pchText, CString & strText, UINT nCodePage)
{
	if(nullptr == pchText) {
		return false;
	}

	size_t nSize = strlen(pchText);
	if(0 == nSize) {
		return false;
	}

	int nBufferSize = MultiByteToWideChar(nCodePage, 0, pchText, -1, nullptr, 0);
	if(0 == nBufferSize) {
		assert(false);
		return false;
	}

	WCHAR * pchBuffer = new WCHAR[nBufferSize];
	if(nullptr == pchBuffer) {
		assert(false);
		return false;
	}

	int nConvertSize = MultiByteToWideChar(nCodePage, 0, pchText, -1, pchBuffer, nBufferSize);
	if(0 == nConvertSize) {
		assert(false);
		return false;
	}

	strText = pchBuffer;

	delete[] pchBuffer;

	return true;
}

bool DmiHps::CStringToChar(CString strText, char *& pchText, int & nTextSize, UINT nCodePage /*= CP_UTF8*/)
{
	if(true == strText.IsEmpty()) {
		return false;
	}

	int nSize = WideCharToMultiByte(nCodePage, 0, strText, -1, NULL, 0, NULL, NULL);
	pchText = new char[nSize];
	if(nullptr == pchText) {
		assert(FALSE);
		return false;
	}

	nTextSize = WideCharToMultiByte(nCodePage, 0, (LPCWSTR) strText, -1, pchText, nSize, NULL, NULL);

	if(0 == nTextSize) {
		assert(FALSE);
		delete[] pchText;
		return false;
	}

	return true;
}

CString DmiHps::ToString(const HPS::UTF8 & chText)
{
	HPS::WCharArray wtext;
	chText.ToWStr(wtext);
	return wtext.data();
}

//== HPS 관련 Function ==============================================================================

// 1. 주어진 Sgement의 Properties를 대상 SgementKey에 복사.
bool DmiHps::CopySegmentProperties(HPS::SegmentKey & cSource, HPS::SegmentKey & cDestination)
{
	HPS::CameraKit cCameraKit;
	if(true == cSource.ShowCamera(cCameraKit)) {
		cDestination.SetCamera(cCameraKit);
	}

	HPS::MaterialMappingKit cMaterialMappingKit;
	if(true == cSource.ShowMaterialMapping(cMaterialMappingKit)) {
		cDestination.SetMaterialMapping(cMaterialMappingKit);
	}

	HPS::VisibilityKit cVisibilityKit;
	if(true == cSource.ShowVisibility(cVisibilityKit)) {
		cDestination.SetVisibility(cVisibilityKit);
	}

	HPS::LightingAttributeKit cLightingAttributeKit;
	if(true == cSource.ShowLightingAttribute(cLightingAttributeKit)) {
		cDestination.SetLightingAttribute(cLightingAttributeKit);
	}

	HPS::DrawingAttributeKit cDrawingAttributeKit;
	if(true == cSource.ShowDrawingAttribute(cDrawingAttributeKit)) {
		cDestination.SetDrawingAttribute(cDrawingAttributeKit);
	}

	return true;
}

// 2. Face Alpha 값을 가져오는 함수
bool DmiHps::GetFrontFaceAlpha(HPS::SegmentKey cSegKey, float & cAlpha)
{
	HPS::MaterialMappingKit cMaterialMappingKit;
	if(false == cSegKey.ShowMaterialMapping(cMaterialMappingKit)) {
		return false;
	}

	HPS::Material::Type cType;
	HPS::MaterialKit cMaterialKit;
	float fMaterialIndex = 0;
	if(false == cMaterialMappingKit.ShowFrontFaceMaterial(cType, cMaterialKit, fMaterialIndex)) {
		return false;
	}

	HPS::RGBAColor cDiffuseColor;
	if(false == cMaterialKit.ShowDiffuseColor(cDiffuseColor)) {
		return false;
	}

	cAlpha = cDiffuseColor.alpha;

	return true;
}

