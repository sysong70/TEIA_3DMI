#include "stdafx.h"
#include "Renderer.h"
#include "Application.h"

#include "AbstractViewPE.h"
#include "ColorMapping.h"
#include "DbAbstractViewportData.h"
#include "Ge/GeLine3d.h"
#include "Ge/GePlane.h"
#include "RxVariantValue.h"



const double ZOOM_FACTOR = 0.8;



CoordConvertor::CoordConvertor()
{
}



CoordConvertor::~CoordConvertor(void)
{
	m_pDevice = NULL;
}



void CoordConvertor::Initialize(OdGsLayoutHelperPtr pDevice)
{
	m_pDevice = pDevice;
}



OdGePoint3d CoordConvertor::ToEyeToWorld(int x, int y)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return OdGePoint3d();
	}

	OdGePoint3d wcsPt(x, y, 0.0);

	if (pView->isPerspective()) {
		wcsPt.z = pView->projectionMatrix()(2, 3);
	}

	wcsPt.transformBy((pView->screenMatrix() * pView->projectionMatrix()).inverse());

	wcsPt.z = 0.0;
	wcsPt.transformBy(OdAbstractViewPEPtr(pView)->eyeToWorld(pView));

	return wcsPt;
}

OdGePoint3d CoordConvertor::ToEyeToWorld(CPoint point)
{
	return ToEyeToWorld(point.x, point.y);
}



bool CoordConvertor::ToUcsToWorld(OdGePoint3d& wcsPt)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return false;
	}

	OdGsClientViewInfo vi;
	ActiveView()->clientViewInfo(vi);

	OdDbObjectPtr pVpObj = OdDbObjectId(vi.viewportObjectId).safeOpenObject();
	OdDbAbstractViewportDataPtr pAVD(pVpObj);

	OdGePoint3d ucsOrigin;
	OdGeVector3d ucsXAxis;
	OdGeVector3d ucsYAxis;

	pAVD->getUcs(pVpObj, ucsOrigin, ucsXAxis, ucsYAxis);
	OdGePlane plane;
	plane.set(ucsOrigin, ucsXAxis, ucsYAxis);

	OdGeLine3d line(wcsPt, OdAbstractViewPEPtr(pView)->direction(pView));

	return plane.intersectWith(line, wcsPt);
}



OdGePoint3d CoordConvertor::ToScreenCoord(int x, int y)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return OdGePoint3d();
	}

	OdGePoint3d scrPt(x, y, 0.0);
	scrPt.transformBy((pView->screenMatrix() * pView->projectionMatrix()).inverse());
	scrPt.z = 0.0;

	return scrPt;
}

OdGePoint3d CoordConvertor::ToScreenCoord(const OdGePoint3d& wcsPt)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return OdGePoint3d();
	}

	OdGePoint3d scrPt(wcsPt);
	OdGsClientViewInfo viewInfo;
	pView->clientViewInfo(viewInfo);

	OdRxObjectPtr pObj = OdDbObjectId(viewInfo.viewportObjectId).openObject();
	OdAbstractViewPEPtr pVp(pObj);

	OdGeVector3d vecY = pVp->upVector(pObj);
	OdGeVector3d vecZ = pVp->direction(pObj);
	OdGeVector3d vecX = vecY.crossProduct(vecZ).normal();
	OdGeVector2d offset = pVp->viewOffset(pObj);
	OdGePoint3d prTarg = pVp->target(pObj) - vecX * offset.x - vecY * offset.y;

	scrPt.x = vecX.dotProduct(wcsPt - prTarg);
	scrPt.y = vecY.dotProduct(wcsPt - prTarg);
	scrPt.z = 0.0;

	return scrPt;
}

OdGePoint3d CoordConvertor::ToScreenCoord(CPoint point)
{
	return ToScreenCoord(point.x, point.y);
}



OdGsViewPtr CoordConvertor::ActiveView()
{
	return m_pDevice->activeView();
}



Renderer::Renderer()
{
	m_clrBackground = RGB(0x3b, 0x44, 0x53);
}



Renderer::~Renderer()
{
}



void Renderer::PostPaintSignal(bool lock)
{
	auto signal = new SignalArgs::Paint(m_viewId);
	if (lock) {
		SendSignal(signal);
	}
	else {
		PushSignal(signal);
	}
}



void Renderer::OnCommand(SignalArgs::Command* signal)
{
	if (signal->GlobalName == "Open") {
		OpenFile(signal->Options);
	}
	else {
		DEBUG_STOP;
	}
}



bool Renderer::OnInitialize(SignalArgs::Initialize* signal)
{
	DEBUG_VALID(signal);
	DEBUG_VALID(signal->hWnd);

	// prepare delivery
	m_delivery.ViewId = signal->ViewId;
	m_delivery.SetSender(TheApp.SendSignalToUi);

	m_hWnd = signal->hWnd;

	OpenFile(signal->FilePath);

	return true;
}



void Renderer::OnLButtonDown(SignalArgs::Mouse* signal)
{}



void Renderer::OnLButtonUp(SignalArgs::Mouse* signal)
{}



void Renderer::OnMButtonDown(SignalArgs::Mouse* signal)
{}



void Renderer::OnMButtonUp(SignalArgs::Mouse* signal)
{}



void Renderer::OnRButtonDown(SignalArgs::Mouse* signal)
{}



void Renderer::OnRButtonUp(SignalArgs::Mouse* signal)
{}



void Renderer::OnMouseMove(SignalArgs::Mouse* signal)
{}



void Renderer::OnMouseWheel(SignalArgs::Mouse* signal)
{
	OdGsViewPtr pView = GetGsView();
	OdGePoint3d position(pView->position());
	position.transformBy(pView->worldToDeviceMatrix());

	int x = (int)OdRound(position.x);
	int y = (int)OdRound(position.y);

	x = signal->X - x;
	y = signal->Y - y;

	Dolly(-x, -y);
	pView->zoom(signal->Delta > 0 ? ZOOM_FACTOR : 1.0 / ZOOM_FACTOR);
	Dolly(x, y);

	m_pDevice->update();
}



void Renderer::OnPaint(SignalArgs::Paint* signal)
{
	if (m_pDevice.isNull() == false) {
		try {
			m_pDevice->update();
		}
		catch (...) {
			DEBUG_STOP;
		}
	}
}



void Renderer::OnResize(SignalArgs::Resize* signal)
{
	if (m_pDevice.isNull() == false && signal->Valid) {
		// Update the client rectangle
		OdGsDCRect rect(OdGsDCPoint(0, signal->Height), OdGsDCPoint(signal->Width, 0));
		m_pDevice->onSize(rect);
		//m_pDevice->update();
	}
}



void Renderer::OnText(SignalArgs::Text* signal)
{}

//--------------------------------------------------------------------------------------------------

void Renderer::CloseFile()
{
}



void Renderer::OpenFile(Json::Object& options)
{
	OpenFile(options.GetString(SKW_FILEPATH));
}

void Renderer::OpenFile(CString filePath)
{
	m_filePath = filePath;

	if (m_filePath.IsEmpty()) {
		DEBUG_RETURN;
	}

	try {
		m_pDatabase = TheApp.readFile(m_filePath.GetBuffer(), true, false);
	} catch (OdError&) {
		DEBUG_RETURN;
	}

	OdGiContextForDbDatabase::setDatabase(m_pDatabase);
	enableGsModel(true);

	if (CreateDevice(true, true) == false) {
		DEBUG_RETURN;
	}

	m_delivery.view.SetValidation();
}



void Renderer::RedrawWindow(LPRECT lpRect)
{
	::RedrawWindow(m_hWnd, lpRect, nullptr, 0);
}

//--------------------------------------------------------------------------------------------------

bool Renderer::CreateDevice(bool recreate, bool zoomExtents)
{
	CRect rc;
	::GetClientRect(m_hWnd, &rc);

	// Load the vectorization module
	OdGsModulePtr pGs = ::odrxDynamicLinker()->loadModule(OdWinOpenGLModuleName);
	if (pGs.isNull()) {
		RETURN_FALSE;
	}

	// Create a new OdGsDevice object, and associate with the vectorization GsDevice
	m_pDevice = pGs->createDevice();
	if (m_pDevice.isNull()) {
		RETURN_FALSE;
	}

	// Return a pointer to the dictionary entity containing the device properties
	OdRxDictionaryPtr pProperties = m_pDevice->properties();

	// Set the window handle for this GsDevice
	pProperties->putAt("WindowHWND", OdRxVariantValue((OdIntPtr)m_hWnd));

	// Define a device coordinate rectangle equal to the client rectangle
	OdGsDCRect gsRect(rc.left, rc.right, rc.bottom, rc.top);

	// Set the device background color and palette
	m_pDevice->setBackgroundColor(m_clrBackground);
	m_pDevice->setLogicalPalette(CurrentPalette(), 256);

	if (database() == nullptr) {
		RETURN_FALSE;
	}

	// Set up the views for the active layout
	m_pDevice = OdDbGsManager::setupActiveLayoutViews(m_pDevice, this);

	// Return true if and only the current layout is a paper space layout
	BOOL bModelSpace = (m_pDatabase->getTILEMODE() == 0);

	// Set the viewport border properties
	//SetViewportBorderProperties(m_pDevice, !bModelSpace);

	//if (zoomExtents) {
	//	ViewZoomExtents();
	//}

	// Update the client rectangle
	OdGsDCRect rect(OdGsDCPoint(rc.left, rc.bottom), OdGsDCPoint(rc.right, rc.top));
	m_pDevice->onSize(rect);

	// Redraw the window
	RedrawWindow();

	return true;
}



const ODCOLORREF* Renderer::CurrentPalette()
{
	return ::odcmAcadPalette(m_clrBackground);
}



void Renderer::Dolly(int x, int y)
{
	// Get the view
	OdGsViewPtr pView = GetGsView();
	// Set up the dolly vector
	OdGeVector3d Vector(-x, -y, 0.0);
	Vector.transformBy((pView->screenMatrix() * pView->projectionMatrix()).inverse());
	// Perform the dolly
	pView->dolly(Vector);
}



OdGsViewPtr Renderer::GetGsView()
{
	return m_pDevice->viewAt(0);
}
