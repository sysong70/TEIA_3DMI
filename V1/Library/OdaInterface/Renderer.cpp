#include "stdafx.h"

#include "Application.h"
#include "Renderer.h"

#include "AbstractViewPE.h"
#include "ColorMapping.h"
#include "DbAbstractViewportData.h"
#include "Ge/GeLine3d.h"
#include "Ge/GePlane.h"
#include "RxVariantValue.h"

#include "File.h"
#include <atltypes.h>

//**************************************************************************************************

CoordConvertor::CoordConvertor()
{
}



CoordConvertor::~CoordConvertor(void)
{
	DevicePtr = NULL;
}



void CoordConvertor::Initialize(OdGsLayoutHelperPtr pDevice)
{
	DevicePtr = pDevice;
}



OdGePoint3d CoordConvertor::ToEyeToWorld(int x, int y)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return OdGePoint3d();
	}

	OdGePoint3d wcsPoint(x, y, 0.0);

	if (pView->isPerspective()) {
		wcsPoint.z = pView->projectionMatrix()(2, 3);
	}

	wcsPoint.transformBy((pView->screenMatrix() * pView->projectionMatrix()).inverse());
	wcsPoint.z = 0.0;
	wcsPoint.transformBy(OdAbstractViewPEPtr(pView)->eyeToWorld(pView));

	return wcsPoint;
}

OdGePoint3d CoordConvertor::ToEyeToWorld(CPoint point)
{
	return ToEyeToWorld(point.x, point.y);
}



CPoint CoordConvertor::ToWorldToEye(const OdGePoint3d& wcsPoint)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return {};
	}

	OdGePoint3d eyePoint = wcsPoint;
	eyePoint.transformBy(pView->worldToDeviceMatrix());

	return CPoint(eyePoint.x, eyePoint.y);
}



bool CoordConvertor::ToUcsToWorld(OdGePoint3d& wcsPoint)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return false;
	}

	OdGsClientViewInfo cvi;
	ActiveView()->clientViewInfo(cvi);

	OdDbObjectPtr pVpObj = OdDbObjectId(cvi.viewportObjectId).safeOpenObject();
	OdDbAbstractViewportDataPtr pAVD(pVpObj);

	OdGePoint3d ucsOrigin;
	OdGeVector3d ucsXAxis;
	OdGeVector3d ucsYAxis;

	pAVD->getUcs(pVpObj, ucsOrigin, ucsXAxis, ucsYAxis);
	OdGePlane plane;
	plane.set(ucsOrigin, ucsXAxis, ucsYAxis);

	OdGeLine3d line(wcsPoint, OdAbstractViewPEPtr(pView)->direction(pView));

	return plane.intersectWith(line, wcsPoint);
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

OdGePoint3d CoordConvertor::ToScreenCoord(const OdGePoint3d& wcsPoint)
{
	OdGsViewPtr pView = ActiveView();
	if (pView.isNull()) {
		return OdGePoint3d();
	}

	OdGePoint3d screenPoint(wcsPoint);
	OdGsClientViewInfo cvi;
	pView->clientViewInfo(cvi);

	OdRxObjectPtr pObj = OdDbObjectId(cvi.viewportObjectId).openObject();
	OdAbstractViewPEPtr pVp(pObj);

	OdGeVector3d vecY = pVp->upVector(pObj);
	OdGeVector3d vecZ = pVp->direction(pObj);
	OdGeVector3d vecX = vecY.crossProduct(vecZ).normal();
	OdGeVector2d offset = pVp->viewOffset(pObj);
	OdGePoint3d prTarg = pVp->target(pObj) - vecX * offset.x - vecY * offset.y;

	screenPoint.x = vecX.dotProduct(wcsPoint - prTarg);
	screenPoint.y = vecY.dotProduct(wcsPoint - prTarg);
	screenPoint.z = 0.0;

	return screenPoint;
}

OdGePoint3d CoordConvertor::ToScreenCoord(CPoint point)
{
	return ToScreenCoord(point.x, point.y);
}



OdGsViewPtr CoordConvertor::ActiveView()
{
	return DevicePtr->activeView();
}

//**************************************************************************************************

bool Renderer::PostSignal(SignalParams* pSignal)
{
	DEBUG_VALID(pSignal);

	// TODO - filter signal
	if (pSignal->Target == Sgn::ETarget::View) {
		switch ((SgnView::Action)pSignal->Action) {
		case SgnView::Action::OnInitialize:
		case SgnView::Action::OnMouseWheel:
		case SgnView::Action::OnPaint:
		case SgnView::Action::OnResize:
			return __super::PostSignal(pSignal);

		default:
			break;
		}
	}

	return UserIo.PostSignal(pSignal);
}



bool Renderer::SendSignal(SignalParams* pSignal)
{
	DEBUG_VALID(pSignal);
	bool process = false;

	if (pSignal->Target == Sgn::ETarget::View) {
		switch ((SgnView::Action)pSignal->Action) {
		case SgnView::Action::OnInitialize:
			return __super::PostSignal(pSignal);

		case SgnView::Action::OnMouseWheel:
			OnMouseWheel(pSignal);
			process = true;
			break;

		case SgnView::Action::OnPaint:
			OnPaint(pSignal);
			process = true;
			break;

		case SgnView::Action::OnResize:
			OnResize(pSignal);
			process = true;
			break;

		default:
			break;
		}

		if (process == false) {
			return UserIo.SendSignal(pSignal);
		}
	}
	else if (pSignal->Target == Sgn::ETarget::UserIO) {
		return UserIo.SendSignal(pSignal);
	}
	else {
		DEBUG_STOP;
	}

	REMOVE_POINTER(pSignal);
	return true;
}



bool Renderer::SendPaintSignal(bool useThread, SignalParams* pSignal)
{
	if (pSignal == nullptr) {
		pSignal = new PaintSignal(ViewId);
	}

	if (useThread) {
		return PostSignal(pSignal);
	}
	else {
		return SendSignal(pSignal);
	}
}

//--------------------------------------------------------------------------------------------------

bool Renderer::OnInitialize(SignalParams* pSignal)
{
	InitializeSignal* signal = (InitializeSignal*)pSignal;
	DEBUG_VALID(signal);
	DEBUG_VALID(signal->WindowHandle);

	ViewId = signal->ViewId;
	WindowHandle = signal->WindowHandle;

	return OpenFile(signal->FilePath);
}



bool Renderer::OnMouseWheel(SignalParams* pSignal)
{
	static const double zoomFactor = 0.8;

	MouseSignal* signal = (MouseSignal*)pSignal;

	OdGsViewPtr pView = GetGsView();
	OdGePoint3d position(pView->position());
	position.transformBy(pView->worldToDeviceMatrix());

	int x = (int)OdRound(position.x);
	int y = (int)OdRound(position.y);

	x = signal->X - x;
	y = signal->Y - y;

	Dolly(-x, -y);
	pView->zoom(signal->Delta > 0 ? zoomFactor : 1.0 / zoomFactor);
	Dolly(x, y);

	//:CHECK
	TrackerBase::SetPixelDensity(GetGsView());
	RedrawWindow();

	return true;
}



bool Renderer::OnPaint(SignalParams* pSignal)
{
	PaintSignal& signal = *(PaintSignal*)pSignal;

	if (GsDevicePtr.isNull()) {
		return false;
	}

	CRect rect = signal.GetRect();
	RedrawWindow(rect.IsRectNull() ? nullptr : &rect);

	if (signal.Options.IsEmpty() == false) {
		Delivery.UserIO.SetDynamicInput(signal.Options);
	}

	return true;
}



bool Renderer::OnResize(SignalParams* pSignal)
{
	ResizeSignal* signal = (ResizeSignal*)pSignal;

	if (GsDevicePtr.isNull() == false && signal->Valid) {
		// Update the client rectangle
		OdGsDCRect rect(OdGsDCPoint(0, signal->Height), OdGsDCPoint(signal->Width, 0));
		GsDevicePtr->onSize(rect);

		return true;
	}
	else {
		return false;
	}
}

//--------------------------------------------------------------------------------------------------

bool Renderer::OpenFile(Json::Object& options)
{
	return OpenFile(options.GetString(SKW_FILEPATH));
}

bool Renderer::OpenFile(CString filePath)
{
	FilePath = filePath;
	if (FilePath.IsEmpty()) {
		return true;
	}

	try {
		// CHECK
		if (File::GetFileSize(FilePath) > 1000000) {
			Delivery.MainFrame.ShowProgressBar();
			Delivery.MainFrame.SetProgressMessage(filePath);
		}

		Delivery.MainFrame.AddProgressLog(Sgn::EStatus::Succeed, L"Start reading file");
		DatabasePtr = TheApp.readFile(FilePath.GetBuffer(), true, false);

		OdGiContextForDbDatabase::setDatabase(DatabasePtr);
		enableGsModel(true);

		Delivery.MainFrame.AddProgressLog(Sgn::EStatus::Succeed, L"Create rendering device");
		if (CreateDevice(true, true)) {
			// WARNING - do not set this in constructor (device, gsview)
			UserIo.SetRenderer(this);

			Delivery.MainFrame.HideProgressBar();
			Delivery.View.SetValidation();
		}
	}
	catch (const OdError& err) {
		Delivery.MainFrame.AddProgressLog(Sgn::EStatus::Fail, (LPCTSTR)err.description().c_str());
		RETURN_FALSE;
	}

	return true;
}



void Renderer::RedrawWindow(LPRECT lpRect)
{
	if (GsDevicePtr.isNull()) {
		DEBUG_STOP;
	}

	//if (GsDevicePtr->isValid()) {
	//	return;
	//}

	//:WARNING - OpenGL error
	try {
		if (lpRect != nullptr) {
			OdGsDCRect rect(lpRect->left, lpRect->right, lpRect->top, lpRect->bottom);
			GsDevicePtr->update(&rect);
		}
		else {
			GsDevicePtr->update();
		}
	}
	catch (...) {
		return;
	}

	Delivery.View.PaintOverlap();
}

//--------------------------------------------------------------------------------------------------

Renderer::Renderer(int viewId, SendSignalFunc fp, bool useThreadIo)
	: ViewId(viewId)
	, UserIo(useThreadIo)
{
	Delivery.ViewId = viewId;
	Delivery.SetSender(fp);
	//BackgroundColor = RGB(0x3b, 0x44, 0x53);
	BackgroundColor = RGB(0x21, 0x28, 0x30);

	Create();
}



Renderer::~Renderer()
{
	UserIo.Terminate();
}



OdDbDatabase* Renderer::GetDatabase()
{
	return DatabasePtr.get();
}



OdGsViewPtr Renderer::GetGsView()
{
	return GsDevicePtr->viewAt(0);
}

//--------------------------------------------------------------------------------------------------

bool Renderer::CreateDevice(bool recreate, bool zoomExtents)
{
	CRect rc;
	::GetClientRect(WindowHandle, &rc);

	// Load the vectorization module
	OdGsModulePtr pGs = ::odrxDynamicLinker()->loadModule(OdWinOpenGLModuleName);
	if (pGs.isNull()) {
		RETURN_FALSE;
	}

	// Create a new OdGsDevice object, and associate with the vectorization GsDevice
	GsDevicePtr = pGs->createDevice();
	if (GsDevicePtr.isNull()) {
		RETURN_FALSE;
	}

	// Return a pointer to the dictionary entity containing the device properties
	OdRxDictionaryPtr pProperties = GsDevicePtr->properties();

	// Set the window handle for this GsDevice
	if (pProperties->has(OD_T("WindowHWND"))) {
		pProperties->putAt("WindowHWND", OdRxVariantValue((OdIntPtr)WindowHandle));
	}
	if (pProperties->has(OD_T("DoubleBufferEnabled"))) {
		pProperties->putAt(OD_T("DoubleBufferEnabled"), OdRxVariantValue(true));
	}
	if (pProperties->has(OD_T("EnableSoftwareHLR"))) {
		pProperties->putAt(OD_T("EnableSoftwareHLR"), OdRxVariantValue(true));
	}
	if (pProperties->has(OD_T("DiscardBackFaces"))) {
		pProperties->putAt(OD_T("DiscardBackFaces"), OdRxVariantValue(true));
	}

	// Set the device background color and palette
	GsDevicePtr->setBackgroundColor(BackgroundColor);
	GsDevicePtr->setLogicalPalette(CurrentPalette(), 256);

	if (database() == nullptr) {
		RETURN_FALSE;
	}

	// Set up the views for the active layout
	GsDevicePtr = OdDbGsManager::setupActiveLayoutViews(GsDevicePtr, this);

	// Return true if and only the current layout is a paper space layout
	BOOL bModelSpace = (DatabasePtr->getTILEMODE() == 0);

	// Set the viewport border properties
	//SetViewportBorderProperties(GsDevicePtr, !bModelSpace);

	//if (zoomExtents) {
	//	ViewZoomExtents();
	//}

	// Update the client rectangle
	OdGsDCRect rect(OdGsDCPoint(rc.left, rc.bottom), OdGsDCPoint(rc.right, rc.top));
	GsDevicePtr->onSize(rect);
	GsDevicePtr->update();

	Coordinate.Initialize(GsDevicePtr);
	TrackerBase::SetPixelDensity(GetGsView());

	return true;
}



const ODCOLORREF* Renderer::CurrentPalette()
{
	return ::odcmAcadPalette(BackgroundColor);
}



void Renderer::Dolly(int x, int y)
{
	// Get the view
	OdGsViewPtr pGsView = GetGsView();
	// Set up the dolly vector
	OdGeVector3d vector(-x, -y, 0.0);
	vector.transformBy((pGsView->screenMatrix() * pGsView->projectionMatrix()).inverse());
	// Perform the dolly
	pGsView->dolly(vector);
}
