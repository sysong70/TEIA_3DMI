#pragma once

#include "3DF.h"

namespace H3DF
{
	namespace Tracer
	{
		void API_3DF CreateLog(CString strFileame);

		void API_3DF ContentsLog(HC_KEY nInKey, bool bInOpenFlag);
		
		//== Segement 관련 Log 함수 =================================================================
		void SegmentLog(HC_KEY nInKey, bool bInOpenFlag = true);
		void PortfoliosSegmentLog(SegmentKey & cInSegment);

		void StyleLog(HC_KEY nInKey);

		void GlyphDefinitionsLog();

		void ShapeDefinitionsLog();

		void TextLog(HC_KEY nInKey);
		void TextFontLog();

		void VisibilityLog();

		void ColorLog();

		void RenderingOptionsLog();

		void HeuristicsLog();

		void LineWeightLog();

		void WindowPatternLog();

		void CameraLog();

		void HandednessLog();

		void TextAlignmentLog();

		void WindowLog();

		void WindowFrameLog();
	}
}