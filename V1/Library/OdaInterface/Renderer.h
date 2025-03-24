#pragma once

#include "Connector.h"
#include "EventDelegator.h"
#include "UserIO.h"

#include "DbGsManager.h"
#include "GiContextForDbDatabase.h"
#include "Gs/Gs.h"

//--------------------------------------------------------------------------------------------------

class CoordConvertor
{
	OdGsLayoutHelperPtr DevicePtr;

public:

	CoordConvertor(void);

	~CoordConvertor(void);

public:

	void Initialize(OdGsLayoutHelperPtr pDevice);

	OdGePoint3d ToEyeToWorld(int x, int y);

	OdGePoint3d ToEyeToWorld(CPoint point);

	CPoint ToWorldToEye(const OdGePoint3d& wcsPoint);

	bool ToUcsToWorld(OdGePoint3d& wcsPoint);

	OdGePoint3d ToScreenCoord(int x, int y);

	OdGePoint3d ToScreenCoord(CPoint point);

	OdGePoint3d ToScreenCoord(const OdGePoint3d& wcsPoint);

private:

	OdGsViewPtr ActiveView();
};

//--------------------------------------------------------------------------------------------------

class Renderer
	: public OdGiContextForDbDatabase
	, public EventDelegator
{
public: // OdGiContextForDbDatabase

	using OdRxObject::operator new;

	using OdRxObject::operator delete;

	void addRef() override {}

	void release() override {}

public: // EventDelegator

	bool PostSignal(SignalParams* pSignal) override;

	bool SendSignal(SignalParams* pSignal) override;

	bool SendPaintSignal(bool useThread = false, SignalParams* pSignal = nullptr);

protected:

	bool OnClose() override { RETURN_FALSE; }

	bool OnInitialize(SignalParams* pSignal) override;

	bool OnMouseWheel(SignalParams* pSignal) override;

	bool OnPaint(SignalParams* pSignal) override;

	bool OnResize(SignalParams* pSignal) override;

public: // Special Command

	bool OpenFile(Json::Object& options);

	bool OpenFile(CString filePath);

	void RedrawWindow(LPRECT lpRect = nullptr);

public:

	OdDbDatabasePtr DatabasePtr;
	// Vectorizer device
	OdGsDevicePtr GsDevicePtr;
	// Drawing background color
	ODCOLORREF BackgroundColor;

	int ViewId = -1;
	SgnDelivery2d Delivery;
	HWND WindowHandle = nullptr;
	CString FilePath;

	CoordConvertor Coordinate;
	UserIO UserIo;

public:

	Renderer(int viewId, SendSignalFunc fp, bool useThreadIo);

	virtual ~Renderer();

	OdDbDatabase* GetDatabase();

	OdGsViewPtr GetGsView();

public:

	bool CreateDevice(bool recreate, bool zoomExtents);

	const ODCOLORREF* CurrentPalette();

	void Dolly(int x, int y);
};
