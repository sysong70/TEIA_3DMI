#include "StdAfx.h"

#include "3DF.Tracer.h"

#include "Impl/3DF.Impl.h"

#include "Segment.h"
#include "3DF.Utility.h"


#include <hc.h>
#include <HTools.h>

#include "../stdafx.h"

using namespace H3DF;

void H3DF::Tracer::CreateLog(CString strFileName)
{
	if (true == strFileName.IsEmpty()) {
		return;
	}

	CString strFilePathName = LogManager::GetExecuteDirectory() + L"Log\\" + strFileName;

	LogManager::SetCurrentId(LOG_3DF_TRACE_ID);
	LogManager::ResetTabIndex(LOG_3DF_TRACE_ID);
	LogManager::SetFilePathName(LOG_3DF_TRACE_ID, strFilePathName);
	LogManager::SetCreateFile(LOG_3DF_TRACE_ID, true);
	LogManager::SetWriteLog(LOG_3DF_TRACE_ID, true);

	LogManager::SetWriteTimeLog(LOG_3DF_TRACE_ID, true);
	LOG(strFilePathName);
	LogManager::SetWriteTimeLog(LOG_3DF_TRACE_ID, false);
}

void H3DF::Tracer::ContentsLog(HC_KEY nInKey, bool bInOpenFlag)
{
	HC_KEY nKey = nInKey;
	if(INVALID_KEY == nInKey) {
		nKey = HC_Open_Segment("/");
	}

	H3DF::Type eType = H3DF::Utility::GetType(nKey);

	if (H3DF::Type::SegmentKey == eType) {
		SegmentLog(nKey, bInOpenFlag);
	}
	else {
		CString strType = H3DF::Utility::GetTypeString(eType);
		LOG(LOG_3DF_TRACE_ID, L"%s: [%d]", strType, nKey);
	}

	if (INVALID_KEY == nInKey) {
		HC_Close_Segment();
	}
}

//== Segement 관련 Log 함수 =========================================================================
void H3DF::Tracer::SegmentLog(HC_KEY nInKey, bool bInOpenFlag)
{
	SegmentKey cSegment(nInKey);

	CStringA strName = cSegment.Name(false);

	if ("driver" == strName) {
		return;
	}

	int nCount = 0;

	if(true == bInOpenFlag) {
		cSegment.Open();
	}

	HC_Begin_Contents_Search(".", "everything");
	{
		HC_Show_Contents_Count(&nCount);

		LOG(LOG_3DF_TRACE_ID, "Segment: '%s' [%d], Child Count: %d", strName, nInKey, nCount);

		LogIncTab(LOG_3DF_TRACE_ID);

// 		CStringA strOption;
// 		HC_Show_Heuristics(strOption.GetBuffer(MVO_BUFFER_SIZE));
// 		LOG(LOG_3DF_TRACE_ID, "Heuristics: '%s'", strOption);
// 		strOption.ReleaseBuffer();

		HC_KEY nChildKey;
		char chType[MVO_BUFFER_SIZE];

		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			HC_Find_Contents(chType, &nChildKey);

			if (0 == strcmp("segment", chType)) {
				SegmentLog(nChildKey);
			}
			else if (0 == strcmp("style", chType)) {
				StyleLog(nChildKey);
			}
			else if (0 == strcmp("glyph definitions", chType)) {
				GlyphDefinitionsLog();
			}
			else if (0 == strcmp("shape definitions", chType)) {
				ShapeDefinitionsLog();
			}
			else if (0 == strcmp("text font", chType)) {
				TextFontLog();
			}
			else if (0 == strcmp("text", chType)) {
 				TextLog(nChildKey);
 			}
			else if (0 == strcmp("visibility", chType)) {
				VisibilityLog();
			}
			else if (0 == strcmp("color", chType)) {
				ColorLog();
			}
 			else if (0 == strcmp("rendering options", chType)) {
 				RenderingOptionsLog();
 			}
			else if (0 == strcmp("heuristics", chType)) {
				HeuristicsLog();
			}
			else if (0 == strcmp("line weight", chType)) {
				LineWeightLog();
			}
			else if (0 == strcmp("window pattern", chType)) {
				WindowPatternLog();
			}
			else if (0 == strcmp("camera", chType)) {
				CameraLog();
			}
			else if (0 == strcmp("handedness", chType)) {
				HandednessLog();
			}
			else if (0 == strcmp("text alignment", chType)) {
				TextAlignmentLog();
			}
			else if (0 == strcmp("window frame", chType)) {
				WindowFrameLog();
			}
			else if (0 == strcmp("window", chType)) {
				WindowLog();
			}
			else {
				LOG(LOG_3DF_TRACE_ID, "%d: %d, %s", nIndex, nChildKey, chType);
			}
		}

		LogDecTab(LOG_3DF_TRACE_ID);
	}
	HC_End_Contents_Search();

	if(true == bInOpenFlag) {
		cSegment.Close();
	}
}

void H3DF::Tracer::PortfoliosSegmentLog(SegmentKey & cInSegment)
{
	LogIncTab(LOG_3DF_TRACE_ID);

	SegmentKeyArray acChildren;
	cInSegment.ShowSubsegments(acChildren);

	for (auto & cChild : acChildren) {
		SegmentLog(cChild.KeyValue());
	}

	LogDecTab(LOG_3DF_TRACE_ID);
}

void H3DF::Tracer::StyleLog(HC_KEY nInKey)
{
	StyleKey cStyle(nInKey);

	Style::Type cType;
	SegmentKey cSegment;
	CStringA strName;

	if (true == cStyle.ShowSource(cType, cSegment, strName)) {
		CStringA strText;
		
		CStringA strType;
		switch (cType)		
		{
		case H3DF::Style::Type::Segment:
			strType = "Segment";
			break;
		case H3DF::Style::Type::Named:
			strType = "Named";
			break;
		case H3DF::Style::Type::None:
			strType = "None";
			break;
		}

		CStringA strSegmentName = cSegment.Name(false);

		LOG(LOG_3DF_TRACE_ID, "Style: '%s', Source: '%s' [%s]", strName, strSegmentName, strType);
	}
	else {
		LOG("Style '%s'", cStyle.Name(false));
	}
}

void H3DF::Tracer::GlyphDefinitionsLog()
{
	HC_Begin_Glyph_Search();
	{
		int nCount = 0;
		HC_Show_Glyph_Count(&nCount);

		LOG(LOG_3DF_TRACE_ID, "Glyph Definitions: Count [%d]", nCount);

		LogIncTab(LOG_3DF_TRACE_ID);

		if (nCount > 0) {
			char chBuffer[MVO_BUFFER_SIZE];

			while (HC_Find_Glyph(chBuffer)) {
				CStringA strName(chBuffer);
				int nSize = 0;
				HC_Show_Glyph_Size(strName, &nSize);

				HC_Show_Glyph(strName, chBuffer);

				CStringA strData;

				for (int nIndex = 0; nIndex < nSize; nIndex++) {
					if (nIndex > 0) {
						strData.AppendFormat(" ");
					}
					strData.AppendFormat("%d", chBuffer[nIndex]);
				}

				LOG(LOG_3DF_TRACE_ID, "'%s': %s", strName, strData);
			}
		}

		LogDecTab(LOG_3DF_TRACE_ID);
	}
	HC_End_Glyph_Search();

	return;
}

void H3DF::Tracer::ShapeDefinitionsLog()
{
	HC_Begin_Shape_Search();
	{
		int nCount = 0;
		HC_Show_Shape_Count(&nCount);

		LOG(LOG_3DF_TRACE_ID, "Shape Definitions: Count [%d]", nCount);

		LogIncTab(LOG_3DF_TRACE_ID);

		if (nCount > 0) {
			std::vector<float> vfData;
			char chBuffer[MVO_BUFFER_SIZE];

			while (HC_Find_Shape(chBuffer))	{
				CStringA strName(chBuffer);
				int nSize = 0;
				HC_Show_Shape_Size(strName, &nSize);

				vfData.resize(nSize);
				HC_Show_Shape(strName, vfData.data());

				CStringA strData;

				for (int nIndex = 0; nIndex < nSize; nIndex++) {
					if (nIndex > 0) {
						strData.AppendFormat(", ");
					}
					strData.AppendFormat("%f", vfData[nIndex]);
				}

				strName.Replace("_5f", "_");

				LOG(LOG_3DF_TRACE_ID, "'%s': %s", strName, strData);

//				found.insert(name);
			}
		}

		LogDecTab(LOG_3DF_TRACE_ID);
	}
	HC_End_Shape_Search();

	return;
}

void H3DF::Tracer::TextFontLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Text_Font(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Text Font: '%s'", pchBuffer);
}

void H3DF::Tracer::TextLog(HC_KEY nInKey)
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	
	char pchBuffer[MVO_BUFFER_SIZE];

	HC_Show_Text(nInKey, &x, &y, &z, pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Text: '%s' [%f, %f, %f]", pchBuffer, x, y, z);

	LogIncTab(LOG_3DF_TRACE_ID);

	pchBuffer[0] = '\0';
	HC_Open_Geometry(nInKey); {
		HC_Show_Text_Font(pchBuffer);
		if (0 < strlen(pchBuffer)) {
			LOG(LOG_3DF_TRACE_ID, "Text Font: '%s'", pchBuffer);
		}
	} HC_Close_Geometry();

	LogDecTab(LOG_3DF_TRACE_ID);
}

void H3DF::Tracer::VisibilityLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Visibility(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Visibility: '%s'", pchBuffer);
}

void H3DF::Tracer::ColorLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Color(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Color: '%s'", pchBuffer);
}

void H3DF::Tracer::RenderingOptionsLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Rendering_Options(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Rendering Options: '%s'", pchBuffer);
}

void H3DF::Tracer::HeuristicsLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Heuristics(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Heuristics: '%s'", pchBuffer);
}

void H3DF::Tracer::LineWeightLog()
{
	float fLineWeight = 0;
	HC_Show_Line_Weight(&fLineWeight);
	LOG(LOG_3DF_TRACE_ID, "Line Weight: %d", fLineWeight);
}

void H3DF::Tracer::WindowPatternLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Window_Pattern(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Window Pattern: '%s'", pchBuffer);
}

void H3DF::Tracer::CameraLog()
{
	HPoint position, target;
	HVector up;
	float width, height;
	char projection[MVO_BUFFER_SIZE];

	HC_Show_Camera(&position, &target, &up, &width, &height, projection);

	LOG(LOG_3DF_TRACE_ID, "Camera: Position[%f,%f,%f], Target[%f,%f,%f], Up[%f,%f,%f], Width[%f], Height[%f], Projection[%s]", position.x, position.y, position.z, target.x, target.y, target.z, 
		up.x, up.y, up.z, width, height, projection);
}

void H3DF::Tracer::HandednessLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Handedness(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Handedness: '%s'", pchBuffer);
}

void H3DF::Tracer::TextAlignmentLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Text_Alignment(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Text Alignmen: '%s'", pchBuffer);
}

void H3DF::Tracer::WindowLog()
{
	float left, right, bottom, top;
	HC_Show_Window(&left, &right, &bottom, &top);
	LOG(LOG_3DF_TRACE_ID, "Window : Left[%f], Right[%f], Bottom[%f], Top[%f]", left, right, bottom, top);
}

void H3DF::Tracer::WindowFrameLog()
{
	char pchBuffer[MVO_BUFFER_SIZE];
	HC_Show_Window_Frame(pchBuffer);
	LOG(LOG_3DF_TRACE_ID, "Window Frame: '%s'", pchBuffer);
}







