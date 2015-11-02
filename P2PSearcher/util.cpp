#include "util.h"

// UTF8:CP_UTF8, GB2312:CP_ACP
std::wstring CharToWChar(const char *text, UINT code){
	int len = MultiByteToWideChar(code, 0, text, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(code, 0, text, -1, wstr, len);

	std::wstring result(wstr);
	delete[] wstr;

	return result;
}

std::string WCharToChar(const wchar_t *text, UINT code){
	int len = WideCharToMultiByte(code, 0, text, -1, NULL, 0, NULL, NULL);
	char *str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(code, 0, text, -1, str, len, NULL, NULL);

	std::string result(str);
	delete[] str;

	return result;
}

std::string UTF8WCharToGB2312Char(const wchar_t *text){
	std::string temp1 = WCharToChar(text, CP_UTF8);
	std::wstring temp2 = CharToWChar(temp1.c_str(), CP_UTF8);
	std::string result = WCharToChar(temp2.c_str(), CP_ACP);

	return result;
}

std::string GB2312WCharToUTF8Char(const wchar_t *text){
	std::string temp1 = WCharToChar(text, CP_ACP);
	std::wstring temp2 = CharToWChar(temp1.c_str(), CP_ACP);
	std::string result = WCharToChar(temp2.c_str(), CP_UTF8);

	return result;
}

void Debug(const char *msg, HWND hMainWnd){
	MessageBox(hMainWnd, msg, "信息", MB_OK);
}

void Debug(const wchar_t *msg, HWND hMainWnd){
	MessageBoxW(hMainWnd, msg, L"信息", MB_OK);
}

void Debug(const int msg, HWND hMainWnd){
	Debug(std::to_string(msg).c_str(), hMainWnd);
}

void Debug(const std::string &msg, HWND hMainWnd){
	Debug(msg.c_str(), hMainWnd);
}

void ShowErrorCodeText()
{
	LPVOID lpMsgBuf;
	DWORD nErrorCode = GetLastError();

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, nErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (PTSTR)&lpMsgBuf, 0, NULL);
	if (lpMsgBuf){
		Debug((LPCSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
	else{
		Debug("Format failed");
		Debug(nErrorCode);
	}
}

// 是否是64位的Windows系统
BOOL IsWow64()
{
	BOOL bIsWow64 = false;

	// IsWow64Process is not available on all supported versions of Windows.
	// Use GetModuleHandle to get a handle to the DLL that contains the function
	// and GetProcAddress to get a pointer to the function if available.

	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}

	return bIsWow64;
}

BOOL CopyToClipboard(const char* data, const std::size_t data_size)
{
	if (!OpenClipboard(NULL))
		return FALSE;

	EmptyClipboard();

	HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, data_size + 1);
	char *buffer = (char *)GlobalLock(clipbuffer);
	strcpy(buffer, data);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

	return TRUE;
}

std::string GetTextFromClipboard()
{
	std::string text;

	if (!OpenClipboard(NULL))
		return text;

	HGLOBAL hMem = GetClipboardData(CF_TEXT);
	if (NULL != hMem)
	{
		char* lpStr = (char*)GlobalLock(hMem);
		if (NULL != lpStr)
		{
			text.append(lpStr);
			GlobalUnlock(hMem);
		}
	}
	CloseClipboard();

	return text;
}

// 字符转十六进制
std::wstring charToHexW(wchar_t c)
{
	std::wstring result;
	char first, second;

	first = (c & 0xF0) / 16;
	first += first > 9 ? 'A' - 10 : '0';
	second = c & 0x0F;
	second += second > 9 ? 'A' - 10 : '0';

	result.append(1, first);
	result.append(1, second);

	return result;
}

// URL编码
std::wstring form_urlencode_w(const std::wstring& src)
{
	std::wstring result;
	std::wstring::const_iterator iter;

	for (iter = src.begin(); iter != src.end(); ++iter) {
		switch (*iter) {
		case ' ':
			result.append(1, '+');
			break;
			// alnum
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case '0': case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9':
			// mark
		case '-': case '_': case '.': case '!': case '~': case '*': case '\'': case '(': case ')':
			result.append(1, *iter);
			break;
			// escape
		default:
			result.append(1, '%');
			result.append(charToHexW(*iter));
			break;
		}
	}

	return result;
}

// 字符转十六进制
std::string charToHex(char c)
{
	std::string result;
	char first, second;

	first = (c & 0xF0) / 16;
	first += first > 9 ? 'A' - 10 : '0';
	second = c & 0x0F;
	second += second > 9 ? 'A' - 10 : '0';

	result.append(1, first);
	result.append(1, second);

	return result;
}

// URL编码
std::string form_urlencode(const std::string& src)
{
	std::string result;
	std::string::const_iterator iter;

	for (iter = src.begin(); iter != src.end(); ++iter) {
		switch (*iter) {
		case ' ':
			result.append(1, '+');
			break;
			// alnum
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case '0': case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9':
			// mark
		case '-': case '_': case '.': case '!': case '~': case '*': case '\'': case '(': case ')':
			result.append(1, *iter);
			break;
			// escape
		default:
			result.append(1, '%');
			result.append(charToHex(*iter));
			break;
		}
	}

	return result;
}