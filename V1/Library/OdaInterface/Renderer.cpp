#include "stdafx.h"
#include "Renderer.h"
#include "Application.h"

#include "AbstractViewPE.h"
#include "ColorMapping.h"
#include "DbAbstractViewportData.h"
#include "Ge/GeLine3d.h"
#include "Ge/GePlane.h"
#include "RxVariantValue.h"

#include "File.h"



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



bool Renderer::OnCommand(SignalArgs::Base* pSignal)
{
	SignalArgs::Command* signal = (SignalArgs::Command*)pSignal;

	if (signal->GlobalName == "Open") {
		return OpenFile(signal->Options);
	}
	else {
		RETURN_FALSE;
	}
}



bool Renderer::OnInitialize(SignalArgs::Base* pSignal)
{
	SignalArgs::Initialize* signal = (SignalArgs::Initialize*)pSignal;
	DEBUG_VALID(signal);
	DEBUG_VALID(signal->hWnd);

	// prepare delivery
	m_delivery.ViewId = signal->ViewId;
	m_delivery.SetSender(TheApp.SendSignalToUi);

	m_hWnd = signal->hWnd;

	return OpenFile(signal->FilePath);
}



bool Renderer::OnMouseWheel(SignalArgs::Base* pSignal)
{
	SignalArgs::Mouse* signal = (SignalArgs::Mouse*)pSignal;

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

	return true;
}



bool Renderer::OnPaint(SignalArgs::Base* pSignal)
{
	//SignalArgs::Paint* signal = (SignalArgs::Paint*)pSignal;

	if (m_pDevice.isNull() == false) {
		try {
			m_pDevice->update();
			return true;
		}
		catch (...) {
			RETURN_FALSE;
		}
	}
	else {
		return false;
	}
}



bool Renderer::OnResize(SignalArgs::Base* pSignal)
{
	SignalArgs::Resize* signal = (SignalArgs::Resize*)pSignal;

	if (m_pDevice.isNull() == false && signal->Valid) {
		// Update the client rectangle
		OdGsDCRect rect(OdGsDCPoint(0, signal->Height), OdGsDCPoint(signal->Width, 0));
		m_pDevice->onSize(rect);

		return true;
	}
	else {
		return false;
	}
}

//--------------------------------------------------------------------------------------------------

void Renderer::CloseFile()
{
}



bool Renderer::OpenFile(Json::Object& options)
{
	return OpenFile(options.GetString(SKW_FILEPATH));
}

bool Renderer::OpenFile(CString filePath)
{
	m_filePath = filePath;
	if (m_filePath.IsEmpty()) {
		return true;
	}

	try {
		//:CEHCK
		if (File::GetFileSize(m_filePath) > 1000000) {
			m_delivery.mainFrame.ShowProgress();
			m_delivery.progress.SetMessage(filePath);
		}

		m_delivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Start reading file");
		m_pDatabase = TheApp.readFile(m_filePath.GetBuffer(), true, false);

		OdGiContextForDbDatabase::setDatabase(m_pDatabase);
		enableGsModel(true);

		m_delivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Create rendering device");
		if (CreateDevice(true, true)) {
			m_delivery.mainFrame.HideProgress();
			m_delivery.view.SetValidation();
		}
	} catch (OdError&) {
		m_delivery.mainFrame.HideProgress();
		RETURN_FALSE;
	}

	return true;
}



void Renderer::RedrawWindow(LPRECT lpRect)
{
	::RedrawWindow(m_hWnd, lpRect, nullptr, 0);
}



void Renderer::UpdateWindow()
{
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
	m_pDevice->update();

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
