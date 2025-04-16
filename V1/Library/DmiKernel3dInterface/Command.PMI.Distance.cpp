#include "StdAfx.h"

#include "Command.PMI.Distance.h"

#include "Command.Step.h"

#include <3DF/Window.h>
#include <3DF/Line.h>

// 1. Plane을 갖는 Distance Dimension
// 2. ID: MEASURE_3D_CMD_Basic_Distance

using namespace KERNEL;
using namespace KERNEL::Command;

namespace KERNEL
{
	namespace Command
	{
		namespace PMI
		{
			class DistanceStep
			{
			public:
				static void Draw1(Step * pcInStep, H3DF::SegmentKey cInSegment);
				static void Draw2(Step * pcInStep, H3DF::SegmentKey cInSegment);
				static void Draw3(Step * pcInStep, H3DF::SegmentKey cInSegment);
			};
		}
	}
}

// Step에서 받아온 Data를 이용해서 Draw를 수행한다.
void KERNEL::Command::PMI::DistanceStep::Draw1(Step * pcInStep, H3DF::SegmentKey cInSegment)
{
}

void KERNEL::Command::PMI::DistanceStep::Draw2(Step * pcInStep, H3DF::SegmentKey cInSegment)
{
	if (nullptr == pcInStep || false == cInSegment.IsValidate()) {
		DEBUG_STOP;
		return;
	}

	// 저장되어 있는 Event가 1개가 아니면 오류
	if (1 != pcInStep->GetSavedEvents().size()) {
		DEBUG_STOP;
		return;
	}

	H3DF::PointArray acPoints;
	acPoints.resize(2);

	acPoints[0] = pcInStep->GetSavedEvents()[0].GetMouseWorldPoint();
	acPoints[1] = pcInStep->GetEvent().GetMouseWorldPoint();

	H3DF::LineKey cLine = cInSegment.InsertLine(acPoints.size(), acPoints.data());
	int i = 0;
}

void KERNEL::Command::PMI::DistanceStep::Draw3(Step * pcInStep, H3DF::SegmentKey cInSegment)
{
	if (nullptr == pcInStep || false == cInSegment.IsValidate()) {
		DEBUG_STOP;
		return;
	}

	// 저장되어 있는 Event가 1개가 아니면 오류
	if (1 != pcInStep->GetSavedEvents().size()) {
		DEBUG_STOP;
		return;
	}

	H3DF::PointArray acPoints;
	acPoints.resize(2);

	acPoints[0] = pcInStep->GetSavedEvents()[0].GetMouseWorldPoint();
	acPoints[1] = pcInStep->GetEvent().GetMouseWorldPoint();

	H3DF::LineKey cLine = cInSegment.InsertLine(acPoints.size(), acPoints.data());
	int i = 0;
}


// == Distance Class ===============================================================================

KERNEL::Command::PMI::Distance::Distance(const Session * pcInSession) :
	Set(pcInSession)
{
	Step * pcStep1 = new Step();
	pcStep1->SetInputType(Step::InputType::Coordinate);
	pcStep1->SetMessage(L"Input first coordiate/첫번째 좌표 입력");
	pcStep1->SetInformation(L"Enter the first coordinate for the distance dimension./거리 치수를 위한 첫번째 좌표를 입력하세요.");
	pcStep1->SetDrawFunction(DistanceStep::Draw1);
	PushBack(pcStep1);
 
	Step * pcStep2 = new Step();
	pcStep2->SetInputType(Step::InputType::Coordinate);
	pcStep2->SetMessage(L"Input second coordiate/두번째 좌표 입력");
	pcStep2->SetInformation(L"Complete the distance dimension by entering a second coordinate./두번째 좌표를 입력해서 거리 치수를 완성하세요.");
	pcStep2->SetDrawFunction(DistanceStep::Draw2);
	PushBack(pcStep2);

	Step * pcStep3 = new Step();
	pcStep2->SetInputType(Step::InputType::Coordinate);
	pcStep2->SetMessage(L"Input third coordiate/세번째 좌표 입력");
	pcStep2->SetInformation(L"Complete the distance dimension by entering a third coordinate./세번째 좌표를 입력해서 거리 치수를 완성하세요.");
	pcStep2->SetDrawFunction(DistanceStep::Draw3);
	PushBack(pcStep2);
}

KERNEL::Command::Type KERNEL::Command::PMI::Distance::GetType() const
{
	return KERNEL::Command::Type::PMI_Distance;
}
