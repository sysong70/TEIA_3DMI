#pragma once

#include "Window.View.h"

//--------------------------------------------------------------------------------------------------

namespace Window
{
	class View2d : public View
	{
	protected:

		View2d();

		DECLARE_DYNCREATE(View2d)

	public:

		friend class MainFrame;

		~View2d() override;

		Signal::Delivery& GetDelivery() override;

		void ReceiveSignal(Json::Object* pData) override;

		bool SetContextMenu(Json::Object* pData) override;

		void ShowContextMenu(Json::Object* pData) override;

	protected:

		void OnDraw(CDC* pDC) override;

		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnCommand(UINT id);

		afx_msg void OnContextCommand(UINT id);

		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

		DECLARE_MESSAGE_MAP()

	protected:

		void CreateCommandPrompt() override;

	protected:

		// TEST
		void CreateHistoryBar(Control::EPivot pivot = Control::EPivot::BottomCenter) override {}

		void CreateToolBar() override;

	protected:

		Component::LayerPanel m_layerPanel;

		void CreatePanelTabs() override;
	};
}
