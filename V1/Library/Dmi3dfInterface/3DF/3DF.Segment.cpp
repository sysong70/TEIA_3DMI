#include "StdAfx.h"

#include <HTools.h>

#include "3DF.Database.h"
#include "3DF.Segment.h"
#include "3DF.Selectability.h"
#include "3DF.Bounding.h"
#include "3DF.Line.h"

USING_3DF_NAMESPACE

SegmentKey::SegmentKey() :
	m_cSelectabilityControl(*this)
{
}

SegmentKey::SegmentKey(CString strInName) :
	m_cSelectabilityControl(*this)
{
	if(false == strInName.IsEmpty()) {
		m_nKey = HC_Create_Segment(H_ASCII_TEXT(strInName));
	}
	else {
		m_nKey = HC_Create_Segment(nullptr);
	}
}

SegmentKey::SegmentKey(HC_KEY nKey) :
	m_cSelectabilityControl(*this)
{
	m_nKey = nKey;
}

SegmentKey::SegmentKey(SegmentKey const & cInThat) :
	m_cSelectabilityControl(*this)
{
	m_nKey = cInThat.KeyValue();
	//m_cSelectabilityControl = cInThat.GetSelectabilityControl();
}

SegmentKey::~SegmentKey()
{
}

SegmentKey & SegmentKey::operator = (SegmentKey const & cOther)
{
	m_nKey = cOther.KeyValue();

	m_nModelIncludeKey = cOther.ModelInclude().KeyValue();
	m_nStylesIncludeKey = cOther.StylesInclude().KeyValue();

	return *this;
}

SegmentKey SegmentKey::Subsegment() const
{
	CString strText;
	Open();
	SegmentKey cSubsegment(strText);
	Close();

	return cSubsegment;
}

SegmentKey SegmentKey::Subsegment(LPCTSTR chFormat, ...) const
{
	CString strText;
	va_list argList;

	va_start(argList, chFormat);
	strText.FormatV(chFormat, argList);
	va_end(argList);

	Open();
	SegmentKey cSubsegment(strText);
	Close();

	return cSubsegment;
}

CString SegmentKey::Name() const
{
	CString strOutName;

	char chSegName[MVO_BUFFER_SIZE];
	HC_Show_Segment(m_nKey, chSegName);

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
	_3DF::PointArray acPoints;
	_3DF::VectorArray acNormals;
	_3DF::IntArray acFacelist;

	cInKit.ShowPoints(acPoints);
	cInKit.ShowNormals(acNormals);
	cInKit.ShowFacelist(acFacelist);

	Open();
	HC_KEY nShellKey = HC_Insert_Shell(static_cast<int>(acPoints.size()), acPoints.data(), static_cast<int>(acFacelist.size()), acFacelist.data());

	if(INVALID_KEY != nShellKey) {
		if(false == acNormals.empty()) {
			HC_MSet_Vertex_Normals(nShellKey, 0, static_cast<int>(acNormals.size()), acNormals.data());
		}
	}

	Close();

	ShellKey cShell(nShellKey);
	return cShell;
}

//== Line 관련 함수 ==============================================================================
LineKey SegmentKey::InsertLine(size_t in_count, Point const pcInPoints[])
{
	Open();
	HC_KEY nKey = HC_Insert_Polyline(in_count, pcInPoints);
	Close();

	LineKey cLine(nKey);
	return cLine;
}

//== Material Mapping 관련 함수 ==================================================================
SegmentKey & SegmentKey::SetMaterialMapping(_3DF::MaterialMappingKit const & cInKit)
{
	Open();
	//----- Color 설정 -----

	//bool MaterialMappingKit::ShowFaceChannel(Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const
	Material::Channel eInChannel = Material::Channel::DiffuseColor;
	Material::Type eType;
	RGBAColor cRgbaColor;
	CString strTextureName;
	float fValue = 0.0f;

	//----- Face 설정 -----
	if(true == cInKit.ShowFaceChannel(Material::Channel::DiffuseColor, eType, cRgbaColor, strTextureName, fValue)) {
		CString strColorText;
		strColorText.Format(L"faces = (diffuse = (r=%f g=%f b=%f))", cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	if(true == cInKit.ShowFaceChannel(Material::Channel::Specular, eType, cRgbaColor, strTextureName, fValue)) {
		CString strColorText;
		strColorText.Format(L"faces = (specular = (r=%f g=%f b=%f))", cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	if(true == cInKit.ShowFaceChannel(Material::Channel::Emission, eType, cRgbaColor, strTextureName, fValue)) {
		CString strColorText;
		strColorText.Format(L"faces = (emission = (r=%f g=%f b=%f))", cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	//----- Line 설정 -----
	if(true == cInKit.ShowLineChannel(Material::Channel::DiffuseColor, eType, cRgbaColor, strTextureName, fValue)) {
		CString strColorText;
		strColorText.Format(L"lines = (diffuse = (r=%f g=%f b=%f))", cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
		HC_Set_Color(H_ASCII_TEXT(strColorText));
	}

	Close();

	return *this;
}

//== Control 관련 함수 ===============================================================================
SelectabilityControl SegmentKey::GetSelectabilityControl()
{
	return m_cSelectabilityControl;
}

SelectabilityControl const SegmentKey::GetSelectabilityControl() const
{
	return m_cSelectabilityControl;

/*
	if(nullptr != m_pcSelectabilityControl) {
		return *m_pcSelectabilityControl;
	}

	m_pcSelectabilityControl = new SelectabilityControl(*this);
	if(nullptr == m_pcSelectabilityControl) {
		assert(false);
	}

	return *m_pcSelectabilityControl;
*/
}

SegmentKey & SegmentKey::SetSelectability(CString strList)
{
	Open();
	HC_Set_Selectability(H_ASCII_TEXT(strList));
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

void SegmentKey::SetVisibility(CString strList)
{
	Open();
	HC_Set_Visibility(H_ASCII_TEXT(strList));
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
// 	_3DF::PortfolioKey cPortfolio(cPortfolios.KeyValue());
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