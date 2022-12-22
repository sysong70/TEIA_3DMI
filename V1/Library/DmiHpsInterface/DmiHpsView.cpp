#include "stdafx.h"

#include "DmiHpsView.h"

#include "sprk.h"

#include "DmiModelHandler.h"
#include "DmiHpsInterface.h"

#include "Operator/DmiOrbitSelectOperator.h"

#define LONG_DASH_LN_PAT		"LongDashLinePattern"
#define DOTTED_LN_PAT			"DottedLinePattern"
#define DASHED_LN_PAT			"DashedLinePattern"
#define DASH_DOT_LN_PAT			"DashDotLinePattern"
#define DASH_2DOT_LN_PAT		"Dash2DotLinePattern"


using namespace HPS;

DmiHpsView::DmiHpsView(DmiHpsInterface * pcHpsInterface, DWORD_PTR nId) :
	m_pcHpsInterface(pcHpsInterface),
	m_nId(nId)
{
	m_hWnd = nullptr;
	m_bCapsLockState = IsCapsLockOn();

	CreatePortfolio();
}

DmiHpsView::~DmiHpsView()
{
	m_cCanvas.Delete();
}

// 명령어를 상위 Command Analyzer로 전달전달 받은 명령어 처리
bool DmiHpsView::SendExecuteCommand(Json::Object & cCommand)
{
	if(nullptr == m_pcHpsInterface) {
		return false;
	}

	cCommand.SetDwordPtr("Id", m_nId);

//	m_pcHpsInterface->SendSignal(cCommand.ToString().GetBuffer());
	return true;
}

//== HPS 관련 함수 ===================================================================================

// 1. HPS 초기화 부분 : #HPS
bool DmiHpsView::InitialUpdate(HWND hWnd, DmiModelHandler * pcModelHandler)
{
	m_hWnd = hWnd;

	if(HPS::Type::None == m_cCanvas.Type())
	{
		// Setup to use the DX11 driver
		HPS::ApplicationWindowOptionsKit cWindowOpts;
		cWindowOpts.SetDriver(HPS::Window::Driver::Default3D);

		// Create our Sprockets Canvas with the specified driver
		m_cCanvas = HPS::Factory::CreateCanvas(reinterpret_cast<HPS::WindowHandle>(hWnd), "dmi_canvas", cWindowOpts);
		
		// Create a new Sprockets View and attach it to our Sprockets.Canvas
 		HPS::View cView = HPS::Factory::CreateView("dmi_view");
		//HPS::View cView = pcModelHandler->GetCADModel().ActivateDefaultCapture().FitWorld();
 		m_cCanvas.AttachViewAsLayout(cView);

		//pcModelHandler->GetCADModel().GetModel().GetSegmentKey().GetPerformanceControl().SetStaticModel(HPS::Performance::StaticModel::Attribute);
		//SetupSceneDefaults(pcModelHandler->GetCADModel().GetModel());

		SetupSceneDefaults(pcModelHandler->GetModel());
	}

	// m_cCanvas.Update(HPS::Window::UpdateType::Refresh);

	// #HPS: Model Key 설정
	m_cModelKey = pcModelHandler->GetModel().GetSegmentKey();

	// Line Style, Highlight Style등을 정의
	CreatePortfolio();

	return true;
}

// 2. 기본 Scene값을 설정
void DmiHpsView::SetupSceneDefaults(HPS::Model const & cModel)
{
	if(true == cModel.Empty()) {
		return;
	}

	// Attach created model
	GetCanvas().GetFrontView().AttachModel(cModel);

	// Subscribe _errorHandler to handle errors
	HPS::Database::GetEventDispatcher().Subscribe(m_cErrorHandler, HPS::Object::ClassID<HPS::ErrorEvent>());

	// Subscribe _warningHandler to handle warnings
	HPS::Database::GetEventDispatcher().Subscribe(m_cWarningHandler, HPS::Object::ClassID<HPS::WarningEvent>());

	HPS::View cView = m_cCanvas.GetFrontView();

	//m_cCanvas.GetWindowKey().GetSelectionOptionsControl().SetAlgorithm(HPS::Selection::Algorithm::Visual);

	// Frame Rate를 사용하지 않음.
 	m_cCanvas.SetFrameRate(0);
// 
	cView.GetSegmentKey().GetDrawingAttributeControl().SetWorldHandedness(Drawing::Handedness::Right);

	cView.GetSegmentKey().GetCullingControl().SetExtent(2).SetDeferralExtent(100);
	cView.GetSegmentKey().GetCullingControl().SetDeferralExtent(100);
	cView.GetSegmentKey().GetCullingControl().SetBackFace(true);

	// #HPS-Option : Windows 배경 color 설정 
	cView.GetSegmentKey().GetMaterialMappingControl().SetWindowColor(HPS::RGBAColor((float)(59 / 255.0), (float) (68 / 255.0), (float) (83 / 255.0), 1.0f));

// 	cView.GetSegmentKey().sHOWWI GetMaterialMappingControl().ShowWindowColor(out_type, out_rgba_color, out_value);
// 	cView.GetSegmentKey().GetMaterialMappingControl().ShowWindowContrastColor(out_type, out_rgba_color, out_value);

	//cView.GetSegmentKey().GetSubwindowControl().SetBackground(HPS::Subwindow::Background::SolidColor);

	//cView.GetSegmentKey().GetMaterialMappingControl().SetWindowColor(HPS::RGBAColor(1, 0, 0, 0.5));// .SetWindowContrastColor(HPS::RGBColor(1, 0, 0)); // SetWindowColor(HPS::RGBAColor(1.0f, 1.0f, 1.0f, 1.0f));
 	//cView.GetSegmentKey().GetSubwindowControl().SetBackground(HPS::Subwindow::Background::SolidColor);

	/*cView.GetSegmentKey().GetSubwindowControl().Set*/

/*
	cView.GetSegmentKey().GetPortfolioControl().Set(HPS::PortfolioKey)
		GetDrawingAttributeControl().set
		.GetVisualEffectsControl().SetEyeDomeLightingBackColor(HPS::RGBColor(1, 0, 0));
*/

	//----- Camera control -----
	cView.GetSegmentKey().GetCameraControl().SetProjection(HPS::Camera::Projection::Orthographic);

	// 최초 시작 View 방향 설정. (순차적으로 쌓아가는 것이 가능함) #HPS-Option
	// in_theta The number of degrees to move the the camera position to the right.
	// in_phi The number of degrees to move the the camera position up.
// 	cView.GetSegmentKey().GetCameraControl().Orbit(0, 90);
// 	cView.GetSegmentKey().GetCameraControl().Orbit(45, 0);
// 	cView.GetSegmentKey().GetCameraControl().Orbit(0, 35);

	//cView.GetSegmentKey().GetCameraControl().SetField(1000, 1000);

	cView.GetNavigationCubeControl().SetVisibility(true);
	cView.GetNavigationCubeControl().SetInteractivity(true);
	//cView.GetNavigationCubeControl().SetLocation(HPS::NavigationCubeControl::Location::Custom, HPS::Rectangle(0.8f, 1.0f, 0.7f, 1.0f));
	cView.GetNavigationCubeControl().SetLocation(HPS::NavigationCubeControl::Location::Custom, HPS::Rectangle(0.7f, 1.0f, 0.7f, 1.0f));

	// Matrix를 이용해서 맞출수는 있는데, Click했을때 작동을 이상하게 함. 연구가 필요함.
	HPS::MatrixKit cMat;
 	cMat.data[0] = 0, cMat.data[1] = -1, cMat.data[2] = 0;
 	cMat.data[4] = 1, cMat.data[5] = 0, cMat.data[6] = 0;
 	cMat.data[8] = 0, cMat.data[9] = 0, cMat.data[10] = 1;
	//HPS::MatrixKit cInvertMat = cMat.Invert();
	cView.GetNavigationCubeControl().GetSegmentKey().SetModellingMatrix(cMat);

// 	cView.GetNavigationCubeControl().SetSize(cView.GetNavigationCubeControl().GetSize() * 0.8f);
// 	HPS::Rectangle cPosition = cView.GetNavigationCubeControl().GetPosition();
// 	cPosition.left = 0.80;
// 	cView.GetNavigationCubeControl().SetPosition(cPosition);

	cView.GetAxisTriadControl().SetVisibility(true).SetLocation(HPS::AxisTriadControl::Location::Custom, HPS::Rectangle(-1.0f, -0.8f, -1.0f, -0.7f));
	cView.GetAxisTriadControl().SetInteractivity(true);

// 	cView.GetAxisTriadControl().SetSize(cView.GetAxisTriadControl().GetSize() * 0.8f);
// 	HPS::Rectangle cPosition1 = cView.GetAxisTriadControl().GetPosition();
// 	cPosition1.right = -0.80;
// 	cView.GetAxisTriadControl().SetPosition(cPosition1);

	cView.GetSegmentKey().GetTransparencyControl().SetAlgorithm(Transparency::Algorithm::DepthPeeling).
		SetDepthPeelingLayers(3).SetMethod(Transparency::Method::Blended);

	SetMainDistantLight();
}

// 3. Selection 관련 SetUp (Building a highlight style) #HPS-Option
// 사용 안함.
void DmiHpsView::SetupSelection()
{
	// Building a highlight style
	m_cSelectPortfolio = HPS::Database::CreatePortfolio();
	m_cModelKey.GetPortfolioControl().Push(m_cSelectPortfolio);

	HPS::NamedStyleDefinition cHighlightStyle = m_cSelectPortfolio.DefineNamedStyle("Dmi_Selection_Style", HPS::Database::CreateRootSegment());
	cHighlightStyle.GetSource().GetMaterialMappingControl().SetFaceColor(HPS::RGBAColor(0.0f, 0.0f, 1.0f));
	cHighlightStyle.GetSource().GetMaterialMappingControl().SetEdgeColor(HPS::RGBAColor(1.0f, 0.0f, 0.0f));
	cHighlightStyle.GetSource().GetVisibilityControl().SetFaces(true).SetEdges(true);

	HPS::View cView = m_cCanvas.GetAttachedLayout().GetAttachedView();

	HPS::HighlightOperator * pcHighlightOperator = new HPS::HighlightOperator();

	// operator becomes 'active' after pushing it onto the operator stack
	cView.GetOperatorControl().Push(pcHighlightOperator);

	// set the highlight style, which we previously defined
	HPS::HighlightOptionsKit cHOK("Dmi_Highlight_Style");

	pcHighlightOperator->SetHighlightOptions(cHOK);
}

// View 업데이트
void DmiHpsView::Update()
{
// 	HPS::UpdateNotifier cUpdateNotifier = m_cCanvas.GetWindowKey().UpdateWithNotifier(HPS::Window::UpdateType::Exhaustive);
// 	cUpdateNotifier.Wait();

	//:Ken - 20220810, check hps_qt_sandbox
	//m_cCanvas.Update(HPS::Window::UpdateType::Exhaustive);
	m_cCanvas.Update(HPS::Window::UpdateType::Refresh);
	//m_cCanvas.GetWindowKey().Update(HPS::Window::UpdateType::Exhaustive);
}

void DmiHpsView::RefreshUpdate()
{
	m_cCanvas.Update(HPS::Window::UpdateType::Refresh);
}

// Highlight된 Select Operator를 해제.
void DmiHpsView::Unhighlight()
{
	HPS::HighlightOptionsKit cHighlightOptions;
	cHighlightOptions.SetStyleName(HIGHLIGHT_STYLE_NAME).SetNotification(true);

	m_cCanvas.GetWindowKey().GetHighlightControl().Unhighlight(cHighlightOptions);
	HPS::Database::GetEventDispatcher().InjectEvent(HPS::HighlightEvent(HPS::HighlightEvent::Action::Unhighlight, HPS::SelectionResults(), cHighlightOptions));
	HPS::Database::GetEventDispatcher().InjectEvent(HPS::ComponentHighlightEvent(HPS::ComponentHighlightEvent::Action::Unhighlight, 
		GetCanvas(), 0, HPS::ComponentPath(), cHighlightOptions));
}

void DmiHpsView::ResizeCanvas(int width, int height)
{
	HPS::UpdateNotifier cUpdateNotifier = m_cCanvas.GetWindowKey().UpdateWithNotifier(HPS::Window::UpdateType::Exhaustive);
	cUpdateNotifier.Wait(); 
}

// Line type등을 생성하는 함수
void DmiHpsView::CreatePortfolio()
{
	m_cPortfolio = HPS::Database::CreatePortfolio();

	// dashed line pattern 생성
	m_cPortfolio.DefineLinePattern(LONG_DASH_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::LongDash));
	m_cPortfolio.DefineLinePattern(DOTTED_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dotted));
	m_cPortfolio.DefineLinePattern(DASHED_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dashed));
	m_cPortfolio.DefineLinePattern(DASH_DOT_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::DashDot));
	m_cPortfolio.DefineLinePattern(DASH_2DOT_LN_PAT, HPS::LinePatternKit::GetDefault(HPS::LinePattern::Default::Dash2Dot));


//	GetModelHandler()->GetModel().GetSeg

/*
	PortfolioKey pKey;
	modelKey.GetPortfolioControl().ShowTop(pKey);
	pKey.DefineGlyph("myGlyph", GlyphKit::GetDefault(Glyph::Default::Plus));

	SegmentKey highlightStyle = modelKey.Subsegment();
	highlightStyle.GetMaterialMappingControl().
		SetFaceColor(RGBAColor(1, 0.5f, 0, 1)).
		SetEdgeColor(RGBAColor(1, 0.5f, 0, 1));
	pKey.DefineNamedStyle("orangeHighlight", highlightStyle);
*/

}

void DmiHpsView::SetSegementLinePattern(SegmentKey & cSegKey, HPS::LinePattern::Default eLinePatter)
{
	char const * pchLinePatternString = nullptr;

	switch((HPS::LinePattern::Default) eLinePatter)
	{
		case HPS::LinePattern::Default::LongDash:
			pchLinePatternString = LONG_DASH_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dotted:
			pchLinePatternString = DOTTED_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dashed:
			pchLinePatternString = DASHED_LN_PAT;
			break;

		case HPS::LinePattern::Default::DashDot:
			pchLinePatternString = DASH_DOT_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dash2Dot:
			pchLinePatternString = DASH_2DOT_LN_PAT;
			break;

		default:
			return;
			break;
	}

	HPS::PortfolioControl cPortfolioControl = cSegKey.GetPortfolioControl().Push(m_cPortfolio);
	size_t nCount = cPortfolioControl.GetCount();
	
	cSegKey.GetLineAttributeControl().SetPattern(pchLinePatternString);
}

void DmiHpsView::SetSegementEdgePattern(SegmentKey & cSegKey, HPS::LinePattern::Default eLinePatter)
{
	char const * pchLinePatternString = nullptr;

	switch((HPS::LinePattern::Default) eLinePatter)
	{
		case HPS::LinePattern::Default::LongDash:
			pchLinePatternString = LONG_DASH_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dotted:
			pchLinePatternString = DOTTED_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dashed:
			pchLinePatternString = DASHED_LN_PAT;
			break;

		case HPS::LinePattern::Default::DashDot:
			pchLinePatternString = DASH_DOT_LN_PAT;
			break;

		case HPS::LinePattern::Default::Dash2Dot:
			pchLinePatternString = DASH_2DOT_LN_PAT;
			break;

		default:
			return;
			break;
	}

	HPS::PortfolioControl cPortfolioControl = cSegKey.GetPortfolioControl().Push(m_cPortfolio);
	size_t nCount = cPortfolioControl.GetCount();

	cSegKey.GetEdgeAttributeControl().SetPattern(pchLinePatternString);
}

// == Mouse 관련 함수 ================================================================================

HPS::MouseEvent	DmiHpsView::BuildMouseEvent(HPS::MouseEvent::Action action, HPS::MouseButtons buttons, LONG x, LONG y, UINT flags, size_t click_count, float scalar)
{
	// Convert location to window space
	HPS::Point cPoint(static_cast<float>(x), static_cast<float>(y), 0);
	m_cCanvas.GetWindowKey().ConvertCoordinate(HPS::Coordinate::Space::Pixel, cPoint, HPS::Coordinate::Space::Window, cPoint);

	if(action == HPS::MouseEvent::Action::Scroll) {
		return HPS::MouseEvent(action, scalar, cPoint, MapModifierKeys(flags), click_count);
	}

	return HPS::MouseEvent(action, cPoint, buttons, MapModifierKeys(flags), click_count);
}

// ----- L Button 관련 함수 -----
bool DmiHpsView::LButtonDown(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::ButtonDown, HPS::MouseButtons::ButtonLeft(), x, y, nFlags, 1));
}

bool DmiHpsView::LButtonUp(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::ButtonUp, HPS::MouseButtons::ButtonLeft(), x, y, nFlags, 0));
}

// ----- M Button 관련 함수 -----
bool DmiHpsView::MButtonDown(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::ButtonDown, HPS::MouseButtons::ButtonRight(), x, y, nFlags, 1));
}

bool DmiHpsView::MButtonUp(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::ButtonUp, HPS::MouseButtons::ButtonRight(), x, y, nFlags, 0));
}

// ----- R Button 관련 함수 -----
bool DmiHpsView::RButtonDown(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::ButtonDown, HPS::MouseButtons::ButtonRight(), x, y, nFlags, 1));
}

bool DmiHpsView::RButtonUp(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::ButtonUp, HPS::MouseButtons::ButtonRight(), x, y, nFlags, 0));
}

bool DmiHpsView::MouseMove(UINT nFlags, LONG x, LONG y)
{
	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::Move, HPS::MouseButtons(), x, y, nFlags, 0));
}

bool DmiHpsView::MouseWheel(UINT nFlags, int zDelta, LONG x, LONG y)
{
	// 화면 좌표 변환을 해야 Zoom center가 제대로 나옴.
	HPS::WindowHandle hwnd;
	static_cast<ApplicationWindowKey>(m_cCanvas.GetWindowKey()).GetWindowOptionsControl().ShowWindowHandle(hwnd);

	POINT point;
	point.x = x;
	point.y = y;

	//::ScreenToClient(reinterpret_cast<HWND>(hwnd), &point);

	return m_cCanvas.GetWindowKey().GetEventDispatcher().InjectEvent(
		BuildMouseEvent(HPS::MouseEvent::Action::Scroll, HPS::MouseButtons(), point.x, point.y, nFlags, 0, static_cast<float>(zDelta)));
}

HPS::ModifierKeys DmiHpsView::MapModifierKeys(UINT flags)
{
	HPS::ModifierKeys	modifier;

	// Map shift and control modifiers to HPS::InputEvent modifiers
	if((flags & MK_SHIFT) != 0)
	{
		if(GetAsyncKeyState(VK_LSHIFT) & 0x8000)
			modifier.LeftShift(true);
		else
			modifier.RightShift(true);
	}

	if((flags & MK_CONTROL) != 0)
	{
		if(GetAsyncKeyState(VK_LCONTROL) & 0x8000)
			modifier.LeftControl(true);
		else
			modifier.RightControl(true);
	}

	if(m_bCapsLockState)
		modifier.CapsLock(true);

	return modifier;
}

bool DmiHpsView::ZoomFit()
{
	HPS::View cFrontView = GetCanvas().GetFrontView();
	
//	frontView.SetRenderingMode(Rendering::Mode::PhongWithLines);
	cFrontView.SetRenderingMode(Rendering::Mode::Phong);

	HPS::CameraKit zoomFitCamera;
	if(!m_cZoomToKeyPath.Empty())
	{
		HPS::BoundingKit bounding;
		m_cZoomToKeyPath.ShowNetBounding(true, bounding);
		cFrontView.ComputeFitWorldCamera(bounding, zoomFitCamera);
	}
	else {
		cFrontView.ComputeFitWorldCamera(zoomFitCamera);
	}

	cFrontView.SmoothTransition(zoomFitCamera, 0.0);

/*
	HPS::View frontView = GetCanvas().GetFrontView();
	HPS::CameraKit zoomFitCamera;

	frontView.SetRenderingMode(Rendering::Mode::PhongWithLines);
	//frontView.SetRenderingMode(Rendering::Mode::Wireframe);
	//frontView.SetRenderingMode(Rendering::Mode::Wireframe);
	
	frontView.ComputeFitWorldCamera(zoomFitCamera);
	frontView.SmoothTransition(zoomFitCamera);
	frontView.Update();
*/

	return true;
}

// == Model 관련 함수 ================================================================================
bool DmiHpsView::AttachModelHandler(DmiModelHandler * pcModelHandler)
{
/*
	if(true == pcModelHandler->GetCADModel().Empty()) {
		return false;
	}

	AttachView(pcModelHandler->GetCADModel().ActivateDefaultCapture().FitWorld(), pcModelHandler->GetCADModel());

*/
	return true;
}

bool DmiHpsView::AttachView(HPS::View & cNewView, HPS::CADModel cCadModel)
{
	// 	_preZoomToKeyPathCamera.Reset();
	// 	_zoomToKeyPath.Reset();

	if(false == cCadModel.Empty())
	{
		cCadModel.ResetVisibility(m_cCanvas);
		m_cCanvas.GetWindowKey().GetHighlightControl().UnhighlightEverything();

		m_cModelKey = cCadModel.GetModel().GetSegmentKey();
	}

	//_preZoomToKeyPathCamera.Reset();
	m_cZoomToKeyPath.Reset();

	HPS::View cOldView = m_cCanvas.GetFrontView();
	m_cCanvas.AttachViewAsLayout(cNewView);

	HPS::OperatorPtrArray operators;
	auto cOldViewOperatorCtrl = cOldView.GetOperatorControl();
	auto cNewViewOperatorCtrl = cNewView.GetOperatorControl();
	cOldViewOperatorCtrl.Show(HPS::Operator::Priority::Low, operators);
	cNewViewOperatorCtrl.Set(operators, HPS::Operator::Priority::Low);
	cOldViewOperatorCtrl.Show(HPS::Operator::Priority::Default, operators);
	cNewViewOperatorCtrl.Set(operators, HPS::Operator::Priority::Default);
	cOldViewOperatorCtrl.Show(HPS::Operator::Priority::High, operators);
	cNewViewOperatorCtrl.Set(operators, HPS::Operator::Priority::High);

	SetupSceneDefaults(cCadModel.GetModel());

	//cNewView.SetRenderingMode(Rendering::Mode::Phong);

	// Set default operators.
	SetupDefaultOperators();

	//SetMainDistantLight();

	//GetCanvas().GetFrontView().SetRenderingMode(Rendering::Mode::Phong);
	//GetCanvas().GetFrontView().SetRenderingMode(Rendering::Mode::FastHiddenLine);
//	Rendering::Mode::FastHiddenLine

	cOldView.Delete();

	return true;
}

bool DmiHpsView::AttachView(HPS::View & cNewView)
{
	m_cZoomToKeyPath.Reset();

	HPS::View cOldView = m_cCanvas.GetFrontView();
	m_cCanvas.AttachViewAsLayout(cNewView);

	HPS::OperatorPtrArray operators;
	auto cOldViewOperatorCtrl = cOldView.GetOperatorControl();
	auto cNewViewOperatorCtrl = cNewView.GetOperatorControl();
	cOldViewOperatorCtrl.Show(HPS::Operator::Priority::Low, operators);
	cNewViewOperatorCtrl.Set(operators, HPS::Operator::Priority::Low);
	cOldViewOperatorCtrl.Show(HPS::Operator::Priority::Default, operators);
	cNewViewOperatorCtrl.Set(operators, HPS::Operator::Priority::Default);
	cOldViewOperatorCtrl.Show(HPS::Operator::Priority::High, operators);
	cNewViewOperatorCtrl.Set(operators, HPS::Operator::Priority::High);

	SetMainDistantLight();

	cOldView.Delete();

	return true;
}

void DmiHpsView::ActivateCapture(HPS::ComponentPath & capture_path)
{
	HPS::Exchange::Capture capture = (HPS::Exchange::Capture) (capture_path.Front());
	HPS::View newView = capture.Activate(capture_path);
	auto newViewSegment = newView.GetSegmentKey();
	HPS::CameraKit newCamera;
	newViewSegment.ShowCamera(newCamera);

	newCamera.UnsetNearLimit();
	auto defaultCameraWithoutNearLimit = HPS::CameraKit::GetDefault().UnsetNearLimit();
	if(newCamera == defaultCameraWithoutNearLimit)
	{
		HPS::View oldView = GetCanvas().GetFrontView();
		HPS::CameraKit oldCamera;
		oldView.GetSegmentKey().ShowCamera(oldCamera);

		newViewSegment.SetCamera(oldCamera);
		newView.FitWorld();
	}

	AttachViewWithSmoothTransition(newView);
}

void DmiHpsView::AttachViewWithSmoothTransition(HPS::View & newView)
{
	HPS::View oldView = GetCanvas().GetFrontView();
	HPS::CameraKit oldCamera;
	oldView.GetSegmentKey().ShowCamera(oldCamera);

	auto newViewSegment = newView.GetSegmentKey();
	HPS::CameraKit newCamera;
	newViewSegment.ShowCamera(newCamera);

	AttachView(newView);

	newViewSegment.SetCamera(oldCamera);
	newView.SmoothTransition(newCamera);
}

//== Component Tree Function ===================================================================
void DmiHpsView::OnSelection(DmiHpsComponentTreeItem * pcComponentItem)
{
	if(nullptr == pcComponentItem) {
		assert(false);
		return;
	}

	HPS::ComponentTree::ItemType itemType = pcComponentItem->GetItemType();
	HPS::Component::ComponentType componentType = pcComponentItem->GetComponent().GetComponentType();

	if((itemType == HPS::ComponentTree::ItemType::ExchangeComponent && componentType != HPS::Component::ComponentType::ExchangeFilter)
		|| itemType == HPS::ComponentTree::ItemType::ParasolidComponent
		|| itemType == HPS::ComponentTree::ItemType::DWGComponent)
	{
		if(componentType == HPS::Component::ComponentType::ExchangeView)
		{
			auto capture_path = pcComponentItem->GetPath();
			ActivateCapture(capture_path);
		}
		else if(componentType == HPS::Component::ComponentType::ExchangeDrawingSheet)
		{
			HPS::View newView = HPS::Exchange::Sheet(pcComponentItem->GetComponent()).Activate();
			AttachView(newView);
		}
		else if(pcComponentItem->IsHidden() == false)
		{
			if(componentType != HPS::Component::ComponentType::DWGBlockTable &&
				componentType != HPS::Component::ComponentType::DWGLayerTable &&
				componentType != HPS::Component::ComponentType::DWGLayer)
			{
				Unhighlight();

				pcComponentItem->Highlight();

				Update();
			}
		}
	}
}

//== Operators 관련 함수 =============================================================================
void DmiHpsView::SetupDefaultOperators()
{
	// Orbit is on top and will be replaced when the operator is changed
	GetCanvas().GetFrontView().GetOperatorControl()
		.Push(new HPS::ZoomOperator(MouseButtons::ButtonMiddle()))
		.Push(new HPS::PanOperator(MouseButtons::ButtonRight()))
		.Push(new DmiOrbitSelectOperator(this, MouseButtons::ButtonLeft()))
		.Push(new HPS::MouseWheelOperator(), Operator::Priority::Low);
}

//== Light 관련 함수 =================================================================================
void DmiHpsView::SetMainDistantLight(HPS::Vector const & lightDirection)
{
	HPS::DistantLightKit light;
	light.SetDirection(lightDirection);
	light.SetCameraRelative(true);
	SetMainDistantLight(light);
}

void DmiHpsView::SetMainDistantLight(HPS::DistantLightKit const & light)
{
	// Delete previous light before inserting new one
	if(m_cMainDistantLight.Type() != HPS::Type::None) {
		m_cMainDistantLight.Delete();
	}

	m_cMainDistantLight = GetCanvas().GetFrontView().GetSegmentKey().InsertDistantLight(light);
}