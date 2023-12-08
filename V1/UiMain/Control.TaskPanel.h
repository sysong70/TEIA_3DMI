#pragma once

#include "Control.h"
#include "Control.PropList.h"
#include "Component.TaskBar.h"
#include <Json.h>



namespace Control
{
	class TaskPanel : public CWnd
	{
	public:

		friend class Component::TaskBar;

		TaskPanel(UINT commandId, Json::Object* pUiData);

		~TaskPanel() override;

		bool Initialize(CWnd* pParentWnd);

	protected:

		virtual void ConstructBody();

		virtual void ConstructFooter();

		virtual void ConstructHeader();

		virtual void OnReset(CBCGPProp* pProp);

	public:

		virtual void SetData(Json::Object& data);

		virtual void SetDefaultData(Json::Object& data);

	public:

		CSize GetSize();

		CString GetTitle();

		Json::Object& GetUiData();

		Component::TaskBar& GetTaskBar();

	protected:

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM wp, LPARAM lp);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);

		DECLARE_MESSAGE_MAP()

	protected:

		Control::PropList m_propList;

		int m_nHeaderHeight = 0;
		int m_nFooterHeight = 0;

		UINT m_commandId = 0;
		Json::Object* m_pUiData = nullptr;

		Json::Object m_data;
		Json::Object m_defaultData;
		bool m_bInitialized = false;
	};
}
