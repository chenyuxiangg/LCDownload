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
	void setActiveTaskMaxNum(const size_t& maxnum );		//���ÿɻ�����������Ŀ(Taskmanager)
	void setThreadMaxNum(const size_t& maxnum);				//�����̳߳�������߳�����(Taskmanager)
	void setWorkDir(const string& workdir);					//���ù���Ŀ¼������Ŀ¼����ʱ�ļ��������ļ��ı���·��
	void setDownDir(const string& downdir);					//��������Ŀ¼
	void enableMultiDown(const bool& enable);				//�����Ƿ�������߳�����(Task)
	void enableBreakpoint(const bool& enable);				//�����Ƿ�����ϵ�����(Task)
	string getCurPath();									//��ȡ��ǰ��ִ�г�������·��
	string getWorkDir();									//��ȡ����Ŀ¼
	string getDownDir();									//��ȡ����Ŀ¼
	size_t getActiveTaskMaxNum();							//��ȡ���ɻ������
	bool enabledMultiDown();								//��ȡ�Զ��߳����ص�֧��
	bool enabledBreakPoint();								//��ȡ�Զϵ�������֧��
	string toString();										//���ظ�����ַ���ʽ

private:
	string				_m_s_curpath;
	string				_m_s_workdir;
	string				_m_s_downdir;
	spt_taskmanager		_m_sp_taskmanager;
};

#endif
