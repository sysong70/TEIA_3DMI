#pragma once

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		using SendSignalFunc = void (*)(const wchar_t *);
		using AssignSendSignalFunc = void (*)(SendSignalFunc);

		class Session
		{
		public:
			Session();
			~Session();

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
		};
	}
}

extern SESSION::Manager::Session theSessionManager;