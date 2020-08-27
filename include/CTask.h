#ifndef CTASK_H
#define CTASK_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include <curl.h>

/*
* 记录curl_handle相关信息的结构
*/
struct C_curl_easy_info {
	CURL* handle;
	curl_slist* header;

	C_curl_easy_info() {
		handle = NULL;
		header = NULL;
	}
};

/*
* 当任务支持分段下载时，使用libcurl的multi句柄进行文件分段传输
*/
class CProcess;
class CTask
{
public:
	CTask(string url);		//当支持多线程时下载的线程数量由构造函数(指定g_down_seg_num)确定
	CTask();
	~CTask();
	void startTask();
	void stopTask();
	void cleanTask();
	void stopSave();
	void cancelTask();
	string toString();

private:
	void doWorkForSeg(long star = 0);								//断点续传工作函数
	void doWorkForChunked();										//不支持断点续传时的工作函数
	void doContinue();												//断点后的续传函数
	void addTransfer(size_t i, long& range_start, long& range_end);
	void mergeTask();												//合并已下载的文件
	void cleanCache();												//清空缓存
	static size_t writeCB(char* data, size_t n, size_t l, void* userp);
	static int progress_callback(void* clientsp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

private:
	CProcess*					_m_pro_processes;
	CURLM*						_m_curlm_handle;		//多接口句柄
	size_t						_m_st_seg_num;			//分段数量
	size_t						_m_st_para_num;			//并行下载数量
	bool						_m_b_stop;				//默认false
	bool						_m_b_del;				//默认false
	bool						_m_b_enabled_seg;		//是否支持分段下载（也就是服务器是否支持range请求）默认true
	bool						_m_b_enabled_breakpoint;//是否允许断点续传，默认为true
	bool						_m_b_continue;			//继续下载
	vector<string*>				_m_vs_files;			//存储的临时文件名
	vector<C_curl_easy_info*>	_m_vc_curl_handles_info;//存储简单接口
	string						_m_s_current_path;		//程序运行的当前路径
	string						_m_s_download_path;		//程序下载路径
	string						_m_s_url;
	string						_m_s_filename;			//下载的文件名
	string						_m_s_cachedir;			//缓存目录名
};

#endif

