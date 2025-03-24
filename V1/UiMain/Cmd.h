#pragma once

#include "Cmd.Resource.h"

//--------------------------------------------------------------------------------------------------

class CmdBase;



#define RunCommand2d(ui, id) \
__super::Run(pView); \
\
ViewWnd->GetDelivery2d()->View.OnCommand(id); \
CtlTaskBar& taskBar = TheApp.GetMainFrame().TaskBarCtl; \
\
Json::Object& uiData = TheAppResources.GetTask(ui); \
TaskPanelCtl = new CtlTaskPanel(id, &uiData); \
TaskPanelCtl->Initialize(&taskBar); \
\
taskBar.SetPanel(TaskPanelCtl); \
taskBar.Show(pView);



#define RunCommand3d(ui, id) \
__super::Run(pView); \
\
ViewWnd->GetDelivery3d()->view.OnCommand(id); \
CtlTaskBar& taskBar = TheApp.GetMainFrame().TaskBarCtl; \
\
Json::Object& uiData = TheAppResources.GetTask(ui); \
TaskPanelCtl = new CtlTaskPanel(id, &uiData); \
TaskPanelCtl->Initialize(&taskBar); \
\
taskBar.SetPanel(TaskPanelCtl); \
taskBar.Show(pView);
