#ifndef CTASKMANAGER_H
#define CTASKMANAGER_H

#include <threadpool.hpp>
#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
using namespace boost::threadpool;
using std::vector;
using std::string;

class CTask;
class CTaskManager
{
public:
	using spt_task = boost::shared_ptr<CTask>;
	CTaskManager();
	~CTaskManager() {}
	void addTask();
	void delTask();
	string toString();										//���ظ�����ַ���ʽ
	void setActiveTaskMaxNum(const size_t& maxnum);			//���ÿɻ�����������Ŀ
	void setThreadMaxNum(const size_t& maxnum);				//�����̳߳�������߳�����

private:
	void getCache();

private:
	string				_m_s_url;
	long				_m_l_dlcur;
	long				_m_l_dltotal;
	pool				_m_pool_threadpool;
	size_t				_m_st_active_task_max_num;
	size_t				_m_st_treadpool_max_num;
	vector<spt_task>	_m_vsp_tasks;
};

#endif