#pragma once

#include "3DF.h"
#include "hc.h"
#include <Json.h>

namespace H3DF
{
	namespace Operator
	{
		class KeyboardTest
		{
		public:

			KeyboardTest(WindowKey * pWindow);

		public:

			void OnKeyboard(Json::Object & input);

		protected:

			WindowKey * m_pWindow = nullptr;
			HC_KEY m_textKey = HC_ERROR_KEY;
			HC_KEY m_cursorKey = HC_ERROR_KEY;

			CString m_text;
			int m_row = 0;
			int m_column = 0;
		};
	}
}
