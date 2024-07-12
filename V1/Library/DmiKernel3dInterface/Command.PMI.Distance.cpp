#include "StdAfx.h"

#include "Command.PMI.Distance.h"

#include "Command.Step.h"

#include <3DF/Window.h>

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
				static void Draw1(Step * pcInStep);
				static void Draw2(Step * pcInStep);
			};
		}
	}
}

// Step에서 받아온 Data를 이용해서 Draw를 수행한다.
void KERNEL::Command::PMI::DistanceStep::Draw1(Step * pcInStep)
{
	int i = 0;
	//Window::Draw::Text(pcInStep->GetMessage(), pcInStep->GetPoint());
}

void KERNEL::Command::PMI::DistanceStep::Draw2(Step * pcInStep)
{
	int j = 0;
	//Window::Draw::Text(pcInStep->GetMessage(), pcInStep->GetPoint());
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
}

KERNEL::Command::Type KERNEL::Command::PMI::Distance::GetType() const
{
	return KERNEL::Command::Type::PMI_Distance;
}
