#pragma once

//--------------------------------------------------------------------------------------------------

namespace Command
{
	class Base;
	// 3D
	class VisualEffects3d; // Base
}

#define RunCommand(ui, id) \
__super::Run(pView); \
\
pView->GetDelivery().view.OnCommand(id); \
Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar(); \
\
Json::Object& uiData = TheAppResources.GetTask(ui); \
m_pPanel = new Control::TaskPanel(id, &uiData); \
m_pPanel->Initialize(&taskBar); \
\
taskBar.SetPanel(m_pPanel); \
taskBar.Show(pView); \
