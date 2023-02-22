#pragma once

#include "EventDelegator.h"

#include "DbGsManager.h"
#include "GiContextForDbDatabase.h"
#include "Gs/Gs.h"

#include <atltypes.h>

class OdEdInputTracker;



class CoordConvertor
{
public:

	CoordConvertor(void);

	~CoordConvertor(void);

public:

	void Initialize(OdGsLayoutHelperPtr pDevice);

	OdGePoint3d ToEyeToWorld(int x, int y);
	OdGePoint3d ToEyeToWorld(CPoint point);

	bool ToUcsToWorld(OdGePoint3d& wcsPt);

	OdGePoint3d ToScreenCoord(int x, int y);
	OdGePoint3d ToScreenCoord(CPoint point);
	OdGePoint3d ToScreenCoord(const OdGePoint3d& wcsPt);

private:

	OdGsLayoutHelperPtr m_pDevice;

	OdGsViewPtr ActiveView();
};



class Renderer
	: public EventDelegator
	, public OdGiContextForDbDatabase
{
	HWND m_hWnd;
	int m_viewId;
	CString m_filePath;
	Signal::Delivery m_delivery;

	OdDbDatabasePtr m_pDatabase;

	OdGsDevicePtr m_pDevice;			// Vectorizer device
	ODCOLORREF m_clrBackground;			// Drawing background color
	OdGsView::RenderMode m_eRenderMode;	// Render mode
	bool m_bLeftButton;					// Flag for left mouse button press
	bool m_bMiddleButton;				// Flag for middle mouse button press
	bool m_bRightButton;				// Flag for right mouse button press
	CPoint m_MousePosition;				// Position of mouse pointer
	BOOL m_bZoomWindow;					// Flag for zoom window mode
	OdGePoint3dArray m_Points;			// Mouse clicks
	CPoint m_MouseClick;				// Location of mouse click
	OdEdInputTracker* m_pTracker;		// Input tracker

public:

	using OdRxObject::operator new;

	using OdRxObject::operator delete;

	void addRef() override {}

	void release() override {}

public:

	Renderer();

	virtual ~Renderer();

	void PostPaintSignal(bool lock = false);

protected:

	bool OnClose() override { RETURN_FALSE; }

	bool OnCommand(SignalArgs::Base* pSignal) override;

	bool OnInitialize(SignalArgs::Base* pSignal) override;

	bool OnLButtonDown(SignalArgs::Base* pSignal) override { return false; }

	bool OnLButtonUp(SignalArgs::Base* pSignal) override { return false; }

	bool OnMButtonDown(SignalArgs::Base* pSignal) override { return false; }

	bool OnMButtonUp(SignalArgs::Base* pSignal) override { return false; }

	bool OnRButtonDown(SignalArgs::Base* pSignal) override { return false; }

	bool OnRButtonUp(SignalArgs::Base* pSignal) override { return false; }

	bool OnMouseMove(SignalArgs::Base* pSignal) override { return false; }

	bool OnMouseWheel(SignalArgs::Base* pSignal) override;

	bool OnPaint(SignalArgs::Base* pSignal) override;

	bool OnResize(SignalArgs::Base* pSignal) override;

	bool OnText(SignalArgs::Base* pSignal) override { return false; }

public: // Command

	void CloseFile();

	bool OpenFile(Json::Object& options);

	bool OpenFile(CString filePath);

	void RedrawWindow(LPRECT lpRect = nullptr);
	// first single call
	void UpdateWindow();

public:

	bool CreateDevice(bool recreate, bool zoomExtents);

	const ODCOLORREF* CurrentPalette();

	void Dolly(int x, int y);

	OdGsViewPtr GetGsView();
};
