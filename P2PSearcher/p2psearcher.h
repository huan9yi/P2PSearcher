#ifndef _P2P_SEARCHER_H
#define _P2P_SEARCHER_H

#include "StdAfx.h"

#include "htmlayout.h"

struct SearchResult{
	std::string name;
	std::string type;
	std::string size;
	std::string hot;
	std::string url;
};

struct SearchObject{
	int max_search_time;
	int current_search_time;
	std::vector<SearchResult>(*SearchFunction)(std::string &keyword, int);
};

void AppInitial();
void OnSelectSelectionChanged(HELEMENT select);
void OnHyperlinkClick(HELEMENT a);
void OnButtonClick(HELEMENT button);
DWORD WINAPI SearchThread(IN PVOID param);
void InitSearch();
std::vector<SearchResult> DoSearch(std::string &keyword);
void ClearResult();
void ShowResult(std::vector<SearchResult> result);
void ShowInfo(std::string info, const wchar_t *img_path);
const wchar_t * CheckAndGetKeyword();
void TriggerStatistic();
DWORD WINAPI TrackThread(IN PVOID);

#endif