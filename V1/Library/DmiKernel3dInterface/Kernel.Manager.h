#pragma once

#include "Json.h"

#include <map>

namespace KERNEL
{
	class Kernel;

	using SendSignalFunc = void (*)(const wchar_t *);
	using AssignSendSignalFunc = void (*)(SendSignalFunc);

	class Manager
	{
	public:
		Manager();

		void ExecuteSignal(const wchar_t * pchBuffer);
		void SetSendSignalFunc(SendSignalFunc pcSendSignalFunc);

		SendSignalFunc GetSendSignalFunc() { return m_pcSendSignalFunc; }

	private:
		SendSignalFunc m_pcSendSignalFunc = nullptr;

		// 생성되는 Session들을 저장하는 Map 컨테이너
		std::map<int, Kernel *> m_mpcSessions;
	};
};

extern KERNEL::Manager theKernelManager;