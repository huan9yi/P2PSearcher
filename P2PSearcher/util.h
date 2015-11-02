#ifndef _UTIL_H
#define _UTIL_H

#include "StdAfx.h"

std::wstring CharToWChar(const char *, UINT code);
std::string WCharToChar(const wchar_t *, UINT code);
std::string UTF8WCharToGB2312Char(const wchar_t *);
std::string GB2312WCharToUTF8Char(const wchar_t *text);

void Debug(const char *, HWND hMainWnd = NULL);
void Debug(const wchar_t *, HWND hMainWnd = NULL);
void Debug(const int , HWND hMainWnd = NULL);
void Debug(const std::string &, HWND hMainWnd = NULL);
void ShowErrorCodeText();

BOOL CopyToClipboard(const char*, const std::size_t);
std::string GetTextFromClipboard();

BOOL IsWow64();

std::wstring charToHexW(wchar_t);
std::wstring form_urlencode_w(const std::wstring&);
std::string charToHex(char);
std::string form_urlencode(const std::string&);

#endif