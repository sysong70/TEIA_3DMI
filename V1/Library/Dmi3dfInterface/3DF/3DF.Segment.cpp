#include "StdAfx.h"

#include <HTools.h>

#include "3DF.Database.h"
#include "3DF.Segment.h"
#include "3DF.Bounding.h"
#include "3DF.Line.h"
#include "3DF.Circle.h"

#include "3DF.Selectability.h"
#include "3DF.Visibility.h"
#include "3DF.Material.h"
#include "3DF.MarkerAttribute.h"

#include "3DF.Camera.h"

#include "./Private/3DF.SegmentPrivate.h"

USING_3DF_NAMESPACE



SegmentKey::SegmentKey(CString strInName)
{
	HC_KEY nKey = INVALID_KEY;

	if(false == strInName.IsEmpty()) {
		nKey = HC_Create_Segment(H_ASCII_TEXT(strInName));
	}
	else {
		nKey = HC_Create_Segment(nullptr);
	}

	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();
	pcImpl->SetKeyValue(nKey);
	
	m_pcImpl = pcImpl;
}

SegmentKey::SegmentKey(HC_KEY nInKey)
{
	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

SegmentKey::SegmentKey(SegmentKey const & cInThat)
{
	SegmentKeyPrivate * pcImpl = new SegmentKeyPrivate();
	m_pcImpl = pcImpl;

	Set(cInThat);
}

SegmentKey::~SegmentKey()
{
}

void SegmentKey::Set(SegmentKey const & cInThat)
{
	Key::Set(cInThat);

	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	SegmentKeyPrivate * pcInThatImpl = (SegmentKeyPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

SegmentKey & SegmentKey::operator = (SegmentKey const & cInThat)
{
	Key::Set(cInThat);

	m_nModelIncludeKey = cInThat.ModelInclude().KeyValue();
	m_nStylesIncludeKey = cInThat.StylesInclude().KeyValue();

	return *this;
}

//== Segment 관련 함수 ===============================================================================

/*
void SegmentKey::Open()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalOpen();
}

void SegmentKey::Open() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalOpen();
}

void SegmentKey::Close()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalClose();
}

void SegmentKey::Close() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->LocalClose();
}
*/

void SegmentKey::Open()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Open();
}

void SegmentKey::Close()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Close();
}

//== Sub Segment 관련 함수 ===========================================================================
SegmentKey const SegmentKey::Subsegment()
{
	CString strText;

// 	HC_KEY nKey = HC_Create_Segment_Key_By_Key(KeyValue(), nullptr);
// 
// 	SegmentKey cSubsegment(nKey);

	SegmentKeyPrivate::LocalOpen(*this);
	SegmentKey cSubsegment(strText);
	SegmentKeyPrivate::LocalClose(*this);

	return cSubsegment;
}

SegmentKey const SegmentKey::Subsegment(LPCTSTR chFormat, ...)
{
	CString strText;
	va_list argList;

	va_start(argList, chFormat);
	strText.FormatV(chFormat, argList);
	va_end(argList);

// 	HC_KEY nKey = HC_Create_Segment_Key_By_Key(KeyValue(), H_ASCII_TEXT(strText));
// 
// 	SegmentKey cSubsegment(nKey);

	SegmentKeyPrivate::LocalOpen(*this);
	SegmentKey cSubsegment(strText);
	SegmentKeyPrivate::LocalClose(*this);

	return cSubsegment;
}

size_t SegmentKey::ShowSubsegments() const
{
	int nSegmentCount = 0;

	SegmentKeyPrivate::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "segments");
	{
		HC_Show_Contents_Count(&nSegmentCount);
	}
	HC_End_Contents_Search();

	SegmentKeyPrivate::LocalClose(*this);

	return nSegmentCount;
}

size_t SegmentKey::ShowSubsegments(SegmentKeyArray & cOutChildren) const
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

CString SegmentKey::Name() const
{
	CString strOutName;

	char chSegName[MVO_BUFFER_SIZE];
	HC_Show_Segment(KeyValue(), chSegName);

	strOutName = chSegName;
	
	return strOutName;
}

SegmentKey & SegmentKey::SetName(CString strInName)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Rename_Segment(".", H_ASCII_TEXT(strInName));
	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

//== Include 관련 함수 ===============================================================================
IncludeKey SegmentKey::IncludeSegment(SegmentKey const & cInSegment)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nIncludeKey = HC_Include_Segment_By_Key(cInSegment.KeyValue());
	IncludeKey cInclude(nIncludeKey);
	SegmentKeyPrivate::LocalClose(*this);

	return cInclude;
}

//== Shell 관련 함수 =================================================================================
ShellKey SegmentKey::InsertShell(ShellKit const & cInKit)
{
	TDF::PointArray const * pacPoints = nullptr;
	TDF::VectorArray const * pacNormals = nullptr;
	TDF::IntArray const * pacFacelist = nullptr;
	TDF::FloatArray const * paParameters = nullptr;
	TDF::RGBAColorArray const * paColors = nullptr;

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

//== Line 관련 함수 ==================================================================================
LineKey SegmentKey::InsertLine(size_t in_count, Point const pcInPoints[])
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Polyline((int) in_count, pcInPoints);
	SegmentKeyPrivate::LocalClose(*this);

	LineKey cLine(nKey);
	return cLine;
}

//== Circle 관련 함수 ============================================================================
CircleKey SegmentKey::InsertCircle(Point const & cInCenter, float fInRadius, Vector const & cInNormal)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Circle_By_Radius(&cInCenter, fInRadius, &cInNormal);
	SegmentKeyPrivate::LocalClose(*this);

	CircleKey cCircle(nKey);
	return cCircle;
}

//== Marker 관련 함수 ================================================================================
MarkerKey SegmentKey::InsertMarker(Point const & cInPosition)
{
	return InsertMarker(cInPosition.x, cInPosition.y, cInPosition.z);
}

MarkerKey SegmentKey::InsertMarker(double x, double y, double z)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Marker(x, y, z);
	SegmentKeyPrivate::LocalClose(*this);

	MarkerKey cMarker(nKey);
	return cMarker;
}

MarkerAttributeControl SegmentKey::GetMarkerAttributeControl()
{
	MarkerAttributeControl cMarkerAttributeControl(KeyValue());
	return cMarkerAttributeControl;
}

MarkerAttributeControl const SegmentKey::GetMarkerAttributeControl() const
{
	MarkerAttributeControl cMarkerAttributeControl(KeyValue());
	return cMarkerAttributeControl;
}

//== Material Mapping 관련 함수 ======================================================================
MaterialMappingControl SegmentKey::GetMaterialMappingControl()
{
	MaterialMappingControl cMaterialMappingControl(*this);
	return cMaterialMappingControl;
}

MaterialMappingControl const SegmentKey::GetMaterialMappingControl() const
{
	MaterialMappingControl cMaterialMappingControl(*(SegmentKey *) this);
	return cMaterialMappingControl;
}

SegmentKey & SegmentKey::SetMaterialMapping(TDF::MaterialMappingKit const & cInKit)
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
			HC_Set_Color(H_ASCII_TEXT(strText));
		}
		else if (Material::Type::ModulatedTexture == eType) {
		}
	}

	if (true == cInKit.ShowFaceChannel(Material::Channel::Mirror, eType, cRgbaColor, strTextureName)) {
		if (Material::Type::TextureName == eType) {
			CString strText;
			strText.Format(L"faces = (environment = %s, mirror = (r = 0.5 g = 0.5 b = 0.5))", strTextureName);
			HC_Set_Color(H_ASCII_TEXT(strText));
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

//== Control 관련 함수 ===============================================================================
SelectabilityControl SegmentKey::GetSelectabilityControl()
{
	SelectabilityControl cSelectabilityControl(*this);
	return cSelectabilityControl;
}

SelectabilityControl const SegmentKey::GetSelectabilityControl() const
{
	SelectabilityControl cSelectabilityControl(*(SegmentKey *) this);
	return cSelectabilityControl;
}

SegmentKey & SegmentKey::SetSelectability(CString strList)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Selectability(H_ASCII_TEXT(strList));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Visibility Control 관련 함수 ================================================================
VisibilityControl SegmentKey::GetVisibilityControl()
{
	VisibilityControl cVisibilityControl(*this);
	return cVisibilityControl;
}

VisibilityControl const SegmentKey::GetVisibilityControl() const
{
	VisibilityControl cVisibilityControl(*(SegmentKey *)this);
	return cVisibilityControl;
}

SegmentKey & SegmentKey::SetVisibility(CString strList)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Visibility(H_ASCII_TEXT(strList));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Condition 관련 함수 =============================================================================
SegmentKey & SegmentKey::SetCondition(CString strInCondition)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Conditions(H_ASCII_TEXT(strInCondition));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Heuristics 관련 함수 ============================================================================
SegmentKey & SegmentKey::SetHeuristics(CString strInHeuristics)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Heuristics(H_ASCII_TEXT(strInHeuristics));
	SegmentKeyPrivate::LocalClose(*this);
	return *this;
}

//== Portfolio Control 관련 함수 =====================================================================
/*
PortfolioControl SegmentKey::GetPortfolioControl()
{
	return m_cPortfolioControl;
}

SelectabilityControl const SegmentKey::GetPortfolioControl() const
{
	return m_cPortfolioControl;
}
*/

//== StyleControl Control 관련 함수 ==================================================================
StyleControl SegmentKey::GetStyleControl()
{
	StyleControl cStyleControl(*this);
	return cStyleControl;
}

void SegmentKey::SetRenderingOptions(CString strList)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Rendering_Options(H_ASCII_TEXT(strList));
	SegmentKeyPrivate::LocalClose(*this);
}

void SegmentKey::SetColorByIndex(CString strList, int nIndex)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Color_By_Index(H_ASCII_TEXT(strList), nIndex);
	SegmentKeyPrivate::LocalClose(*this);
}

void SegmentKey::SetMarkerSymbol(CString strSymbol)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Marker_Symbol(H_ASCII_TEXT(strSymbol));
	SegmentKeyPrivate::LocalClose(*this);
}

//== Camera 관련 함수 ================================================================================
// 	SegmentKey & SetCamera(CameraKit const & cInKit);
// 	SegmentKey & UnsetCamera();

bool SegmentKey::ShowCamera(CameraKit & cOutKit) const
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

// Model용 Segment를 구성한다.
void SegmentKey::ConfigureSegmentModel()
{
	//---- model include 구성 -----
	// Model이 들아갈 Include 구성을 한다.
	// 이 영역에, Modeling data와 Style data를 구성한다.
	SegmentKeyPrivate::LocalOpen(*this);

	SegmentKey cModelInclude = Subsegment(L"model_include");
	cModelInclude.SetVisibility(L"off");

	BoundingKit cBounding;
	cBounding.SetExclusion(true);
	cModelInclude.SetBounding(cBounding);

	SegmentKey cIncludeSegment = cModelInclude.Subsegment();
	cIncludeSegment = cIncludeSegment.Subsegment();
	m_nModelIncludeKey = cIncludeSegment.KeyValue();
	m_nStylesIncludeKey = cIncludeSegment.Subsegment(L"styles").KeyValue();

	//SegmentKey cPortfolios = Subsegment(L"portfolios");

// 	m_cPartsIncludeSegment = cIncludeSegment.Subsegment(L"parts");
// 	m_cPoccsIncludeSegment = cIncludeSegment.Subsegment(L"poccs");
// 	m_cRisIncludeSegment = cIncludeSegment.Subsegment(L"ris");

	SegmentKeyPrivate::LocalClose(*this);

	//---- Portfolio 구성 -----
	// 앞으로 사용을 위해서 미리 한개를 만들어 놓는다.
// 	TDF::PortfolioKey cPortfolio(cPortfolios.KeyValue());
// 	m_cPortfolioControl.Push(cPortfolio);

/*
	HC_KEY nPrtfolioKey = HC_Open_Segment("/portfolios");
	HC_Set_Priority(nPrtfolioKey, 0);
	HC_KEY nNewPortfolioKey = HC_Create_Segment("");
	HC_Close_Segment();

	int nCount = 0;
	HC_Open_Segment_By_Key(nRootKey);
		HC_Begin_Contents_Search(".", "segments"); 
		HC_Show_Contents_Count(&nCount);
	HC_Close_Segment();

	PortfolioKey cPortfolioKey(nNewPortfolioKey);
	

	char chModelOwner[MVO_BUFFER_SIZE];
	HC_KEY nModelOwnerKey = HC_Show_Owner_By_Key(m_nKey, chModelOwner);

	char chModelOwner1[MVO_BUFFER_SIZE];
	HC_KEY nModelOwnerKey1 = HC_Show_Owner_By_Key(nModelOwnerKey, chModelOwner1);

	HC_KEY nTestRoot = HC_Open_Segment("/");
	HC_Close_Segment();

	int i = 0;
*/
}

SegmentKey SegmentKey::ModelInclude()
{
	SegmentKey cModelInclude(m_nModelIncludeKey);
	return cModelInclude;
}

SegmentKey SegmentKey::ModelInclude() const
{
	SegmentKey cModelInclude(m_nModelIncludeKey);
	return cModelInclude;
}

SegmentKey SegmentKey::StylesInclude()
{
	SegmentKey cStylesInclude(m_nStylesIncludeKey);
	return cStylesInclude;
}

SegmentKey SegmentKey::StylesInclude() const
{
	SegmentKey cStylesInclude(m_nStylesIncludeKey);
	return cStylesInclude;
}

SegmentKey & SegmentKey::SetModellingMatrix(MatrixKit const & cInKit)
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Set_Modelling_Matrix(cInKit.m_fData);
	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

SegmentKey & SegmentKey::SegmentKey::UnsetModellingMatrix()
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_UnSet_Modelling_Matrix();
	SegmentKeyPrivate::LocalClose(*this);

	return *this;
}

bool SegmentKey::ShowModellingMatrix(MatrixKit & cOutKit) const
{
	SegmentKeyPrivate::LocalOpen(*this);
	HC_Show_Modelling_Matrix(cOutKit.m_fData);
	SegmentKeyPrivate::LocalClose(*this);

	return true;
}


SegmentKey & SegmentKey::SetBounding(BoundingKit const & cInKit)
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