#ifndef CPROCESS_H
#define CPROCESS_H

#include <string>
using std::string;


//����libcurl�е�progressʵ��
class CProcess
{
public:
	CProcess();
	~CProcess(){}
	void setTotalSize(long size);
	void increaceCurSize(long size);
	long getTotalSize();
	long getCurSize();
	string toString();

private:
	long _m_total_dl_size;
	long _m_cur_dl_size;
};

#endif

