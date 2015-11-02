#include "data_source_html.h"

size_t CurlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t real_size = size * nmemb;
	char *data = (char*)ptr;
	std::string *html = (std::string *)(userdata);
	html->append(data, real_size);

	return real_size;
}

CURLcode CurlRequest(std::string url, std::string *html){
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl_handle = curl_easy_init();

	if (curl_handle){
		// Header
		//struct curl_slist *headers = NULL;
		//headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
		/*
		headers = curl_slist_append(headers, "Accept-Encoding:gzip, deflate, sdch");
		headers = curl_slist_append(headers, "Accept-Language:zh-CN,zh;q=0.8");
		headers = curl_slist_append(headers, "Cache-Control:max-age=0");
		headers = curl_slist_append(headers, "Connection:keep-alive");
		headers = curl_slist_append(headers, "Host:www.baidu.com");
		headers = curl_slist_append(headers, "Referer:https://www.baidu.com/");
		headers = curl_slist_append(headers, "Upgrade-Insecure-Requests:1");
		headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.85 Safari/537.36");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
		*/

		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)html);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10);
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.85 Safari/537.36");

		res = curl_easy_perform(curl_handle);

		curl_easy_cleanup(curl_handle);
		//curl_slist_free_all(headers);
	}

	curl_global_cleanup();

	return res;
}

std::vector<SearchResult> SearchHTMLdhtseek(std::string &keyword, int current_search_time)
{
	std::string url("http://www.dhtseek.com/search/");
	url.append(keyword).append("/").append(std::to_string(current_search_time)).append("-1.html");

	std::string pattern = "<a href=\"/wiki/.+? target=\"_blank\">(.+?)</a>"\
		".+?<span class=\"cpill fileType\\d\">([^<]+)</span>"\
		".+?</b>.+?<b.*?>([^<]+)</b>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^\"]+)\" class=\"download\">";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], match[2], match[3], "", match[4] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLbtbook(std::string &keyword, int current_search_time)
{
	std::string url("http://www.btbook.net/search/");
	url.append(keyword).append("/").append(std::to_string(current_search_time)).append("-1.html");

	std::string pattern = "<a href=\"/wiki/.+? target=\"_blank\">(.+?)</a>"\
		".+?<span class=\"cpill fileType\\d\">([^<]+)</span>"\
		".+?</b>.+?<b.*?>([^<]+)</b>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^\"]+)\" class=\"download\">";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], match[2], match[3], "", match[4] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLbtdao(std::string &keyword, int current_search_time)
{
	std::string url("http://www.btdao.net/list/");
	url.append(keyword).append("-s1d-").append(std::to_string(current_search_time)).append(".html");

	std::string pattern = "<a title=\"([^\"]+)\""\
		".+? href=\"/info/([^\"]+)\""\
		".+?<span>([^<]+)</span>";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[3], "", std::string("magnet:?xt=urn:btih:") + std::string(match[2]) };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLcili8(std::string &keyword, int current_search_time)
{
	std::string url("http://www.cili8.org/s/");
	url.append(keyword).append("_rel_").append(std::to_string(current_search_time)).append(".html");

	std::string pattern = "<a href=\"http://www\\.cili8\\.org/detail/([^\\.]+)\\.html\" "\
		"target=\"_blank\">(.+?)</a>"\
		".+?<span class=\"cpill  fileType\\d\">([^<]+)</span>"\
		".+?</b>.+?<b.*?>([^<]+)</b>";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[2], match[3], match[4], "", std::string("magnet:?xt=urn:btih:") + std::string(match[1]) };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLdianyingll(std::string &keyword, int current_search_time)
{
	std::string url("http://www.dianyingll.com/s?k=");
	url.append(keyword).append("&page=").append(std::to_string(current_search_time));

	std::string pattern = "<a href=\"/info/.+? class=\"highlight-col u-name\">([^<]+)</a>"\
		".+?<span class=\"label label-underline\">([^<]+)</span>"\
		".+?<a id=\"magnet_id\" target=\"_blank\" href=\"(magnet:\\?xt=urn:btih:[^&]+)&";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[2], "", match[3] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLbtants(std::string &keyword, int current_search_time)
{
	std::string url("http://www.btants.com/search/");
	url.append(keyword).append("-first-asc-").append(std::to_string(current_search_time));

	std::string pattern = "<a href=\"/detail/.+? target=\"_blank\">(.+?)</a>"\
		".+?<b class=\"cpill yellow-pill\">([^<]+)</b>"\
		".+?<b class=\"cpill yellow-pill\">([^<]+)</b>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^\"]+)\"";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[3], match[2], match[4] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLcililian(std::string &keyword, int current_search_time)
{
	std::string url("http://cililian.me/list/");
	url.append(keyword).append("/").append(std::to_string(current_search_time)).append(".html");

	std::string pattern = "<a name='file_title' .+?>(.+?)</a>"\
		".+?<span>([^<]+)</span>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^\"]+)\"";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[2], "", match[3] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLbreadsearch(std::string &keyword, int current_search_time)
{
	std::string url("http://www.breadsearch.com/search/");
	url.append(keyword).append("/").append(std::to_string(current_search_time));

	std::string pattern = "<a href=\"/link/.+? target=\"_blank\">(.+?)</a>"\
		".+?<span class=\"list-value\">.+?<span class=\"list-value\">([^<]+)</span>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^\"]+)\"";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[2], "", match[3] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLcilisou(std::string &keyword, int current_search_time)
{
	std::string url("http://www.cilisou.cn/s.php?q=");
	url.append(keyword).append("&p=").append(std::to_string(current_search_time));

	std::string pattern = "<pre class=\"snippet\">(.+?)</pre>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^&]+)&"\
		".+?<span class=\"attr_val\">([^<]+)</span>";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { std::regex_replace(std::string(match[1]), std::regex("<br>"), ""), "", match[3], "", match[2] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLtorrentkitty(std::string &keyword, int current_search_time)
{
	std::string url("http://www.torrentkitty.net/search/");
	url.append(keyword).append("/").append(std::to_string(current_search_time));

	std::string pattern = "<tr><td class=\"name\">([^<]+)</td>"\
		"<td class=\"size\">([^<]+)</td>"\
		".+?<a href=\"(magnet:\\?xt=urn:btih:[^&]+)&";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[2], "", match[3] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLtorrentkittyco(std::string &keyword, int current_search_time)
{
	std::string url("http://www.torrentkitty.co/search/");
	url.append(keyword).append("_ctime_").append(std::to_string(current_search_time)).append(".html");

	std::string pattern = "<h5 class=\"item-title\"><a href=\"http://www\\.torrentkitty\\.co/([^\\.]+)\\.html\" "\
		"target=\"_blank\">(.+?)</a>"\
		".+?<td width=\"100px\"><span class=\"label label-info\"><b>([^<]+)</b>";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[2], "", match[3], "", std::string("magnet:?xt=urn:btih:") + std::string(match[1]) };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}

std::vector<SearchResult> SearchHTMLdonkey4u(std::string &keyword, int current_search_time)
{
	std::string url("http://donkey4u.com/search/");
	url.append(keyword).append("?mode=list&page=").append(std::to_string(current_search_time));

	std::string pattern = "<tr .+? hash=.+?<td>([^<]+)"\
		"<.+?<td width='70' align='right'>([^<]+)"\
		"<.+?<td width='100' align='right'>([^<]+)"\
		"<.+?<a .*?href=\"([^\"]+)";

	std::string html;
	std::vector<SearchResult> result;

	CURLcode res = CurlRequest(url, &html);
	if (res == CURLE_OK){
		// regex解析大数量的\r\n时会失败，所以先把它们替换掉
		for (auto &s : html){
			if (s == '\r' || s == '\n'){
				s = 32; // space
			}
		}

		std::regex my_regex(pattern);
		auto regex_begin = std::sregex_iterator(html.begin(), html.end(), my_regex);
		auto regex_end = std::sregex_iterator();

		for (auto i = regex_begin; i != regex_end; i++){
			std::smatch match = *i;
			SearchResult sr = { match[1], "", match[2], match[3], match[4] };
			result.push_back(sr);
		}
	}
	else{
		//Debug(curl_easy_strerror(res));
	}

	return result;
}