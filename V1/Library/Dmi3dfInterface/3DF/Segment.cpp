#include "StdAfx.h"

#include <HTools.h>

#include "Database.h"
#include "Segment.h"
#include "Bounding.h"
#include "Reference.h"
#include "Line.h"
#include "Circle.h"
#include "CuttingSection.h"

#include "Selectability.h"
#include "Visibility.h"
#include "VisualEffects.h"
#include "Material.h"
#include "MarkerAttribute.h"
#include "LineAttribute.h"
#include "Performance.h"
#include "AttributeLock.h"
#include "Condition.h"
#include "DrawingAttribute.h"
#include "ColorInterpolation.h"
#include "Culling.h"
#include "Portfolio.h"
#include "TextAttribute.h"

#include "Camera.h"

#include "3DF.Utility.h"

#include "./Impl/SegmentImpl.h"
#include "./Impl/SearchImpl.h"

using namespace H3DF;

H3DF::SegmentKey::SegmentKey()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<SegmentKeyImpl>();
}

H3DF::SegmentKey::SegmentKey(CStringA strInName)
{
	if (staticType != Type()) {
		return;
	}

	HC_KEY nKey = INVALID_KEY;

	if(false == strInName.IsEmpty()) {
		nKey = HC_Create_Segment(strInName);
	}
	else {
		nKey = HC_Create_Segment(nullptr);
	}

	if (INVALID_KEY == nKey) {
		DEBUG_STOP;
	}
	
	m_pcImpl = std::make_unique<SegmentKeyImpl>();
	static_cast<SegmentKeyImpl *>(m_pcImpl.get())->SetKeyValue(nKey);
}

H3DF::SegmentKey::SegmentKey(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	if (INVALID_KEY == nInKey) {
		return;
	}

	m_pcImpl = std::make_unique<SegmentKeyImpl>();
	static_cast<SegmentKeyImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);
}

H3DF::SegmentKey::SegmentKey(SegmentKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::SegmentKey::~SegmentKey()
{
/*
	if (nullptr != pcImpl->m_pcBoundingKit) {
		delete pcImpl->m_pcBoundingKit;
		pcImpl->m_pcBoundingKit = nullptr;
	}
*/
}

SegmentKey & H3DF::SegmentKey::operator = (SegmentKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

//== Segment 관련 함수 ===============================================================================

SegmentKey & H3DF::SegmentKey::Open()
{
	auto pcImpl = dynamic_cast<SegmentKeyImpl *>(m_pcImpl.get());
	if (nullptr != pcImpl) {
		pcImpl->Open();
	}
	else {
		DEBUG_STOP;
	}

	return *this;
}

SegmentKey & H3DF::SegmentKey::Close()
{
	auto pcImpl = dynamic_cast<SegmentKeyImpl *>(m_pcImpl.get());
	if (nullptr != pcImpl) {
		pcImpl->Close();
	}
	else {
		DEBUG_STOP;
	}

	return *this;
}

CStringA H3DF::SegmentKey::Name(bool bIncludePath) const
{
	CStringA strOutName;

	char chSegName[MVO_BUFFER_SIZE] = "\n";
	HC_Show_Segment(KeyValue(), chSegName);

	if (false == bIncludePath) {
		HC_Parse_String(chSegName, "/", -1, chSegName);
	}

	strOutName = chSegName;

	return strOutName;
}

SegmentKey & H3DF::SegmentKey::SetName(CStringA strInName)
{
	SegmentKeyImpl::LocalOpen(*this); {
		HC_Rename_Segment(".", strInName);
	} SegmentKeyImpl::LocalClose(*this);

	return *this;
}

//== Sub Segment 관련 함수 ===========================================================================
SegmentKey const H3DF::SegmentKey::Subsegment()
{
	SegmentKeyImpl::LocalOpen(*this);
	SegmentKey cSubsegment(L"");
	SegmentKeyImpl::LocalClose(*this);

	return cSubsegment;
}

SegmentKey const H3DF::SegmentKey::Subsegment(LPCSTR chFormat, ...)
{
	CStringA strText;
	va_list argList;

	va_start(argList, chFormat);
	strText.FormatV(chFormat, argList);
	va_end(argList);

	SegmentKeyImpl::LocalOpen(*this);
	SegmentKey cSubsegment(strText);
	SegmentKeyImpl::LocalClose(*this);

	return cSubsegment;
}

size_t H3DF::SegmentKey::ShowSubsegments() const
{
	int nSegmentCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "segments"); {
		HC_Show_Contents_Count(&nSegmentCount);
	} HC_End_Contents_Search();

	SegmentKeyImpl::LocalClose(*this);

	return nSegmentCount;
}

size_t H3DF::SegmentKey::ShowSubsegments(SegmentKeyArray & cOutChildren) const
{
	int nSegmentCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

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
			cOutChildren.emplace_back(nChildSegkey);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyImpl::LocalClose(*this);

	return nSegmentCount;
}

//== Flush 관련 함수 =============================================================================
void H3DF::SegmentKey::Flush(Search::Type eInTypeToRemove, Search::Space eInSearchSpace)
{
	CString strType = SearchImpl::GetSearchTypeString(eInTypeToRemove);
	CString strSearchSpace = SearchImpl::GetSearchSpaceString(eInSearchSpace);

	SegmentKeyImpl::LocalOpen(*this); {
		HC_Flush_Contents(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));
	} SegmentKeyImpl::LocalClose(*this);
}

void H3DF::SegmentKey::Flush(SearchTypeArray const & aInTypesToRemove, Search::Space eInSearchSpace)
{
	SegmentKeyImpl::LocalOpen(*this);

	CString strType;
	
	for (auto eType : aInTypesToRemove)
	{
		if (false == strType.IsEmpty()) {
			strType += ", ";
		}

		strType += SearchImpl::GetSearchTypeString(eType);
	}

	CString strSearchSpace = SearchImpl::GetSearchSpaceString(eInSearchSpace);

	HC_Flush_Contents(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));

	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::SegmentKey::Flush(size_t nInTypesCount, Search::Type const peInTypesToRemove[], Search::Space eInSearchSpace)
{
	SegmentKeyImpl::LocalOpen(*this); {

		CString strType;

		for (size_t nIndex = 0; nIndex < nInTypesCount; nIndex++) {
			if (false == strType.IsEmpty()) {
				strType += ", ";
			}

			strType += SearchImpl::GetSearchTypeString(peInTypesToRemove[nIndex]);
		}

		CString strSearchSpace = SearchImpl::GetSearchSpaceString(eInSearchSpace);

		HC_Flush_Contents(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));

	} SegmentKeyImpl::LocalClose(*this);
}

size_t H3DF::SegmentKey::Find(Search::Type eInRequest, Search::Space eInSearchSpace, SearchResults & cOutResults) const
{
	SegmentKeyImpl::LocalOpen(*this);

	CString strType = SearchImpl::GetSearchTypeString(eInRequest);
	CString strSearchSpace = SearchImpl::GetSearchSpaceString(eInSearchSpace);

	SearchResultsImpl * pcResultsImpl = static_cast<SearchResultsImpl *>(cOutResults.GetImpl());
	DEBUG_VALID(pcResultsImpl);

	HC_Begin_Contents_Search(Utility::ToChar(strSearchSpace), Utility::ToChar(strType));
	{
		int nCount = 0;
		HC_Show_Contents_Count(&nCount);

		HC_KEY nKey;
		char chType[MVO_BUFFER_SIZE];

		for (int nIndex = 0; nIndex < nCount; nIndex++) {
			HC_Find_Contents(chType, &nKey);

			// String을 바탕으로 Key값을 생성
			Key cKey = H3DF::SearchResultsImpl::GetKey(chType, nKey);
			pcResultsImpl->PushBack(cKey);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyImpl::LocalClose(*this);

	return cOutResults.GetCount();
}

//== Include 관련 함수 ===============================================================================
IncludeKey H3DF::SegmentKey::IncludeSegment(SegmentKey const & cInSegment)
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_KEY nIncludeKey = HC_Include_Segment_By_Key(cInSegment.KeyValue());
	SegmentKeyImpl::LocalClose(*this);

	IncludeKey cInclude(nIncludeKey);
	return cInclude;
}

IncludeKey H3DF::SegmentKey::IncludeSegment(SegmentKey const & cInSegment, ConditionalExpression const & cInConditional)
{
	CStringA strCodition;
	cInConditional.ShowCondition(strCodition);

	SegmentKeyImpl::LocalOpen(*this);
	HC_KEY nIncludeKey = HC_Conditional_Include_By_Key(cInSegment.KeyValue(), strCodition);
	SegmentKeyImpl::LocalClose(*this);

	IncludeKey cInclude(nIncludeKey);
	return cInclude;
}

size_t H3DF::SegmentKey::ShowIncluders() const
{
	int nIncludeCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "include"); {
		HC_Show_Contents_Count(&nIncludeCount);
	} HC_End_Contents_Search();

	SegmentKeyImpl::LocalClose(*this);

	return nIncludeCount;
}

size_t H3DF::SegmentKey::ShowIncluders(SegmentKeyArray & aOutSegments) const
{
	int nIncludeCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "include");
	{
		HC_Show_Contents_Count(&nIncludeCount);

		HC_KEY nIncludeKey;
		char chType[MVO_BUFFER_SIZE];
		char chPathName[MVO_BUFFER_SIZE];

		for (int i = 0; i < nIncludeCount; i++)
		{
			HC_Find_Contents(chType, &nIncludeKey);

			HC_KEY nSegmentKey = HC_Show_Include_Segment(nIncludeKey, chPathName);

			SegmentKey cIncludeSegment(nSegmentKey);
			aOutSegments.push_back(cIncludeSegment);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyImpl::LocalClose(*this);

	return nIncludeCount;
}

size_t H3DF::SegmentKey::ShowIncluders(IncludeKeyArray & aOutIncludes) const
{
	int nIncludeCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

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

	SegmentKeyImpl::LocalClose(*this);

	return nIncludeCount;
}

//== Reference 관련 함수 =============================================================================

ReferenceKey H3DF::SegmentKey::ReferenceGeometry(Key const & cInKey)
{
// 	SegmentKeyImpl::LocalOpen(*this);
// 	HC_KEY nReferenceKey = HC_Reference_Geometry_By_Key(cInKey.KeyValue());
// 	SegmentKeyImpl::LocalClose(*this);

	HC_KEY nReferenceKey = HC_Reference_Geometry_Key_By_Key(KeyValue(), cInKey.KeyValue());
	ReferenceKey cReference(nReferenceKey);

	return cReference;
}

/*
size_t H3DF::SegmentKey::ShowReferrers(SegmentKeyArray & aOutSegments) const
{
	SegmentKeyImpl::LocalOpen(*this);

	int nCount = 0;
	HC_Show_Referrers_Count(KeyValue(), &nCount);

	HC_KEY nKey;
	char chType[MVO_BUFFER_SIZE];

	for (int nIndex = 0; nIndex < nCount; nIndex++) {
		HC_Show_Referrer(KeyValue(), nIndex, chType, &nKey);
		SegmentKey cSegment(nKey);
		aOutSegments.push_back(cSegment);
	}

	SegmentKeyImpl::LocalClose(*this);

	return nCount;
}

int FindInSegmentReferencesInternal(HC_KEY startkey, vlist_s * foundlist)
{
	HC_KEY search_key, seg_key;
	char type[64];
	int ret = true;;

	HC_Open_Segment_By_Key(startkey);
	HC_Begin_Contents_Search(".", "segments, includes, reference");
	while (HC_Find_Contents(type, &search_key)) {
		if (streq(type, "include") || streq(type, "segment")) {
			HC_KEY key = search_key;

			if (streq(type, "include")) {
				key = HC_KShow_Include_Segment(search_key);
			}

			if (FindInSegmentReferencesInternal(text, key, keys, keynum, keypos, foundlist) == FIND_RESULT_ERROR_PATH_SIZE)
				ret = FIND_RESULT_ERROR_PATH_SIZE;
			}

		}
		else if (streq(type, "reference")) {
			static char segname[4096];
			seg_key = HC_KShow_Reference_Geometry(search_key);
			HC_Show_Key_Type(seg_key, type);
			if (streq(type, "segment")) {
				HC_Show_Segment(seg_key, segname);
				if (strstr(segname, text)) {
					Keylist * klitem = new Keylist;
					for (int i = 0; i < keypos; i++)
						klitem->keys[i] = keys[i];
					klitem->keys[keypos] = search_key;
					klitem->keynum = keypos + 1;
					vlist_add_last(foundlist, klitem);
				}
			}
		}
	}
	HC_End_Contents_Search();
	HC_Close_Segment();

	return ret;
}
*/
size_t H3DF::SegmentKey::ShowReferrers(ReferenceKeyArray & aOutReferences) const
{
	int nIncludeCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

	HC_Begin_Contents_Search(".", "reference");// reference geometry");
	{
		HC_Show_Contents_Count(&nIncludeCount);

		HC_KEY nKey;
		char chType[MVO_BUFFER_SIZE];

		for (int i = 0; i < nIncludeCount; i++)
		{
			HC_Find_Contents(chType, &nKey);

			ReferenceKey cReference(nKey);
			aOutReferences.push_back(cReference);
		}
	}
	HC_End_Contents_Search();

	SegmentKeyImpl::LocalClose(*this);

	return nIncludeCount;
}

//== Cutting Section 관련 함수 ===============================================================
CuttingSectionKey H3DF::SegmentKey::InsertCuttingSection(Plane const & cInPlane)
{
	HC_KEY nKey = INVALID_KEY;

	SegmentKeyImpl::LocalOpen(*this); {
		nKey = HC_Insert_Cutting_Plane(cInPlane.a, cInPlane.b, cInPlane.c, cInPlane.d);
	} SegmentKeyImpl::LocalClose(*this);

	CuttingSectionKey cCuttingSection(nKey);
	return cCuttingSection;
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

	SegmentKeyImpl::LocalOpen(*this);

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

	SegmentKeyImpl::LocalClose(*this);

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
	SegmentKeyImpl::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Polyline((int) in_count, pcInPoints);
	SegmentKeyImpl::LocalClose(*this);

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
	SegmentKeyImpl::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Circle_By_Radius(&cInCenter, fInRadius, &cInNormal);
	SegmentKeyImpl::LocalClose(*this);

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
	SegmentKeyImpl::LocalOpen(*this);
	HC_KEY nKey = HC_Insert_Marker(x, y, z);
	SegmentKeyImpl::LocalClose(*this);

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
	SegmentKeyImpl * pcImpl = new SegmentKeyImpl();

	SegmentKeyImpl::LocalOpen(*this);

	Material::Type eType = Material::Type::None;
	RGBAColor cRgbaColor;
	CStringA strTextureName, strTextureOptions;

	if (true == cInKit.ShowFaceChannel(Material::Channel::DiffuseColor, eType, cRgbaColor, strTextureName, strTextureOptions)) {
		if (Material::Type::RGBAColor == eType) {
			pcImpl->SetColor(L"faces ", cRgbaColor);
		}
	}

	if (true == cInKit.ShowFaceChannel(Material::Channel::DiffuseTexture, eType, cRgbaColor, strTextureName, strTextureOptions)) {
		if (Material::Type::TextureName == eType) {
			CString strText;
			strText.Format(L"faces = (%s)", strTextureName);
			HC_Set_Color(Utility::ToChar(strText));
		}
		else if (Material::Type::ModulatedTexture == eType) {
			CStringA strText;
			strText.Format("faces = (diffuse = (r=%f g=%f b=%f) %s)", cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue, strTextureName);
			HC_Set_Color(strText);
		}

		if (false == strTextureName.IsEmpty() && false == strTextureOptions.IsEmpty()) {
			HC_Define_Local_Texture(strTextureName, strTextureOptions);
		}
		
/*
		// 말단 노드에 texture를 생성해야 보기 좋게 된다 
		H_FORMAT_TEXT texture_options("source = image %u", tdata.m_uiPictureIndex);

		if (tdata.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection)
		{
			texture_options.Append(", parameterization source = reflection vector");
		}
		else
		{
			texture_options.Append(", parameterization source = uv");
		}

		HC_Define_Local_Texture(H_FORMAT_TEXT("texture_%u", adata.m_uiTextureDefinitionIndex), texture_options);
*/
	}

	if (true == cInKit.ShowFaceChannel(Material::Channel::Mirror, eType, cRgbaColor, strTextureName, strTextureOptions)) {
		if (Material::Type::TextureName == eType) {
			CStringA strText;
			strText.Format("faces = (environment = %s, mirror = (r = 0.5 g = 0.5 b = 0.5))", strTextureName);
			HC_Set_Color(strText);
		}
		else if (Material::Type::ModulatedTexture == eType) {
			CStringA strText;
			strText.Format("faces = (environment = %s, diffuse = (r=%f g=%f b=%f), mirror = (r = 0.5 g = 0.5 b = 0.5))", strTextureName, cRgbaColor.red, cRgbaColor.green, cRgbaColor.blue);
			HC_Set_Color(strText);
		}

		if (false == strTextureName.IsEmpty() && false == strTextureOptions.IsEmpty()) {
			HC_Define_Local_Texture(strTextureName, strTextureOptions);
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

	if (true == cInKit.ShowVertexChannel(Material::Channel::DiffuseColor, eType, cRgbaColor, strTextureName, strTextureOptions)) {
		if (Material::Type::RGBAColor == eType) {
			pcImpl->SetColor(L"vertex", cRgbaColor);
		}
	}

	SegmentKeyImpl::LocalClose(*this);

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

//== Selectability Control 관련 함수 =========================================================
SelectabilityControl H3DF::SegmentKey::GetSelectabilityControl()
{
	SelectabilityControl cSelectabilityControl(*this);
	return cSelectabilityControl;
}

SelectabilityControl const H3DF::SegmentKey::GetSelectabilityControl() const
{
	SelectabilityControl cSelectabilityControl(*(SegmentKey *)this);
	return cSelectabilityControl;
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
	SegmentKeyImpl::LocalOpen(*this); {
		HC_Set_Visibility(Utility::ToChar(strList));
	} SegmentKeyImpl::LocalClose(*this);
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

//== Heuristics 관련 함수 ============================================================================
SegmentKey & H3DF::SegmentKey::SetHeuristics(CString strInHeuristics)
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_Set_Heuristics(Utility::ToChar(strInHeuristics));
	SegmentKeyImpl::LocalClose(*this);
	return *this;
}

//== Portfolio Control 관련 함수 =====================================================================
PortfolioControl H3DF::SegmentKey::GetPortfolioControl()
{
	PortfolioControl cPortfolioControl(*this);
	return cPortfolioControl;
}

PortfolioControl const H3DF::SegmentKey::GetPortfolioControl() const
{
	PortfolioControl cPortfolioControl(*(SegmentKey *) this);
	return cPortfolioControl;
}

//== StyleControl Control 관련 함수 ==================================================================
StyleControl H3DF::SegmentKey::GetStyleControl()
{
	StyleControl cStyleControl(*this);
	return cStyleControl;
}

//== Condition 관련 함수 =============================================================================
// Sets chInCondition as the only condition on this segment, replacing any existing conditions.
SegmentKey & H3DF::SegmentKey::SetCondition(CStringA strInCondition)
{
	if (false == strInCondition.IsEmpty()) {
		SegmentKeyImpl::LocalOpen(*this); {
			HC_Set_Conditions(strInCondition);
		}SegmentKeyImpl::LocalClose(*this);
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

		SegmentKeyImpl::LocalOpen(*this); {
			HC_Set_Conditions(strConditions);
		}SegmentKeyImpl::LocalClose(*this);
	}
	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetConditions()
{
	SegmentKeyImpl::LocalOpen(*this); {
		HC_UnSet_Conditions();
	}SegmentKeyImpl::LocalClose(*this);

	return *this;
}

ConditionControl H3DF::SegmentKey::GetConditionControl()
{
	ConditionControl cControl(*this);
	return cControl;
}

ConditionControl const H3DF::SegmentKey::GetConditionControl() const
{
	ConditionControl cControl(*(SegmentKey *)this);
	return cControl;
}

void H3DF::SegmentKey::SetRenderingOptions(CString strList)
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_Set_Rendering_Options(Utility::ToChar(strList));
	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::SegmentKey::SetColorByIndex(CString strList, int nIndex)
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_Set_Color_By_Index(Utility::ToChar(strList), nIndex);
	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::SegmentKey::SetMarkerSymbol(CString strSymbol)
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_Set_Marker_Symbol(Utility::ToChar(strSymbol));
	SegmentKeyImpl::LocalClose(*this);
}

//== Camera 관련 함수 ================================================================================
SegmentKey & H3DF::SegmentKey::SetCamera(CameraKit const & cInKit)
{
	CameraKit cCameraInfo;
	ShowCamera(cCameraInfo);

	CameraControl cCamerCtrl = GetCameraControl();

	Point cPosition;
	if (true == cInKit.ShowPosition(cPosition)) {
		cCamerCtrl.SetPosition(cPosition);
	}

	Point cTarget;
	if (true == cInKit.ShowTarget(cTarget)) {
		cCamerCtrl.SetTarget(cTarget);
	}

	Vector cUpVector;
	if (true == cInKit.ShowUpVector(cUpVector)) {
		cCamerCtrl.SetUpVector(cUpVector);
	}

	Camera::Projection eType;
	float fOblique_Y_Skew = 0, fOblique_X_Skew = 0;
	if (true == cInKit.ShowProjection(eType, fOblique_X_Skew, fOblique_Y_Skew)) {
		cCamerCtrl.SetProjection(eType, fOblique_X_Skew, fOblique_Y_Skew);
	}

	float fWidth, fHeight;
	if (true == cInKit.ShowField(fWidth, fHeight)) {
		cCamerCtrl.SetField(fWidth, fHeight);
	}

	float fNearLimit;
	if (true == cInKit.ShowNearLimit(fNearLimit)) {
		cCamerCtrl.SetNearLimit(fNearLimit);
	}

	return *this;
}

SegmentKey & H3DF::SegmentKey::SetCamera(MatrixKit & cInMatrix)
{
	CameraKit cCameraInfo;
	ShowCamera(cCameraInfo);

	Point cTarget;
	cCameraInfo.ShowTarget(cTarget);

	Point cPosition;
	cCameraInfo.ShowPosition(cPosition);

	Vector cVector = cPosition - cTarget;
	double dLength = cVector.Length();

	// 카메라의 위치를 설정한다
	// 카메라 위치 설정
	Point cCameraPosition = cPosition;
	// 카메라가 바라보는 방향 설정. Matrix Z축의 반대 방향으로 설정한다.
	Point cCameraTarget = cPosition - cInMatrix.ZAxis() * dLength;

	CameraControl cCamerCtrl = GetCameraControl();

	cCamerCtrl.SetTarget(cCameraTarget);
	cCamerCtrl.SetPosition(cCameraPosition);
	// 화면상에서 위쪽을 가리키는 방향.
	cCamerCtrl.SetUpVector(cInMatrix.YAxis());

	return *this;
}

// 	SegmentKey & UnsetCamera();

bool H3DF::SegmentKey::ShowCamera(CameraKit & cOutKit) const
{
	SegmentKeyImpl::LocalOpen(*this); {

		Point cPosition;
		Point cTarget;
		Vector cUpVector;
		float fWidth, fHeight;
		CStringA strProjecionType;

		HC_Show_Net_Camera(&cPosition, &cTarget, &cUpVector, &fWidth, &fHeight, strProjecionType.GetBuffer(MVO_BUFFER_SIZE));
		strProjecionType.ReleaseBuffer();

		cOutKit.SetUpVector(cUpVector);
		cOutKit.SetPosition(cPosition);
		cOutKit.SetTarget(cTarget);

		Camera::Projection eType = Camera::Projection::Default;
		if ("perspective" == strProjecionType) {
			eType = Camera::Projection::Perspective;
		}
		else if ("orthographic" == strProjecionType) {
			eType = Camera::Projection::Orthographic;
		}
		else if ("stretched" == strProjecionType) {
			eType = Camera::Projection::Stretched;
		}

		cOutKit.SetProjection(eType);
		cOutKit.SetField(fWidth, fHeight);

	} SegmentKeyImpl::LocalClose(*this);

	return true;
}

CameraControl H3DF::SegmentKey::GetCameraControl()
{
	CameraControl cCameraControl(*this);
	return cCameraControl;
}

CameraControl const H3DF::SegmentKey::GetCameraControl() const
{
	CameraControl cCameraControl(*(SegmentKey *)this);
	return cCameraControl;
}

//== Model Segment 관련 함수 =====================================================================
SegmentKey & H3DF::SegmentKey::SetModellingMatrix(MatrixKit const & cInKit)
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_Set_Modelling_Matrix(cInKit.m_fData);
	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetModellingMatrix()
{
	SegmentKeyImpl::LocalOpen(*this);
	HC_UnSet_Modelling_Matrix();
	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

bool H3DF::SegmentKey::ShowModellingMatrix(MatrixKit & cOutKit) const
{
	bool bStatus = false;
	SegmentKeyImpl::LocalOpen(*this); {
		if (0 < HC_Show_Existence("modelling matrix")) {
			HC_Show_Modelling_Matrix(cOutKit.m_fData);
			bStatus = true;
		}
	} SegmentKeyImpl::LocalClose(*this);

	return true;
}

//== Bounding 관련 함수 ==============================================================================
SegmentKey & H3DF::SegmentKey::SetBounding(BoundingKit const & cInKit)
{
	auto pcImpl = dynamic_cast<SegmentKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	// 정의된 BoundingKit이 없으면 새로 생성한다
	if (nullptr == pcImpl->m_pcBoundingKit) {
		pcImpl->m_pcBoundingKit = std::make_unique<BoundingKit>();
	}

	*pcImpl->m_pcBoundingKit = cInKit;

	bool bExclusion = false;
	if (true == cInKit.ShowExclusion(bExclusion)) {
		if (true == bExclusion) {
			SegmentKeyImpl::LocalOpen(*this); 
			HC_Set_Heuristics("exclude bounding");
			SegmentKeyImpl::LocalClose(*this);
		}
	}

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetBounding()
{
	auto pcImpl = dynamic_cast<SegmentKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (nullptr != pcImpl->m_pcBoundingKit) {
		pcImpl->m_pcBoundingKit.reset();
	}

	return *this;
}

bool H3DF::SegmentKey::ShowBounding(BoundingKit & cOutkit) const
{
	auto pcImpl = dynamic_cast<SegmentKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	// 정의된 BoundingKit이 없으면 새로 생성한다
	if (nullptr == pcImpl->m_pcBoundingKit) {
		pcImpl->m_pcBoundingKit = std::make_unique<BoundingKit>();
	}
	
	SimpleSphere cSphere;
	SimpleCuboid cCuboid;

	if (false == pcImpl->m_pcBoundingKit->ShowVolume(cSphere, cCuboid)) {
		SegmentKeyImpl::LocalOpen(*this); {
			HC_Compute_Circumsphere(".", (HPoint *)&cSphere.center, &cSphere.radius);
			HC_Compute_Circumcuboid(".", (HPoint *)&cCuboid.cMin, (HPoint *)&cCuboid.cMax);
		} SegmentKeyImpl::LocalClose(*this);

		if (false == cSphere.IsValid() || false == cCuboid.IsValid()) {
			return false;
		}

		pcImpl->m_pcBoundingKit->SetVolume(cSphere);
		pcImpl->m_pcBoundingKit->SetVolume(cCuboid);
	}
	
	cOutkit = *pcImpl->m_pcBoundingKit;

	return true;
}

BoundingControl H3DF::SegmentKey::GetBoundingControl()
{
	BoundingControl cControl(*this);
	return cControl;
}

BoundingControl const H3DF::SegmentKey::GetBoundingControl() const
{
	BoundingControl cControl(*(SegmentKey *) this);
	return cControl;
}

//== Attribute Lock 관련 함수 ================================================================
// 		SegmentKey & SetAttributeLock(AttributeLockKit const & in_kit);
// 		SegmentKey & UnsetAttributeLock();
// 		bool ShowAttributeLock(AttributeLockKit & out_kit) const;
AttributeLockControl H3DF::SegmentKey::GetAttributeLockControl()
{
	AttributeLockControl cControl(*this);
	return cControl;
}

AttributeLockControl const H3DF::SegmentKey::GetAttributeLockControl() const
{
	AttributeLockControl cControl(*(SegmentKey *)this);
	return cControl;
}

//== Drawing Attribute 관련 함수 =====================================================================
SegmentKey & H3DF::SegmentKey::SetDrawingAttribute(DrawingAttributeKit const & cInKit)
{
	DrawingAttributeControl cControl(*this);

	float fNear = 0, fFar = 0;
	if (true == cInKit.ShowDepthRange(fNear, fFar)) {
		cControl.SetDepthRange(fNear, fFar);
	}

	int nBuckets = 0;
	if (true == cInKit.ShowFaceDisplacement(nBuckets)) {
	}

	return *this;
}

DrawingAttributeControl H3DF::SegmentKey::GetDrawingAttributeControl()
{
	DrawingAttributeControl cControl(*this);
	return cControl;
}

DrawingAttributeControl const H3DF::SegmentKey::GetDrawingAttributeControl() const
{
	DrawingAttributeControl cControl(*(SegmentKey *) this);
	return cControl;
}

ColorInterpolationControl H3DF::SegmentKey::GetColorInterpolationControl()
{
	ColorInterpolationControl cControl(*this);
	return cControl;
}

ColorInterpolationControl const H3DF::SegmentKey::GetColorInterpolationControl() const
{
	ColorInterpolationControl cControl(*(SegmentKey *) this);
	return cControl;
}

CullingControl H3DF::SegmentKey::GetCullingControl()
{
	CullingControl cControl(*this);
	return cControl;
}

CullingControl const H3DF::SegmentKey::GetCullingControl() const
{
	CullingControl cControl(*(SegmentKey *) this);
	return cControl;
}

//== Priority 관련 함수 ==============================================================================
SegmentKey & H3DF::SegmentKey::SetPriority(int nInPriority)
{
	HC_Set_Priority(KeyValue(), nInPriority);
	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetPriority()
{
	HC_UnSet_Priority(KeyValue());
	return *this;
}

bool H3DF::SegmentKey::ShowPriority(int & nOutPriority) const\
{
	return (bool)HC_Show_Priority(KeyValue(), &nOutPriority);
}

//== Text 관련 함수 ===================================================================================
// TextKey H3DF::SegmentKey::InsertText(Point const & cInPosition, CStringA strInText)
// {
// 
// }

//== TextAttribute 관련 함수 ========================================================================
TextAttributeControl H3DF::SegmentKey::GetTextAttributeControl()
{
	TextAttributeControl cControl(*this);
	return cControl;
}

TextAttributeControl const H3DF::SegmentKey::GetTextAttributeControl() const
{
	TextAttributeControl cControl(*(SegmentKey *) this);
	return cControl;
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
	SegmentKeyImpl::LocalOpen(*this);

	HC_Set_User_Data(nInIndex, pnInData, (long)nInBytes);

	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::SetUserData(intptr_t nInIndex, ByteArray const & aInData)
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_Set_User_Data(nInIndex, aInData.data(), (long)aInData.size());

	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetUserData(intptr_t nInIndex)
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_UnSet_One_User_Data(nInIndex);

	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetUserData(size_t nInCount, intptr_t const pnInIndices[])
{
	SegmentKeyImpl::LocalOpen(*this);

	for (size_t nIndex = 0; nIndex < nInCount; ++nIndex) {
		HC_UnSet_One_User_Data(pnInIndices[nIndex]);
	}

	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetUserData(IntPtrTArray const & pnInIndices)
{
	SegmentKeyImpl::LocalOpen(*this);

	for (size_t nIndex = 0; nIndex < pnInIndices.size(); ++nIndex) {
		HC_UnSet_One_User_Data(pnInIndices[nIndex]);
	}

	SegmentKeyImpl::LocalClose(*this);

	return *this;
}

SegmentKey & H3DF::SegmentKey::UnsetAllUserData()
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_UnSet_User_Data();

	SegmentKeyImpl::LocalClose(*this);

	return *this;

}

size_t H3DF::SegmentKey::ShowUserDataCount() const
{
	SegmentKeyImpl::LocalOpen(*this);

	size_t nCount = abs(HC_Show_User_Data_Indices(nullptr, 0));

	SegmentKeyImpl::LocalClose(*this);

	return nCount;
}

bool H3DF::SegmentKey::ShowUserData(IntPtrTArray & aOutIndices, ByteArrayArray & aOutData) const
{
	SegmentKeyImpl::LocalOpen(*this);

	long nSize = HC_Show_User_Data_Indices(nullptr, 0);

	if (0 != nSize) {

		nSize = abs(nSize);
		aOutIndices.resize(nSize);

		nSize = HC_Show_User_Data_Indices(aOutIndices.data(), nSize);
		if (0 != nSize) {

			aOutData.resize(nSize);

			for (size_t nIndex = 0; nIndex < aOutIndices.size(); ++nIndex) {
				long nBytes = HC_Show_One_User_Data(aOutIndices[nIndex], nullptr, 0);
				nBytes = abs(nBytes);
				aOutData[nIndex].resize(nBytes);

				HC_Show_One_User_Data(aOutIndices[nIndex], aOutData[nIndex].data(), nBytes);
			}
		}
	}

	SegmentKeyImpl::LocalClose(*this);

	return (0 < nSize) ? true : false;
}

bool H3DF::SegmentKey::ShowUserDataIndices(IntPtrTArray & aOutIndices) const
{	
	SegmentKeyImpl::LocalOpen(*this);

	long nSize = HC_Show_User_Data_Indices(nullptr, 0);
	if (0 != nSize) {
		nSize = abs(nSize);
		aOutIndices.resize(nSize);

		nSize = HC_Show_User_Data_Indices(aOutIndices.data(), nSize);
	}

	SegmentKeyImpl::LocalClose(*this);

	return (0 < nSize) ? true : false;
}

bool H3DF::SegmentKey::ShowUserData(intptr_t nInIndex, ByteArray & aOutData) const
{
	SegmentKeyImpl::LocalOpen(*this);

	long nSize = HC_Show_One_User_Data(nInIndex, nullptr, 0);
	if (0 != nSize) {

		nSize = abs(nSize);

		aOutData.resize(nSize);

		nSize = HC_Show_One_User_Data(nInIndex, aOutData.data(), nSize);
	}

	SegmentKeyImpl::LocalClose(*this);

	return (0 < nSize) ? true : false;
}

bool H3DF::SegmentKey::ShowRenderingOptions(CStringA & strList) const
{
	bool bFlag = false;
	SegmentKeyImpl::LocalOpen(*this);

	if (0 < HC_Show_Existence("rendering options")) {
		HC_Show_Rendering_Options(strList.GetBuffer(MVO_BUFFER_SIZE));
		bFlag = true;
	}

	SegmentKeyImpl::LocalClose(*this);

	return bFlag;
}

bool H3DF::SegmentKey::ShowVisibility(CStringA & strList) const
{
	bool bFlag = false;
	SegmentKeyImpl::LocalOpen(*this);
	
	if (0 < HC_Show_Existence("visibility")) {
		HC_Show_Visibility(strList.GetBuffer(MVO_BUFFER_SIZE));
		bFlag = true;
	}

	SegmentKeyImpl::LocalClose(*this);

	return bFlag;
}

bool H3DF::SegmentKey::ShowSelectability(CStringA & strList) const
{
	bool bFlag = false;
	SegmentKeyImpl::LocalOpen(*this);

	if (0 < HC_Show_Existence("selectability")) {
		HC_Show_Selectability(strList.GetBuffer(MVO_BUFFER_SIZE));
		bFlag = true;
	}

	SegmentKeyImpl::LocalClose(*this);

	return bFlag;
}

bool H3DF::SegmentKey::ShowHeuristics(CStringA & strList) const
{
	bool bFlag = false;

	SegmentKeyImpl::LocalOpen(*this);

	if (0 < HC_Show_Existence("heuristics")) {
		HC_Show_Heuristics(strList.GetBuffer(MVO_BUFFER_SIZE));
		bFlag = true;
	}

	SegmentKeyImpl::LocalClose(*this);

	return bFlag;
}

bool H3DF::SegmentKey::ShowDriverOptions(CStringA & strList) const
{
	bool bFlag = false;

	SegmentKeyImpl::LocalOpen(*this);

	if (0 < HC_Show_Existence("driver options")) {

		HC_Show_Driver_Options(strList.GetBuffer(MVO_BUFFER_SIZE));
		bFlag = true;
	}

	SegmentKeyImpl::LocalClose(*this);

	return bFlag;
}