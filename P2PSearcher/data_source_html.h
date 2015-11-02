#ifndef _DATA_SOURCE_HTML_H
#define _DATA_SOURCE_HTML_H

#include "StdAfx.h"

#include "p2psearcher.h"

#include "curl/curl.h"

size_t CurlWriteCallback(char *, size_t, size_t, void *);
CURLcode CurlRequest(std::string, std::string *);

std::vector<SearchResult> SearchHTMLdhtseek(std::string &, int);
std::vector<SearchResult> SearchHTMLbtbook(std::string &, int);
std::vector<SearchResult> SearchHTMLbtdao(std::string &, int);
std::vector<SearchResult> SearchHTMLcili8(std::string &, int);
std::vector<SearchResult> SearchHTMLdianyingll(std::string &, int);
std::vector<SearchResult> SearchHTMLbtants(std::string &, int);
std::vector<SearchResult> SearchHTMLcililian(std::string &, int);
std::vector<SearchResult> SearchHTMLbreadsearch(std::string &, int);
std::vector<SearchResult> SearchHTMLcilisou(std::string &, int);
std::vector<SearchResult> SearchHTMLtorrentkitty(std::string &, int);
std::vector<SearchResult> SearchHTMLtorrentkittyco(std::string &, int);
std::vector<SearchResult> SearchHTMLdonkey4u(std::string &, int);

#endif