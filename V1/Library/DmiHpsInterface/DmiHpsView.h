#pragma once

#include "Handler/DmiEventHandler.h"

#include "ComponentTree/DmiHpsComponentTree.h"

#include <Json.h>

class DmiModelHandler;
class DmiHpsInterface;

class DmiHpsView
{
public:
	DmiHpsView(DmiHpsInterface * pcHpsInterface, DWORD_PTR nId);
	~DmiHpsView();

	// 명령어를 상위 Command Analyzer로 전달전달 받은 명령어 처리
	bool SendExecuteCommand(Json::Object & cCommand);

	// Returns the single HPS::Canvas attached to our HPS canvas
	HPS::Canvas & GetCanvas() { return m_cCanvas; }
	HWND GetWnd() { return m_hWnd; }

	DmiModelHandler * GetModelHandler() { return m_pcModelHander; }
	void SetModelHandler(DmiModelHandler * pcModelHandler) { m_pcModelHander = pcModelHandler; }

	HPS::SegmentKey & GetModelKey() { return m_cModelKey; }

	// View 관련 함수
	bool InitialUpdate(HWND hWnd, DmiModelHandler * pcModelHandler);
	void Update();
	void RefreshUpdate();

	//:Ken - 20220810, width and height are required for later web images
	void ResizeCanvas(int width = 0, int height = 0);
	
	void CreatePortfolio();
	void SetSegementLinePattern(HPS::SegmentKey & cSegKey, HPS::LinePattern::Default eLinePatter);
 	void SetSegementEdgePattern(HPS::SegmentKey & cSegKey, HPS::LinePattern::Default eLinePatter);

	//== Mouse 관련 함수 =============================================================================
	bool LButtonUp(UINT nFlags, LONG x, LONG y);
	bool LButtonDown(UINT nFlags, LONG x, LONG y);

	bool MButtonUp(UINT nFlags, LONG x, LONG y);
	bool MButtonDown(UINT nFlags, LONG x, LONG y);

	bool RButtonUp(UINT nFlags, LONG x, LONG y);
	bool RButtonDown(UINT nFlags, LONG x, LONG y);

	bool MouseMove(UINT nFlags, LONG x, LONG y);

	bool MouseWheel(UINT nFlags, int zDelta, LONG x, LONG y);
private:
	// Constructs HPS.InputEvent.ModifierKeys from MFC flags
	HPS::ModifierKeys MapModifierKeys(UINT flags);

	// Helper method used to construct a HPS::MouseEvent
	HPS::MouseEvent	BuildMouseEvent(HPS::MouseEvent::Action action, HPS::MouseButtons buttons, LONG x, LONG y, UINT flags, size_t click_count, float scalar = 0);

public:
	//== View control function =====================================================================
	bool ZoomFit();

	bool AttachModelHandler(DmiModelHandler * pcModelHandler); // CADModel�� ���� �Լ�

	bool AttachView(HPS::View & cNewView, HPS::CADModel cCadModel);
	bool AttachView(HPS::View & cNewView);

	void ActivateCapture(HPS::ComponentPath & capture_path);
	void AttachViewWithSmoothTransition(HPS::View & newView);

protected:
	// Sets HPS scene defaults for new and opened documents.
	void SetupSceneDefaults(HPS::Model const & cModel);
	
	// Sets the default operator stack on the current view
	void SetupDefaultOperators();

	// Sets the direction for a camera-relative, colorless, main distant light in the scene.
	void SetMainDistantLight(HPS::Vector const & lightDirection = HPS::Vector(1, 0, -1.5f));

	// Sets a main distant light in the scene.
	void SetMainDistantLight(HPS::DistantLightKit const & light);

	//== Component Tree Function ===================================================================
public:
	void OnSelection(DmiHpsComponentTreeItem * pcComponentItem);

private:
	HWND m_hWnd;

	// HPS canvas associated with this MFC View
	HPS::Canvas m_cCanvas;

	// Error handler associated with this MFC View
	DmiErrorHandler m_cErrorHandler;

	// Warning handler associated with this MFC View
	DmiWarningHandler m_cWarningHandler;

	// KeyPath used for ZoomToSegment
	HPS::KeyPath m_cZoomToKeyPath;

	// Main distant light in Sprockets view.
	HPS::DistantLightKey m_cMainDistantLight;

	HPS::SegmentKey m_cModelKey;

	bool m_bCapsLockState;
	inline bool IsCapsLockOn()
	{
		if((GetKeyState(VK_CAPITAL) & 0x0001) != 0)
			return true;
		return false;
	}

	DmiHpsInterface * m_pcHpsInterface = nullptr;

	DmiHpsComponentTreePtr m_pcComponentTree = nullptr;

	DmiModelHandler * m_pcModelHander = nullptr;

	HPS::PortfolioKey m_cPortfolio;

	// == Selection 관련 ============================================================================
public:
	// Highlight된 Select Operator를 해제.
	void Unhighlight();

	// Selection 관련 SetUp
	void SetupSelection();

private:
	DWORD_PTR m_nId = 0xffffff;
	HPS::PortfolioKey m_cSelectPortfolio;

};
