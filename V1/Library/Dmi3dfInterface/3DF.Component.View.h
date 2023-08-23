#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF/3DF.h"

#include "3DF.Signal.h"
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
			bool ExecuteMouseSignal(int nAction, Json::Object & cInObject);

		protected:
			bool LButtonUp(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);
			bool LButtonDown(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);

			bool MButtonUp(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);
			bool MButtonDown(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);

			bool RButtonUp(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);
			bool RButtonDown(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);

			bool MouseMove(H3DF::Canvas * pcCanvas, int nFlags, int x, int y);

			bool MouseWheel(H3DF::Canvas * pcCanvas, int nFlags, int zDelta, int x, int y, Json::Object & cInObject);

		public:
			//:Ken - 20230607
			bool ExecuteKeyboardSignal(int nAction, Json::Object & cInObject);

		private:
			H3DF::Canvas * m_pcCanvas = nullptr;
			H3DF::Model * m_pcHoopsModel = nullptr;
		};
	};
};