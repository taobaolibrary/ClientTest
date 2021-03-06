// 接口类。接口：基于消息通讯机制的线程间通讯接口。
// 仙剑修，2002.8.28

#ifndef	MESSAGEPORT_H
#define MESSAGEPORT_H

#ifndef WINVER                  
#define WINVER 0x0501          
#endif

#ifndef _WIN32_WINNT            
#define _WIN32_WINNT 0x0501    
#endif

#pragma warning(disable:4786)
#include <list>
#include <vector>
#include "windows.h"
#include "./I_MessagePort.h"
#include "./MyHeap.h"
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////////////////
// 通用定义
/////////////////////////////////////////////////////////////////////////////////////////////////
MSGPORT_BEGIN
const int	MAX_MSGPACKSIZE = 2048;			// 消息包数据的最大尺寸
struct	CMessagePacket
{
public:
	unsigned int		m_nPortFrom;
	unsigned int		m_nPacket;
	unsigned int		m_nVarType;
	char	m_bufData[MAX_MSGPACKSIZE];
	MYHEAP_DECLARATION(s_heap);
};
MSGPORT_END
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
class	CMessagePort : private IMessagePort
{
protected:
	CMessagePort(int nPort) 
	{ 
		m_id			= nPort; 
		m_nState		= STATE_CLOSED; 
		m_debugPackets	= 0;
		InitializeCriticalSection(&m_cs);
	}
	virtual ~CMessagePort	() 
	{ 
		EnterCriticalSection(&m_cs);
		Clear(); 
		LeaveCriticalSection(&m_cs);
		DeleteCriticalSection(&m_cs);
	}

	IMessagePort*	GetInterface() { return (IMessagePort*)this; }

protected: // Interface
	virtual bool	IsOpen	() {  return m_nState == STATE_OK; }

	// 取本接口的ID号
	virtual int		GetID	() {  return m_id; }
	virtual int		GetSize	() { return m_setPort.size(); }

	// 初始化，设置接口ID号，开始接收消息。可重复调用(PORT_ID不能改变)。
	virtual bool	Open	();
	// 关闭接口，不再接收消息。可重复调用。
	virtual bool	Close	();

	// 发送消息到指定接口。包含消息ID、数据类型、数据。return false: 发送不成功，或已经关闭
	virtual bool	Send	(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);
	// 接收指定接口(或所有接口)发来的消息。可指定消息ID，也可不指定。VARTYPE无意义。return false: 没有收到数据
	virtual bool	Recv	(int nPort, int nPacket, VAR_TYPE nVarType, void* buf, CMessageStatus *pStatus = NULL);

protected:	// 交叉调用函数，需要互斥
	bool		PushMsg		(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);

	size_t		GetMsgSize()	{ return m_setMsg.size(); }//20070518彭正帮:public保证调用
private:	// 内部函数，不需要互斥
	bool		PopMsg		(int *nPort, int *nPacket, VAR_TYPE *nVarType, void* buf);
	static int	SIZE_OF_TYPE		(int type);

	// 由SET的函数调用
	bool		Init();
	void		Clear();

protected:
	int			m_id;
	enum { STATE_OK, STATE_CLOSED };
	int			m_nState;
	typedef	list<message_port::CMessagePacket*>	MSG_SET;			//?? 可进行回收
	MSG_SET		m_setMsg;
	CRITICAL_SECTION m_cs;
	int			m_debugPackets;
	MSG_SET		m_setRecycle;

////////////////////////////////////////////////////////////////////////////////////////
// 共用静态对象
// 所有接口的添加必须在首次通讯前完成。通讯期间接口集不会有变化。完成所有通讯后，才能清空接口。
// 注意：目前只支持“静态”接口集。(★该部分不支持线程安全★)
public:
	static bool	InitPortSet(int nPortNum);
	static void	ClearPortSet();
	static IMessagePort*	GetInterface(int nPort) 
	{
		if (nPort >= 0 && nPort < (int)m_setPort.size()) 
		{
			return m_setPort[nPort]->GetInterface(); 
		}
		else 
		{
			return NULL;
		}
	}

	static int	GetQueueSize(int nPort)				
	{
		if(nPort >= 0 && nPort < (int)m_setPort.size()) 
		{
			return m_setPort[nPort]->m_setMsg.size(); 
		}
		else 
		{
			return -1;
		}
	}

protected: //static
	typedef	vector<CMessagePort*>	PORT_SET;			
	static PORT_SET					m_setPort;	// “静态”表，运行期间不修改。
};


#endif // MESSAGEPORT_H














