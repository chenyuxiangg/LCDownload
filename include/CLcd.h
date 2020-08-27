#ifndef CLCD_H
#define CLCD_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include <boost/smart_ptr.hpp>
using namespace boost;

class CTaskManager;
class CLcd
{
public:
	using spt_taskmanager =shared_ptr<CTaskManager>;

	CLcd();
	~CLcd();
	void createTask();
	void setActiveTaskMaxNum(const size_t& maxnum );		//设置可活动的任务最大数目(Taskmanager)
	void setThreadMaxNum(const size_t& maxnum);				//设置线程池中最大线程数量(Taskmanager)
	void setWorkDir(const string& workdir);					//设置工作目录，工作目录即临时文件、参数文件的保存路径
	void setDownDir(const string& downdir);					//设置下载目录
	void enableMultiDown(const bool& enable);				//设置是否允许多线程下载(Task)
	void enableBreakpoint(const bool& enable);				//设置是否允许断点续传(Task)
	string getCurPath();									//获取当前可执行程序所在路径
	string getWorkDir();									//获取工作目录
	string getDownDir();									//获取下载目录
	size_t getActiveTaskMaxNum();							//获取最大可活动任务数
	bool enabledMultiDown();								//获取对多线程下载的支持
	bool enabledBreakPoint();								//获取对断点续传的支持
	string toString();										//返回该类的字符形式

private:
	string				_m_s_curpath;
	string				_m_s_workdir;
	string				_m_s_downdir;
	spt_taskmanager		_m_sp_taskmanager;
};

#endif
