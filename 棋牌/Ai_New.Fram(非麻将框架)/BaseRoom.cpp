///基本房间
#include "StdAfx.h"
#include "BaseRoom.h"

BEGIN_MESSAGE_MAP(CBaseRoom, CDialog)
//	ON_WM_SIZE()
END_MESSAGE_MAP()


//进度条定时器 ID
#define ID_PRO_TIME					100			
/*******************************************************************************************************/


/*******************************************************************************************************/

//构造函数
CBaseRoom::CBaseRoom(UINT uTemplate) :CDialog(uTemplate)
{
	m_TCPSocket = new CTCPClientSocket(this);
}

//析构函数
CBaseRoom::~CBaseRoom()
{
	if (m_TCPSocket != NULL)
		delete m_TCPSocket;
}

//初始化函数
BOOL CBaseRoom::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return TRUE;
}


//发送网络消息函数
int CBaseRoom::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return m_TCPSocket->SendData(bMainID,bAssistantID,bHandleCode);//处理主类型
}

//发送网络消息函数
int CBaseRoom::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return m_TCPSocket->SendData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
}

/*******************************************************************************************************/

