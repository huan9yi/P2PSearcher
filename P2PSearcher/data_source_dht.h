#ifndef _DATA_SOURCE_DHT_H
#define _DATA_SOURCE_DHT_H

#include "StdAfx.h"

#include "p2psearcher.h"

#include "libtorrent/session.hpp"
#include "libtorrent/alert_types.hpp"

DWORD WINAPI DHTThread(IN PVOID);
void HandleAlerts(std::shared_ptr<libtorrent::session> &sp, std::deque<libtorrent::alert*>* palerts, std::unordered_map<std::string, int> &info_hashs_map);
void SaveInfoHash(std::unordered_map<std::string, int> &info_hash_map);
std::vector<SearchResult> SearchDHT(std::string &keyword, int current_search_time);

#endif