#include "global.h"
#include "CTask.h"
#include "CHttp.h"
#include "CProcess.h"
#include "util.h"

#include <fstream>
using std::ofstream;
using std::ifstream;

#include <cstdio>
#include <cstdlib>

/*
* lastupdate: 2020/7/19 14:33
* description: 需要考虑创建“取消下载“方法；继续书写start方法。
*/

CTask::CTask(string url) :
	_m_st_seg_num(g_down_seg_num != 0 ? g_down_seg_num : 3),
	_m_st_para_num(g_down_para_num != 0 ? g_down_para_num : 3),
	_m_b_stop(true), _m_b_del(false), _m_s_url(url), _m_s_filename(""), 
	_m_b_enabled_breakpoint(true),_m_b_enabled_seg(true),
	_m_b_continue(false)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_m_curlm_handle = curl_multi_init();
	curl_multi_setopt(_m_curlm_handle, CURLMOPT_MAXCONNECTS, _m_st_para_num);

	//配置服务器是否允许分段
	CHttp http;
	_m_b_enabled_seg = (_m_b_enabled_seg && http.enabledSeg(url));
	if (!_m_b_enabled_seg)
		_m_b_enabled_breakpoint = false;

	//配置下载文件的长度
	_m_pro_processes->setTotalSize(http.getTotalContentSize(url));

	//在下载目录创建一个隐藏的cache目录，存放各个任务的缓存信息
	_m_s_cachedir = _m_s_download_path + "\\cache";
	if (_access(_m_s_cachedir.c_str(), 0) == -1) {
		_mkdir(_m_s_cachedir.c_str());
		SetFileAttributes(n2w(_m_s_cachedir), FILE_ATTRIBUTE_HIDDEN);
	}

	//设置文件名
	size_t index = url.find_last_of("/\\");
	if (index == string::npos) {
		//TODO: log

		return;
	}
	else {
		_m_s_filename = url.substr(index + 1);
	}

	char buff[1000];
	_getcwd(buff, sizeof(buff));				//获取当前路径
	_m_s_current_path = string(buff);
	_m_s_download_path = g_dlpath == "" ? _m_s_current_path : g_dlpath;
	_m_pro_processes = new CProcess;
}

CTask::~CTask()
{
	//清理简单句柄
	for (int i = 0; i < _m_vc_curl_handles_info.size(); ++i) {
		if (_m_vc_curl_handles_info[i]->handle != nullptr) {
			if (_m_vc_curl_handles_info[i]->header != NULL)
				curl_slist_free_all(_m_vc_curl_handles_info[i]->header);
			curl_easy_cleanup(_m_vc_curl_handles_info[i]->handle);
			delete _m_vc_curl_handles_info[i];
		}
	}

	//处理多句柄
	curl_multi_cleanup(_m_curlm_handle);

	//处理全局
	curl_global_cleanup();
}

void CTask::startTask()
{
	_m_b_stop = false;
	if (!_m_b_stop && _m_b_continue) {
		doContinue();
	}
	if (!_m_b_stop && !_m_b_del && (_m_b_enabled_seg | _m_b_enabled_breakpoint))
		doWorkForSeg();
	else
		doWorkForChunked();
}

void CTask::stopTask()
{
	_m_b_stop = true;
}

void CTask::cleanTask()
{
	//将生成的临时文件删除
	int res = 0;
	for (auto str : _m_vs_files) {
		res = remove((*str).c_str());
		if (res != 0) {
			//TODO: log ok
		}
		else {
			//TODO: log error
		}
	}
}

void CTask::stopSave()
{
	//创建文件并记录当前下载的情况
	string tmpfile = _m_s_cachedir + "\\" + _m_s_filename + "cache";
	ofstream of(tmpfile, std::ios::trunc | std::ios::binary | std::ios::out);
	if (!of.is_open()) {
		//TODO: log

		return;
	}
	of << "url:" << _m_s_url << std::endl;
	of << "dlnow:" << _m_pro_processes->getCurSize() << std::endl;
	of << "dltoal:" << _m_pro_processes->getTotalSize() << std::endl;
	of.close();

	mergeTask();
}

void CTask::cancelTask()
{
	_m_b_stop = true;
	_m_b_del = true;
}

void CTask::mergeTask()
{
	//合并已下载的文件
	ofstream of(_m_s_cachedir + "\\" + _m_s_filename, std::ios::ate | std::ios::binary | std::ios::out);
	if (!of.is_open()) {
		//TODO: log
		return;
	}
	else {
		for (auto s : _m_vs_files) {
			string tmp;
			ifstream infile(*s, std::ios::in | std::ios::binary);
			infile >> tmp;
			of << tmp;
			infile.close();
		}
		of.close();
		cleanTask();
	}
}

void CTask::cleanCache()
{
	//删除断点续传缓存
	string tmpfile = _m_s_cachedir + "\\" + _m_s_filename;
	string tmpfile_cache = _m_s_cachedir + "\\" + _m_s_filename + "cache";
	if (_access(tmpfile.c_str(), 0) != -1) {
		remove(tmpfile.c_str());
	}
	//删除下载缓存
	if (_access(tmpfile_cache.c_str(), 0) != -1) {
		remove(tmpfile_cache.c_str());
	}
}

string CTask::toString()
{
	return "CTask";
}

void CTask::doWorkForSeg(long star)
{
	CURLMsg* msg;
	size_t transfers = 0;
	int msgs_left = -1;
	int still_alive = 1;
	size_t num = min(_m_st_para_num, _m_st_seg_num);
	long start = star;
	long totalsize = _m_pro_processes->getTotalSize();
	long end = (totalsize-start-(totalsize/_m_st_seg_num)*(_m_st_seg_num-1 <= 0 ? 1 : _m_st_seg_num - 1))-1;
	for (transfers = 0; transfers < num; ++transfers) {
		addTransfer(transfers, start, end);
	}
	do {
		curl_multi_perform(_m_curlm_handle, &still_alive);
		while ((msg = curl_multi_info_read(_m_curlm_handle, &msgs_left))) {
			if (msg->msg == CURLMSG_DONE) {
				CURL* handle = msg->easy_handle;
				curl_multi_remove_handle(_m_curlm_handle, handle);
				for (int i = 0; i < _m_vc_curl_handles_info.size(); ++i) {
					if (_m_vc_curl_handles_info[i]->handle == handle) {
						if (_m_vc_curl_handles_info[i]->header != NULL)
							curl_slist_free_all(_m_vc_curl_handles_info[i]->header);
						curl_easy_cleanup(_m_vc_curl_handles_info[i]->handle);
						_m_vc_curl_handles_info[i]->handle = NULL;
					}
				}
			}
			else {
				//TODO: log
			}
			if (transfers < _m_st_seg_num)
				addTransfer(transfers++, start, end);
		}
		if (still_alive)
			curl_multi_wait(_m_curlm_handle, NULL, 0, 1000, NULL);
	} while ((!_m_b_stop) && (!_m_b_del) && (still_alive || (transfers < _m_st_seg_num)));

	if (!_m_b_del && _m_b_stop && _m_b_enabled_breakpoint) {
		stopSave();
	}
	if (_m_b_del) {
		cleanTask();
		cleanCache();
	}
}

void CTask::doWorkForChunked() {
	CURL* handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, _m_s_url.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CTask::writeCB);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&_m_s_filename);
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, CTask::progress_callback);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, (void*)_m_pro_processes);
	curl_easy_setopt(handle, CURLOPT_PRIVATE, _m_s_url.c_str());
	curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_perform(handle);
	curl_easy_cleanup(handle);
}

void CTask::doContinue()
{
	long start = _m_pro_processes->getCurSize();
	doWorkForSeg(start);
}

void CTask::addTransfer(size_t i,long& range_start,long& range_end)
{
	string* str = new string(_m_s_download_path + "." + _m_s_filename + std::to_string(i));
	_m_vs_files.push_back(str);

	CURL* handle = curl_easy_init();
	C_curl_easy_info* handle_info = new C_curl_easy_info;
	curl_easy_setopt(handle, CURLOPT_URL, _m_s_url.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CTask::writeCB);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)str);
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, CTask::progress_callback);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, (void*)_m_pro_processes);
	curl_easy_setopt(handle, CURLOPT_PRIVATE, _m_s_url.c_str());
	curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
	if (_m_b_enabled_seg) {
		string r = "Range: bytes=" + std::to_string(range_start) + "-" + std::to_string(range_end);
		handle_info->header = curl_slist_append(handle_info->header, r.c_str());
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, handle_info->header);
	}
	_m_vc_curl_handles_info.push_back(handle_info);
	range_start = range_end + 1;
	range_end = range_end + _m_pro_processes->getTotalSize() / _m_st_seg_num;
}

size_t CTask::writeCB(char* data, size_t n, size_t l, void* userp)
{
	string* filename = (string*)userp;
	ofstream of(*filename, std::ios::ate | std::ios::binary);
	if (!of.is_open()) {
		//TODO: 记录log

		return 0;
	}
	of << data;
	of.close();
	return n * l;
}

int CTask::progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	CProcess* process = (CProcess*)clientp;
	process->increaceCurSize(dlnow);
	return 0;
}
