#include "StdAfx.h"

#include <HTools.h>

#include "Database.h"
#include "Segment.h"
#include "Bounding.h"
#include "Line.h"
#include "Circle.h"

#include "Visibility.h"
#include "VisualEffects.h"
#include "Material.h"
#include "MarkerAttribute.h"
#include "LineAttribute.h"
#include "Performance.h"

#include "Camera.h"

#include "3DF.Utility.h"

#include "./Private/SegmentPrivate.h"
#include "./Private/SearchPrivate.h"

USING_3DF_NAMESPACE

H3DF::SegmentKey::SegmentKey(CString strInName)
{
	HC_KEY nKey = INVALID_KEY;

	if(false == strInName.IsEmpty()) {
		nKey = HC_Create_Segment(Utility::ToChar(strInName));
	}
	else {
		nKey = HC_Create_Segment(nullptr);
	}

	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();
	pcImpl->SetKeyValue(nKey);
	
	m_pcImpl = pcImpl;
}

H3DF::SegmentKey::SegmentKey(HC_KEY nInKey)
{
	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

H3DF::SegmentKey::SegmentKey(SegmentKey const & cInThat)
{
	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();
	m_pcImpl = pcImpl;

	Set(cInThat);
}

H3DF::SegmentKey::~SegmentKey()
{
}

void H3DF::SegmentKey::Set(SegmentKey const & cInThat)
{
//	Key::Set(cInThat);

	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	SegmentKeyPrivate * pcInThatImpl = (SegmentKeyPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

SegmentKey & H3DF::SegmentKey::operator = (SegmentKey const & cInThat)
{
//	Key::Set(cInThat);

	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	SegmentKeyPrivate * pcInThatImpl = (SegmentKeyPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);

	return *this;
}

//== Segment 관련 함수 ===============================================================================

/*
void H3DF::SegmentKey::Open()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalOpen();
}

void H3DF::SegmentKey::Open() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalOpen();
}

void H3DF::SegmentKey::Close()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalClose();
}

void H3DF::SegmentKey::Close() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalClose();
}
*/

SegmentKey & H3DF::SegmentKey::Open()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Open();

	return *this;
}

SegmentKey & H3DF::SegmentKey::Close()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Close();

	return *this;
}

CString H3DF::SegmentKey::Name() const
{
	CString strOutName;

	char chSegName[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Segment(KeyValue(), chSegName);

	H3DF::Utility::CharToUnicode(chSegName, strOutName);

	return strOutName;
}

SegmentKey & H3DF::SegmentKey::SetName(CString strInName)
{
	SegmentKeyPrivate::LocalOpen(*this); {
		HC_Rename_Segment(".", Utility::ToChar(strInName));
	} SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

//== Sub Segment 관련 함수 ===========================================================================
SegmentKey const H3DF::SegmentKey::Subsegment()
{
	SegmentKeyPrivate::LocalOpen(*this);
	SegmentKey cSubsegment(L"");
	SegmentKeyPrivate::LocalClose(*this);

	return cSubsegment;
}

SegmentKey const H3DF::SegmentKey::Subsegment(LPCTSTR chFormat, ...)
{
	CString strText;
	va_list argList;

	va_start(argList, chFormat);
	strText.FormatV(chFormat, argList);
	va_end(argList);

// 	HC_KEY nKey = HC_Create_Segment_Key_By_Key(KeyValue(), Utility::ToChar(strText));
// 
// 	SegmentKey cSubsegment(nKey);

	SegmentKeyPrivate::LocalOpen(*this);
	SegmentKey cSubsegment(strText);
	SegmentKeyPrivate::LocalClose(*this);

	return cSubsegment;
}

size_t H3DF::SegmentKey::ShowSubsegments() const
{
	int nSegmentCount = 0;

	SegmentKeyPrivate::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "segments"); {
		HC_Show_Contents_Count(&nSegmentCount);
	} HC_End_Contents_Search();

	SegmentKeyPrivate::LocalClose(*this);

	return nSegmentCount;
}

size_t H3DF::SegmentKey::ShowSubsegments(SegmentKeyArray & cOutChildren) const
{
	int nSegmentCount = 0;

	SegmentKeyPrivate::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "segments");
	{
		HC_Show_Contents_Count(&nSegmentCount);

		HC_KEY nChildSegkey;
		char chType[MVO_BUFFER_SIZE];

		for (int i = 0; i < nSegmentCount; i++)
		{
			HC_Find_Contents(chType, &nChildSegkey);

// 			if (streq(chType, "include")) {	// Include도 포함한다
// 				childSegkey = HC_KShow_Include_Segment(childSegkey);
// 				childSegkey = HC_KShow_Original_Key(childSegkey);
// 			}

			SegmentKey cChildSegment(nChildSegkey);
			cOutChildren.push_back(nChildSegkey);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyPrivate::LocalClose(*this);

	return nSegmentCount;
}

//== Flush 관련 함수 =============================================================================
void H3DF::SegmentKey::Flush(Search::Type eInTypeToRemove, Search::Space eInSearchSpace)
{
	SegmentKeyPrivate::LocalOpen(*this);

	CString strType = SearchPrivate::GetSearchTypeString(eInTypeToRemove);
	CString strSearchSpace = SearchPrivate::GetSearchSpaceString(eInSearchSpace);

	HC_Flush_Contents(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));

	SegmentKeyPrivate::LocalClose(*this);
}

void H3DF::SegmentKey::Flush(SearchTypeArray const & aInTypesToRemove, Search::Space eInSearchSpace)
{
	SegmentKeyPrivate::LocalOpen(*this);

	CString strType;
	
	for (auto eType : aInTypesToRemove)
	{
		if (false == strType.IsEmpty()) {
			strType += ", ";
		}

		strType += SearchPrivate::GetSearchTypeString(eType);
	}

	CString strSearchSpace = SearchPrivate::GetSearchSpaceString(eInSearchSpace);

	HC_Flush_Contents(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));

	SegmentKeyPrivate::LocalClose(*this);
}

void H3DF::SegmentKey::Flush(size_t nInTypesCount, Search::Type const peInTypesToRemove[], Search::Space eInSearchSpace)
{
	SegmentKeyPrivate::LocalOpen(*this);

	CString strType;

	for (size_t nIndex = 0 ; nIndex < nInTypesCount ; nIndex++)
	{
		if (false == strType.IsEmpty()) {
			strType += ", ";
		}

		strType += SearchPrivate::GetSearchTypeString(peInTypesToRemove[nIndex]);
	}

	CString strSearchSpace = SearchPrivate::GetSearchSpaceString(eInSearchSpace);

	HC_Flush_Contents(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));

	SegmentKeyPrivate::LocalClose(*this);
}

//== Include 관련 함수 ===============================================================================
IncludeKey H3DF::SegmentKey::IncludeSegment(SegmentKey const & cInSegment)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nIncludeKey = HC_Include_Segment_By_Key(cInSegment.KeyValue());
	IncludeKey cInclude(nIncludeKey);
	SegmentKeyPrivate::LocalClose(*this);

	return cInclude;
}

size_t H3DF::SegmentKey::ShowIncluders(SegmentKeyArray & aOutSegments) const
{
	int nSegmentCount = 0;

	SegmentKeyPrivate::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "include");
	{
		HC_Show_Contents_Count(&nSegmentCount);

		HC_KEY nIncludeKey;
		char chType[MVO_BUFFER_SIZE];
		char chPathName[MVO_BUFFER_SIZE];

		for (int i = 0; i < nSegmentCount; i++)
		{
			HC_Find_Contents(chType, &nIncludeKey);

			HC_KEY nSegmentKey = HC_Show_Include_Segment(nIncludeKey, chPathName);

			SegmentKey cIncludeSegment(nSegmentKey);
			aOutSegments.push_back(cIncludeSegment);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyPrivate::LocalClose(*this);

	return nSegmentCount;
}

size_t H3DF::SegmentKey::ShowIncluders(IncludeKeyArray & aOutIncludes) const
{
	int nIncludeCount = 0;

	SegmentKeyPrivate::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "include");
	{
		HC_Show_Contents_Count(&nIncludeCount);

		HC_KEY nIncludeKey;
		char chType[MVO_BUFFER_SIZE];

		for (int i = 0; i < nIncludeCount; i++)
		{
			HC_Find_Contents(chType, &nIncludeKey);

			IncludeKey cInclude(nIncludeKey);
			aOutIncludes.push_back(cInclude);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyPrivate::LocalClose(*this);

	return nIncludeCount;
}


//== Shell 관련 함수 =================================================================================
ShellKey H3DF::SegmentKey::InsertShell(ShellKit const & cInKit)
{
	H3DF::PointArray const * pacPoints = nullptr;
	H3DF::VectorArray const * pacNormals = nullptr;
	H3DF::IntArray const * pacFacelist = nullptr;
	H3DF::FloatArray const * paParameters = nullptr;
	H3DF::RGBAColorArray const * paColors = nullptr;

	cInKit.ShowPoints(pacPoints);
	cInKit.ShowNormals(pacNormals);
	cInKit.ShowFacelist(pacFacelist);
	cInKit.ShowParameters(paParameters);
	cInKit.ShowColors(paColors);

	SegmentKeyPrivate::LocalOpen(*this);

	HC_KEY nShellKey = INVALID_KEY;
	if (nullptr != pacFacelist) {
		nShellKey = HC_Insert_Shell(static_cast<int>(pacPoints->size()), pacPoints->data(), static_cast<int>(pacFacelist->size()), pacFacelist->data());
	}
	else {
		nShellKey = HC_Insert_Shell(static_cast<int>(pacPoints->size()), pacPoints->data(), 0, nullptr);
	}
	

	if(INVALID_KEY != nShellKey) {
		if (nullptr != pacNormals) {
			if (false == pacNormals->empty()) {
				HC_MSet_Vertex_Normals(nShellKey, 0, static_cast<int>(pacNormals->size()), pacNormals->data());
			}
		}

		if (nullptr != paParameters) {
			if (false == paParameters->empty()) {
				HC_MSet_Vertex_Parameters(nShellKey, 0, static_cast<int>(paParameters->size() / 2), 2, paParameters->data());
			}
		}

// 		if(false == aColors.empty()) {
// 			HC_MSet_Vertex_Colors_By_Value(nShellKey, "face", 0, "rgb", static_cast<int>(aColors.size()), aColors.data());
// 		}
	}

	SegmentKeyPrivate::LocalClose(*this);

	ShellKey cShell(nShellKey);
	return cShell;
}

//== Edge 관련 함수 ==================================================================================
EdgeAttributeControl H3DF::SegmentKey::GetEdgeAttributeControl()
{
	EdgeAttributeControl cControl(*this);
	return cControl;
}

//== Line 관련 함수 ==================================================================================
LineKey H3DF::SegmentKey::InsertLine(size_t in_count, Point const pcInPoints[])
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Polyline((int) in_count, pcInPoints);
	SegmentKeyPrivate::LocalClose(*this);

	LineKey cLine(nKey);
	return cLine;
}

LineAttributeControl H3DF::SegmentKey::GetLineAttributeControl()
{
	LineAttributeControl cControl(*this);
	return cControl;
}

LineAttributeControl const H3DF::SegmentKey::GetLineAttributeControl() const
{
	LineAttributeControl cControl(*(SegmentKey *)this);
	return cControl;
}

//== Circle 관련 함수 ============================================================================
CircleKey H3DF::SegmentKey::InsertCircle(Point const & cInCenter, float fInRadius, Vector const & cInNormal)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Circle_By_Radius(&cInCenter, fInRadius, &cInNormal);
	SegmentKeyPrivate::LocalClose(*this);

	CircleKey cCircle(nKey);
	return cCircle;
}

//== Marker 관련 함수 ================================================================================
MarkerKey H3DF::SegmentKey::InsertMarker(Point const & cInPosition)
{
	return InsertMarker(cInPosition.x, cInPosition.y, cInPosition.z);
}

MarkerKey H3DF::SegmentKey::InsertMarker(double x, double y, double z)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Marker(x, y, z);
	SegmentKeyPrivate::LocalClose(*this);

	MarkerKey cMarker(nKey);
	return cMarker;
}

MarkerAttributeControl H3DF::SegmentKey::GetMarkerAttributeControl()
{
	MarkerAttributeControl cMarkerAttributeControl(KeyValue());
	return cMarkerAttributeControl;
}

MarkerAttributeControl const H3DF::SegmentKey::GetMarkerAttributeControl() const
{
	MarkerAttributeControl cMarkerAttributeControl(KeyValue());
	return cMarkerAttributeControl;
}

//== Material Mapping 관련 함수 ======================================================================
MaterialMappingControl H3DF::SegmentKey::GetMaterialMappingControl()
{
	MaterialMappingControl cMaterialMappingControl(*this);
	return cMaterialMappingControl;
}

MaterialMappingControl const H3DF::SegmentKey::GetMaterialMappingControl() const
{
	MaterialMappingControl cMaterialMappingControl(*(SegmentKey *) this);
	return cMaterialMappingControl;
}

SegmentKey & H3DF::SegmentKey::SetMaterialMapping(H3DF::MaterialMappingKit const & cInKit)
{
	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();

	SegmentKeyPrivate::LocalOpen(*this);

	Material::Type eType = Material::Type::None;
	RGBAColor cRgbaColor;
	CString strTextureName;

	if (true == cInKit.ShowFaceChannel(Material::Channel::DiffuseColor, eType, cRgbaColor, strTextureName)) {
		if (Material::Type::RGBAColor == eType) {
			pcImpl->SetColor(L"faces", cRgbaColor);
		}
	}

	if (true == cInKit.ShowFaceChannel(Material::Channel::DiffuseTexture, eType, cRgbaColor, strTextureName)) {
		if (Material::Type::TextureName == eType) {
			CString strText;
			strText.Format(L"faces = (%s)", strTextureName);
			HC_Set_Color(Utility::ToChar(strText));
		}
		else if (Material::Type::ModulatedTexture == eType) {
		}
	}

	if (true == cInKit.ShowFaceChannel(Material::Channel::Mirror, eType, cRgbaColor, strTextureName)) {
		if (Material::Type::TextureName == eType) {
			CString strText;
			strText.Format(L"faces = (environment = %s, mirror = (r = 0.5 g = 0.5 b = 0.5))", strTextureName);
			HC_Set_Color(Utility::ToChar(strText));
		}
		else if (Material::Type::ModulatedTexture == eType) {
		}
	}

	if (true == cInKit.ShowLineColor(cRgbaColor)) {
		pcImpl->SetColor(L"lines", cRgbaColor);
	}

	if (true == cInKit.ShowMarkerColor(cRgbaColor)) {
		pcImpl->SetColor(L"markers", cRgbaColor);
	}

	if (true == cInKit.ShowTextColor(cRgbaColor)) {
		pcImpl->SetColor(L"text", cRgbaColor);
	}

	if (true == cInKit.ShowVertexChannel(Material::Channel::DiffuseColor, eType, cRgbaColor, strTextureName)) {
		if (Material::Type::RGBAColor == eType) {
			pcImpl->SetColor(L"vertex", cRgbaColor);
		}
	}

	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

//== Performance Control 관련 함수 ===================================================================
SegmentKey & H3DF::SegmentKey::SetPerformance(PerformanceKit const & cInKit)
{
	PerformanceControl cPerformanceControl(*this);

	Performance::DisplayLists eDisplayList;
	if (true == cInKit.ShowDisplayLists(eDisplayList)) {
		cPerformanceControl.SetDisplayLists(eDisplayList);
	}

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetPerformance()
{
	return *this;
}

bool H3DF::SegmentKey::ShowPerformance(PerformanceKit & cOutKit) const
{
	return true;
}

PerformanceControl H3DF::SegmentKey::GetPerformanceControl()
{
	PerformanceControl cPerformanceControl(*this);
	return cPerformanceControl;
}

PerformanceControl const H3DF::SegmentKey::GetPerformanceControl() const
{
	PerformanceControl cPerformanceControl(*(SegmentKey *) this);
	return cPerformanceControl;
}

//== Visibility Control 관련 함수 ====================================================================
VisibilityControl H3DF::SegmentKey::GetVisibilityControl()
{
	VisibilityControl cVisibilityControl(*this);
	return cVisibilityControl;
}

VisibilityControl const H3DF::SegmentKey::GetVisibilityControl() const
{
	VisibilityControl cVisibilityControl(*(SegmentKey *)this);
	return cVisibilityControl;
}

SegmentKey & H3DF::SegmentKey::SetVisibility(CString strList)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Visibility(Utility::ToChar(strList));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Visual Effects 관련 함수 ========================================================================
VisualEffectsControl H3DF::SegmentKey::GetVisualEffectsControl()
{
	VisualEffectsControl cVisibilityControl(*this);
	return cVisibilityControl;
}

VisualEffectsControl const H3DF::SegmentKey::GetVisualEffectsControl() const
{
	VisualEffectsControl cVisibilityControl(*(SegmentKey *)this);
	return cVisibilityControl;
}

//== Condition 관련 함수 =============================================================================
SegmentKey & H3DF::SegmentKey::SetCondition(CString strInCondition)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Conditions(Utility::ToChar(strInCondition));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Heuristics 관련 함수 ============================================================================
SegmentKey & H3DF::SegmentKey::SetHeuristics(CString strInHeuristics)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Heuristics(Utility::ToChar(strInHeuristics));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Portfolio Control 관련 함수 =====================================================================
/*
PortfolioControl H3DF::SegmentKey::GetPortfolioControl()
{
	return m_cPortfolioControl;
}

SelectabilityControl const H3DF::SegmentKey::GetPortfolioControl() const
{
	return m_cPortfolioControl;
}
*/

//== StyleControl Control 관련 함수 ==================================================================
StyleControl H3DF::SegmentKey::GetStyleControl()
{
	StyleControl cStyleControl(*this);
	return cStyleControl;
}

//== Condition 관련 함수 =============================================================================
// Sets chInCondition as the only condition on this segment, replacing any existing conditions.
SegmentKey & H3DF::SegmentKey::SetCondition(char const * chInCondition)
{
	if (nullptr != chInCondition) {
		SegmentKeyPrivate::LocalOpen(*this); {
			HC_Set_Conditions(chInCondition);
		}SegmentKeyPrivate::LocalClose(*this);
	}

	return *this;
}

// Sets astrInConditions as the only conditions on this segment, replacing any existing conditions.
SegmentKey & H3DF::SegmentKey::SetConditions(AStringArray const & astrInConditions)
{
	if (false == astrInConditions.empty()) {
		CStringA strConditions;

		for (size_t nIndex = 0; nIndex < astrInConditions.size(); ++nIndex) {
			if (0 < nIndex) {
				strConditions += ", ";
			}
			strConditions += astrInConditions[nIndex];
		}

		SegmentKeyPrivate::LocalOpen(*this); {
			HC_Set_Conditions(strConditions);
		}SegmentKeyPrivate::LocalClose(*this);
	}
	return *this;
}

SegmentKey & H3DF::SegmentKey::SetConditions(size_t nInCount, CStringA const pchInConditions[])
{
	CStringA strConditions;

	for (size_t nIndex = 0; nIndex < nInCount; ++nIndex) {
		if (0 < nIndex) {
			strConditions += ", ";
		}
		strConditions += pchInConditions[nIndex];
	}

	SegmentKeyPrivate::LocalOpen(*this); {
		HC_Set_Conditions(strConditions);
	}SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

void H3DF::SegmentKey::SetRenderingOptions(CString strList)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Rendering_Options(Utility::ToChar(strList));
	SegmentKeyPrivate::LocalClose(*this);
}

void H3DF::SegmentKey::SetColorByIndex(CString strList, int nIndex)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Color_By_Index(Utility::ToChar(strList), nIndex);
	SegmentKeyPrivate::LocalClose(*this);
}

void H3DF::SegmentKey::SetMarkerSymbol(CString strSymbol)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Marker_Symbol(Utility::ToChar(strSymbol));
	SegmentKeyPrivate::LocalClose(*this);
}

//== Camera 관련 함수 ================================================================================
// 	SegmentKey & SetCamera(CameraKit const & cInKit);
// 	SegmentKey & UnsetCamera();

bool H3DF::SegmentKey::ShowCamera(CameraKit & cOutKit) const
{
	SegmentKeyPrivate::LocalOpen(*this);

	Point cPosition;
	Point cTarget;
	Vector cUpVector;
	float fWidth, fHeight;
	char chProjecionType[MVO_BUFFER_SIZE];

	HC_Show_Net_Camera(&cPosition, &cTarget, &cUpVector, &fWidth, &fHeight, chProjecionType);

	cOutKit.SetUpVector(cUpVector);
	cOutKit.SetPosition(cPosition);
	cOutKit.SetTarget(cTarget);

	Camera::Projection eType = Camera::Projection::Default;
	if (0 == strieq(chProjecionType, "Perspective")) {
		eType = Camera::Projection::Perspective;
	}
	else if (0 == strieq(chProjecionType, "Orthographic")) {
		eType = Camera::Projection::Orthographic;
	}
	else if (0 == strieq(chProjecionType, "Stretched")) {
		eType = Camera::Projection::Stretched;
	}
	cOutKit.SetProjection(eType);

	cOutKit.SetField(fWidth, fHeight);

	SegmentKeyPrivate::LocalClose(*this);

	return true;
}

//== Model Segment 관련 함수 =====================================================================
SegmentKey & H3DF::SegmentKey::SetModellingMatrix(MatrixKit const & cInKit)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Modelling_Matrix(cInKit.m_fData);
	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetModellingMatrix()
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_UnSet_Modelling_Matrix();
	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

bool H3DF::SegmentKey::ShowModellingMatrix(MatrixKit & cOutKit) const
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Show_Modelling_Matrix(cOutKit.m_fData);
	SegmentKeyPrivate::LocalClose(*this);

	return true;
}

//== Bounding 관련 함수 ==============================================================================
SegmentKey & H3DF::SegmentKey::SetBounding(BoundingKit const & cInKit)
{
	bool bExclusion = false;

	cInKit.ShowExclusion(bExclusion);

	if(true == bExclusion) {
		SegmentKeyPrivate::LocalOpen(*this);
		HC_Set_Heuristics("exclude bounding");
		SegmentKeyPrivate::LocalClose(*this);
	}

	return *this;
}

//== User Data 관련 함수 =============================================================================
SegmentKey & H3DF::SegmentKey::SetUserData(IntPtrTArray const & aInIndices, ByteArrayArray const & aInData)
{
	if (aInIndices.size() == aInData.size()) {
		for (size_t nIndex = 0; nIndex < aInIndices.size(); ++nIndex) {
			SetUserData(aInIndices[nIndex], aInData[nIndex]);
		}
	}

	return *this;
}

SegmentKey & H3DF::SegmentKey::SetUserData(intptr_t nInIndex, size_t nInBytes, BYTE const pnInData[])
{
	SegmentKeyPrivate::LocalOpen(*this);

	HC_Set_User_Data(nInIndex, pnInData, (long)nInBytes);

	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::SetUserData(intptr_t nInIndex, ByteArray const & aInData)
{
	SegmentKeyPrivate::LocalOpen(*this);

	HC_Set_User_Data(nInIndex, aInData.data(), (long)aInData.size());

	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetUserData(intptr_t nInIndex)
{
	SegmentKeyPrivate::LocalOpen(*this);

	HC_UnSet_One_User_Data(nInIndex);

	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetUserData(size_t nInCount, intptr_t const pnInIndices[])
{
	SegmentKeyPrivate::LocalOpen(*this);

	for (size_t nIndex = 0; nIndex < nInCount; ++nIndex) {
		HC_UnSet_One_User_Data(pnInIndices[nIndex]);
	}

	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetUserData(IntPtrTArray const & pnInIndices)
{
	SegmentKeyPrivate::LocalOpen(*this);

	for (size_t nIndex = 0; nIndex < pnInIndices.size(); ++nIndex) {
		HC_UnSet_One_User_Data(pnInIndices[nIndex]);
	}

	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetAllUserData()
{
	SegmentKeyPrivate::LocalOpen(*this);

	HC_UnSet_User_Data();

	SegmentKeyPrivate::LocalClose(*this);

	return *this;

}

size_t H3DF::SegmentKey::ShowUserDataCount() const
{
	SegmentKeyPrivate::LocalOpen(*this);

	size_t nCount = abs(HC_Show_User_Data_Indices(nullptr, 0));

	SegmentKeyPrivate::LocalClose(*this);

	return nCount;
}

bool H3DF::SegmentKey::ShowUserData(IntPtrTArray & aOutIndices, ByteArrayArray & aOutData) const
{
	SegmentKeyPrivate::LocalOpen(*this);

	long nSize = HC_Show_User_Data_Indices(nullptr, 0);
	nSize = abs(nSize);
	aOutIndices.resize(nSize);

	nSize = HC_Show_User_Data_Indices(aOutIndices.data(), nSize);
	if (0 == nSize) {
		return false;
	}

	aOutData.resize(nSize);

	for (size_t nIndex = 0; nIndex < aOutIndices.size(); ++nIndex) {
		long nBytes = HC_Show_One_User_Data(aOutIndices[nIndex], nullptr, 0);
		nBytes = abs(nBytes);
		aOutData[nIndex].resize(nBytes);

		HC_Show_One_User_Data(aOutIndices[nIndex], aOutData[nIndex].data(), nBytes);
	}

	SegmentKeyPrivate::LocalClose(*this);

	return true;
}

bool H3DF::SegmentKey::ShowUserDataIndices(IntPtrTArray & aOutIndices) const
{	
	SegmentKeyPrivate::LocalOpen(*this);

	long nSize = HC_Show_User_Data_Indices(nullptr, 0);
	nSize = abs(nSize);
	aOutIndices.resize(nSize);

	nSize = HC_Show_User_Data_Indices(aOutIndices.data(), nSize);
	if (0 == nSize) {
		return false;
	}

	SegmentKeyPrivate::LocalClose(*this);

	return true;
}

bool H3DF::SegmentKey::ShowUserData(intptr_t nInIndex, ByteArray & aOutData) const
{
	SegmentKeyPrivate::LocalOpen(*this);

	long nSize = HC_Show_One_User_Data(nInIndex, nullptr, 0);
	nSize = abs(nSize);

	aOutData.resize(nSize);

	nSize = HC_Show_One_User_Data(nInIndex, aOutData.data(), nSize);
	if (0 == nSize) {
		return false;
	}

	SegmentKeyPrivate::LocalClose(*this);

	return true;
}