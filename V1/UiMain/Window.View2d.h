#pragma once

#include "Window.View.h"



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

		void ReceiveSignal(Json::Object* pData) override;

	protected:

		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

		DECLARE_MESSAGE_MAP()

	protected:

		Component::LayerPanel m_layerPanel;

		void CreateToolBar() override;

		void CreatePanelTabs() override;
	};
}
