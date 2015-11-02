
#if !defined(AFX_WIN32_H__E73612AD_4251_45A2_A562_6168D17D88CD__INCLUDED_)
#define AFX_WIN32_H__E73612AD_4251_45A2_A562_6168D17D88CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "htmlayout.h"

bool ExtractDLL();
ATOM MyRegisterClass(HINSTANCE);
bool InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HTMLayoutNotifyHandler(UINT, WPARAM, LPARAM, LPVOID);
LRESULT OnLoadData(LPNMHL_LOAD_DATA);
LRESULT LoadResourceData(HWND, LPCWSTR);
LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR);
bool GetHtmlResource(LPCSTR, /*out*/PBYTE&, /*out*/DWORD&);
void MinimizeWindow();
void ToTray();

#endif // !defined(AFX_WIN32_H__E73612AD_4251_45A2_A562_6168D17D88CD__INCLUDED_)
