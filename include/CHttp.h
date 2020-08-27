#ifndef CHTTP_H
#define CHTTP_H

#include <curl.h>

#include <string>
#include <map>
using std::string;
using std::map;

//使用前请确保已经调用curl_global_init,不建议单独使用该类，应该结合CTask类使用
class CHttp
{
public:
	CHttp();
	~CHttp();
	map<string,string> Header();
	bool enabledSeg(string url);
	long getTotalContentSize(string url);

private:
	void parse();
	static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata);

private:
	string _m_s_raw_header;
	map<string, string> _m_mapss_header;
};

#endif

