#include "StdAfx.h"

#include "3DF.Utility.h"

#include "3DF.Segment.h"

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