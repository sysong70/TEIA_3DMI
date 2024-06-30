#pragma once

#include "../Kernel.h"

#include <3DF/Math.h>

#include "../Command.Step.h"

namespace KERNEL
{
	namespace Command
	{
		class StepImpl : public Impl
		{
		public:
			void Copy(StepImpl * pcInThat);

			// 입력받아야 하는 값의 타입
			KERNEL::Command::Step::InputType m_eInputType = KERNEL::Command::Step::InputType::None;
			CString m_strText;
			H3DF::Point m_cPoint;
			H3DF::PointArray m_aPoints;
			float m_fValue = 0.f;

			// UI에 표시할 메시지
			CString m_strMessage; // Message
			CString m_strInformation; // 명령어 정보

			void (*m_pfDrawFunction)(Step * pcInStep) = nullptr;
		};
	}
}