#include "StdAfx.h"

#include "3DF.Utility.h"

#include "3DF.Segment.h"

using namespace std::chrono;

USING_3DF_NAMESPACE

bool Utility::ChangeSubSegmentColor(SegmentKey & cInTargetSegment, MaterialMappingKit const & cInKit, bool bRecursive)
{
	SegmentKeyArray cSubSegments;

	size_t nSubCount = cInTargetSegment.ShowSubsegments(cSubSegments);

	for (size_t nIndex = 0; nIndex < nSubCount; nIndex++) {
		cSubSegments[nIndex].SetMaterialMapping(cInKit);
		if (true == bRecursive) {
			ChangeSubSegmentColor(cSubSegments[nIndex], cInKit, true);
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

void Utility::Set3DfOptionString(char * chBuffer, char * chOption)
{
	if (0 == strlen(chOption)) {
		return;
	}

	if (0 == strlen(chBuffer)) {
		sprintf(chBuffer, "%s", chOption);
	}
	else {
		sprintf(chBuffer, "%s, %s", chBuffer, chOption);
	}
}

CString Utility::HexStr(DWORD_PTR nValue)
{
	CString strText;
	strText.Format(L"0x%llx", nValue);
	return strText;
}

CString Utility::GetTypeString(Type eType)
{
	CString strText;

	switch (eType)
	{
		case TDF::Type::None:
			strText = L"None";
			break;

		case TDF::Type::GenericMask:
			strText = L"GenericMask";
			break;

		case TDF::Type::SelectionResults:
			strText = L"SelectionResults";
			break;

		case TDF::Type::SelectionItem:
			strText = L"SelectionItem";
			break;

		case TDF::Type::SelectionResultsIterator:
			strText = L"SelectionResultsIterator";
			break;

		case TDF::Type::Kit:
			strText = L"Kit";
			break;

		case TDF::Type::CircleKit:
			strText = L"CircleKit";
			break;

		case TDF::Type::LineKit:
			strText = L"LineKit";
			break;

		case TDF::Type::ShellKit:
			strText = L"ShellKit";
			break;

		case TDF::Type::CameraKit:
			strText = L"CameraKit";
			break;

		case TDF::Type::Key:
			strText = L"Key";
			break;

		case TDF::Type::IncludeKey:
			strText = L"IncludeKey";
			break;

		case TDF::Type::PortfolioKey:
			strText = L"PortfolioKey";
			break;

		case TDF::Type::StyleKey:
			strText = L"StyleKey";
			break;

		case TDF::Type::SegmentKey:
			strText = L"SegmentKey";
			break;

		case TDF::Type::WindowKey:
			strText = L"WindowKey";
			break;

		case TDF::Type::GeometryKey:
			strText = L"GeometryKey";
			break;

		case TDF::Type::ReferenceKey:
			strText = L"ReferenceKey";
			break;

		case TDF::Type::CircleKey:
			strText = L"CircleKey";
			break;

		case TDF::Type::CircularArcKey:
			strText = L"CircularArcKey";
			break;

		case TDF::Type::CircularWedgeKey:
			strText = L"CircularWedgeKey";
			break;

		case TDF::Type::CuttingSectionKey:
			strText = L"CuttingSectionKey";
			break;

		case TDF::Type::CylinderKey:
			strText = L"CylinderKey";
			break;

		case TDF::Type::EllipseKey:
			strText = L"EllipseKey";
			break;

		case TDF::Type::EllipticalArcKey:
			strText = L"EllipticalArcKey";
			break;

		case TDF::Type::InfiniteLineKey:
			strText = L"InfiniteLineKey";
			break;

		case TDF::Type::LineKey:
			strText = L"LineKey";
			break;

		case TDF::Type::PolygonKey:
			strText = L"PolygonKey";
			break;

		case TDF::Type::ShellKey:
			strText = L"ShellKey";
			break;

		case TDF::Type::SphereKey:
			strText = L"SphereKey";
			break;

		case TDF::Type::TextKey:
			strText = L"TextKey";
			break;

		case TDF::Type::Control:
			strText = L"Control";
			break;

		case TDF::Type::SelectionControl:
			strText = L"SelectionControl";
			break;

		case TDF::Type::HighlightControl:
			strText = L"HighlightControl";
			break;

		default:
			strText.Format(L"Unknown Type: %d", (int)eType);
			break;
	}

	return strText;
}