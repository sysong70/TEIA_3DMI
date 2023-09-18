#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF/3DF.h"

#include "Signal.h"
#include "../Signal/Signal.h"

namespace H3DF
{
	namespace Component
	{
		class API_3DF View
		{
		public:
			void Initialize(Json::Object & cInObject, Signal::Delivery & cInstance);
			void Destruct();
			void Paint(Json::Object & cInObject);
			void Resize(int x, int y);

			void SaveHsfFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);

			void LoadPointCloudFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);

			//== Command 관련 함수 ===========================================================================
			void CancelCommands();

			//== Mouse 관련 함수 =============================================================================
			//bool ExecuteMouseSignal(int nAction, Json::Object & cInObject);

			bool LButtonUp(int nFlags, int x, int y);
			bool LButtonDown(int nFlags, int x, int y);

			bool MButtonUp(int nFlags, int x, int y);
			bool MButtonDown(int nFlags, int x, int y);

			bool RButtonUp(int nFlags, int x, int y);
			bool RButtonDown(int nFlags, int x, int y);

			bool MouseMove(int nFlags, int x, int y);

			bool MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop);

		public:
			//:Ken - 20230607
			bool ExecuteKeyboardSignal(int nAction, Json::Object & cInObject);

		private:
			H3DF::Canvas * m_pcCanvas = nullptr;
			H3DF::Model * m_pcHoopsModel = nullptr;
		};
	};
};