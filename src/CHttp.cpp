#include "CHttp.h"

CHttp::CHttp():_m_s_raw_header("")
{

}

CHttp::~CHttp()
{

}

map<string, string> CHttp::Header()
{
	return _m_mapss_header;
}

bool CHttp::enabledSeg(string url)
{
	CURL* curl_handle = curl_easy_init();
	struct curl_slist* header = NULL;
	header = curl_slist_append(header, "Range: bytes=0-0");
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, CHttp::header_callback);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void*)&_m_s_raw_header);
	CURLcode res = curl_easy_perform(curl_handle);
	if (res != CURLE_OK) {
		//TODO: 记录log

		curl_easy_cleanup(curl_handle);
		return false;
	}
	parse();
	if (_m_mapss_header["status_code"] == "206") {
		curl_easy_cleanup(curl_handle);
		return true;
	}
	curl_easy_cleanup(curl_handle);
	return false;
}

long CHttp::getTotalContentSize(string url)
{
	CURL* curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, CHttp::header_callback);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void*)&_m_s_raw_header);
	CURLcode res = curl_easy_perform(curl_handle);
	if (res != CURLE_OK) {
		//TODO: 记录log

		curl_easy_cleanup(curl_handle);
		return -1;
	}
	parse();
	long ltotal = 0;
	if (_m_mapss_header["status_code"] == "200") {
		if (_m_mapss_header["Content-Length"] != "") {
			string totalsize = _m_mapss_header["Content-Length"];
			ltotal = stol(totalsize);
		}
	}
	curl_easy_cleanup(curl_handle);
	return ltotal;
}

void CHttp::parse()
{
	if (_m_s_raw_header.empty()) return;

	//获取http版本、状态码以及状态
	size_t crlf_index = _m_s_raw_header.find("\r\n");
	std::string version_code_status = _m_s_raw_header.substr(0, crlf_index);
	if (version_code_status.empty()) return;

	size_t span_index = version_code_status.find(" ");
	_m_mapss_header["version"] = version_code_status.substr(0, span_index);
	version_code_status = version_code_status.substr(span_index + 1);
	span_index = version_code_status.find(" ");
	_m_mapss_header["status_code"] = version_code_status.substr(0, span_index);
	_m_mapss_header["status"] = version_code_status.substr(span_index + 1);

	//获取其他键值对
	size_t start = crlf_index;
	while (crlf_index != string::npos) {
		crlf_index = _m_s_raw_header.find("\r\n", crlf_index + 2);
		if (crlf_index == string::npos) break;
		string line = _m_s_raw_header.substr(start, crlf_index - start);
		start = crlf_index + 2;
		size_t colon_index = line.find(":");
		if (colon_index == string::npos) {
			continue;
		}
		_m_mapss_header[line.substr(0, colon_index)] = line.substr(colon_index + 2);
	}
}

size_t CHttp::header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
	string* str = (string*)userdata;
	*str += string(buffer);
	return nitems * size;
}
