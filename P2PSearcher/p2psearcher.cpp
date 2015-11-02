#include "p2psearcher.h"
#include "main_window.h"
#include "data_source_html.h"
#include "data_source_dht.h"
#include "util.h"

#include <Winhttp.h>

// Global Variables
int window_width = 800;
int window_height = 600;
char szTitle[] = "笨笨Q P2PSearcher"; // The title bar text
char szWindowClass[] = "笨笨Q P2PSearcher"; // The title bar text
const char *app_name = "P2PSearcher-F9FD750D-95FB-4C7A-96F4-F252BAC33BCB"; // unique app name base on GUID
double version = 1.0;

// Emvironment Variables
const wchar_t *IMG_OK = L"url(res:info_ok.png)";
const wchar_t *IMG_ERROR = L"url(res:info_error.png)";
const wchar_t *IMG_LOADING = L"url(res:info_loading.gif)";

// UTF-8编码（名称 类型 大小 资源 操作）
static const std::string table_head = "<tr .header>\
	<th>\xe5\x90\x8d\xe7\xa7\xb0</th>\
	<th>\xe7\xb1\xbb\xe5\x9e\x8b</th>\
	<th>\xe5\xa4\xa7\xe5\xb0\x8f</th>\
	<th>\xe8\xb5\x84\xe6\xba\x90</th>\
	<th>\xe6\x93\x8d\xe4\xbd\x9c</th>\
</tr>";

// External
extern HINSTANCE hInst;
extern HWND hMainWnd;

static volatile bool thread_active = false;
static volatile bool allow_thread = true;
HANDLE thread_handle = nullptr;

std::random_device rd;
std::mt19937 engine(rd());
static std::vector<SearchObject> search_objects;

void AppInitial()
{
	// 设置焦点
	htmlayout::dom::element root = htmlayout::dom::element::root_element(hMainWnd);
	htmlayout::dom::element input = root.get_element_by_id("keyword");
	input.set_state(STATE_FOCUS);

	// version text
	std::ostringstream version_info;
	version_info << "v" << std::fixed << std::setprecision(1) << version;
	htmlayout::dom::element version_info_element = root.get_element_by_id("version_info");
	version_info_element.set_html((unsigned char*)version_info.str().c_str(), version_info.str().size() + 1, SIH_REPLACE_CONTENT);

	// start dht
	//CreateThread(NULL, 0, DHTThread, NULL, 0, NULL);

	// status info
	ShowInfo("Ready", IMG_OK);
}

void OnSelectSelectionChanged(HELEMENT select){}

void OnHyperlinkClick(HELEMENT a){
	htmlayout::dom::element el = a;
	const wchar_t *href = el.get_attribute("href");
	if (href)
	{
		std::string url = UTF8WCharToGB2312Char(href);
		CopyToClipboard(url.c_str(), url.size());
	}
}

void OnButtonClick(HELEMENT button)
{
	htmlayout::dom::element el = button;
	const wchar_t *id = el.get_attribute("id");
	if (id)
	{
		if (wcscmp(L"action_minimize_window", id) == 0)
		{
			// Minimize or to tray
			//MinimizeWindow();
			ToTray();
		}
		else if (wcscmp(L"action_close_window", id) == 0)
		{
			DestroyWindow(hMainWnd);
		}
		else if (wcscmp(L"action_search", id) == 0)
		{
			const wchar_t *keyword = CheckAndGetKeyword();
			if (keyword){
				// terminate the thread
				if (thread_active){
					if (thread_handle != nullptr){
						allow_thread = false;

						int nRes = WaitForSingleObject(thread_handle, 10000);
						if (nRes != WAIT_OBJECT_0) {
							TerminateThread(thread_handle, 0);
						}

						if (thread_handle != nullptr)
						{
							CloseHandle(thread_handle);
						}
					}
					thread_active = false;
				}

				allow_thread = true;
				thread_handle = CreateThread(NULL, 0, SearchThread, NULL, 0, NULL);
			}
		}
	}
}

DWORD WINAPI SearchThread(IN PVOID param){
	thread_active = true;
	ShowInfo("Searching...", IMG_LOADING);

	const wchar_t *keyword_wchar = CheckAndGetKeyword();
	std::string keyword_utf8 = GB2312WCharToUTF8Char(keyword_wchar);
	std::string keyword = form_urlencode(keyword_utf8);

	ClearResult();
	InitSearch();

	while (allow_thread){
		std::vector<SearchResult> result = DoSearch(keyword);
		if (result.size() > 0){
			ShowResult(result);
			Sleep(2000);
		}
		else{
			allow_thread = false;
			break;
		}
	}

	thread_active = false;
	ShowInfo("Search done", IMG_OK);

	return 0;
}

void InitSearch(){
	search_objects = {
		{ 5, 1, SearchDHT },
		{ 5, 1, SearchHTMLdhtseek },
		{ 5, 1, SearchHTMLbtbook },
		{ 5, 1, SearchHTMLbtdao },
		{ 5, 1, SearchHTMLcili8 },
		{ 5, 1, SearchHTMLdianyingll },
		{ 5, 1, SearchHTMLbtants },
		{ 5, 1, SearchHTMLcililian },
		{ 5, 1, SearchHTMLbreadsearch },
		{ 5, 1, SearchHTMLcilisou },
		{ 5, 1, SearchHTMLtorrentkitty },
		{ 5, 1, SearchHTMLtorrentkittyco },
		{ 5, 1, SearchHTMLdonkey4u },
	};
}

std::vector<SearchResult> DoSearch(std::string &keyword)
{
	// 乱序，不连续搜索同一个网站
	std::shuffle(search_objects.begin(), search_objects.end(), engine);

	for (SearchObject &so : search_objects){
		if (so.current_search_time <= so.max_search_time){
			std::vector<SearchResult> result = so.SearchFunction(keyword, so.current_search_time);;
			if (result.size() > 0){
				so.current_search_time += 1;
				return result;
			}
			else{
				so.current_search_time = so.max_search_time + 1;
			}
		}
	}

	return std::vector<SearchResult>();
}

// 清空搜索结果
void ClearResult(){
	htmlayout::dom::element root = htmlayout::dom::element::root_element(hMainWnd);
	htmlayout::dom::element table = root.get_element_by_id("result");
	table.set_html((const unsigned char*)table_head.c_str(), table_head.size(), SIH_REPLACE_CONTENT);
}

// 显示搜索结果
void ShowResult(std::vector<SearchResult> result)
{
	htmlayout::dom::element root = htmlayout::dom::element::root_element(hMainWnd);
	htmlayout::dom::element table = root.get_element_by_id("result");
	//htmlayout::dom::element th = table.child(table.children_count());

	std::string table_content;
	for (const SearchResult &sr : result){
		table_content.append("<tr>");
		table_content.append("<td width='60%'>").append(sr.name).append("</td>");
		table_content.append("<td width='10%'>").append(sr.type).append("</td>");
		table_content.append("<td width='10%'>").append(sr.size).append("</td>");
		table_content.append("<td width='10%'>").append(sr.hot).append("</td>");
		table_content.append("<td width='10%'><a href='").append(sr.url).append("'>Copy</a></td>");
		table_content.append("</tr>");
	}
	table.set_html((const unsigned char*)table_content.c_str(), table_content.size(), SIH_APPEND_AFTER_LAST);
	//th.set_html((const unsigned char*)table_content.c_str(), table_content.size(), SIH_APPEND_AFTER_LAST);
}

void ShowInfo(std::string info, const wchar_t *img_path){
	htmlayout::dom::element root = htmlayout::dom::element::root_element(hMainWnd);

	// text
	htmlayout::dom::element text_info = root.get_element_by_id("text_info");
	text_info.set_html((unsigned char*)info.c_str(), info.size() + 1, SIH_REPLACE_CONTENT);

	// img
	htmlayout::dom::element img_info = root.get_element_by_id("img_info");
	img_info.set_style_attribute("background-image", img_path);
}

const wchar_t * CheckAndGetKeyword(){
	htmlayout::dom::element root = htmlayout::dom::element::root_element(hMainWnd);
	htmlayout::dom::element keyword_element = root.get_element_by_id("keyword");

	const wchar_t *keyword = keyword_element.text().c_str();
	if (keyword && wcscmp(keyword, L"") != 0){
		return keyword;
	}
	else{
		Debug("请输入搜索关键字");
		return NULL;
	}
}