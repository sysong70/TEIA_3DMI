#include "StdAfx.h"

#include "3DF.Tracer.h"

#include "Segment.h"
#include "3DF.Utility.h"

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

void H3DF::Tracer::ContentsLog(HC_KEY nInKey)
{
	H3DF::Type eType = H3DF::Utility::GetType(nInKey);

	if (H3DF::Type::SegmentKey == eType) {
		SegmentLog(nInKey);
	}
	else {
		CString strType = H3DF::Utility::GetTypeString(eType);
		LOG(LOG_3DF_TRACE_ID, L"%s: [%d]", strType, nInKey);
	}
}

//== Segement 관련 Log 함수 =========================================================================
void H3DF::Tracer::SegmentLog(HC_KEY nInKey)
{
	SegmentKey cSegment(nInKey);
	cSegment.Open();

	CStringA strName = cSegment.Name(false);

	int nCount = 0;

	HC_Begin_Contents_Search(".", "everything");
	{
		HC_Show_Contents_Count(&nCount);

		LOG(LOG_3DF_TRACE_ID, "Segment: '%s' [%d], Child Count: %d", strName, nInKey, nCount);

		LogIncTab(LOG_3DF_TRACE_ID);

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
			else if (0 == strcmp("shape definitions", chType)) {
				ShapeDefinitionsLog();
			}
			else if (0 == strcmp("text font", chType)) {
				TextFontLog();
			}

// 			else if (0 == strcmp("text", chType)) {
// 				TextLog(nChildKey);
// 			}
			else {
				LOG(LOG_3DF_TRACE_ID, "%d: %d, %s", nIndex, nChildKey, chType);
			}
		}

		LogDecTab(LOG_3DF_TRACE_ID);
	}
	HC_End_Contents_Search();

	cSegment.Close();

	
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
			CStringA strName;
			while (HC_Find_Shape(strName.GetBuffer(MVO_BUFFER_SIZE))) {
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
	CStringA strOption;
	//HC_Show_Text_Font(strOption.GetBuffer(MVO_BUFFER_SIZE));
	HC_Show_One_Text_Font("background", strOption.GetBuffer(MVO_BUFFER_SIZE));
	LOG(LOG_3DF_TRACE_ID, "Text Font: '%s'", strOption);
	strOption.ReleaseBuffer();
}

void H3DF::Tracer::TextLog(HC_KEY nInKey)
{
/*
	HC_Begin_Text_Search();
	{
		int nCount = 0;
		HC_Show_Text_Count(&nCount);
		LOG(LOG_3DF_TRACE_ID, "Text: Count [%d]", nCount);
		LogIncTab(LOG_3DF_TRACE_ID);
		if (nCount > 0) {
			CStringA strName;
			while (HC_Find_Text(strName.GetBuffer(MVO_BUFFER_SIZE))) {
				LOG(LOG_3DF_TRACE_ID, "'%s'", strName);
			}
		}
		LogDecTab(LOG_3DF_TRACE_ID);
	}
	HC_End_Text_Search();*/
}