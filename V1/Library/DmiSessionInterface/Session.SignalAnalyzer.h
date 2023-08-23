#pragma once

#include "Json.h"

#include <chrono>

namespace SESSION
{
	using SendSignalFunc = void (*)(const wchar_t *);
	using AssignSendSignalFunc = void (*)(SendSignalFunc);

	class SignalAnalyzer
	{
	public:
		SignalAnalyzer();

		void ExecuteSignal(const wchar_t * pchBuffer);
		void SetSendSignalFunc(SendSignalFunc pcSendSignalFunc);

		SendSignalFunc GetSendSignalFunc() { return m_pcSendSignal; }

	protected:
		bool LoadSessionInterface(const CString & strFilePath);

	private:
		bool m_bIsValid = false;
		HINSTANCE m_hInstance = nullptr;
		DWORD m_nErrorCode = -1;

		SendSignalFunc m_pcSendSignal = nullptr;
		AssignSendSignalFunc m_pcSetReceiver = nullptr;

	protected:
		std::vector<std::string> m_vstrHpsErrMsgVector;
	};
};

extern SESSION::SignalAnalyzer theSignalAnalyzer;