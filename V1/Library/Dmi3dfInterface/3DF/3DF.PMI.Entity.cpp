#include "StdAfx.h"

#include "3DF.PMI.Entity.h"

#include "3DF.Line.h"
#include "3DF.Polygon.h"

#include <HTools.h>
#include <utf_utils.h>

#include <WStr.h>

USING_3DF_PMI_NAMESPACE

Entity::Entity(HC_KEY nInKey) :
	Key(nInKey)
{
}

Entity::Entity(Key const & cInThat) :
	Key(cInThat)
{
}

Entity::Entity(Entity const & cInThat) :
	Key(cInThat)
{
}

void Entity::Set(Entity const & cInThat)
{
	Key::Set(cInThat);
}

Entity const & Entity::operator=(Entity const & cInThat)
{
	Set(cInThat);
	return *this;
}

void Entity::SetFrame(Frame const & cInFrame)
{
	Open();

	TDF::RGBColor cCurrentColor;
	TDF::RGBColor cColor;
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
			TDF::Point * points = new TDF::Point[nPointCount];
			pcPolylines[i].GetPoints(nPointCount, points);
		
			HC_Insert_Polyline(nPointCount, points);

			delete[] points;
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	Close();
}

void Entity::GetDrawing(Drawing & cOutDrawing) const
{
/*
	cOutDrawing = Drawing();

	BasePrivate * my_impl = (BasePrivate *)m_pcImpl;

	unsigned int count = get_polygon_count(pcImpl->m_segment_key, "drawing");
	if (count == 0)
		return;

	PolygonArray polygons(count);
	get_polygons(pcImpl->m_segment_key, "drawing", polygons.data());

	out_drawing.SetPolygons(count, polygons.data());
*/
}

void Entity::SetDrawing(Drawing const & cInDrawing)
{
	unsigned int nCount = cInDrawing.GetPolygonCount();
	
	PolygonArray aPolygons;
	aPolygons.SetCount(nCount);

	cInDrawing.GetPolygons(nCount, aPolygons.GetData());

	SetPolygons("drawing", nCount, aPolygons.GetData());
}

unsigned int Entity::GetLeaderLineCount() const
{
	Open();

	int count = 0;

	HC_Open_Segment("leader"); {
		HC_Begin_Contents_Search("...", "polylines, lines"); {
			HC_Show_Contents_Count(&count);
		} HC_End_Contents_Search();
	} HC_Close_Segment();

	Close();

	return static_cast<unsigned int>(count);
}

void Entity::SetLeaderLines(unsigned int in_count, Polyline const * in_leader_lines)
{
	Open();

	HC_Open_Segment("leader");
	{
		HC_Flush_Contents(".", "everything");

		for (unsigned int i = 0; i < in_count; i++)
		{
			HC_Open_Segment("");
			{
				unsigned int count = in_leader_lines[i].GetPointCount();
				TDF::Point * points = new TDF::Point[count];
				in_leader_lines[i].GetPoints(count, points);

				HC_Insert_Polyline(count, points);

				delete[] points;

				TDF::RGBColor color;
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

	Close();
}

void Entity::SetLeaderSymbols(unsigned int nInCount, TDF::Polygon const * pcInLeaderSymbols)
{
	SetPolygons("leader/symbol", nInCount, pcInLeaderSymbols);
}

void Entity::SetPolygons(char const * pchInSegmentName, unsigned int nInCount, TDF::Polygon const * pcInPolygons)
{
	Open();

	HC_Open_Segment(pchInSegmentName);
	{
		TDF::RGBColor color_current;

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

			TDF::RGBColor color;
			pcInPolygons[i].GetRGBColor(color);
			if (color != color_current)
			{
				HC_Close_Segment();
				HC_Open_Segment("");
				SetColor(pcInPolygons[i]);
				color_current = color;
			}

			unsigned int nPointCount = pcInPolygons[i].GetPointCount();
			TDF::Point * pcPoints = new TDF::Point[nPointCount];
			pcInPolygons[i].GetPoints(nPointCount, pcPoints);
			HC_Insert_Polygon(nPointCount, pcPoints);
			delete[] pcPoints;
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	Close();
}

void Entity::SetColor(const TDF::Polyline & cPolyline)
{
	TDF::RGBColor cColor;
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

void Entity::SetColor(const TDF::Polygon & cPolygon)
{
	TDF::RGBColor cColor;
	cPolygon.GetRGBColor(cColor);

	if (cColor.red >= 0 && cColor.green >= 0 && cColor.blue >= 0) {
		HC_Set_Color_By_Value("faces", "RGB", cColor.red, cColor.green, cColor.blue);
	}
}

void Entity::SetDisplayParallelToScreen(bool const bInParallel)
{
	Open();

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

	Close();
}

bool Entity::IsDisplayParallelToScreen() const
{
	bool is_parallel_to_screen = false;

	Open();

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

	Close();

	return is_parallel_to_screen;
}

unsigned int Entity::GetStringsAndTextAttributesCount(CString strInSegmentName) const
{
	int nCount = 0;

	Open();

	HC_KEY nKey = HC_Open_Segment(H_FORMAT_TEXT("text/%s", H_ASCII_TEXT(strInSegmentName)));
	assert(INVALID_KEY != nKey);
	{
		HC_Begin_Contents_Search("...", "text");
		{
			HC_Show_Contents_Count(&nCount);
		}
		HC_End_Contents_Search();
	}
	HC_Close_Segment();

	Close();

	return static_cast<unsigned int>(nCount);
}

void Entity::GetStringsAndTextAttributes(CString strInSegmentName, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	char type[64];
	HC_KEY key;
	int i = 0;

	Open();

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

		HC_Open_Segment(H_ASCII_TEXT(strInSegmentName));
		{
			HC_Begin_Contents_Search("...", "text");
			{
				while (HC_Find_Contents(type, &key))
				{
					int length = 0;
					HC_Show_Text_Length(key, &length);

					TDF::Point cInsertionPoint;
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
								TDF::RGBColor color;
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

	Close();
}

void Entity::SetStringsAndTextAttributes(CString strInSegmentName, unsigned int nInCount,
	CString const * pstrInStrings, TextAttributes const * pcInTextAttributes, bool is_parallel_to_screen)
{
	Open();

	HC_KEY nKey = HC_Open_Segment("text");
	assert(INVALID_KEY != nKey);
	{
		HC_Set_Text_Alignment("<v");
		HC_Set_Text_Font(H_FORMAT_TEXT("transforms = %s", (is_parallel_to_screen ? "off" : "on")));
		HC_Set_Heuristics("no culling");

		HC_Open_Segment(H_ASCII_TEXT(strInSegmentName));
		{
			HC_Flush_Contents(".", "everything");

			for (size_t nIndex = 0; nIndex < nInCount; nIndex++)
			{
				HC_Open_Segment("");
				{
					Font::Size::Units font_size_units;
					pcInTextAttributes[nIndex].GetFontSizeUnits(font_size_units);

					bool const frame_draw_mode = is_parallel_to_screen && (font_size_units == Font::Size::Units::PixelUnits || 
						font_size_units == Font::Size::Units::UnknownUnits);

					bool const face_view_mode = is_parallel_to_screen && font_size_units == Font::Size::Units::WorldSpaceUnits;


					TDF::Point cInsertionPoint;
					pcInTextAttributes[nIndex].GetInsertionPoint(cInsertionPoint);

					H_UTF8 utf8(pstrInStrings[nIndex]);

					if (frame_draw_mode)
					{
						HC_KEY text = HC_Insert_Text_With_Encoding(0., 0., 0., "utf8", WStr::ToUtf8(pstrInStrings[nIndex]));

						char character_attributes[MVO_BUFFER_SIZE];
						sprintf(character_attributes, "horizontal offset= %d.points", (int)cInsertionPoint.x);
						HC_MSet_Character_Attributes(text, 0, pstrInStrings[nIndex].GetLength(), character_attributes);

						sprintf(character_attributes, "vertical offset= %d.points", (int)cInsertionPoint.y);
						HC_MSet_Character_Attributes(text, 0, pstrInStrings[nIndex].GetLength(), character_attributes);
					}
					else if (face_view_mode)
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
						if (frame_draw_mode)
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

					TDF::RGBColor cColor;
					pcInTextAttributes[nIndex].GetRGBColor(cColor);

					if (cColor.red >= 0 && cColor.green >= 0 && cColor.blue >= 0) {
						HC_Set_Color_By_Value("text", "RGB", cColor.red, cColor.green, cColor.blue);
					}

					PMI::Orientation cOrientation;
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

	Close();
}


//== Datum =========================================================================================
DatumEntity::DatumEntity(HC_KEY nInKey) :
	Entity(nInKey)
{
	if (INVALID_KEY != nInKey) {
		Open();
		HC_Set_User_Options("pmi_type = datum");
		Close();
	}
}

DatumEntity::DatumEntity(Key const & cInThat) :
	Entity(cInThat)
{
}

DatumEntity::DatumEntity(DatumEntity const & cInThat) :
	Entity(cInThat)
{
}

void DatumEntity::Set(DatumEntity const & cInThat)
{
	Key::Set(cInThat);
}

DatumEntity const & DatumEntity::operator=(DatumEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

Datum::Type DatumEntity::GetDatumType() const
{
	Open();

	Datum::Type eDatumType = Datum::Type::Unknown;

	if (HC_Show_Existence("user options = datum_type"))
	{
		char type[1024];

		HC_Show_One_User_Option("datum_type", type);

		if (streq(type, "identifier")) {
			eDatumType = PMI::Datum::Type::Identifier;
		}
			
		else if (streq(type, "target")) {
			eDatumType = PMI::Datum::Type::Target;

		}
		else {
			assert(0);

		}
	}

	Close();

	return eDatumType;
}

void DatumEntity::SetDatumType(Datum::Type const eInType)
{
	Open();

	switch (eInType)
	{
		case Datum::Type::Identifier:
			HC_Set_User_Options("datum_type = identifier");
			break;
		case Datum::Type::Target:
			HC_Set_User_Options("datum_type = target");
			break;
		case Datum::Type::Unknown:
			HC_UnSet_One_User_Option("datum_type");
			break;

		default:
			assert(0);
			break;
	}

	Close();
}

unsigned int DatumEntity::GetLabelCount() const
{
	return GetStringsAndTextAttributesCount(L"labels");
}

void DatumEntity::GetLabels(unsigned int & nOutCount, CString * pstrOutLabels, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetLabelCount();

	if (nOutCount == 0 || nullptr == pcOutTextAttributes) {
		return;
	}
		
	GetStringsAndTextAttributes(L"labels", pstrOutLabels, pcOutTextAttributes);
}

void DatumEntity::SetLabels(unsigned int nInCount, CString const * pstrInlabels, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"labels", nInCount, pstrInlabels, pcInTextAttributes, this->IsDisplayParallelToScreen());
}


//== DimensionEntity ===============================================================================
DimensionEntity::DimensionEntity(HC_KEY nInKey) :
	Entity(nInKey)
{
	if (INVALID_KEY != nInKey) {
		Open();
		HC_Set_User_Options("pmi_type = dimension");
		Close();
	}
}

DimensionEntity::DimensionEntity(Key const & cInThat) :
	Entity(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		Open();
		HC_Set_User_Options("pmi_type = dimension");
		Close();
	}
}

DimensionEntity::DimensionEntity(DimensionEntity const & cInThat) :
	Entity(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		Open();
		HC_Set_User_Options("pmi_type = dimension");
		Close();
	}
}

void DimensionEntity::Set(DimensionEntity const & cInThat)
{
	Entity::Set(cInThat);
}

DimensionEntity const & DimensionEntity::operator=(DimensionEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

Dimension::Type DimensionEntity::GetDimensionType() const
{
	PMI::Dimension::Type eDimensionType = PMI::Dimension::Type::UnknownType;

	if (INVALID_KEY == KeyValue()) {
		return eDimensionType;
	}

	Open();

	if (HC_Show_Existence("user options = dimension_type"))
	{
		char type[1024];

		HC_Show_One_User_Option("dimension_type", type);

		if (streq(type, "basic"))
			eDimensionType = PMI::Dimension::Type::BasicType;
		else if (streq(type, "reference"))
			eDimensionType = PMI::Dimension::Type::ReferenceType;
		else if (streq(type, "tolerance"))
			eDimensionType = PMI::Dimension::Type::ToleranceType;
		else
			assert(0);
	}

	Close();

	return eDimensionType;
}

void DimensionEntity::SetDimensionType(Dimension::Type const cInType)
{
	if (INVALID_KEY == KeyValue()) {
		return;
	}

	Open();

	switch (cInType)
	{
		case PMI::Dimension::Type::BasicType:
			HC_Set_User_Options("dimension_type = basic");
			break;
		case PMI::Dimension::Type::ReferenceType:
			HC_Set_User_Options("dimension_type = reference");
			break;
		case PMI::Dimension::Type::ToleranceType:
			HC_Set_User_Options("dimension_type = tolerance");
			break;
		case PMI::Dimension::Type::UnknownType:
			HC_UnSet_One_User_Option("dimension_type");
			break;
		default:
			assert(0);
	}

	Close();
}

Dimension::SubType DimensionEntity::GetDimensionSubType() const
{
	PMI::Dimension::SubType eDimensionSubtype = PMI::Dimension::SubType::UnknownSubType;

	if (INVALID_KEY == KeyValue()) {
		return eDimensionSubtype;
	}

	Open();

	if (HC_Show_Existence("user options = dimension_subtype"))
	{
		char subtype[1024];

		HC_Show_One_User_Option("dimension_subtype", subtype);

		if (streq(subtype, "angle"))
			eDimensionSubtype = PMI::Dimension::SubType::AngleSubType;
		else if (streq(subtype, "chamfer"))
			eDimensionSubtype = PMI::Dimension::SubType::ChamferSubType;
		else if (streq(subtype, "diameter"))
			eDimensionSubtype = PMI::Dimension::SubType::DiameterSubType;
		else if (streq(subtype, "distance"))
			eDimensionSubtype = PMI::Dimension::SubType::DistanceSubType;
		else if (streq(subtype, "length"))
			eDimensionSubtype = PMI::Dimension::SubType::LengthSubType;
		else if (streq(subtype, "radius"))
			eDimensionSubtype = PMI::Dimension::SubType::RadiusSubType;
		else if (streq(subtype, "slope"))
			eDimensionSubtype = PMI::Dimension::SubType::SlopeSubType;
		else
			assert(0);
	}

	Close();

	return eDimensionSubtype;
}

void DimensionEntity::SetDimensionSubType(PMI::Dimension::SubType const eInSubType)
{
	if (INVALID_KEY == KeyValue()) {
		return;
	}

	Open();

	switch (eInSubType)
	{
		case PMI::Dimension::SubType::AngleSubType:
			HC_Set_User_Options("dimension_subtype = angle");
			break;
		case PMI::Dimension::SubType::ChamferSubType:
			HC_Set_User_Options("dimension_subtype = chamfer");
			break;
		case PMI::Dimension::SubType::DiameterSubType:
			HC_Set_User_Options("dimension_subtype = diameter");
			break;
		case PMI::Dimension::SubType::DistanceSubType:
			HC_Set_User_Options("dimension_subtype = distance");
			break;
		case PMI::Dimension::SubType::LengthSubType:
			HC_Set_User_Options("dimension_subtype = length");
			break;
		case PMI::Dimension::SubType::RadiusSubType:
			HC_Set_User_Options("dimension_subtype = radius");
			break;
		case PMI::Dimension::SubType::SlopeSubType:
			HC_Set_User_Options("dimension_subtype = slope");
			break;
		case PMI::Dimension::SubType::UnknownSubType:
			HC_UnSet_One_User_Option("dimension_subtype");
			break;
		default:
			assert(0);
	}

	Close();
}

unsigned int DimensionEntity::GetStringCount() const
{
	return GetStringsAndTextAttributesCount(L"strings");
}

void DimensionEntity::GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetStringCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr)) {
		return;
	}

	GetStringsAndTextAttributes(L"strings", pstrOutStrings, pcOutTextAttributes);
}

void DimensionEntity::SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"strings", nInCount, pstrInStrings, pcInTextAttributes, IsDisplayParallelToScreen());
}

//== GenericEntity =================================================================================

GenericEntity::GenericEntity(HC_KEY nInKey) :
	Entity(nInKey)
{
	if (INVALID_KEY != nInKey) {
		Open();
		HC_Set_User_Options("pmi_type = generic");
		Close();
	}
}

GenericEntity::GenericEntity(Key const & cInThat) :
	Entity(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		Open();
		HC_Set_User_Options("pmi_type = generic");
		Close();
	}
}

GenericEntity::GenericEntity(GenericEntity const & cInThat) :
	Entity(cInThat)
{
	if (INVALID_KEY != KeyValue()) {
		Open();
		HC_Set_User_Options("pmi_type = generic");
		Close();
	}
}

void GenericEntity::Set(GenericEntity const & cInThat)
{
	Entity::Set(cInThat);
}

GenericEntity const & GenericEntity::operator=(GenericEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int GenericEntity::GetStringCount() const
{
	return GetStringsAndTextAttributesCount("strings");
}

void GenericEntity::GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetStringCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr)) {
		return;
	}

	GetStringsAndTextAttributes(L"strings", pstrOutStrings, pcOutTextAttributes);
}

void GenericEntity::SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"strings", nInCount, pstrInStrings, pcInTextAttributes);
}

void GenericEntity::SetDisplayParallelToScreen(bool const in_parallel)
{
	Entity::SetDisplayParallelToScreen(in_parallel);
}

bool GenericEntity::IsDisplayParallelToScreen() const
{
	return Entity::IsDisplayParallelToScreen();
}

//== NoteEntity ====================================================================================
NoteEntity::NoteEntity(HC_KEY nInKey):
	Entity(nInKey)
{
	if (INVALID_KEY != nInKey) {
		Open();
		HC_Set_User_Options("pmi_type = note");
		Close();
	}
}

NoteEntity::NoteEntity(Key const & cInThat) :
	Entity(cInThat)
{

}

NoteEntity::NoteEntity(NoteEntity const & cInThat) :
	Entity(cInThat)
{
}

void NoteEntity::Set(NoteEntity const & cInThat)
{
	Entity::Set(cInThat);
}

NoteEntity const & NoteEntity::operator=(NoteEntity const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int NoteEntity::GetStringCount() const
{
	return GetStringsAndTextAttributesCount(L"strings");
}

void NoteEntity::GetStrings(unsigned int & nOutCount, CString pstrOutStrings[], TextAttributes pcOutTextAttributes[]) const
{
	nOutCount = GetStringCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr))
		return;

	GetStringsAndTextAttributes(L"strings", pstrOutStrings, pcOutTextAttributes);
}

void NoteEntity::SetStrings(unsigned int nInCount, CString const pstrInStrings[], TextAttributes const pcInTextAttributes[])
{
	SetStringsAndTextAttributes(L"strings", nInCount, pstrInStrings, pcInTextAttributes, this->IsDisplayParallelToScreen());
}

//== RoughnessEntity ===============================================================================

RoughnessEntity::RoughnessEntity(HC_KEY nInKey) : 
	Entity(nInKey)
{
	if (nInKey != INVALID_KEY)
	{
		Open();
			HC_Set_User_Options("pmi_type = roughness");
		Close();
	}
}

RoughnessEntity::RoughnessEntity(Key const & cInThat) :
	Entity(cInThat)
{
}

RoughnessEntity::RoughnessEntity(RoughnessEntity const & cInThat):
	Entity(cInThat)
{
}

void RoughnessEntity::Set(RoughnessEntity const & cInThat)
{
	Entity::Set(cInThat);
}

RoughnessEntity const & RoughnessEntity::operator=(RoughnessEntity const & that)
{
	Set(that);
	return *this;
}

Roughness::Obtention::Type RoughnessEntity::GetObtentionType() const
{
	Open();

	Roughness::Obtention::Type eObtentionType = Roughness::Obtention::Type::Unknown;

	if (HC_Show_Existence("user options = obtention_type"))
	{
		char type[64];

		HC_Show_One_User_Option("obtention_type", type);

		if (streq(type, "not_defined"))
			eObtentionType = Roughness::Obtention::Type::NotDefined;
		else if (streq(type, "machining_mandatory"))
			eObtentionType = Roughness::Obtention::Type::MachiningMandatory;
		else if (streq(type, "machining_forbidden"))
			eObtentionType = Roughness::Obtention::Type::MachiningForbidden;
		else
			assert(0);
	}

	Close();

	return eObtentionType;
}

void RoughnessEntity::SetObtentionType(Roughness::Obtention::Type const eInObtentionType)
{
	Open();

	switch (eInObtentionType)
	{
		case Roughness::Obtention::Type::NotDefined:
			HC_Set_User_Options("obtention_type = not_defined");
			break;
		case Roughness::Obtention::Type::MachiningMandatory:
			HC_Set_User_Options("obtention_type = machining_mandatory");
			break;
		case Roughness::Obtention::Type::MachiningForbidden:
			HC_Set_User_Options("obtention_type = machining_forbidden");
			break;
		case Roughness::Obtention::Type::Unknown:
			HC_UnSet_One_User_Option("obtention_type");
			break;
		default:
			assert(0);
	}

	Close();
}

Roughness::Applicability::Type RoughnessEntity::GetApplicabilityType() const
{
	Roughness::Applicability::Type eApplicabilityType = Roughness::Applicability::Type::Unknown;

	Open();

	if (HC_Show_Existence("user options = applicability_type"))
	{
		char type[64];

		HC_Show_One_User_Option("applicability_type", type);

		if (streq(type, "specified_surface"))
			eApplicabilityType = Roughness::Applicability::Type::SpecifiedSurface;
		else if (streq(type, "surrounding_surfaces"))
			eApplicabilityType = Roughness::Applicability::Type::SurroundingSurfaces;
		else if (streq(type, "basic_surface"))
			eApplicabilityType = Roughness::Applicability::Type::BasicSurface;
		else if (streq(type, "all_surfaces"))
			eApplicabilityType = Roughness::Applicability::Type::AllSurfaces;
		else
			assert(0);
	}

	Close();

	return eApplicabilityType;
}

void RoughnessEntity::SetApplicabilityType(Roughness::Applicability::Type const in_applicability_type)
{
	Open();

	switch (in_applicability_type)
	{
		case Roughness::Applicability::Type::SpecifiedSurface:
			HC_Set_User_Options("applicability_type = specified_surface");
			break;
		case Roughness::Applicability::Type::SurroundingSurfaces:
			HC_Set_User_Options("applicability_type = surrounding_surfaces");
			break;
		case Roughness::Applicability::Type::BasicSurface:
			HC_Set_User_Options("applicability_type = basic_surface");
			break;
		case Roughness::Applicability::Type::AllSurfaces:
			HC_Set_User_Options("applicability_type = all_surfaces");
			break;
		case Roughness::Applicability::Type::Unknown:
			HC_UnSet_One_User_Option("applicability_type");
			break;
		default:
			assert(0);
	}

	Close();
}

Roughness::Mode::Type RoughnessEntity::GetModeType() const
{
	Roughness::Mode::Type mode_type = Roughness::Mode::Type::Unknown;

	Open();

	if (HC_Show_Existence("user options = mode_type"))
	{
		char type[64];

		HC_Show_One_User_Option("mode_type", type);

		if (streq(type, "none"))
			mode_type = Roughness::Mode::Type::None;
		else if (streq(type, "multidirectional"))
			mode_type = Roughness::Mode::Type::MultiDirectional;
		else if (streq(type, "circular"))
			mode_type = Roughness::Mode::Type::Circular;
		else if (streq(type, "radial"))
			mode_type = Roughness::Mode::Type::Radial;
		else if (streq(type, "particular"))
			mode_type = Roughness::Mode::Type::Particular;
		else if (streq(type, "with_angles"))
			mode_type = Roughness::Mode::Type::WithAngles;
		else if (streq(type, "parallel"))
			mode_type = Roughness::Mode::Type::Parallel;
		else if (streq(type, "perpendicular"))
			mode_type = Roughness::Mode::Type::Perpendicular;
		else
			assert(0);
	}

	Close();

	return mode_type;
}

void RoughnessEntity::SetModeType(Roughness::Mode::Type const eInModeType)
{
	Open();

	switch (eInModeType)
	{
		case Roughness::Mode::Type::None:
			HC_Set_User_Options("mode_type = none");
			break;
		case Roughness::Mode::Type::MultiDirectional:
			HC_Set_User_Options("mode_type = multidirectional");
			break;
		case Roughness::Mode::Type::Circular:
			HC_Set_User_Options("mode_type = circular");
			break;
		case Roughness::Mode::Type::Radial:
			HC_Set_User_Options("mode_type = radial");
			break;
		case Roughness::Mode::Type::Particular:
			HC_Set_User_Options("mode_type = particular");
			break;
		case Roughness::Mode::Type::WithAngles:
			HC_Set_User_Options("mode_type = with_angles");
			break;
		case Roughness::Mode::Type::Parallel:
			HC_Set_User_Options("mode_type = parallel");
			break;
		case Roughness::Mode::Type::Perpendicular:
			HC_Set_User_Options("mode_type = perpendicular");
			break;
		case Roughness::Mode::Type::Unknown:
			HC_UnSet_One_User_Option("mode_type");
			break;
		default:
			assert(0);
	}

	Close();
}

unsigned int RoughnessEntity::GetFieldCount() const
{
	return GetStringsAndTextAttributesCount(L"fields");
}

void RoughnessEntity::GetFields(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const
{
	nOutCount = GetFieldCount();

	if (nOutCount == 0 || (pstrOutStrings == nullptr && pcOutTextAttributes == nullptr)) {
		return;
	}

	GetStringsAndTextAttributes(L"fields", pstrOutStrings, pcOutTextAttributes);
}

void RoughnessEntity::SetFields(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes)
{
	SetStringsAndTextAttributes(L"fields", nInCount, pstrInStrings, pcInTextAttributes);
}

//== Orientation ===================================================================================

class OrientationPrivate : public TDF::PrivateImpl
{
public:
	OrientationPrivate() {}

	void Copy(OrientationPrivate * pcInThat)
	{
		m_cMatrix = pcInThat->m_cMatrix;
	}

	TDF::Matrix m_cMatrix;
};

Orientation::Orientation()
{
	m_pcImpl = new OrientationPrivate();
}

Orientation::Orientation(Orientation const & cInThat)
{
	m_pcImpl = new OrientationPrivate();
	Set(cInThat);
}

void Orientation::Set(Orientation const & cInThat)
{
	OrientationPrivate * pcImpl = (OrientationPrivate *)m_pcImpl;
	OrientationPrivate * pcInThatImpl = (OrientationPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Orientation const & Orientation::operator=(Orientation const & cInThat)
{
	Set(cInThat);
	return *this;
}

void Orientation::GetMatrix(float out_matrix[16]) const
{
	OrientationPrivate * pcImpl = (OrientationPrivate *)m_pcImpl;

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		out_matrix[nIndex] = pcImpl->m_cMatrix.r[nIndex];
	}

	// memcpy(out_matrix, pcImpl->m_cMatrix.GetData(), 16 * sizeof(float));
}

void Orientation::SetMatrix(TDF::Matrix const & cInMatrix)
{
	OrientationPrivate * pcImpl = (OrientationPrivate *)m_pcImpl;
	pcImpl->m_cMatrix = cInMatrix;
}

//== TextAttributes ================================================================================

class TextAttributesPrivate : public TDF::PrivateImpl
{
public:
	TextAttributesPrivate()
	{
		m_insertion_point.Set(0, 0, 0);
		m_font_size = 0.0f;
		m_font_size_units = Font::Size::Units::UnknownUnits;
		m_font_name = 0;
		m_color.Set(-1, -1, -1);
		m_format = 0;
		m_width_scale = 1.;
	}

	~TextAttributesPrivate()
	{
		delete[] m_font_name;
	}

	void Copy(TextAttributesPrivate * pcInThat)
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

	TDF::Point m_insertion_point;
	float m_font_size;
	Font::Size::Units m_font_size_units;
	char * m_font_name;
	Orientation m_orientation;
	TDF::RGBColor m_color;
	char m_format;
	float m_width_scale;
};

TextAttributes::TextAttributes()
{
	m_pcImpl = new TextAttributesPrivate();
}

TextAttributes::TextAttributes(TextAttributes const & cInThat)
{
	m_pcImpl = new TextAttributesPrivate();
	Set(cInThat);
}

void TextAttributes::Set(TextAttributes const & cInThat)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	TextAttributesPrivate * pcInThatImpl = (TextAttributesPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

TextAttributes const & TextAttributes::operator=(TextAttributes const & cInThat)
{
	Set(cInThat);
	return *this;
}

void TextAttributes::GetInsertionPoint(TDF::Point & cOutPoint) const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	cOutPoint = pcImpl->m_insertion_point;
}

void TextAttributes::SetInsertionPoint(TDF::Point const & cInPoint)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_insertion_point = cInPoint;
}

void TextAttributes::GetFontSize(float & fOutSize) const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	fOutSize = pcImpl->m_font_size;
}

void TextAttributes::SetFontSize(float const nInSize)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_font_size = nInSize;
}

void TextAttributes::GetFontSizeUnits(Font::Size::Units & eOutUnits) const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	eOutUnits = pcImpl->m_font_size_units;
}

void TextAttributes::SetFontSizeUnits(Font::Size::Units const eInUnits)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_font_size_units = eInUnits;
}

void TextAttributes::GetFontName(char * pchOutFontName) const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	pchOutFontName[0] = '\0';

	if (pcImpl->m_font_name) {
		strcpy(pchOutFontName, pcImpl->m_font_name);
	}
}

void TextAttributes::SetFontName(char const * pchInFontName)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	delete[] pcImpl->m_font_name;

	size_t font_name_size = strlen(pchInFontName);

	pcImpl->m_font_name = new char[font_name_size + 1];
	strcpy(pcImpl->m_font_name, pchInFontName);
}

void TextAttributes::GetOrientation(PMI::Orientation & cOutOrientation) const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	cOutOrientation = pcImpl->m_orientation;
}

void TextAttributes::SetOrientation(Orientation const & cInOrientation)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_orientation = cInOrientation;
}

void TextAttributes::GetRGBColor(TDF::RGBColor & cOutColor) const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	cOutColor = pcImpl->m_color;
}

void TextAttributes::SetRGBColor(TDF::RGBColor const & cInColor)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_color = cInColor;
}

bool TextAttributes::IsBold() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x02) == 0x02;
}

bool TextAttributes::IsItalic() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x04) == 0x04;
}
bool TextAttributes::IsUnderlined() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x08) == 0x08;
}
bool TextAttributes::IsStrikedThrough() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x10) == 0x10;
}
bool TextAttributes::IsOverlined() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x20) == 0x20;
}
bool TextAttributes::IsStreched() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x40) == 0x40;
}
bool TextAttributes::IsWired() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x80) == 0x80;
}
bool TextAttributes::IsFixedWidth() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;

	return (pcImpl->m_format & 0x100) == 0x100;
}

void TextAttributes::SetFormat(char const & chInFormat)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_format = chInFormat;
}

double TextAttributes::WidthScale() const
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	return pcImpl->m_width_scale;
}

void TextAttributes::SetWidthScale(const double dWidthScale)
{
	TextAttributesPrivate * pcImpl = (TextAttributesPrivate *)m_pcImpl;
	pcImpl->m_width_scale = static_cast<float>(dWidthScale);
}

//== Options =======================================================================================
class OptionsPrivate : public TDF::PrivateImpl
{
public:
	OptionsPrivate()
	{
		m_bDisplayParallelToScreen = false;
	}
	~OptionsPrivate()
	{
	}

	void Copy(OptionsPrivate * that)
	{
		m_bDisplayParallelToScreen = that->m_bDisplayParallelToScreen;
	}

	bool m_bDisplayParallelToScreen;
};

Options::Options()
{
	m_pcImpl = new OptionsPrivate();
}

Options::Options(Options const & cInThat)
{
	m_pcImpl = new OptionsPrivate();
	Set(cInThat);
}

void Options::Set(Options const & cInThat)
{
	OptionsPrivate * pcImpl = (OptionsPrivate *)m_pcImpl;
	OptionsPrivate * pcInThatImpl = (OptionsPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Options const & Options::operator=(Options const & cInThat)
{
	Set(cInThat);
	return *this;
}


bool Options::IsDisplayParallelToScreen() const
{
	OptionsPrivate * pcImpl = (OptionsPrivate *)m_pcImpl;
	return pcImpl->m_bDisplayParallelToScreen;
}

void Options::SetDisplayParallelToScreen(const bool in_parallel)
{
	OptionsPrivate * pcImpl = (OptionsPrivate *)m_pcImpl;
	pcImpl->m_bDisplayParallelToScreen = in_parallel;
}

//== Frame =========================================================================================

class FramePrivate : public TDF::PrivateImpl
{
public:
	void Copy(FramePrivate * that)
	{
		m_aPolylines.Copy(that->m_aPolylines);
	}

	TDF::PolylineArray	m_aPolylines;
};

Frame::Frame()
{
	m_pcImpl = new FramePrivate();
}

Frame::Frame(Frame const & cInThat)
{
	m_pcImpl = new FramePrivate();
	Set(cInThat);
}

void Frame::Set(Frame const & cInThat)
{
	FramePrivate * pcImpl = (FramePrivate *)m_pcImpl;
	FramePrivate * pcInThatImpl = (FramePrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Frame const & Frame::operator=(Frame const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int Frame::GetPolylineCount() const
{
	FramePrivate * pcImpl = (FramePrivate *)m_pcImpl;
	return static_cast<unsigned int>(pcImpl->m_aPolylines.GetCount());
}

void Frame::GetPolylines(unsigned int & nOutCount, TDF::Polyline * pcOutPolylines) const
{
	FramePrivate * pcImpl = (FramePrivate *)m_pcImpl;

	nOutCount = (unsigned int)pcImpl->m_aPolylines.GetCount();

	if (nullptr == pcOutPolylines) {
		return;
	}

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPolylines[i] = pcImpl->m_aPolylines[i];
	}
}

void Frame::SetPolylines(unsigned int nInCount, TDF::Polyline const * pcInPolylines)
{
	FramePrivate * pcImpl = (FramePrivate *)m_pcImpl;
	pcImpl->m_aPolylines.SetCount(nInCount);

	for (unsigned int nIndex = 0; nIndex < nInCount; nIndex++) {
		pcImpl->m_aPolylines[nIndex]= pcInPolylines[nIndex];
	}
}

//== Drawing =======================================================================================

class DrawingPrivate : public TDF::PrivateImpl
{
public:
	void Copy(DrawingPrivate * that)
	{
		m_polygons.Copy(that->m_polygons);
	}

	TDF::PolygonArray m_polygons;
};

Drawing::Drawing()
{
	m_pcImpl = new DrawingPrivate();
}

Drawing::Drawing(Drawing const & cInThat)
{
	m_pcImpl = new DrawingPrivate();
	Set(cInThat);
}

void Drawing::Set(Drawing const & cInThat)
{
	DrawingPrivate * pcImpl = (DrawingPrivate *)m_pcImpl;
	DrawingPrivate * pcInThatImpl = (DrawingPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Drawing const & Drawing::operator=(Drawing const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int Drawing::GetPolygonCount() const
{
	DrawingPrivate * pcImpl = (DrawingPrivate *)m_pcImpl;
	return static_cast<unsigned int>(pcImpl->m_polygons.GetCount());
}

void Drawing::GetPolygons(unsigned int & nOutCount, TDF::Polygon * pcOutPolygons) const
{
	DrawingPrivate * pcImpl = (DrawingPrivate *)m_pcImpl;
	nOutCount = (unsigned int)pcImpl->m_polygons.GetCount();

	if (nullptr == pcOutPolygons) {
		return;
	}

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPolygons[i] = pcImpl->m_polygons[i];
	}
}

void Drawing::SetPolygons(unsigned int nInCount, TDF::Polygon const * pcInPolygons)
{
	DrawingPrivate * pcImpl = (DrawingPrivate *)m_pcImpl;
	pcImpl->m_polygons.SetCount(nInCount);

	for (unsigned int i = 0; i < nInCount; i++)
		pcImpl->m_polygons[i] = pcInPolygons[i];
}