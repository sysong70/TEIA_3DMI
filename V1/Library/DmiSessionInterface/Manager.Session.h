#pragma once

#include "Manager.Root.h"

#include "Json.h"
#include <map>

namespace SESSION
{
	namespace Manager
	{
		using SendSignalFunc = void (*)(const wchar_t *);
		using AssignSendSignalFunc = void (*)(SendSignalFunc);

		class Session : public Root
		{
		public:
			Session();
			~Session();

			Manager::Type Type() override;

			void ExecuteSignal(const wchar_t * pchBuffer);
			void SetSendSignalFunc(SendSignalFunc pcSendSignalFunc);

			SendSignalFunc GetSendSignalFunc() { return m_pcSendSignalTo3dKernel; }

		protected:
			bool Load3dKernelInterface(const CString & strFilePath);
			void Free3dKernelInterface();

		private:
			bool m_bIsValid = false;
			HINSTANCE m_hInstance = nullptr;
			DWORD m_nErrorCode = -1;

			SendSignalFunc m_pcSendSignalTo3dKernel = nullptr;
			AssignSendSignalFunc m_pcSetReceiverFrom3dKernel = nullptr;
		};
	}
}

extern SESSION::Manager::Session theSessionManager;