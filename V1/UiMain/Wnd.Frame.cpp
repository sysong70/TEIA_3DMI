#include "stdafx.h"

#include "Wnd.Frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

//:REF - https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles

///	WS_BORDER
//	The window has a thin-line border.
///	WS_CAPTION
//	The window has a title bar (includes the WS_BORDER style).
///	WS_CHILD
//	The window is a child window.
//	A window with this style cannot have a menu bar.
//	This style cannot be used with the WS_POPUP style.
///	WS_CHILDWINDOW
//	Same as the WS_CHILD style.
///	WS_CLIPCHILDREN
//	Excludes the area occupied by child windows when drawing occurs within the parent window.
//	This style is used when creating the parent window.
///	WS_CLIPSIBLINGS
//	Clips child windows relative to each other;
//	that is, when a particular child window receives a WM_PAINT message,
//	the WS_CLIPSIBLINGS style clips all other overlapping child windows out of the region of the child window to be updated.
//	If WS_CLIPSIBLINGS is not specified and child windows overlap, it is possible,
//	when drawing within the client area of a child window, to draw within the client area of a neighboring child window.
///	WS_DISABLED
//	The window is initially disabled.
//	A disabled window cannot receive input from the user.
// 	To change this after a window has been created, use the EnableWindow function.
///	WS_DLGFRAME
//	The window has a border of a style typically used with dialog boxes.
//	A window with this style cannot have a title bar.
///	WS_GROUP
//	The window is the first control of a group of controls.
//	The group consists of this first control and all controls defined after it, up to the next control with the WS_GROUP style.
//	The first control in each group usually has the WS_TABSTOP style so that the user can move from group to group.
//	The user can subsequently change the keyboard focus from one control in the group to the next control in the group
//	by using the direction keys.
//	You can turn this style on and off to change dialog box navigation.
//	To change this style after a window has been created, use the SetWindowLong function.
///	WS_HSCROLL
//	The window has a horizontal scroll bar.
///	WS_ICONIC
//	The window is initially minimized. Same as the WS_MINIMIZE style.
///	WS_MAXIMIZE
//	The window is initially maximized.
///	WS_MAXIMIZEBOX
//	The window has a maximize button.
//	Cannot be combined with the WS_EX_CONTEXTHELP style.
//	The WS_SYSMENU style must also be specified.
///	WS_MINIMIZE
//	The window is initially minimized.
//	Same as the WS_ICONIC style.
///	WS_MINIMIZEBOX
//	The window has a minimize button.
//	Cannot be combined with the WS_EX_CONTEXTHELP style.
//	The WS_SYSMENU style must also be specified.
///	WS_OVERLAPPED
//	The window is an overlapped window.
//	An overlapped window has a title bar and a border.
//	Same as the WS_TILED style.
///	WS_OVERLAPPEDWINDOW	(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
//	The window is an overlapped window.
//	Same as the WS_TILEDWINDOW style.
///	WS_POPUP
//	The window is a pop-up window.
//	This style cannot be used with the WS_CHILD style.
///	WS_POPUPWINDOW	(WS_POPUP | WS_BORDER | WS_SYSMENU)
//	The window is a pop-up window.
//	The WS_CAPTION and WS_POPUPWINDOW styles must be combined to make the window menu visible.
///	WS_SIZEBOX
//	The window has a sizing border.
//	Same as the WS_THICKFRAME style.
///	WS_SYSMENU
//	The window has a window menu on its title bar.
//	The WS_CAPTION style must also be specified.
///	WS_TABSTOP
//	The window is a control that can receive the keyboard focus when the user presses the TAB key.
//	Pressing the TAB key changes the keyboard focus to the next control with the WS_TABSTOP style.
//	You can turn this style on and off to change dialog box navigation.
//	To change this style after a window has been created, use the SetWindowLong function.
//	For user-created windows and modeless dialogs to work with tab stops,
//	alter the message loop to call the IsDialogMessage function.
///	WS_THICKFRAME
//	The window has a sizing border.
//	Same as the WS_SIZEBOX style.
///	WS_TILED
//	The window is an overlapped window.
//	An overlapped window has a title bar and a border.
//	Same as the WS_OVERLAPPED style.
///	WS_TILEDWINDOW	(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
//	The window is an overlapped window.
//	Same as the WS_OVERLAPPEDWINDOW style.
///	WS_VISIBLE
//	The window is initially visible.
//	This style can be turned on and off by using the ShowWindow or SetWindowPos function.
///	WS_VSCROLL
//	The window has a vertical scroll bar.

//:REF - https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles

///	WS_EX_ACCEPTFILES
//	The window accepts drag-drop files.
///	WS_EX_APPWINDOW
//	Forces a top-level window onto the taskbar when the window is visible.
///	WS_EX_CLIENTEDGE
//	The window has a border with a sunken edge.
///	WS_EX_COMPOSITED
//	Paints all descendants of a window in bottom-to-top painting order using double-buffering.
//	Bottom-to-top painting order allows a descendent window to have translucency (alpha) and transparency (color-key) effects,
//	but only if the descendent window also has the WS_EX_TRANSPARENT bit set.
//	Double-buffering allows the window and its descendents to be painted without flicker.
//	This cannot be used if the window has a class style of CS_OWNDC, CS_CLASSDC, or CS_PARENTDC.
//	Windows 2000: This style is not supported.
///	WS_EX_CONTEXTHELP
//	The title bar of the window includes a question mark.
//	When the user clicks the question mark, the cursor changes to a question mark with a pointer.
//	If the user then clicks a child window, the child receives a WM_HELP message.
//	The child window should pass the message to the parent window procedure,
//	which should call the WinHelp function using the HELP_WM_HELP command.
//	The Help application displays a pop-up window that typically contains help for the child window.
//	WS_EX_CONTEXTHELP cannot be used with the WS_MAXIMIZEBOX or WS_MINIMIZEBOX styles.
///	WS_EX_CONTROLPARENT
//	The window itself contains child windows that should take part in dialog box navigation.
//	If this style is specified, the dialog manager recurses into children of this window
//	when performing navigation operations such as handling the TAB key, an arrow key,or a keyboard mnemonic.
///	WS_EX_DLGMODALFRAME
//	The window has a double border;
//	the window can, optionally, be created with a title bar by specifying the WS_CAPTION style in the dwStyle parameter.
///	WS_EX_LAYERED
//	The window is a layered window.
//	This style cannot be used if the window has a class style of either CS_OWNDC or CS_CLASSDC.
//	Windows 8: The WS_EX_LAYERED style is supported for top-level windows and child windows.
//	Previous Windows versions support WS_EX_LAYERED only for top-level windows.
///	WS_EX_LAYOUTRTL
//	If the shell language is Hebrew, Arabic, or another language
//	that supports reading order alignment,
//	the horizontal origin of the window is on the right edge.
//	Increasing horizontal values advance to the left.
///	WS_EX_LEFT
//	The window has generic left-aligned properties.
//	This is the default.
///	WS_EX_LEFTSCROLLBAR
//	If the shell language is Hebrew, Arabic, or another language that supports reading order alignment,
//	the vertical scroll bar (if present) is to the left of the client area.
//	For other languages, the style is ignored.
///	WS_EX_LTRREADING
//	The window text is displayed using left-to-right reading-order properties.
//	This is the default.
///	WS_EX_MDICHILD
//	The window is a MDI child window.
///	WS_EX_NOACTIVATE
//	A top-level window created with this style does not become the foreground window when the user clicks it.
//	The system does not bring this window to the foreground when the user minimizes or closes the foreground window.
//	The window should not be activated through programmatic access or via keyboard navigation by accessible technology,
//	such as Narrator.
//	To activate the window, use the SetActiveWindow or SetForegroundWindow function.
//	The window does not appear on the taskbar by default. To force the window to appear on the taskbar,
//	use the WS_EX_APPWINDOW style.
///	WS_EX_NOINHERITLAYOUT
//	The window does not pass its window layout to its child windows.
///	WS_EX_NOPARENTNOTIFY
//	The child window created with this style does not send the WM_PARENTNOTIFY message to its parent window
//	when it is created or destroyed.
///	WS_EX_NOREDIRECTIONBITMAP
//	The window does not render to a redirection surface.
//	This is for windows that do not have visible content or that use mechanisms other than surfaces to provide their visual.
///	WS_EX_OVERLAPPEDWINDOW	(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
//	The window is an overlapped window.
///	WS_EX_PALETTEWINDOW	(WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)
//	The window is palette window, which is a modeless dialog box that presents an array of commands.
///	WS_EX_RIGHT
//	The window has generic "right-aligned" properties.
//	This depends on the window class.
//	This style has an effect only if the shell language is Hebrew, Arabic, or another language that supports reading-order alignment;
//	otherwise, the style is ignored.
//	Using the WS_EX_RIGHT style for static or edit controls has the same effect as using the SS_RIGHT or ES_RIGHT style, respectively.
//	Using this style with button controls has the same effect as using BS_RIGHT and BS_RIGHTBUTTON styles.
///	WS_EX_RIGHTSCROLLBAR
//	The vertical scroll bar (if present) is to the right of the client area.
//	This is the default.
///	WS_EX_RTLREADING
//	If the shell language is Hebrew, Arabic, or another language that supports reading-order alignment,
//	the window text is displayed using right-to-left reading-order properties.
//	For other languages, the style is ignored.
///	WS_EX_STATICEDGE
//	The window has a three-dimensional border style intended to be used for items that do not accept user input.
///	WS_EX_TOOLWINDOW
//	The window is intended to be used as a floating toolbar.
//	A tool window has a title bar that is shorter than a normal title bar, and the window title is drawn using a smaller font.
//	A tool window does not appear in the taskbar or in the dialog that appears when the user presses ALT+TAB.
//	If a tool window has a system menu, its icon is not displayed on the title bar.
//	However, you can display the system menu by right-clicking or by typing ALT+SPACE.
///	WS_EX_TOPMOST
//	The window should be placed above all non-topmost windows and should stay above them, even when the window is deactivated.
//	To add or remove this style, use the SetWindowPos function.
///	WS_EX_TRANSPARENT
//	The window should not be painted until siblings beneath the window (that were created by the same thread) have been painted.
//	The window appears transparent because the bits of underlying sibling windows have already been painted.
//	To achieve transparency without these restrictions, use the SetWindowRgn function.
///	WS_EX_WINDOWEDGE
//	The window has a border with a raised edge.

BEGIN_MESSAGE_MAP(WndFrame, CWnd)
	ON_WM_CHAR()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()



bool WndFrame::Create()
{
	static bool Registered = false;
	static CString ClassName(L"DmiCustomFrameWnd");

	if (Registered == false) {
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		{
			wc.style = CS_VREDRAW | CS_HREDRAW;
			wc.lpfnWndProc = AfxWndProc;
			wc.hInstance = AfxGetInstanceHandle();
			wc.hIcon = NULL;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.lpszClassName = ClassName;
		}
		auto result = RegisterClass(&wc);
		ASSERT(result);

		Registered = true;
	}

	DWORD dwStyle = 0;
	DWORD dwExStyle = 0;

	//:CHECK - default value?
	dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	dwStyle |= (WndParams.Popup ? WS_POPUP : WS_CHILD);
	dwStyle |= (WndParams.Visible ? WS_VISIBLE : 0);
	//:WARNING - When WS_EX_TRANSPARENT is used, even the background automatically reflects the window behind it.
	dwExStyle |= (WndParams.Transparent ? WS_EX_TRANSPARENT : 0);
	dwExStyle |= (WndParams.Alpha < 255 ? WS_EX_LAYERED : 0);
	dwExStyle |= (WndParams.TopMost ? WS_EX_TOPMOST : 0);

	BOOL success = CWnd::CreateEx(dwExStyle, ClassName, WndParams.Name, dwStyle, WndParams.Rect, WndParams.ParentWnd, WndParams.Id);
	ASSERT(success);

	if (WndParams.Alpha < 255) {
		SetLayeredWindowAttributes(0, WndParams.Alpha, LWA_ALPHA);
	}

	return success;
}



bool WndFrame::Create(const WindowParams& params)
{
	WndParams = params;
	return Create();
}

//--------------------------------------------------------------------------------------------------

BOOL WndFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//:WARNING - if not, CWnd::CreateEx crash!!!
	cs.hMenu = NULL;

#ifdef _DEBUG
	bool WsBorder       = cs.style & WS_BORDER;
	bool WsCaption      = cs.style & WS_CAPTION;
	bool WsChild        = cs.style & WS_CHILD;
	bool WsClipChildren = cs.style & WS_CLIPCHILDREN;
	bool WsClipSiblings = cs.style & WS_CLIPSIBLINGS;
	bool WsDisabled     = cs.style & WS_DISABLED;
	bool WsDlgFrame     = cs.style & WS_DLGFRAME;
	bool WsGroup        = cs.style & WS_GROUP;
	bool WsHScroll      = cs.style & WS_HSCROLL;
	bool WsMaximize     = cs.style & WS_MAXIMIZE;
	bool WsMinimize     = cs.style & WS_MINIMIZE;
	bool WsOverlapped   = cs.style & WS_OVERLAPPED;
	bool WsPopup        = cs.style & WS_POPUP;
	bool WsSysMenu      = cs.style & WS_SYSMENU;
	bool WsTabStop      = cs.style & WS_TABSTOP;
	bool WsThickFrame   = cs.style & WS_THICKFRAME;
	bool WsVisible      = cs.style & WS_VISIBLE;
	bool WsVScroll      = cs.style & WS_VSCROLL;

	bool ExAcceptFiles         = cs.dwExStyle & WS_EX_ACCEPTFILES;
	bool ExAppWindow           = cs.dwExStyle & WS_EX_APPWINDOW;
	bool ExClientEdge          = cs.dwExStyle & WS_EX_CLIENTEDGE;
	bool ExComposited          = cs.dwExStyle & WS_EX_COMPOSITED;
	bool ExContextHelp         = cs.dwExStyle & WS_EX_CONTEXTHELP;
	bool ExControlParent       = cs.dwExStyle & WS_EX_CONTROLPARENT;
	bool ExDlgmodalFrame       = cs.dwExStyle & WS_EX_DLGMODALFRAME;
	bool ExLayered             = cs.dwExStyle & WS_EX_LAYERED;
	bool ExLayoutRtl           = cs.dwExStyle & WS_EX_LAYOUTRTL;
	bool ExLeft                = cs.dwExStyle & WS_EX_LEFT;
	bool ExLeftScrollbar       = cs.dwExStyle & WS_EX_LEFTSCROLLBAR;
	bool ExLtrReading          = cs.dwExStyle & WS_EX_LTRREADING;
	bool ExMdiChild            = cs.dwExStyle & WS_EX_MDICHILD;
	bool ExNoActivate          = cs.dwExStyle & WS_EX_NOACTIVATE;
	bool ExNoInheritLayout     = cs.dwExStyle & WS_EX_NOINHERITLAYOUT;
	bool ExNoParentNotify      = cs.dwExStyle & WS_EX_NOPARENTNOTIFY;
	bool ExNorRdirectionBitmap = cs.dwExStyle & WS_EX_NOREDIRECTIONBITMAP;
	bool ExRight               = cs.dwExStyle & WS_EX_RIGHT;
	bool ExRightScrollbar      = cs.dwExStyle & WS_EX_RIGHTSCROLLBAR;
	bool ExRtlReading          = cs.dwExStyle & WS_EX_RTLREADING;
	bool ExStaticEdge          = cs.dwExStyle & WS_EX_STATICEDGE;
	bool ExToolWindow          = cs.dwExStyle & WS_EX_TOOLWINDOW;
	bool ExTopMost             = cs.dwExStyle & WS_EX_TOPMOST;
	bool ExTransparent         = cs.dwExStyle & WS_EX_TRANSPARENT;
	bool ExWindowEdge          = cs.dwExStyle & WS_EX_WINDOWEDGE;
#endif

	return __super::PreCreateWindow(cs);
}

//--------------------------------------------------------------------------------------------------

#define CallBy2(x, y1, y2) if (WndParams.##x != nullptr) { \
	WndParams.##x(y1, y2); \
} \
__super::x(y1, y2)

#define CallBy3(x, y1, y2, y3) if (WndParams.##x != nullptr) { \
	WndParams.##x(y1, y2, y3); \
} \
__super::x(y1, y2, y3)



void WndFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CallBy3(OnChar, nChar, nRepCnt, nFlags);
}

void WndFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CallBy2(OnContextMenu, pWnd, point);
}

int WndFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

BOOL WndFrame::OnEraseBkgnd(CDC* pDC)
{
	ASSERT(WndParams.OnEraseBkgnd == nullptr);
	return TRUE;
}

void WndFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CallBy3(OnKeyDown, nChar, nRepCnt, nFlags);
}

void WndFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CallBy3(OnKeyUp, nChar, nRepCnt, nFlags);
}

void WndFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	CallBy2(OnLButtonDown, nFlags, point);
}

void WndFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	CallBy2(OnLButtonUp, nFlags, point);
}

void WndFrame::OnMButtonDown(UINT nFlags, CPoint point)
{
	CallBy2(OnMButtonDown, nFlags, point);
}

void WndFrame::OnMButtonUp(UINT nFlags, CPoint point)
{
	CallBy2(OnMButtonUp, nFlags, point);
}

void WndFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	CallBy2(OnMouseMove, nFlags, point);
}

void WndFrame::OnPaint()
{
	ASSERT(WndParams.OnPaint == nullptr);
}

void WndFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	CallBy2(OnRButtonDown, nFlags, point);
}

void WndFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
	CallBy2(OnRButtonUp, nFlags, point);
}

void WndFrame::OnSize(UINT nType, int cx, int cy)
{
	CallBy3(OnSize, nType, cx, cy);
}

void WndFrame::OnSizing(UINT nSide, LPRECT lpRect)
{
	CallBy2(OnSizing, nSide, lpRect);
}

#undef CallBy2
#undef CallBy3
