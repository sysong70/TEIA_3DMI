#pragma once

#include "Application.h"
#include "View.h"

#include "Json.h"

#include <map>

namespace KERNEL
{
	namespace Interface
	{
		using SendSignalFunc = void (*)(const wchar_t *);

		class Session
		{
		public:
			Session();
			~Session();

			void ExecuteSignal(const wchar_t * pchBuffer);
			void SetSendSignalFunc(SendSignalFunc pcSendSignalFunc);

			SendSignalFunc GetSendSignalFunc() { return m_pcSendSignalFunc; }

		protected:
			void ExecuteViewSignal(Json::Object & cObject);

		private:
			bool m_bIsValid = false;
			HINSTANCE m_hInstance = nullptr;
			DWORD m_nErrorCode = -1;

			SendSignalFunc m_pcSendSignalFunc = nullptr;

			KERNEL::Application m_cApplication;

			// 생성되는 View들을 저장하는 Map 컨테이너
			std::map<int, KERNEL::View *> m_mpcViews;
		};
	}
}

extern KERNEL::Interface::Session theSessionInterface;