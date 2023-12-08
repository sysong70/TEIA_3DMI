#pragma once

#include "Dialog.Base.h"
#include "Control.h"



namespace Dialog
{
	class Standard : public Base
	{
		DECLARE_DYNAMIC(Standard)

	public:

		// dialogName in Json
		Standard(CStringA dialogName, CWnd* pParent = nullptr);

		Standard(UINT nIDTemplate, CStringA dialogName, CWnd* pParent = nullptr);

		~Standard() override;

	protected:

		BOOL OnInitDialog() override;

		DECLARE_MESSAGE_MAP()

	protected:

		// dialog structure information from TheResource
		Json::Object* m_pDialogData;

		Json::Object& GetDefaultButtons();

		Json::Object& GetUiData();

		CSize GetWinSize();

		CString GetWinTitle();
	};
}
