#include "CProcess.h"

/*
* ��ȡ�����ӵ�ǰ��������ʱ�򲻼�����д�ǵ��߳�д�����̶߳�û��Ӱ��
*/
CProcess::CProcess():_m_cur_dl_size(0),_m_total_dl_size(0)
{
	
}

void CProcess::setTotalSize(long size)
{
	_m_total_dl_size = size;
}

void CProcess::increaceCurSize(long size)
{
	_m_cur_dl_size += size;
}

long CProcess::getTotalSize()
{
	return _m_total_dl_size;
}

long CProcess::getCurSize()
{
	return -_m_cur_dl_size;
}

string CProcess::toString()
{
	return "CProcess";
}
