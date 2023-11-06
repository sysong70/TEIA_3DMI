#pragma once

#include "Window.View.h"



namespace Window
{
	class View3d : public View
	{
	protected:

		View3d();

		DECLARE_DYNCREATE(View3d)

	public:

		friend class MainFrame;

		~View3d() override;

		void ReceiveSignal(Json::Object* pData) override;

	protected:

		afx_msg void OnCommand(UINT id);

		afx_msg void OnUpdateCommand(CCmdUI* pCmdUI);

		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

		DECLARE_MESSAGE_MAP()

	protected:

		Component::ModelPanel m_modelPanel;
		Component::ViewPanel m_viewPanel;
		Component::LayerPanel m_layerPanel;
		Component::ScenePanel m_scenePanel;

		void CreateToolBar() override;

		void CreatePanelTabs() override;
	};
}
