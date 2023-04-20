#include "StdAfx.h"

#include <HTools.h>

#include "3DF.Database.h"
#include "3DF.Segment.h"
#include "3DF.Bounding.h"
#include "3DF.Line.h"
#include "3DF.Circle.h"

#include "3DF.Selectability.h"
#include "3DF.Visibility.h"
#include "3DF.MaterialMapping.h"
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

void SegmentKey::Open()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Open();
}

void SegmentKey::Open() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Open();
}

void SegmentKey::Close()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Close();
}

void SegmentKey::Close() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->Close();
}

void SegmentKey::ForcedOpen()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->ForcedOpen();
}

void SegmentKey::ForcedClose()
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	pcImpl->ForcedClose();
}

bool SegmentKey::IsOpen() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	return pcImpl->IsOpen();
}

bool SegmentKey::IsForcedOpen() const
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)m_pcImpl;
	return pcImpl->IsForcedOpen();	
}

//== Sub Segment 관련 함수 ===========================================================================
SegmentKey const SegmentKey::Subsegment()
{
	CString strText;

// 	HC_KEY nKey = HC_Create_Segment_Key_By_Key(KeyValue(), nullptr);
// 
// 	SegmentKey cSubsegment(nKey);

	Open();
	SegmentKey cSubsegment(strText);
	Close();

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

	Open();
	SegmentKey cSubsegment(strText);
	Close();

	return cSubsegment;
}

size_t SegmentKey::ShowSubsegments() const
{
	int nSegmentCount = 0;

	Open();

	HC_Begin_Contents_Search(".", "segments");
	{
		HC_Show_Contents_Count(&nSegmentCount);
	}
	HC_End_Contents_Search();

	Close();

	return nSegmentCount;
}

size_t SegmentKey::ShowSubsegments(SegmentKeyArray & cOutChildren) const
{
	int nSegmentCount = 0;

	Open();

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

	Close();

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
	Open();
	HC_Rename_Segment(".", H_ASCII_TEXT(strInName));
	Close();

	return *this;
}

//== Include 관련 함수 ===============================================================================
IncludeKey SegmentKey::IncludeSegment(SegmentKey const & cInSegment)
{
	Open();
	HC_KEY nIncludeKey = HC_Include_Segment_By_Key(cInSegment.KeyValue());
	IncludeKey cInclude(nIncludeKey);
	Close();

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

	Open();

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

	Close();

	ShellKey cShell(nShellKey);
	return cShell;
}

//== Line 관련 함수 ==================================================================================
LineKey SegmentKey::InsertLine(size_t in_count, Point const pcInPoints[])
{
	Open();
	HC_KEY nKey = HC_Insert_Polyline((int) in_count, pcInPoints);
	Close();

	LineKey cLine(nKey);
	return cLine;
}

//== Circle 관련 함수 ============================================================================
CircleKey SegmentKey::InsertCircle(Point const & cInCenter, float fInRadius, Vector const & cInNormal)
{
	Open();
	HC_KEY nKey = HC_Insert_Circle_By_Radius(&cInCenter, fInRadius, &cInNormal);
	Close();

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
	Open();
	HC_KEY nKey = HC_Insert_Marker(x, y, z);
	Close();

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

SegmentKey & SegmentKey::SetMaterialMapping(CString strGeometry, TDF::MaterialMappingKit const & cInKit)
{
	Open();

	//bool MaterialMappingKit::ShowFaceChannel(Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const
	Material::Channel eInChannel = Material::Channel::DiffuseColor;
	RGBAColor cRgbaColor;

	if (true == cInKit.ShowColor(Material::Color::Type::Diffuse, cRgbaColor)) {
		CString strColorText;

		if (1.0f == cRgbaColor.alpha) {
			strColorText.Format(L"%s = (diffuse = (r=%f g=%f b=%f))", strGeometry, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		}
		else {
			float fTransparency = 1.0f - cRgbaColor.alpha;
			strColorText.Format(L"%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strGeometry, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue, fTransparency, fTransparency, fTransparency);
		}

		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	if (true == cInKit.ShowColor(Material::Color::Type::Specular, cRgbaColor)) {
		CString strColorText;
		if (1.0f == cRgbaColor.alpha) {
			strColorText.Format(L"%s = (specular = (r=%f g=%f b=%f))", strGeometry, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		}
		else {
			float fTransparency = 1.0f - cRgbaColor.alpha;
			strColorText.Format(L"%s = (specular = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strGeometry, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue, fTransparency, fTransparency, fTransparency);
		}

		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	if (true == cInKit.ShowColor(Material::Color::Type::Emission, cRgbaColor)) {
		CString strColorText;
		if (1.0f == cRgbaColor.alpha) {
			strColorText.Format(L"%s = (emission = (r=%f g=%f b=%f))", strGeometry, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		}
		else {
			float fTransparency = 1.0f - cRgbaColor.alpha;
			strColorText.Format(L"%s = (emission = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strGeometry, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue, fTransparency, fTransparency, fTransparency);
		}

		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	if (false == cInKit.TextureName().IsEmpty()) {
// 		RGBAColor cRgbaColor;
// 		CString strColorText;
// 		cInKit.ShowColor(Material::Color::Type::Diffuse, cRgbaColor);

		if (true == cInKit.TextureMirror()) {
			CString strText;
			strText.Format(L"faces = (environment = %s, mirror = (r = 0.5 g = 0.5 b = 0.5))", cInKit.TextureName());
			HC_Set_Color(H_ASCII_TEXT(strText));
		}
		else {
			CString strText;
			strText.Format(L"faces = (%s)", cInKit.TextureName());

			HC_Set_Color(H_ASCII_TEXT(strText));
		}

		if (false == cInKit.TextureOption().IsEmpty()) {
			CString strTextureName = cInKit.TextureName();
			CString strTextureOption = cInKit.TextureOption();
			HC_Define_Local_Texture(H_ASCII_TEXT(strTextureName), H_ASCII_TEXT(strTextureOption));
		}
	}

	Close();

	/*
		char chBuffer[MVO_BUFFER_SIZE];
		Open();
		HC_Show_Color(chBuffer);
		Close();
	*/

	return *this;
}

SegmentKey & SegmentKey::SetTextureMapping(CString strGeometry, TDF::MaterialMappingKit const & cInKit)
{
	Open();

	if (false == cInKit.TextureName().IsEmpty()) {
		RGBAColor cRgbaColor;
		CString strColorText;
		cInKit.ShowColor(Material::Color::Type::Diffuse, cRgbaColor);

		if (true == cInKit.TextureMirror()) {
			CString strText;
			strText.Format(L"faces = environment = %s, mirror = (r = 0.5 g = 0.5 b = 0.5))", cInKit.TextureName());
			HC_Set_Color(H_ASCII_TEXT(strText));
		}
		else {
			CString strText;
			strText.Format(L"faces = (%s)", cInKit.TextureName());

			HC_Set_Color(H_ASCII_TEXT(strText));
		}

		if (false == cInKit.TextureOption().IsEmpty()) {
			CString strTextureName = cInKit.TextureName();
			CString strTextureOption = cInKit.TextureOption();
			HC_Define_Local_Texture(H_ASCII_TEXT(strTextureName), H_ASCII_TEXT(strTextureOption));

// 			char chName[MVO_BUFFER_SIZE];
// 			char chOption[MVO_BUFFER_SIZE];
// 			HC_Show_Local_Texture(chName, chOption);
		}
	}

	Close();

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
	Open();
	HC_Set_Selectability(H_ASCII_TEXT(strList));
	Close();
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
	Open();
	HC_Set_Visibility(H_ASCII_TEXT(strList));
	Close();
	return *this;
}

//== Condition 관련 함수 =============================================================================
SegmentKey & SegmentKey::SetCondition(CString strInCondition)
{
	Open();
	HC_Set_Conditions(H_ASCII_TEXT(strInCondition));
	Close();
	return *this;
}

//== Heuristics 관련 함수 ============================================================================
SegmentKey & SegmentKey::SetHeuristics(CString strInHeuristics)
{
	Open();
	HC_Set_Heuristics(H_ASCII_TEXT(strInHeuristics));
	Close();
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
	Open();
	HC_Set_Rendering_Options(H_ASCII_TEXT(strList));
	Close();
}

void SegmentKey::SetColorByIndex(CString strList, int nIndex)
{
	Open();
	HC_Set_Color_By_Index(H_ASCII_TEXT(strList), nIndex);
	Close();
}

void SegmentKey::SetMarkerSymbol(CString strSymbol)
{
	Open();
	HC_Set_Marker_Symbol(H_ASCII_TEXT(strSymbol));
	Close();
}

//== Camera 관련 함수 ================================================================================
// 	SegmentKey & SetCamera(CameraKit const & cInKit);
// 	SegmentKey & UnsetCamera();

bool SegmentKey::ShowCamera(CameraKit & cOutKit) const
{
	Open();

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

	Close();

	return true;
}

//== Model Segment 관련 함수 =====================================================================

// Model용 Segment를 구성한다.
void SegmentKey::ConfigureSegmentModel()
{
	//---- model include 구성 -----
	// Model이 들아갈 Include 구성을 한다.
	// 이 영역에, Modeling data와 Style data를 구성한다.
	Open();

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

	Close();

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
	Open();
	HC_Set_Modelling_Matrix(cInKit.m_fData);
	Close();

	return *this;
}

SegmentKey & SegmentKey::SegmentKey::UnsetModellingMatrix()
{
	Open();
	HC_UnSet_Modelling_Matrix();
	Close();

	return *this;
}

bool SegmentKey::ShowModellingMatrix(MatrixKit & cOutKit) const
{
	Open();
	HC_Show_Modelling_Matrix(cOutKit.m_fData);
	Close();

	return true;
}


SegmentKey & SegmentKey::SetBounding(BoundingKit const & cInKit)
{
	bool bExclusion = false;

	cInKit.ShowExclusion(bExclusion);

	if(true == bExclusion) {
		Open();
		HC_Set_Heuristics("exclude bounding");
		Close();
	}

	return *this;
}