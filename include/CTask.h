#ifndef CTASK_H
#define CTASK_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include <curl.h>

/*
* ��¼curl_handle�����Ϣ�Ľṹ
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
* ������֧�ֶַ�����ʱ��ʹ��libcurl��multi��������ļ��ֶδ���
*/
class CProcess;
class CTask
{
public:
	CTask(string url);		//��֧�ֶ��߳�ʱ���ص��߳������ɹ��캯��(ָ��g_down_seg_num)ȷ��
	CTask();
	~CTask();
	void startTask();
	void stopTask();
	void cleanTask();
	void stopSave();
	void cancelTask();
	string toString();

private:
	void doWorkForSeg(long star = 0);								//�ϵ�������������
	void doWorkForChunked();										//��֧�ֶϵ�����ʱ�Ĺ�������
	void doContinue();												//�ϵ�����������
	void addTransfer(size_t i, long& range_start, long& range_end);
	void mergeTask();												//�ϲ������ص��ļ�
	void cleanCache();												//��ջ���
	static size_t writeCB(char* data, size_t n, size_t l, void* userp);
	static int progress_callback(void* clientsp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

private:
	CProcess*					_m_pro_processes;
	CURLM*						_m_curlm_handle;		//��ӿھ��
	size_t						_m_st_seg_num;			//�ֶ�����
	size_t						_m_st_para_num;			//������������
	bool						_m_b_stop;				//Ĭ��false
	bool						_m_b_del;				//Ĭ��false
	bool						_m_b_enabled_seg;		//�Ƿ�֧�ֶַ����أ�Ҳ���Ƿ������Ƿ�֧��range����Ĭ��true
	bool						_m_b_enabled_breakpoint;//�Ƿ�����ϵ�������Ĭ��Ϊtrue
	bool						_m_b_continue;			//��������
	vector<string*>				_m_vs_files;			//�洢����ʱ�ļ���
	vector<C_curl_easy_info*>	_m_vc_curl_handles_info;//�洢�򵥽ӿ�
	string						_m_s_current_path;		//�������еĵ�ǰ·��
	string						_m_s_download_path;		//��������·��
	string						_m_s_url;
	string						_m_s_filename;			//���ص��ļ���
	string						_m_s_cachedir;			//����Ŀ¼��
};

#endif

