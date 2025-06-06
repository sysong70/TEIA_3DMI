#include "StdAfx.h"

#include "PMI.Entity.h"

#include <HC.h>
#include <HTools.h>

#include "Line.h"
#include "Polygon.h"

#include "Camera.h"
#include "Impl/CameraImpl.h"

#include "Math.Matrix.h"

#include <HTools.h>
#include <utf_utils.h>

#include <WStr.h>

#include "./Impl/SegmentImpl.h"

using namespace H3DF;

namespace H3DF
{
	class PmiKeyImpl : public SegmentKeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<PmiKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const PmiKeyImpl * pcInThat) {
			SegmentKeyImpl::Copy(pcInThat);
		}
	};
}

H3DF::PmiKey::PmiKey(HC_KEY nInKey)
{
// 	if (staticType != Type()) {
// 		return;
// 	}

	if (INVALID_KEY == nInKey) {
		return;
	}

	m_pcImpl = std::make_unique<PmiKeyImpl>();
	static_cast<PmiKeyImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);
}

H3DF::PmiKey::PmiKey(Key const & cInThat)
{
	// 	if (staticType != Type()) {
	// 		return;
	// 	}

	// cInThat이 올바른 Impl(PmiKeyImpl)을 가지고 있으면 복제
	if (cInThat.GetImpl()) {
		// 만약 PmiKeyImpl이 KeyImpl에서 파생된 구조라면 dynamic_cast에 의해서 nullptr이 아닌 정상적인 값이 넘어옴
		auto pcSrcImpl = dynamic_cast<const PmiKeyImpl *>(cInThat.GetImpl());
		if (nullptr != pcSrcImpl) {
			m_pcImpl = pcSrcImpl->Clone();
		}
		else {
			// 타입이 다를 경우 예외 처리 또는 방어적 초기화
			m_pcImpl = std::make_unique<PmiKeyImpl>();
			static_cast<PmiKeyImpl *>(m_pcImpl.get())->Copy((PmiKeyImpl *) (cInThat.GetImpl()));
		}
	}
	else {
		m_pcImpl = std::make_unique<PmiKeyImpl>();
		static_cast<PmiKeyImpl *>(m_pcImpl.get())->Copy((PmiKeyImpl *) (cInThat.GetImpl()));
	}
}

H3DF::PmiKey::PmiKey(PmiKey const & cInThat)
{
// 	if (staticType != Type()) {
// 		return;
// 	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

PmiKey const & H3DF::PmiKey::operator=(PmiKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::PmiKey::GetFrame(Frame & cOutFrame)
{
	char chType[64];
	HC_KEY nKey;
	int nIndex = 0;
	PolylineArray aFramePolylines;

	SegmentKeyImpl::LocalOpen(*this); {
		HC_Open_Segment("frame");
		{
			HC_Begin_Contents_Search("...", "polylines, lines");
			{
				while (HC_Find_Contents(chType, &nKey)) {
					Polyline cPolyline;

					int nCount;
					if (streq(chType, "line")) {
						nCount = 2;
					}
					else {
						HC_Show_Polyline_Count(nKey, &nCount);
					}

					Point * pcPoints = new Point[nCount];
					if (streq(chType, "line")) {
						HC_Show_Line(nKey, &pcPoints[0].x, &pcPoints[0].y, &pcPoints[0].z, &pcPoints[1].x, &pcPoints[1].y, &pcPoints[1].z);
					}
					else {
						HC_Show_Polyline(nKey, &nCount, pcPoints);
					}

					cPolyline.SetPoints(nCount, pcPoints);

					delete[] pcPoints;

					HC_Open_Segment_By_Key(HC_Show_Owner_Original_Key(nKey));
					{
						if (HC_Show_Existence("color = lines")) {
							H3DF::RGBColor cColor;
							char color_space[4] = { "" };
							HC_Show_One_Color_By_Value("lines", color_space, &cColor.red, &cColor.green, &cColor.blue);
							cPolyline.SetRGBColor(cColor);
						}

						if (HC_Show_Existence("line pattern")) {
							char pattern[1024] = { "" };
							HC_Show_Line_Pattern(pattern);
							cPolyline.SetLinePattern(pattern);
						}
					}
					HC_Close_Segment();

					aFramePolylines.push_back(cPolyline);

					nIndex++;
				}
			}
			HC_End_Contents_Search();
		}
		HC_Close_Segment();
	}  SegmentKeyImpl::LocalClose(*this);

	unsigned int cCount = (unsigned int)aFramePolylines.size();
	Polyline * pcPolylines = new Polyline[cCount];
	for (unsigned int ii = 0; ii < cCount; ii++) {
		pcPolylines[ii] = aFramePolylines[ii];
	}

	cOutFrame.SetPolylines(cCount, pcPolylines);

	delete[] pcPolylines;
}

void H3DF::PmiKey::SetFrame(Frame const & cInFrame)
{
	SegmentKeyImpl::LocalOpen(*this);

	H3DF::RGBColor cCurrentColor;
	H3DF::RGBColor cColor;
	char pchPattern[1024] = { "" };
	char pchCurrentPattern[1024] = { "" };

	unsigned int nPolylineCount = cInFrame.GetPolylineCount();
	Polyline * pcPolylines = new Polyline[nPolylineCount];
	cInFrame.GetPolylines(nPolylineCount, pcPolylines);

	HC_Open_Segment("frame");
	{
		HC_Flush_Contents(".", "everything");

		pcPolylines[0].GetRGBColor(cCurrentColor);
		pcPolylines[0].GetLinePattern(pchCurrentPattern);

		HC_Open_Segment("");
		SetColor(pcPolylines[0]);

		for (unsigned int i = 0; i < nPolylineCount; i++)
		{
			//new segment if new graphics

			pcPolylines[i].GetRGBColor(cColor);
			pcPolylines[i].GetLinePattern(pchPattern);
			if (cColor != cCurrentColor
				|| strcmp(pchCurrentPattern, pchPattern) != 0)
			{
				HC_Close_Segment();
				HC_Open_Segment("");
				SetColor(pcPolylines[i]);
				sprintf(pchCurrentPattern, "%s", pchPattern);
				cCurrentColor = cColor;
			}

			unsigned int nPointCount = pcPolylines[i].GetPointCount();
			H3DF::Point * points = new H3DF::Point[nPointCount];
			pcPolylines[i].GetPoints(nPointCount, points);
		
			HC_Insert_Polyline(nPointCount, points);

			delete[] points;
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::PmiKey::GetDrawing(Draw & cOutDrawing) const
{
/*
	cOutDrawing = Draw();

	BasePrivate * my_impl = (BasePrivate *)m_pcImpl;

	unsigned int count = get_polygon_count(pcImpl->m_segment_key, "drawing");
	if (count == 0)
		return;

	PolygonArray polygons(count);
	get_polygons(pcImpl->m_segment_key, "drawing", polygons.data());

	out_drawing.SetPolygons(count, polygons.data());
*/
}

void H3DF::PmiKey::SetDrawing(Draw const & cInDrawing)
{
	unsigned int nCount = cInDrawing.GetPolygonCount();
	
	PolygonArray aPolygons;
	aPolygons.resize(nCount);

	cInDrawing.GetPolygons(nCount, aPolygons.data());

	SetPolygons("drawing", nCount, aPolygons.data());
}

unsigned int H3DF::PmiKey::GetLeaderLineCount() const
{
	SegmentKeyImpl::LocalOpen(*this);

	int count = 0;

	HC_Open_Segment("leader"); {
		HC_Begin_Contents_Search("...", "polylines, lines"); {
			HC_Show_Contents_Count(&count);
		} HC_End_Contents_Search();
	} HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);

	return static_cast<unsigned int>(count);
}

void H3DF::PmiKey::SetLeaderLines(unsigned int in_count, Polyline const * in_leader_lines)
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_Open_Segment("leader");
	{
		HC_Flush_Contents(".", "everything");

		for (unsigned int i = 0; i < in_count; i++)
		{
			HC_Open_Segment("");
			{
				unsigned int count = in_leader_lines[i].GetPointCount();
				H3DF::Point * points = new H3DF::Point[count];
				in_leader_lines[i].GetPoints(count, points);

				HC_Insert_Polyline(count, points);

				delete[] points;

				H3DF::RGBColor color;
				in_leader_lines[i].GetRGBColor(color);

				if (color.red >= 0 && color.green >= 0 && color.blue >= 0)
					HC_Set_Color_By_Value("lines", "RGB", color.red, color.green, color.blue);

				char pattern[1024] = { "" };
				in_leader_lines[i].GetLinePattern(pattern);

				if (pattern[0] != '\0')
					HC_Set_Line_Pattern(pattern);
			}
			HC_Close_Segment();
		}
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::PmiKey::SetLeaderSymbols(unsigned int nInCount, H3DF::Polygon const * pcInLeaderSymbols)
{
	SetPolygons("leader/symbol", nInCount, pcInLeaderSymbols);
}

void H3DF::PmiKey::SetPolygons(char const * pchInSegmentName, unsigned int nInCount, H3DF::Polygon const * pcInPolygons)
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_Open_Segment(pchInSegmentName);
	{
		H3DF::RGBColor color_current;

		HC_Flush_Contents(".", "everything");

		for (unsigned int i = 0; i < nInCount; i++)
		{
			//new segment if new graphics

			if (i == 0)
			{
				pcInPolygons[0].GetRGBColor(color_current);

				HC_Open_Segment("");
				SetColor(pcInPolygons[0]);
			}

			H3DF::RGBColor color;
			pcInPolygons[i].GetRGBColor(color);
			if (color != color_current)
			{
				HC_Close_Segment();
				HC_Open_Segment("");
				SetColor(pcInPolygons[i]);
				color_current = color;
			}

			unsigned int nPointCount = pcInPolygons[i].GetPointCount();
			H3DF::Point * pcPoints = new H3DF::Point[nPointCount];
			pcInPolygons[i].GetPoints(nPointCount, pcPoints);
			HC_Insert_Polygon(nPointCount, pcPoints);
			delete[] pcPoints;
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::PmiKey::SetColor(const H3DF::Polyline & cPolyline)
{
	H3DF::RGBColor cColor;
	cPolyline.GetRGBColor(cColor);

	if (cColor.red >= 0 && cColor.green >= 0 && cColor.blue >= 0) {
		HC_Set_Color_By_Value("lines", "RGB", cColor.red, cColor.green, cColor.blue);
	}

	char pattern[PATTERN_BUFFER_SIZE] = { "" };
	cPolyline.GetLinePattern(pattern);

	if (pattern[0] != '\0') {
		HC_Set_Line_Pattern(pattern);
	}
}

void H3DF::PmiKey::SetColor(const H3DF::Polygon & cPolygon)
{
	H3DF::RGBColor cColor;
	cPolygon.GetRGBColor(cColor);

	if (cColor.red >= 0 && cColor.green >= 0 && cColor.blue >= 0) {
		HC_Set_Color_By_Value("faces", "RGB", cColor.red, cColor.green, cColor.blue);
	}
}

void H3DF::PmiKey::SetDisplayParallelToScreen(bool const bInParallel)
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_Open_Segment("text");
	{
		if (true == bInParallel) {
			HC_Set_Rendering_Options("depth range = (0.0,0.1)");
			HC_Set_Text_Font("transforms = off");
		}
		else {
			HC_Set_Text_Font("transforms = on");
		}
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);
}

bool H3DF::PmiKey::IsDisplayParallelToScreen() const
{
	bool is_parallel_to_screen = false;

	SegmentKeyImpl::LocalOpen(*this);

	int text_segment_count = 0;
	HC_Begin_Segment_Search("text");
	{
		HC_Show_Segment_Count(&text_segment_count);
	}
	HC_End_Segment_Search();

	if (text_segment_count > 0)
	{
		char cval[MVO_BUFFER_SIZE];
		HC_KEY nKey = HC_Open_Segment("text");
		assert(INVALID_KEY != nKey);
		{
			HC_Show_One_Net_Text_Font("transforms", cval);
			if (streq(cval, "off"))
				is_parallel_to_screen = true;
		}
		HC_Close_Segment();
	}

	SegmentKeyImpl::LocalClose(*this);

	return is_parallel_to_screen;
}

unsigned int H3DF::PmiKey::GetStringsAndTextAttributesCount(CString strInSegmentName) const
{
	int nCount = 0;

	SegmentKeyImpl::LocalOpen(*this);

	HC_KEY nKey = HC_Open_Segment(H_FORMAT_TEXT("text/%s", Utility::ToChar(strInSegmentName)));
	assert(INVALID_KEY != nKey);
	{
		HC_Begin_Contents_Search("...", "text");
		{
			HC_Show_Contents_Count(&nCount);
		}
		HC_End_Contents_Search();
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);

	return static_cast<unsigned int>(nCount);
}

void H3DF::PmiKey::GetStringsAndTextAttributes(CString strInSegmentName, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	char type[64];
	HC_KEY key;
	int i = 0;

	SegmentKeyImpl::LocalOpen(*this);

	HC_KEY nKey = HC_Open_Segment("text");
	assert(INVALID_KEY != nKey);
	{
		bool is_parallel_to_screen = false;
		if (HC_Show_Existence("text font=transforms"))
		{
			char cval[64];
			HC_Show_One_Text_Font("transforms", cval);
			is_parallel_to_screen = streq(cval, "off");
		}

		HC_Open_Segment(Utility::ToChar(strInSegmentName));
		{
			HC_Begin_Contents_Search("...", "text");
			{
				while (HC_Find_Contents(type, &key))
				{
					int length = 0;
					HC_Show_Text_Length(key, &length);

					H3DF::Point cInsertionPoint;
					char * text = new char[length + 1];

					HC_Show_Text(key, &cInsertionPoint.x, &cInsertionPoint.y, &cInsertionPoint.z, text);

					if (is_parallel_to_screen) {
						char cval[64];
						HC_Show_One_Character_Attribute(key, 0, "horizontal offset", cval);
						sscanf(cval, "%f", &cInsertionPoint.x);
						HC_Show_One_Character_Attribute(key, 0, "vertical offset", cval);
						sscanf(cval, "%f", &cInsertionPoint.y);
						cInsertionPoint.z = 0;
					}

					if (nullptr != pstrOutStrings) {
						pstrOutStrings[i] = H_UNICODE_TEXT(text);
					}

					delete[] text;

					if (nullptr != pcOutTextAttributes) {

						pcOutTextAttributes[i] = TextAttributes();

						pcOutTextAttributes[i].SetInsertionPoint(cInsertionPoint);

						HC_Open_Segment_By_Key(HC_Show_Owner_Original_Key(key));
						{
							if (HC_Show_Existence("color = text"))
							{
								H3DF::RGBColor color;
								char color_space[4] = { "" };
								HC_Show_One_Color_By_Value("text", color_space, &color.red, &color.green, &color.blue);
								pcOutTextAttributes[i].SetRGBColor(color);
							}

							char chOptions[1024];

							float font_size = 0.0f;
							HC_Show_One_Text_Font("size", chOptions);
							HCLOCALE(sscanf(chOptions, "%f", &font_size));
							pcOutTextAttributes[i].SetFontSize(font_size);

							Font::Size::Units font_size_units = Font::Size::Units::UnknownUnits;
							if (strstr(chOptions, "wsu")) {
								font_size_units = Font::Size::Units::WorldSpaceUnits;
							}
							else {
								font_size_units = Font::Size::Units::PixelUnits;
							}

							pcOutTextAttributes[i].SetFontSizeUnits(font_size_units);

							HC_Show_One_Text_Font("name", chOptions);
							pcOutTextAttributes[i].SetFontName(chOptions);

							Orientation cOrientation;
							float matrix[16];
							HC_Show_Modelling_Matrix(matrix);

							cOrientation.SetMatrix(matrix);
							pcOutTextAttributes[i].SetOrientation(cOrientation);

							if (HC_Show_Existence("text font = width scale"))
							{
								HC_Show_One_Text_Font("width scale", chOptions);
								pcOutTextAttributes[i].SetWidthScale(atof(chOptions));
							}
						}
						HC_Close_Segment();
					}

					i++;
				}
			}
			HC_End_Contents_Search();
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);
}

void H3DF::PmiKey::SetStringsAndTextAttributes(CString strInSegmentName, unsigned int nInCount,
	CString const * pstrInStrings, TextAttributes const * pcInTextAttributes, bool is_parallel_to_screen)
{
	SegmentKeyImpl::LocalOpen(*this);

	HC_KEY nKey = HC_Open_Segment("text");
	assert(INVALID_KEY != nKey);
	{
		HC_Set_Text_Alignment("<v");
		HC_Set_Text_Font(H_FORMAT_TEXT("transforms = %s", (is_parallel_to_screen ? "off" : "on")));
		HC_Set_Heuristics("no culling");

		HC_Open_Segment(Utility::ToChar(strInSegmentName));
		{
			HC_Flush_Contents(".", "everything");

			for (size_t nIndex = 0; nIndex < nInCount; nIndex++)
			{
				HC_Open_Segment("");
				{
					Font::Size::Units font_size_units;
					pcInTextAttributes[nIndex].GetFontSizeUnits(font_size_units);

					bool const bFrameDrawMode = is_parallel_to_screen && (font_size_units == Font::Size::Units::PixelUnits || 
						font_size_units == Font::Size::Units::UnknownUnits);

					bool const bFaceViewMode = is_parallel_to_screen && font_size_units == Font::Size::Units::WorldSpaceUnits;

					H3DF::Point cInsertionPoint;
					pcInTextAttributes[nIndex].GetInsertionPoint(cInsertionPoint);

					H_UTF8 utf8(pstrInStrings[nIndex]);

					if (bFrameDrawMode)
					{
						HC_KEY text = HC_Insert_Text_With_Encoding(0., 0., 0., "utf8", WStr::ToUtf8(pstrInStrings[nIndex]));

						char character_attributes[MVO_BUFFER_SIZE];
						sprintf(character_attributes, "horizontal offset= %d.points", (int)cInsertionPoint.x);
						HC_MSet_Character_Attributes(text, 0, pstrInStrings[nIndex].GetLength(), character_attributes);

						sprintf(character_attributes, "vertical offset= %d.points", (int)cInsertionPoint.y);
						HC_MSet_Character_Attributes(text, 0, pstrInStrings[nIndex].GetLength(), character_attributes);
					}
					else if (bFaceViewMode)
					{
						HC_KEY text = HC_Insert_Text_With_Encoding(0., 0., 0., "utf8", WStr::ToUtf8(pstrInStrings[nIndex]));

						char character_attributes[MVO_BUFFER_SIZE];
						sprintf(character_attributes, "horizontal offset= %f world", cInsertionPoint.x);
						HC_MSet_Character_Attributes(text, 0, pstrInStrings[nIndex].GetLength(), character_attributes);
						sprintf(character_attributes, "vertical offset= %f world", cInsertionPoint.y);
						HC_MSet_Character_Attributes(text, 0, pstrInStrings[nIndex].GetLength(), character_attributes);
					}
					else {
						HC_Insert_Text_With_Encoding(cInsertionPoint.x, cInsertionPoint.y, cInsertionPoint.z, "utf8", utf8.encodedText());// WStr::ToUtf8(pstrInStrings[nIndex]));
					}

					float font_size;
					pcInTextAttributes[nIndex].GetFontSize(font_size);

					char font_name[1024];
					pcInTextAttributes[nIndex].GetFontName(font_name);

					if (font_name[0] != '\0')
					{
						if (bFrameDrawMode)
							HC_Set_Text_Font(H_FORMAT_TEXT("name = %s, size = %f pixels", font_name, font_size));
						else
							HC_Set_Text_Font(H_FORMAT_TEXT("name = %s, size = %f world", font_name, font_size));
					}
					if (pcInTextAttributes[nIndex].WidthScale() != 1.)
						HC_Set_Text_Font(H_FORMAT_TEXT("width scale=%f", pcInTextAttributes[nIndex].WidthScale()));

					if (pcInTextAttributes[nIndex].IsBold())
						HC_Set_Text_Font("bold = on");
					if (pcInTextAttributes[nIndex].IsItalic())
						HC_Set_Text_Font("italic = on");
					if (pcInTextAttributes[nIndex].IsOverlined())
						HC_Set_Text_Font("overline = on");
					if (pcInTextAttributes[nIndex].IsStrikedThrough())
						HC_Set_Text_Font("strikethrough  = on");
					if (pcInTextAttributes[nIndex].IsUnderlined())
						HC_Set_Text_Font("underline  = on");

					if (pcInTextAttributes[nIndex].IsWired())
						HC_Set_Text_Font("exterior = on");

					H3DF::RGBColor cColor;
					pcInTextAttributes[nIndex].GetRGBColor(cColor);

					if (cColor.red >= 0 && cColor.green >= 0 && cColor.blue >= 0) {
						HC_Set_Color_By_Value("text", "RGB", cColor.red, cColor.green, cColor.blue);
					}

					Orientation cOrientation;
					pcInTextAttributes[nIndex].GetOrientation(cOrientation);

					float matrix[16];
					cOrientation.GetMatrix(matrix);

					HC_Set_Modelling_Matrix(matrix);
				}
				HC_Close_Segment();
			}
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	SegmentKeyImpl::LocalClose(*this);
}


//== Datum =========================================================================================

namespace H3DF
{
	class DatumEntityImpl : public PmiKeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<DatumEntityImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const DatumEntityImpl * pcInThat) {
			PmiKeyImpl::Copy(pcInThat);
		}
	};
}

H3DF::DatumEntity::DatumEntity(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	if (INVALID_KEY == nInKey) {
		return;
	}

	m_pcImpl = std::make_unique<DatumEntityImpl>();
	static_cast<DatumEntityImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);

	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *) &nType);
	}
}

H3DF::DatumEntity::DatumEntity(SegmentKey const & cInThat)
{
	// cInThat이 올바른 Impl(DatumEntityImpl)을 가지고 있으면 복제
	if (cInThat.GetImpl()) {
		// 만약 PmiKeyImpl이 KeyImpl에서 파생된 구조라면 dynamic_cast에 의해서 nullptr이 아닌 정상적인 값이 넘어옴
		auto pcSrcImpl = dynamic_cast<const DatumEntityImpl *>(cInThat.GetImpl());
		if (nullptr != pcSrcImpl) {
			m_pcImpl = pcSrcImpl->Clone();
		}
		else {
			// 타입이 다를 경우 예외 처리 또는 방어적 초기화
			m_pcImpl = std::make_unique<DatumEntityImpl>();
			static_cast<DatumEntityImpl *>(m_pcImpl.get())->Copy((DatumEntityImpl *) (cInThat.GetImpl()));
		}
	}
	else {
		m_pcImpl = std::make_unique<DatumEntityImpl>();
		static_cast<DatumEntityImpl *>(m_pcImpl.get())->Copy((DatumEntityImpl *) (cInThat.GetImpl()));
	}

	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t) UserDataIndex::PmiType, sizeof(DWORD), (BYTE *) &nType);
	}
}

H3DF::DatumEntity::DatumEntity(DatumEntity const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;

	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

DatumEntity const & H3DF::DatumEntity::operator=(DatumEntity const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

Datum::Type H3DF::DatumEntity::GetDatumType() const
{
	ByteArray aUserData;
	if (true == ShowUserData((intptr_t)UserDataIndex::DatumType, aUserData)) {
		DWORD nOutType;
		CopyMemory(&nOutType, aUserData.data(), sizeof(DWORD));
		return (Datum::Type)nOutType;
	}

	return Datum::Type::Unknown;
}

void H3DF::DatumEntity::SetDatumType(Datum::Type const eInType)
{
	DWORD nType = (DWORD)eInType;
	SetUserData((intptr_t)UserDataIndex::DatumType, sizeof(DWORD), (BYTE *)&nType);
}

unsigned int H3DF::DatumEntity::GetLabelCount() const
{
	return GetStringsAndTextAttributesCount(L"labels");
}

void H3DF::DatumEntity::GetLabels(unsigned int & nOutCount, CString * pstrOutLabels, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetLabelCount();

	if (nOutCount == 0 || nullptr == pcOutTextAttributes) {
		return;
	}
		
	GetStringsAndTextAttributes(L"labels", pstrOutLabels, pcOutTextAttributes);
}

void H3DF::DatumEntity::SetLabels(unsigned int nInCount, CString const * pstrInlabels, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"labels", nInCount, pstrInlabels, pcInTextAttributes, this->IsDisplayParallelToScreen());
}


//== DimensionEntity ===============================================================================
H3DF::DimensionEntity::DimensionEntity(HC_KEY nInKey) : PmiKey(nInKey)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::DimensionEntity::DimensionEntity(SegmentKey const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::DimensionEntity::DimensionEntity(DimensionEntity const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

void H3DF::DimensionEntity::Set(DimensionEntity const & cInThat)
{
	H3DF::PmiKey::Set(cInThat);
}

DimensionEntity const & H3DF::DimensionEntity::operator=(DimensionEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

Dimension::Type H3DF::DimensionEntity::GetDimensionType() const
{
	ByteArray aUserData;
	if (true == ShowUserData((intptr_t)UserDataIndex::DimensionType, aUserData)) {
		DWORD nOutType;
		CopyMemory(&nOutType, aUserData.data(), sizeof(DWORD));
		return (Dimension::Type)nOutType;
	}

	return Dimension::Type::UnknownType;
}

void H3DF::DimensionEntity::SetDimensionType(Dimension::Type const eInType)
{
	DWORD nType = (DWORD)eInType;
	SetUserData((intptr_t)UserDataIndex::DatumType, sizeof(DWORD), (BYTE *)&nType);
}

Dimension::SubType H3DF::DimensionEntity::GetDimensionSubType() const
{
	ByteArray aUserData;
	if (true == ShowUserData((intptr_t)UserDataIndex::DimensionSubType, aUserData)) {
		DWORD nOutType;
		CopyMemory(&nOutType, aUserData.data(), sizeof(DWORD));
		return (Dimension::SubType)nOutType;
	}

	return Dimension::SubType::UnknownSubType;
}


void H3DF::DimensionEntity::SetDimensionSubType(Dimension::SubType const eInSubType)
{
	DWORD nType = (DWORD)eInSubType;
	SetUserData((intptr_t)UserDataIndex::DimensionSubType, sizeof(DWORD), (BYTE *)&nType);
}

unsigned int H3DF::DimensionEntity::GetStringCount() const
{
	return GetStringsAndTextAttributesCount(L"strings");
}

void H3DF::DimensionEntity::GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetStringCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr)) {
		return;
	}

	GetStringsAndTextAttributes(L"strings", pstrOutStrings, pcOutTextAttributes);
}

void H3DF::DimensionEntity::SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"strings", nInCount, pstrInStrings, pcInTextAttributes, IsDisplayParallelToScreen());
}

//== FeatureControlFrameEntity Class ===============================================================
H3DF::FeatureControlFrameEntity::FeatureControlFrameEntity(HC_KEY nInKey) : PmiKey(nInKey)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::FeatureControlFrameEntity::FeatureControlFrameEntity(SegmentKey const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::FeatureControlFrameEntity::FeatureControlFrameEntity(FeatureControlFrameEntity const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

void H3DF::FeatureControlFrameEntity::Set(FeatureControlFrameEntity const & cInThat)
{
	H3DF::PmiKey::Set(cInThat);
}

FeatureControlFrameEntity const & H3DF::FeatureControlFrameEntity::operator=(FeatureControlFrameEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}


//== GenericEntity Class ===========================================================================

H3DF::GenericEntity::GenericEntity(HC_KEY nInKey) :	PmiKey(nInKey)
{
	if (staticType != Type()) {
		return;
	}

	if (INVALID_KEY == nInKey) {
		return;
	}

	auto pcImpl = static_cast<SegmentKeyImpl *>(m_pcImpl.get());

	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::GenericEntity::GenericEntity(SegmentKey const & cInThat) : PmiKey(cInThat)
{
	auto pcImpl = static_cast<SegmentKeyImpl *>(m_pcImpl.get());

	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::GenericEntity::GenericEntity(GenericEntity const & cInThat) : PmiKey(cInThat)
{
	auto pcImpl = static_cast<SegmentKeyImpl *>(m_pcImpl.get());

	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

void H3DF::GenericEntity::Set(GenericEntity const & cInThat)
{
	H3DF::PmiKey::Set(cInThat);
}

GenericEntity const & H3DF::GenericEntity::operator=(GenericEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int H3DF::GenericEntity::GetStringCount() const
{
	return GetStringsAndTextAttributesCount("strings");
}

void H3DF::GenericEntity::GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetStringCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr)) {
		return;
	}

	GetStringsAndTextAttributes(L"strings", pstrOutStrings, pcOutTextAttributes);
}

void H3DF::GenericEntity::SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"strings", nInCount, pstrInStrings, pcInTextAttributes);
}

void H3DF::GenericEntity::SetDisplayParallelToScreen(bool const in_parallel)
{
	H3DF::PmiKey::SetDisplayParallelToScreen(in_parallel);
}

bool H3DF::GenericEntity::IsDisplayParallelToScreen() const
{
	return H3DF::PmiKey::IsDisplayParallelToScreen();
}

//== NoteEntity ====================================================================================
H3DF::NoteEntity::NoteEntity(HC_KEY nInKey) : PmiKey(nInKey)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::NoteEntity::NoteEntity(SegmentKey const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::NoteEntity::NoteEntity(NoteEntity const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

void H3DF::NoteEntity::Set(NoteEntity const & cInThat)
{
	H3DF::PmiKey::Set(cInThat);
}

NoteEntity const & H3DF::NoteEntity::operator=(NoteEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int H3DF::NoteEntity::GetStringCount() const
{
	return GetStringsAndTextAttributesCount(L"strings");
}

void H3DF::NoteEntity::GetStrings(unsigned int & nOutCount, CString pstrOutStrings[], TextAttributes pcOutTextAttributes[]) const
{
	nOutCount = GetStringCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr))
		return;

	GetStringsAndTextAttributes(L"strings", pstrOutStrings, pcOutTextAttributes);
}

void H3DF::NoteEntity::SetStrings(unsigned int nInCount, CString const pstrInStrings[], TextAttributes const pcInTextAttributes[])
{
	SetStringsAndTextAttributes(L"strings", nInCount, pstrInStrings, pcInTextAttributes, this->IsDisplayParallelToScreen());
}

//== RoughnessEntity ===============================================================================

H3DF::RoughnessEntity::RoughnessEntity(HC_KEY nInKey) :  PmiKey(nInKey)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::RoughnessEntity::RoughnessEntity(SegmentKey const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::RoughnessEntity::RoughnessEntity(RoughnessEntity const & cInThat): PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

void H3DF::RoughnessEntity::Set(RoughnessEntity const & cInThat)
{
	H3DF::PmiKey::Set(cInThat);
}

RoughnessEntity const & H3DF::RoughnessEntity::operator=(RoughnessEntity const & that)
{
	Set(that);
	return *this;
}

Roughness::Obtention::Type H3DF::RoughnessEntity::GetObtentionType() const
{
	ByteArray aUserData;
	if (true == ShowUserData((intptr_t)UserDataIndex::RoughnessObtentionType, aUserData)) {
		DWORD nOutType;
		CopyMemory(&nOutType, aUserData.data(), sizeof(DWORD));
		return (Roughness::Obtention::Type)nOutType;
	}

	return Roughness::Obtention::Type::Unknown;
}

void H3DF::RoughnessEntity::SetObtentionType(Roughness::Obtention::Type const eInObtentionType)
{
	DWORD nType = (DWORD)eInObtentionType;
	SetUserData((intptr_t)UserDataIndex::RoughnessObtentionType, sizeof(DWORD), (BYTE *)&nType);
}

Roughness::Applicability::Type H3DF::RoughnessEntity::GetApplicabilityType() const
{
	ByteArray aUserData;
	if (true == ShowUserData((intptr_t)UserDataIndex::RoughnessObtentionType, aUserData)) {
		DWORD nOutType;
		CopyMemory(&nOutType, aUserData.data(), sizeof(DWORD));
		return (Roughness::Applicability::Type)nOutType;
	}

	return Roughness::Applicability::Type::Unknown;
}

void H3DF::RoughnessEntity::SetApplicabilityType(Roughness::Applicability::Type const eInApplicabilityType)
{
	DWORD nType = (DWORD)eInApplicabilityType;
	SetUserData((intptr_t)UserDataIndex::RoughnessApplicabilityType, sizeof(DWORD), (BYTE *)&nType);
}

Roughness::Mode::Type H3DF::RoughnessEntity::GetModeType() const
{
	ByteArray aUserData;
	if (true == ShowUserData((intptr_t)UserDataIndex::RoughnessModeType, aUserData)) {
		DWORD nOutType;
		CopyMemory(&nOutType, aUserData.data(), sizeof(DWORD));
		return (Roughness::Mode::Type)nOutType;
	}

	return Roughness::Mode::Type::Unknown;
}

void H3DF::RoughnessEntity::SetModeType(Roughness::Mode::Type const eInModeType)
{
	DWORD nType = (DWORD)eInModeType;
	SetUserData((intptr_t)UserDataIndex::RoughnessModeType, sizeof(DWORD), (BYTE *)&nType);
}

unsigned int H3DF::RoughnessEntity::GetFieldCount() const
{
	return GetStringsAndTextAttributesCount(L"fields");
}

void H3DF::RoughnessEntity::GetFields(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetFieldCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr)) {
		return;
	}

	GetStringsAndTextAttributes(L"fields", pstrOutStrings, pcOutTextAttributes);
}

void H3DF::RoughnessEntity::SetFields(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"fields", nInCount, pstrInStrings, pcInTextAttributes);
}

//== Orientation ===================================================================================

class OrientationImpl : public H3DF::PmiKeyImpl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<OrientationImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const OrientationImpl * pcInThat)
	{
		m_cMatrix = pcInThat->m_cMatrix;
	}

	H3DF::MatrixKit m_cMatrix;
};

H3DF::Orientation::Orientation()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<OrientationImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Orientation::Orientation(Orientation const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

H3DF::Orientation const & H3DF::Orientation::operator=(Orientation const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::Orientation::GetMatrix(float out_matrix[16]) const
{
	auto pcImpl = static_cast<OrientationImpl *>(m_pcImpl.get());

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		out_matrix[nIndex] = pcImpl->m_cMatrix.m_fData[nIndex];
	}

	// memcpy(out_matrix, pcImpl->m_cMatrix.GetData(), 16 * sizeof(float));
}

void H3DF::Orientation::SetMatrix(MatrixKit const & cInMatrix)
{
	auto pcImpl = static_cast<OrientationImpl *>(m_pcImpl.get());
	pcImpl->m_cMatrix = cInMatrix;
}

//== TextAttributes ================================================================================

class TextAttributesImpl : public H3DF::PmiKeyImpl
{
public:
	TextAttributesImpl()
	{
		m_insertion_point.Set(0, 0, 0);
		m_font_size = 0.0f;
		m_font_size_units = Font::Size::Units::UnknownUnits;
		m_font_name = 0;
		m_color.Set(-1, -1, -1);
		m_format = 0;
		m_width_scale = 1.;
	}

	~TextAttributesImpl()
	{
		delete[] m_font_name;
	}

	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<TextAttributesImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const TextAttributesImpl * pcInThat)
	{
		m_insertion_point = pcInThat->m_insertion_point;
		m_font_size = pcInThat->m_font_size;
		m_font_size_units = pcInThat->m_font_size_units;

		delete[] m_font_name;

		if (!pcInThat->m_font_name) {
			m_font_name = nullptr;
		}
		else {
			size_t font_name_size = strlen(pcInThat->m_font_name);
			m_font_name = new char[font_name_size + 1];
			strcpy(m_font_name, pcInThat->m_font_name);
		}

		m_orientation = pcInThat->m_orientation;
		m_color = pcInThat->m_color;
		m_format = pcInThat->m_format;
		m_width_scale = pcInThat->m_width_scale;
	}

	H3DF::Point m_insertion_point;
	float m_font_size;
	Font::Size::Units m_font_size_units;
	char * m_font_name;
	Orientation m_orientation;
	H3DF::RGBColor m_color;
	char m_format;
	float m_width_scale;
};

H3DF::TextAttributes::TextAttributes()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<TextAttributesImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::TextAttributes::TextAttributes(TextAttributes const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

TextAttributes const & H3DF::TextAttributes::operator=(TextAttributes const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::TextAttributes::GetInsertionPoint(H3DF::Point & cOutPoint) const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	cOutPoint = pcImpl->m_insertion_point;
}

void H3DF::TextAttributes::SetInsertionPoint(H3DF::Point const & cInPoint)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_insertion_point = cInPoint;
}

void H3DF::TextAttributes::GetFontSize(float & fOutSize) const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	fOutSize = pcImpl->m_font_size;
}

void H3DF::TextAttributes::SetFontSize(float const nInSize)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_font_size = nInSize;
}

void H3DF::TextAttributes::GetFontSizeUnits(Font::Size::Units & eOutUnits) const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	eOutUnits = pcImpl->m_font_size_units;
}

void H3DF::TextAttributes::SetFontSizeUnits(Font::Size::Units const eInUnits)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_font_size_units = eInUnits;
}

void H3DF::TextAttributes::GetFontName(char * pchOutFontName) const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	pchOutFontName[0] = '\0';

	if (pcImpl->m_font_name) {
		strcpy(pchOutFontName, pcImpl->m_font_name);
	}
}

void H3DF::TextAttributes::SetFontName(char const * pchInFontName)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	delete[] pcImpl->m_font_name;

	size_t font_name_size = strlen(pchInFontName);

	pcImpl->m_font_name = new char[font_name_size + 1];
	strcpy(pcImpl->m_font_name, pchInFontName);
}

void H3DF::TextAttributes::GetOrientation(Orientation & cOutOrientation) const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	cOutOrientation = pcImpl->m_orientation;
}

void H3DF::TextAttributes::SetOrientation(Orientation const & cInOrientation)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_orientation = cInOrientation;
}

void H3DF::TextAttributes::GetRGBColor(H3DF::RGBColor & cOutColor) const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	cOutColor = pcImpl->m_color;
}

void H3DF::TextAttributes::SetRGBColor(H3DF::RGBColor const & cInColor)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_color = cInColor;
}

bool H3DF::TextAttributes::IsBold() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x02) == 0x02;
}

bool H3DF::TextAttributes::IsItalic() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x04) == 0x04;
}
bool H3DF::TextAttributes::IsUnderlined() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x08) == 0x08;
}
bool H3DF::TextAttributes::IsStrikedThrough() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x10) == 0x10;
}
bool H3DF::TextAttributes::IsOverlined() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x20) == 0x20;
}
bool H3DF::TextAttributes::IsStreched() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x40) == 0x40;
}
bool H3DF::TextAttributes::IsWired() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x80) == 0x80;
}
bool H3DF::TextAttributes::IsFixedWidth() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());

	return (pcImpl->m_format & 0x100) == 0x100;
}

void H3DF::TextAttributes::SetFormat(char const & chInFormat)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_format = chInFormat;
}

double H3DF::TextAttributes::WidthScale() const
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	return pcImpl->m_width_scale;
}

void H3DF::TextAttributes::SetWidthScale(const double dWidthScale)
{
	auto pcImpl = static_cast<TextAttributesImpl *>(m_pcImpl.get());
	pcImpl->m_width_scale = static_cast<float>(dWidthScale);
}

//== Options =======================================================================================
class OptionsImpl : public H3DF::PmiKeyImpl
{
public:
	OptionsImpl()  {
		m_bDisplayParallelToScreen = false;
	}

	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<OptionsImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const OptionsImpl * that) {
		m_bDisplayParallelToScreen = that->m_bDisplayParallelToScreen;
	}

	bool m_bDisplayParallelToScreen;
};

H3DF::Options::Options()
{
	if (staticType != Type()) {
 		return;
 	}

	m_pcImpl = std::make_unique<OptionsImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Options::Options(Options const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

Options const & H3DF::Options::operator=(Options const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}


bool H3DF::Options::IsDisplayParallelToScreen() const
{
	auto pcImpl = static_cast<OptionsImpl *>(m_pcImpl.get());
	return pcImpl->m_bDisplayParallelToScreen;
}

void H3DF::Options::SetDisplayParallelToScreen(const bool in_parallel)
{
	auto pcImpl = static_cast<OptionsImpl *>(m_pcImpl.get());
	pcImpl->m_bDisplayParallelToScreen = in_parallel;
}

//== Frame =========================================================================================

class FrameImpl : public H3DF::PmiKeyImpl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<FrameImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const FrameImpl * that) {
		m_aPolylines = that->m_aPolylines;
	}

	H3DF::PolylineArray	m_aPolylines;
};

H3DF::Frame::Frame()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<FrameImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Frame::Frame(Frame const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

Frame const & H3DF::Frame::operator=(Frame const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

unsigned int H3DF::Frame::GetPolylineCount() const
{
	auto pcImpl = static_cast<FrameImpl *>(m_pcImpl.get());
	return static_cast<unsigned int>(pcImpl->m_aPolylines.size());
}

void H3DF::Frame::GetPolylines(unsigned int & nOutCount, H3DF::Polyline * pcOutPolylines) const
{
	auto pcImpl = static_cast<FrameImpl *>(m_pcImpl.get());

	nOutCount = (unsigned int)pcImpl->m_aPolylines.size();

	if (nullptr == pcOutPolylines) {
		return;
	}

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPolylines[i] = pcImpl->m_aPolylines[i];
	}
}

void H3DF::Frame::SetPolylines(unsigned int nInCount, H3DF::Polyline const * pcInPolylines)
{
	auto pcImpl = static_cast<FrameImpl *>(m_pcImpl.get());
	pcImpl->m_aPolylines.resize(nInCount);

	for (unsigned int nIndex = 0; nIndex < nInCount; nIndex++) {
		pcImpl->m_aPolylines[nIndex]= pcInPolylines[nIndex];
	}
}

//== Draw =======================================================================================

class DrawingImpl : public H3DF::Impl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<DrawingImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const DrawingImpl * that) {
		m_polygons = that->m_polygons;
	}

	H3DF::PolygonArray m_polygons;
};

H3DF::Draw::Draw()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<DrawingImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Draw::Draw(Draw const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

Draw const & H3DF::Draw::operator=(Draw const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

unsigned int H3DF::Draw::GetPolygonCount() const
{
	auto pcImpl = static_cast<DrawingImpl *>(m_pcImpl.get());
	return static_cast<unsigned int>(pcImpl->m_polygons.size());
}

void H3DF::Draw::GetPolygons(unsigned int & nOutCount, H3DF::Polygon * pcOutPolygons) const
{
	auto pcImpl = static_cast<DrawingImpl *>(m_pcImpl.get());
	nOutCount = (unsigned int)pcImpl->m_polygons.size();

	if (nullptr == pcOutPolygons) {
		return;
	}

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPolygons[i] = pcImpl->m_polygons[i];
	}
}

void H3DF::Draw::SetPolygons(unsigned int nInCount, H3DF::Polygon const * pcInPolygons)
{
	auto pcImpl = static_cast<DrawingImpl *>(m_pcImpl.get());
	pcImpl->m_polygons.resize(nInCount);

	for (unsigned int i = 0; i < nInCount; i++) {
		pcImpl->m_polygons[i] = pcInPolygons[i];
	}
}

//== ViewEntity Class ==============================================================================
H3DF::ViewEntity::ViewEntity(HC_KEY nInSegmentKey) : PmiKey(nInSegmentKey)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::ViewEntity::ViewEntity(SegmentKey & nInSegmentKey) : PmiKey(nInSegmentKey)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

H3DF::ViewEntity::ViewEntity(ViewEntity const & cInThat) : PmiKey(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		DWORD nType = (DWORD) ObjectType();
		SetUserData((intptr_t)UserDataIndex::PmiType, sizeof(DWORD), (BYTE *)&nType);
	}
}

ViewEntity const & H3DF::ViewEntity::operator = (ViewEntity const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ViewEntity::GetCamera(CameraKit & cOutCamera) const
{
	ByteArray aUserData;
	ShowUserData((intptr_t)UserDataIndex::CameraKit, aUserData);

	if (true == aUserData.empty()) {
		DEBUG_STOP;
		return;
	}

	CameraKitImpl * pcImpl = (CameraKitImpl *)cOutCamera.GetImpl();
	DEBUG_VALID(pcImpl);

	CopyMemory(&pcImpl->m_cData, aUserData.data(), sizeof(CameraKitImpl::Data));
}

void H3DF::ViewEntity::SetCamera(CameraKit const & cInCamera)
{
	CameraKitImpl * pcImpl = (CameraKitImpl *)cInCamera.GetImpl();
	DEBUG_VALID(pcImpl);

	SetUserData((intptr_t)UserDataIndex::CameraKit, sizeof(CameraKitImpl::Data), (BYTE *)&pcImpl->m_cData);
}

size_t H3DF::ViewEntity::GetAssociatedEntityCount() const
{
	return ShowIncluders();
}

void H3DF::ViewEntity::GetAssociatedEntities(size_t & nOutCount, PmiKey *& pcOutEntities) const
{
	nOutCount = 0;

	char type[64] = { "" };
	HC_KEY key = INVALID_KEY;

	SegmentKeyImpl::LocalOpen(*this); {
		HC_Begin_Contents_Search(".", "includes");  {
			while (HC_Find_Contents(type, &key)) {
				HC_Open_Segment_By_Key(HC_KShow_Include_Segment(key)); {
					HC_KEY nThisSegment = HC_KCreate_Segment(".");
					char pmi_type[64] = { "" };

					ByteArray aUserData;
					ShowUserData((intptr_t)UserDataIndex::PmiType, aUserData);

					DWORD nType;
					CopyMemory(&nType, aUserData.data(), sizeof(DWORD));

					switch ((H3DF::Type)nType)
					{
						case H3DF::Type::DatumEntity:
							pcOutEntities[nOutCount] = DatumEntity(nThisSegment);
							break;

						case H3DF::Type::DimensionEntity:
							pcOutEntities[nOutCount] = DimensionEntity(nThisSegment);
							break;

						case H3DF::Type::FeatureControlFrameEntity:
							pcOutEntities[nOutCount] = FeatureControlFrameEntity(nThisSegment);
							break;

						case H3DF::Type::GenericEntity:
							pcOutEntities[nOutCount] = GenericEntity(nThisSegment);
							break;

						case H3DF::Type::NoteEntity:
							pcOutEntities[nOutCount] = NoteEntity(nThisSegment);
							break;

						case H3DF::Type::RoughnessEntity:
							pcOutEntities[nOutCount] = RoughnessEntity(nThisSegment);
							break;

						default:
							DEBUG_STOP;
							break;
					}

				} HC_Close_Segment();

				nOutCount++;
			}
		} HC_End_Contents_Search();
	} SegmentKeyImpl::LocalClose(*this);
}