// TimeOut.h: interface for the CTimeOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEOUT_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_)
#define AFX_TIMEOUT_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include "BaseFunc.h"
#include "MyHeap.h"

//////////////////////////////////////////////////////////////////////
// 超时类，以“秒”为单位
class CTimeOut
{
public:
	CTimeOut(int nIntervalSecs = 0)
	{ 
		m_nInterval = nIntervalSecs;
		m_tUpdateTime = 0;
	}		// =0 : 用于数组初始化
//	virtual ~CTimeOut_() {}

public:
	// 设置时间标签
	void	Update		()
	{ 
		m_tUpdateTime = ::time(NULL);
	}
	// 检查是否超时
	bool	IsTimeOut	()
	{
		bool result= ::time(NULL) >= m_tUpdateTime + m_nInterval;
		return result;
	}
	// 用于定时触发的连续时钟。每次触发时为true。
	bool	ToNextTime	()
	{ 
		bool result=IsTimeOut();
		if(result)
		{
			Update();
			return true;
		}
		else 
			return false;
	}
	// 初始化时间片。(但不启动)
	void	SetInterval	(int nSecs)
	{ 
		m_nInterval = nSecs;
	}

	// 开始启动时钟(同SetInterval)
	void	Startup		(int nSecs)
	{ 
		m_nInterval = nSecs;
		Update();
	}
	// 时钟是否触发(只触发一次)，与Startup配合使用，可实现只触发一次的时钟。
	bool	TimeOver	()
	{ 
		if(IsActive() && IsTimeOut())
		{
			Clear();
			return true;
		}
		else
			return false;
	}
	void	Clear		()
	{ 
		m_nInterval = 0;
		m_tUpdateTime = 0;
	}
	bool	IsActive	()
	{ 
		return m_tUpdateTime != 0;
	}
	void	IncInterval	(int nSecs, int nLimit)
	{ 
		m_nInterval = ::CutOverflow(m_nInterval+nSecs, nLimit);
	}
	void	DecInterval	(int nSecs)
	{ 
		m_nInterval = ::CutTrail(m_nInterval-nSecs, 0);
	}

	// 检查是否超时了指定时间片。用于同一时间标签检查多个不同超时事件。(可实现一个对象控制多个时钟)
	bool	IsTimeOut	(int nSecs)
	{
		bool result=::time(NULL) >= m_tUpdateTime + nSecs;
		return result;
	}
	// 用于变长的超时事件。
	bool	ToNextTime	(int nSecs)
	{ 
		bool result=IsTimeOut(nSecs);
		if(result)
		{
			Update();
			return true;
		}
		else 
			return false;
	}
	// 时钟是否触发(只触发一次)，与Startup配合使用，可实现只触发一次的时钟。
	bool	TimeOver	(int nSecs)
	{ 
		if(IsActive() && IsTimeOut(nSecs))
		{
			Clear();
			return true;
		}
		else
			return false;
	}
	// 时钟会积累，但积累值不超过间隔值。其它同ToNextTime(...)
	bool	ToNextTick	(int nSecs);
//	void	AppendInterval	(int nSecs)			{ if(ToNextTime()) m_nInterval=nSecs; else m_nInterval+=nSecs; }	// ToNextTime(): 保证至少有nSecs秒

public: // get
	int		GetRemain()
	{ 
		return m_tUpdateTime ? ::CutRange(m_nInterval - (int)(::time(NULL)-m_tUpdateTime), (int)0, m_nInterval) : 0;
	}
	int		GetInterval()
	{ 
		return m_nInterval;
	}
//06.11.7彭正帮添加,为了支持切换地图组-慎用----begin----------------------------------------------------
	time_t	GetUpdateTime()
	{ 
		return m_tUpdateTime;
	}
	void	SetUpdateTime( time_t time )
	{ 
		m_tUpdateTime = time;
	}
//-----------------------------------------end------------------------------------------------------
protected:
	int		m_nInterval;
	time_t	m_tUpdateTime;
public:
	MYHEAP_DECLARATION(s_heap);
};

//////////////////////////////////////////////////////////////////////
// 超时类，以“毫秒”为单位
class CTimeOutMS
{
public:
	CTimeOutMS(int nIntervalSecs = 0)
	{ 
		m_nInterval = nIntervalSecs;
		m_tUpdateTime = 0;
	}		// =0 : 用于数组初始化
//	virtual ~CTimeOutMS_() {}

public:
	void	Update		()
	{ 
		m_tUpdateTime = clock();
	}
	bool	IsTimeOut	()						
	{//[2009.07.06]
		//return clock() >= m_tUpdateTime+m_nInterval
		clock_t value=clock();
		return DWORD(value) >= DWORD(m_tUpdateTime) + DWORD(m_nInterval);
	}
	bool	ToNextTime	()
	{ 
		if(IsTimeOut())
		{
			Update();
			return true;
		}
		else
			return false;
	}
	void	SetInterval	(int nMilliSecs)
	{ 
		m_nInterval = nMilliSecs;
	}
	void	Startup		(int nMilliSecs)
	{ 
		m_nInterval = nMilliSecs;
		Update();
	}
	bool	TimeOver	()
	{ 
		if(IsActive() && IsTimeOut())
		{
			Clear();
			return true;
		}
		else
			return false;
	}
	void	Clear		()
	{ 
		m_nInterval = 0;
		m_tUpdateTime = 0;
	}
	bool	IsActive	()
	{ 
		return m_tUpdateTime != 0;
	}
	void	IncInterval	(int nMilliSecs, int nLimit)
	{ 
		m_nInterval = ::CutOverflow(m_nInterval+nMilliSecs, nLimit);
	}
	void	DecInterval	(int nMilliSecs)
	{ 
		m_nInterval = ::CutTrail(m_nInterval-nMilliSecs, 0);
	}
	bool	IsTimeOut	(int nMilliSecs)
	{ 
		clock_t value=clock();
		return value >= m_tUpdateTime+nMilliSecs;
	}
	int		LeaveTime()
	{ 
		return (int)(clock() -m_tUpdateTime-m_nInterval);
	}
	bool	ToNextTime	(int nMilliSecs)
	{ 
		if(IsTimeOut(nMilliSecs))
		{
			Update();
			return true;
		}
		else 
			return false;
	}
	bool	TimeOver	(int nMilliSecs)
	{ 
		if(IsActive() && IsTimeOut(nMilliSecs))
		{
			Clear();
			return true;
		}
		else
			return false;
	}
	bool	ToNextTick	(int nMilliSecs);
//	void	AppendInterval	(int nMilliSecs)	{ if(ToNextTime()) m_nInterval=nMilliSecs; else m_nInterval+=nMilliSecs; }	// ToNextTime(): 保证至少有nSecs秒

public: // get
	int		GetRemain();							
	int		GetInterval()
	{ 
		return m_nInterval;
	}
//06.11.7彭正帮添加,为了支持切换地图组,慎用-----begin----------------------------------------------------
	time_t	GetUpdateTime()
	{ 
		return m_tUpdateTime;
	}
	void	SetUpdateTime( time_t time )
	{ 
		m_tUpdateTime = time;
	}
//-----------------------------------------end------------------------------------------------------
protected:
	int		m_nInterval;
	time_t	m_tUpdateTime;
public:
	MYHEAP_DECLARATION(s_heap);
};

#endif // !defined(AFX_TIMEOUT_H__059AED43_D21C_4072_A172_20A7D8B6DF52__INCLUDED_)
