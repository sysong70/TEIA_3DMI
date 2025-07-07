#pragma once

#include "3DF.h"

namespace H3DF
{
	namespace Tracer
	{
		void CreateLog(CString strFileame);

		void ContentsLog(HC_KEY nInKey);
		
		//== Segement 관련 Log 함수 =================================================================
		void SegmentLog(HC_KEY nInKey);
		void PortfoliosSegmentLog(SegmentKey & cInSegment);

		void StyleLog(HC_KEY nInKey);
		void ShapeDefinitionsLog();

		void TextLog(HC_KEY nInKey);
		void TextFontLog();
	}
}