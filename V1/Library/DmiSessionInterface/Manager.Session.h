#pragma once

#include "Manager.Root.h"

#include "../DmiKernel3dInterface/Kernel.Application.h"

#include "Json.h"
#include <map>

namespace SESSION
{
	class Session;

	namespace Manager
	{
		using SendSignalFunc = void (*)(const wchar_t *);

		class Session : public Root
		{
		public:
			Session();
			~Session();

			Manager::Type Type() override;

			void ExecuteSignal(const wchar_t * pchBuffer);
			void SetSendSignalFunc(SendSignalFunc pcSendSignalFunc);

			SESSION::Session * GetSession(HWND hWnd);
			SESSION::Session * GetSession(int nViewId);
			void RemoveSession(int nViewId);

		protected:
			void ExecuteApplicationSignal(Json::Object & cInObject);
			void ExecuteViewSignal(Json::Object & cInObject);
			void ModelPanelSignal(Json::Object & cInObject);
			void ExecuteCommand(Json::Object & cInObject);

		private:
			bool m_bIsValid = false;

			KERNEL::Application m_cApplication;

			std::map<int, SESSION::Session *> m_mpcSessions;

			SendSignalFunc m_pcSendSignal = nullptr;
		};
	}
}

extern SESSION::Manager::Session theSessionManager;