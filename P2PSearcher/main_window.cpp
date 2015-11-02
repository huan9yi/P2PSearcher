#include "resource.h"

#include "main_window.h"
#include "p2psearcher.h"

#include <ole2.h>
#include <Shlwapi.h>

// External
extern int window_width;
extern int window_height;
extern char szTitle[];
extern char szWindowClass[];
extern const char *app_name;

// Global Variables
HINSTANCE hInst; // current instance
HWND hMainWnd; // main window hwnd
const UINT WM_ICON_NOTIFY = WM_APP + 10; // 窗口变为系统托盘图标时的消息

struct DOMEventsHandlerType : htmlayout::event_handler
{
	DOMEventsHandlerType() : event_handler(0xFFFFFFFF) {}

	virtual BOOL handle_event(HELEMENT he, BEHAVIOR_EVENT_PARAMS &params)
	{
		switch (params.cmd)
		{
			case BUTTON_CLICK:              OnButtonClick(params.heTarget); break; // click on button
			case BUTTON_PRESS:              break; // mouse down or key down in button
			case BUTTON_STATE_CHANGED:      break;
			case EDIT_VALUE_CHANGING:       break; // before text change
			case EDIT_VALUE_CHANGED:        break; // after text change
			case SELECT_SELECTION_CHANGED:  OnSelectSelectionChanged(params.heTarget); break; // selection in <select> changed
			case SELECT_STATE_CHANGED:      break; // node in select expanded/collapsed, heTarget is the node
			case POPUP_REQUEST:             break; // request to show popup just received, here DOM of popup element can be modifed.
			case POPUP_READY:               break; // popup element has been measured and ready to be shown on screen, here you can use functions like ScrollToView.
			case POPUP_DISMISSED:           break; // popup element is closed, here DOM of popup element can be modifed again - e.g. some items can be removed to free memory.
			case MENU_ITEM_ACTIVE:          break; // menu item activated by mouse hover or by keyboard
			case MENU_ITEM_CLICK:           break; // menu item click "grey" event codes  - notfications from behaviors from this SDK 
			case HYPERLINK_CLICK:           OnHyperlinkClick(params.heTarget); break; // hyperlink click
			case TABLE_HEADER_CLICK:        break; // click on some cell in table header, target = the cell, reason = index of the cell (column number, 0..n)
			case TABLE_ROW_CLICK:           break; // click on data row in the table, target is the row, target = the row, reason = index of the row (fixed_rows..n)
			case TABLE_ROW_DBL_CLICK:       break; // mouse dbl click on data row in the table, target is the row, target = the row, reason = index of the row (fixed_rows..n)
			case ELEMENT_COLLAPSED:         break; // element was collapsed, so far only behavior:tabs is sending these two to the panels
			case ELEMENT_EXPANDED:          break; // element was expanded,
		}

		return false;
	}
} DOMEventsHandler;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 单实例启动程序
	CreateMutex(NULL, false, app_name);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return -1;
	}

	// 释放依赖的DLL资源文件
	if (!ExtractDLL())
	{
		return -1;
	}

	// required for D&D operations
	OleInitialize(NULL);

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return false;
	}

	AppInitial();

	MSG msg;
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WIN32);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	OleUninitialize();

	return msg.wParam;
}

bool ExtractDLL()
{
	TCHAR temp[MAX_PATH];
	GetTempPath(MAX_PATH, temp);

	SetDllDirectory(temp);

	std::string name = std::string{ temp } + std::string{ "htmlayout.dll" };
	LPCSTR dll_name = name.c_str();

	if (!PathFileExists(dll_name))
	{
		HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(DLL_HTMLAYOUT), "DLL");
		if (NULL == hRsrc)
		{
			return false;
		}

		DWORD dwSize = SizeofResource(NULL, hRsrc);
		if (0 == dwSize)
		{
			return false;
		}

		HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
		if (NULL == hGlobal)
		{
			return false;
		}

		// 返回文件内容
		LPVOID pBuffer = LockResource(hGlobal);
		if (NULL == pBuffer)
		{
			return false;
		}

		HANDLE hFile = CreateFile(dll_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		DWORD unuse = 0;
		if (!WriteFile(hFile, pBuffer, dwSize, &unuse, NULL))
		{
			CloseHandle(hFile);
			return false;
		}

		CloseHandle(hFile);
	}

	return true;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style      = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc  = (WNDPROC)WndProc;
	wcex.cbClsExtra   = 0;
	wcex.cbWndExtra   = 0;
	wcex.hInstance    = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)ICON_64x64);
	wcex.hCursor    = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)ICON_16x16);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	// 窗口居中
	int scrWidth = GetSystemMetrics(SM_CXSCREEN);
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP | WS_MINIMIZEBOX, (scrWidth - window_width) / 2, (scrHeight - window_height) / 2, window_width, window_height, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return false;
	}

	hMainWnd = hWnd;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT  - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT pt, pe;
	static RECT rt, re;
	
	// 鼠标可拖动区域
	static RECT drag_area;

	// HTMLayout could be created as separate window 
	// using CreateWindow API.
	// But in this case we are attaching HTMLayout functionality
	// to the existing window delegating windows message handling to 
	// HTMLayoutProcND function.
	LRESULT lResult;
	BOOL bHandled;
	lResult = HTMLayoutProcND(hWnd, message, wParam, lParam, &bHandled);
	if (bHandled)
	{
		return lResult;
	}

	switch (message)
	{
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			
			GetCursorPos(&pt);
			GetWindowRect(hWnd, &rt);
			re.right = rt.right - rt.left;
			re.bottom = rt.bottom - rt.top;

			drag_area.left = rt.left;
			drag_area.top = rt.top;
			drag_area.right = drag_area.left + re.right;
			drag_area.bottom = drag_area.top + re.bottom * 10 / 100; // 顶部为10%的长度

			break;

		case WM_LBUTTONUP:
			ReleaseCapture();
			break;

		case WM_MOUSEMOVE:
			GetCursorPos(&pe);
			if (PtInRect(&drag_area, pe) && wParam == MK_LBUTTON)
			{
				re.left = rt.left + (pe.x - pt.x);
				re.top = rt.top + (pe.y - pt.y);
				MoveWindow(hWnd, re.left, re.top, re.right, re.bottom, true);

				drag_area.left = re.left;
				drag_area.top = re.top;
				drag_area.right = drag_area.left + re.right;
				drag_area.bottom = drag_area.top + re.bottom * 10 / 100;
			}
			break;

		case WM_ICON_NOTIFY:
			switch (lParam)
			{
				case WM_LBUTTONDOWN:
					ShowWindow(hMainWnd, SW_SHOW);
					break;
			}
			break;

		case WM_ERASEBKGND:
			return true; // as HTMLayout will draw client area in full

		case WM_CREATE:
		{
			// Normally HTMLayout sends its notifications
			// to its parent. 
			// In this particular case we are using callback function to receive and
			// and handle notification. Don't bother the desktop window (parent of this window)
			// by our notfications.
			HTMLayoutSetCallback(hWnd, &HTMLayoutNotifyHandler, 0);

			// attach DOM events handler so we will be able to receive DOM events like BUTTON_CLICK, HYPERLINK_CLICK, etc.
			// HTMLayoutWindowAttachEventHandler( hWnd, 
			//    DOMEventsHandler.element_proc, 
			//   &DOMEventsHandler, 
			//    DOMEventsHandler.subscribed_to);
			htmlayout::attach_event_handler(hWnd, &DOMEventsHandler);

			WCHAR path[2048] = L"file://";
			GetModuleFileNameW(NULL, &path[7], 2048 - 7);

			PBYTE pb; DWORD cb;
			if (GetHtmlResource("default", pb, cb))
			{
				HTMLayoutLoadHtmlEx(hWnd, pb, cb, path); // we use path here so all relative links in the document will resolved against it.
			}
		}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// HTMLayout specific notification handler.
LRESULT CALLBACK HTMLayoutNotifyHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam)
{
	// all HTMLayout notification are comming here.
	NMHDR*  phdr = (NMHDR*)lParam;

	switch (phdr->code)
	{
		case HLN_CREATE_CONTROL:    break; //return OnCreateControl((LPNMHL_CREATE_CONTROL) lParam);
		case HLN_CONTROL_CREATED:   break; //return OnControlCreated((LPNMHL_CREATE_CONTROL) lParam);
		case HLN_DESTROY_CONTROL:   break; //return OnDestroyControl((LPNMHL_DESTROY_CONTROL) lParam);
		case HLN_LOAD_DATA:         return OnLoadData((LPNMHL_LOAD_DATA)lParam);
		case HLN_DATA_LOADED:       break; //return OnDataLoaded((LPNMHL_DATA_LOADED)lParam);
		case HLN_DOCUMENT_COMPLETE: break; //return OnDocumentComplete();
		case HLN_ATTACH_BEHAVIOR:   return OnAttachBehavior((LPNMHL_ATTACH_BEHAVIOR)lParam);
	}

	return 0;
}

LRESULT OnLoadData(LPNMHL_LOAD_DATA pnmld)
{
	return LoadResourceData(pnmld->hdr.hwndFrom, pnmld->uri);
}

LRESULT LoadResourceData(HWND hWnd, LPCWSTR uri)
{
	// Check for trivial case
	if (!uri || !uri[0]) return LOAD_DISCARD;

	if (wcsncmp(uri, L"res:", 4) == 0)
		uri += 4;
	else
		return LOAD_OK; // it is not a "res:*" so proceed with the default loader

	// Retrieve url specification into a local storage since FindResource() expects
	// to find its parameters on stack rather then on the heap under Win9x/Me
	wchar_t achURL[MAX_PATH]; wcsncpy(achURL, uri, MAX_PATH);

	// Separate name and handle external resource module specification
	LPWSTR pszName = achURL;

	// Separate extension if any
	LPWSTR pszExt = wcsrchr(pszName, '.'); if (pszExt) *pszExt++ = '\0';

	// Find specified resource and leave if failed. Note that we use extension
	// as the custom resource type specification or assume standard HTML resource
	// if no extension is specified
	HRSRC hrsrc = 0;
	bool  isHtml = false;
	if (pszExt == 0 || wcsicmp(pszExt, L"HTML") == 0)
	{
		hrsrc = ::FindResourceW(hInst, pszName, (LPCWSTR)RT_HTML);
		isHtml = true;
	}
	else
		hrsrc = ::FindResourceW(hInst, pszName, pszExt);

	if (!hrsrc) return LOAD_OK; // resource not found here - proceed with default loader

	// Load specified resource and check if ok
	HGLOBAL hgres = ::LoadResource(hInst, hrsrc);
	if (!hgres) return LOAD_DISCARD;

	// Retrieve resource data and check if ok
	PBYTE pb = (PBYTE)::LockResource(hgres); if (!pb) return LOAD_DISCARD;
	DWORD cb = ::SizeofResource(hInst, hrsrc); if (!cb) return LOAD_DISCARD;

	// Report data ready
	::HTMLayoutDataReady(hWnd, uri, pb, cb);

	return LOAD_OK;
}

LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab)
{
	// attach custom behaviors
	htmlayout::behavior::handle(lpab);

	// behavior implementations are located om /include/behaviors/ folder
	// to connect them into the chain of available
	// behaviors - just include them into the project.
	return 0;
}

bool GetHtmlResource(LPCSTR pszName, /*out*/PBYTE &pb, /*out*/DWORD &cb)
{
  // Find specified resource and check if ok
  HRSRC hrsrc = ::FindResource(hInst, pszName, MAKEINTRESOURCE(RT_HTML));
  if (!hrsrc)
  {
	  return false;
  }

  // Load specified resource and check if ok
  HGLOBAL hgres = ::LoadResource(hInst, hrsrc);
  if(!hgres) return false;

  // Retrieve resource data and check if ok
  pb = (PBYTE)::LockResource(hgres); if (!pb) return false;
  cb = ::SizeofResource(hInst, hrsrc); if (!cb) return false;

  return true;
}

// 最小化窗口
void MinimizeWindow(){
	ShowWindow(hMainWnd, SW_MINIMIZE);
}

// 最小化窗口到系统托盘
void ToTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hMainWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_ICON_NOTIFY;
	nid.hIcon = LoadIcon(hInst, (LPCTSTR)ICON_64x64);
	strcpy(nid.szTip, szTitle);

	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(hMainWnd, SW_HIDE);
}