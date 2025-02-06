#pragma once

#include "EventDelegator.h"
#include "UserIO.h"

#include "DbGsManager.h"
#include "GiContextForDbDatabase.h"
#include "Gs/Gs.h"

//--------------------------------------------------------------------------------------------------

class CoordConvertor
{
	OdGsLayoutHelperPtr m_pDevice;

public:

	CoordConvertor(void);

	~CoordConvertor(void);

public:

	void Initialize(OdGsLayoutHelperPtr pDevice);

	OdGePoint3d ToEyeToWorld(int x, int y);

	OdGePoint3d ToEyeToWorld(CPoint point);

	bool ToUcsToWorld(OdGePoint3d& wcsPoint);

	OdGePoint3d ToScreenCoord(int x, int y);

	OdGePoint3d ToScreenCoord(CPoint point);

	OdGePoint3d ToScreenCoord(const OdGePoint3d& wcsPoint);

private:

	OdGsViewPtr ActiveView();
};

//--------------------------------------------------------------------------------------------------

class Renderer
	: public EventDelegator
	, public OdGiContextForDbDatabase
{
	friend class CommandParams;

	OdDbDatabasePtr m_pDatabase;
	// Vectorizer device
	OdGsDevicePtr m_pDevice;
	// Drawing background color
	ODCOLORREF m_clrBackground;

	int m_nViewId = -1;
	HWND m_hWnd = nullptr;
	CString m_filePath;

	CoordConvertor m_coordinate;
	UserIO m_io;

public:

	using OdRxObject::operator new;

	using OdRxObject::operator delete;

	void addRef() override {}

	void release() override {}

public:

	Renderer();

	virtual ~Renderer();

	bool PostSignal(SignalArgs::Base* pSignal) override;

	void PostPaintSignal(bool lock = false);

protected: // EventDelegator

	bool OnClose() override { RETURN_FALSE; }
	// WARNING - not UserIO, instance command
	bool OnCommand(SignalArgs::Base* pSignal) override;

	bool OnInitialize(SignalArgs::Base* pSignal) override;

	bool OnMouseWheel(SignalArgs::Base* pSignal) override;

	bool OnPaint(SignalArgs::Base* pSignal) override;

	bool OnResize(SignalArgs::Base* pSignal) override;

public: // Special Command

	bool OpenFile(Json::Object& options);

	bool OpenFile(CString filePath);

	void RedrawWindow(LPRECT lpRect = nullptr);
	// WARNING - first single call
	void UpdateWindow();

public:

	OdDbDatabase* GetDatabase();

	CoordConvertor& GetCoordConvertor();

	Signal::Delivery& GetDelivery();

	OdGsViewPtr GetGsView();

	UserIO& GetUserIO();

public:

	bool CreateDevice(bool recreate, bool zoomExtents);

	const ODCOLORREF* CurrentPalette();

	void Dolly(int x, int y);
};
