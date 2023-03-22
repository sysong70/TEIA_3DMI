#pragma once

#include "Layout.Base.h"
#include "Control.h"



namespace Layout
{
	class ControlWrapper : public Base
	{
	public:

		enum class EContent
		{
			Unknown = 0,
			Button,
			Label,
			CheckBox,
			Radio,
			ComboBox,
		};

		ControlWrapper();

		virtual ~ControlWrapper();

		void Content(CWnd* pValue, EContent type);

	protected:

		CWnd* m_pContent = nullptr;
		EContent m_eType = EContent::Unknown;
	};
}
