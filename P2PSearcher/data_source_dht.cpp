// DHTClient.cpp : 定义控制台应用程序的入口点。
//
#include "data_source_dht.h";
#include "sqlite_manager.h"
#include "util.h"

DWORD WINAPI DHTThread(IN PVOID){
	int session_num = 20;
	int listen_port = 25271;
	int info_hash_item = 5000;
	int upload_rate_limit = 200000;
	int download_rate_limit = 200000;
	int active_downloads = 30;
	int alert_queue_size = 4000;
	int dht_announce_interval = 60;
	int auto_manage_startup = 30;
	int auto_manage_interval = 15;

	std::vector<std::pair<std::string, int>> dht_routers = {
		{ "router.bittorrent.com", 6881 },
		{ "router.utorrent.com", 6881 },
		{ "router.bitcomet.com", 6881 },
		{ "dht.transmissionbt.com", 6881 }
	};

	std::vector<std::shared_ptr<libtorrent::session>> sessions;
	std::unordered_map<std::string, int> info_hash_map;
	bool stop_dht = false;

	// create session
	for (int i = 0; i < session_num; i++)
	{
		auto sp = std::make_shared<libtorrent::session>();

		sp->set_alert_mask(libtorrent::alert::category_t::all_categories);
		libtorrent::error_code error_code;
		sp->listen_on(std::make_pair(listen_port + i, listen_port + i), error_code);
		if (error_code)
		{
			std::cout << "failed to open listen socket: " << error_code.message().c_str() << std::endl;
		}

		for (auto &p : dht_routers)
		{
			sp->add_dht_router(p);
		}
		
		libtorrent::session_settings settings = sp->settings();
		settings.upload_rate_limit = upload_rate_limit;
		settings.download_rate_limit = download_rate_limit;
		settings.active_downloads = active_downloads;
		settings.auto_manage_interval = auto_manage_interval;
		settings.auto_manage_startup = auto_manage_startup;
		settings.dht_announce_interval = dht_announce_interval;
		settings.alert_queue_size = alert_queue_size;
		sp->set_settings(settings);

		sessions.push_back(sp);
	}

	// start dht
	while (!stop_dht)
	{
		for (auto &sp : sessions)
		{
			sp->post_torrent_updates();

			std::deque<libtorrent::alert*> alerts;
			sp->pop_alerts(&alerts);
			HandleAlerts(sp, &alerts, info_hash_map);
		}

		if (info_hash_map.size() > info_hash_item){
			SaveInfoHash(info_hash_map);
		}
		Sleep(1);
	}

	// Save hash_info & remove torrent before exit
	SaveInfoHash(info_hash_map);

	for (auto &sp : sessions)
	{
		auto torrents = sp->get_torrents();
		for (auto &t : torrents)
		{
			sp->remove_torrent(t);
		}
	}

	return 0;
}

void HandleAlerts(std::shared_ptr<libtorrent::session> &sp, std::deque<libtorrent::alert*> *palerts, std::unordered_map<std::string, int> &info_hash_map)
{
	int torrent_upload_limit = 20000;
	int torrent_download_limit = 20000;

	while (palerts->size() > 0)
	{
		auto palert = palerts->front();
		palerts->pop_front();
		std::string info_hash;

		switch (palert->type())
		{
		case libtorrent::add_torrent_alert::alert_type:
		{
			auto p1 = (libtorrent::add_torrent_alert*) palert;
			p1->handle.set_upload_limit(torrent_upload_limit);
			p1->handle.set_download_limit(torrent_download_limit);

			break;
		}
		case libtorrent::dht_announce_alert::alert_type:
		{
			auto p2 = (libtorrent::dht_announce_alert*) palert;
			info_hash = p2->info_hash.to_string();
			int hot = info_hash_map[info_hash];
			info_hash_map[info_hash] = hot + 1;

			break;
		}
		case libtorrent::dht_get_peers_alert::alert_type:
		{
			auto p3 = (libtorrent::dht_get_peers_alert*) palert;
			info_hash = p3->info_hash.to_string();
			int hot = info_hash_map[info_hash];
			info_hash_map[info_hash] = hot + 1;

			break;
		}
		default:
			break;
		}
	}
}

void SaveInfoHash(std::unordered_map<std::string, int> &info_hash_map){
	SQLiteManager sm;
	sm.Open();

	for (auto &item : info_hash_map){
		sm.Update(item.first.c_str(), "", "", "", item.second, "");
	}

	info_hash_map.clear();
}

std::vector<SearchResult> SearchDHT(std::string &keyword, int current_search_time){
	return std::vector<SearchResult>();
}