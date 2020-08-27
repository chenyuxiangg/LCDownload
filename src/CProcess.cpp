#include "CProcess.h"

/*
* 获取和增加当前下载量的时候不加锁，写是单线程写，多线程读没有影响
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
