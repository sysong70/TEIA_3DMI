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

//**************************************************************************************************

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
	OdGsClientViewInfo viewInfo;
	pView->clientViewInfo(viewInfo);

	OdRxObjectPtr pObj = OdDbObjectId(viewInfo.viewportObjectId).openObject();
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
	return m_pDevice->activeView();
}

//**************************************************************************************************

Renderer::Renderer()
{
	// TEST
	//m_clrBackground = RGB(0x3b, 0x44, 0x53);
	m_clrBackground = RGB(0x21, 0x28, 0x30);
}



Renderer::~Renderer()
{
	m_io.Terminate();
}



bool Renderer::PostSignal(SignalArgs::Base* pSignal)
{
	DEBUG_VALID(pSignal);

	// TODO - filter signal
	if (pSignal->Target == Signal::Target::View) {
		switch ((Signal::View::Action)pSignal->Action) {
		case Signal::View::Action::OnCommand:
		case Signal::View::Action::OnInitialize:
		case Signal::View::Action::OnMouseWheel:
		case Signal::View::Action::OnPaint:
		case Signal::View::Action::OnResize:
			return EventDelegator::PostSignal(pSignal);

		default:
			break;
		}
	}

	return m_io.PostSignal(pSignal);
}



void Renderer::PostPaintSignal(bool lock)
{
	auto signal = new SignalArgs::Paint(m_nViewId);
	if (lock) {
		SendSignal(signal);
	}
	else {
		PostSignal(signal);
	}
}



bool Renderer::OnCommand(SignalArgs::Base* pSignal)
{
	SignalArgs::Command& signal = *(SignalArgs::Command*)pSignal;
	// TODO - UNDO, REDO, make id
	if (signal.Id == CUSTOM_3D_CMD_KEN_Test1) {
		if (m_pDatabase->hasUndoMark()) {
			m_pDatabase->undoBack();

			RedrawWindow();
		}
	}
	else if (signal.Id == CUSTOM_3D_CMD_KEN_Test2) {
		if (m_pDatabase->hasRedo()) {
			m_pDatabase->redo();

			RedrawWindow();
		}
	}
	else {
		return m_io.OnCommand(pSignal);
	}

	return true;
}



bool Renderer::OnInitialize(SignalArgs::Base* pSignal)
{
	SignalArgs::Initialize* signal = (SignalArgs::Initialize*)pSignal;
	DEBUG_VALID(signal);
	DEBUG_VALID(signal->hWnd);

	m_nViewId = signal->ViewId;
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

	RedrawWindow();

	return true;
}



bool Renderer::OnPaint(SignalArgs::Base* pSignal)
{
	SignalArgs::Paint* signal = (SignalArgs::Paint*)pSignal;

	if (m_pDevice.isNull() == false) {
		try {
			if (m_pDevice->isValid() == false) {
				OdGsDCRect rect(signal->Left, signal->Right, signal->Top, signal->Bottom);
				m_pDevice->update(&rect);
			}
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
		// CHECK
		if (File::GetFileSize(m_filePath) > 1000000) {
			GetDelivery().mainFrame.ShowProgress();
			GetDelivery().progress.SetMessage(filePath);
		}

		GetDelivery().progress.AddLog(Signal::Progress::Status::Succeed, L"Start reading file");
		m_pDatabase = TheApp.readFile(m_filePath.GetBuffer(), true, false);

		OdGiContextForDbDatabase::setDatabase(m_pDatabase);
		enableGsModel(true);

		GetDelivery().progress.AddLog(Signal::Progress::Status::Succeed, L"Create rendering device");
		if (CreateDevice(true, true)) {
			// WARNING - do not set this in constructor (device, gsview)
			m_io.SetRenderer(this);

			GetDelivery().mainFrame.HideProgress();
			GetDelivery().view.SetValidation();
		}
	}
	catch (const OdError& err) {
		GetDelivery().progress.AddLog(Signal::Progress::Status::Fail, (LPCTSTR)err.description().c_str());
		RETURN_FALSE;
	}

	return true;
}



void Renderer::RedrawWindow(LPRECT lpRect)
{
	if (m_pDevice.isNull()) {
		DEBUG_STOP;
	}

	if (m_pDevice->isValid() == false) {
		if (lpRect != nullptr) {
			OdGsDCRect rect(lpRect->left, lpRect->right, lpRect->bottom, lpRect->top);
			m_pDevice->update(&rect);
		}
		else {
			m_pDevice->update();
		}
	}
}



void Renderer::UpdateWindow()
{
	::InvalidateRect(m_hWnd, nullptr, TRUE);
	::RedrawWindow(m_hWnd, nullptr, nullptr, 0);
}

//--------------------------------------------------------------------------------------------------

OdDbDatabase* Renderer::GetDatabase()
{
	return m_pDatabase.get();
}



CoordConvertor& Renderer::GetCoordConvertor()
{
	return m_coordinate;
}



Signal::Delivery& Renderer::GetDelivery()
{
	return TheApp.GetDelivery(m_nViewId);
}



OdGsViewPtr Renderer::GetGsView()
{
	return m_pDevice->viewAt(0);
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
	if (pProperties->has(OD_T("WindowHWND"))) {
		pProperties->putAt("WindowHWND", OdRxVariantValue((OdIntPtr)m_hWnd));
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

	m_coordinate.Initialize(m_pDevice);

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
