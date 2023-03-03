#include "StdAfx.h"

#include "3DF.Utility.h"

#include "3DF.Segment.h"

using namespace std::chrono;

USING_3DF_NAMESPACE

bool Utility::ChangeSubSegmentColor(SegmentKey & cInTargetSegment, CString strGeometry, MaterialMappingKit const & cInKit, bool bRecursive)
{
	SegmentKeyArray cSubSegments;

	size_t nSubCount = cInTargetSegment.ShowSubsegments(cSubSegments);

	for (size_t nIndex = 0; nIndex < nSubCount; nIndex++) {
		cSubSegments[nIndex].SetMaterialMapping(strGeometry, cInKit);
		if (true == bRecursive) {
			ChangeSubSegmentColor(cSubSegments[nIndex], strGeometry, cInKit, true);
		}
	}

	return true;
}

CString Utility::GetTimeSpanString(milliseconds & ms)
{
	auto secs = duration_cast<seconds>(ms);
	ms -= duration_cast<milliseconds>(secs);
	auto mins = duration_cast<minutes>(secs);
	secs -= duration_cast<seconds>(mins);

	CString strSpanText;

	if (0 < mins.count()) {
		strSpanText.Format(L"%dm %d.%ds", mins.count(), secs.count(), (int)(ms.count() / 10));
	}
	else {
		strSpanText.Format(L"%d.%ds", secs.count(), (int)(ms.count() / 10));
	}

	return strSpanText;
}