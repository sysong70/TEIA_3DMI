#pragma once

//--------------------------------------------------------------------------------------------------

namespace Component
{
	// MainFrame
	class PanelBar;		// CBCGPDockingControlBar
	class RibbonBar;	// CBCGPRibbonBar
	class StatusBar;	// CBCGPRibbonStatusBar
	class TaskBar;		// CWnd
	// Panel
	class LayerPanel;	// Control::Panel
	class ModelPanel;	// Control::Panel
	class ScenePanel;	// Control::Panel
	class ViewPanel;	// Control::Panel
	// View
	class Input;



	// Custom bar title height
	int BarHeaderHeight();

	int BarFooterHeight();

	int BarPadding();
}
