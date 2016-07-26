/*if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[RecommendTable]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[RecommendTable]
GO

CREATE TABLE [dbo].[RecommendTable] (
	[id] [int] NULL ,
	[UserID] [int] NULL ,
	[RecommendID] [int] NULL 
) ON [PRIMARY]
GO*/

#include "StdAfx.h"
#include "UserCustomLogo.h"
#include "GameLogonManage.h"
#include "MD5.h"

CUserCustomLogo::~CUserCustomLogo(void)
{
}

/*
Function	:OnNetMessage
Notes		:网络消息
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
	NetMessageHead	:网络消息头
	void *			:数据指针
	UINT			:数据包的大小
	UING			:未使用，原指IP地址
	UINT			:客户在服务器的SOCKET索引
return		:void
*/
void CUserCustomLogo::OnNetMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	if(pNetHead->bMainID!=MDM_GR_USER_LOGO)
		return;
	switch(pNetHead->bAssistantID)
	{
	case ASS_ULC_UPLOAD:			//上传文件，接收到的数据也包含在里面
		OnUploadFace(pData,uSize,uIndex,dwHandleID);
		break;
	case ASS_ULC_INFOREQUEST:		//客户端请求用户的头像信息
		OnRequestLogoInformation(pData,uSize,uIndex,dwHandleID);
		break;
	case ASS_ULC_DOWNREQUEST:		//请求下载某个用户的头像文件
		OnDownloadRequest(pData,uSize,uIndex,dwHandleID);
		break;	
	case ASS_ULC_DOWNRESULT:		//返回下载的结构，事实上只用于请求下一个数据块
		OnDownloadResult(pData,uSize,uIndex,dwHandleID);
		break;
	}
}

/*
Function	:OnUploadFace
Notes		:接收到上传文件数据包，分块上传
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
	void *			:数据指针
	UINT			:数据包的大小
	UINT			:客户在服务器的SOCKET索引
	UINT			:未知
return		:void
*/
void CUserCustomLogo::OnUploadFace(void *pData,UINT uSize,UINT uIndex,DWORD dwHandleID)
{
	//检查包大小
	if(sizeof(MSG_UL_C_UPLOAD)!=uSize)
		return;
	//取管理窗口
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;
	
	MSG_UL_C_UPLOAD *msg=(MSG_UL_C_UPLOAD*)pData;
	long dwUserID=msg->dwUserID;
	
	//根据用户ID号设置目录，每个用户用一个目录
	CString savePath;
	savePath.Format("%s\\%d",pWnd->m_CustomFacePath,dwUserID/1000);
	::CreateDirectory(savePath,NULL);
	savePath.Format("%s\\%d",pWnd->m_CustomFacePathUpload,dwUserID/1000);
	::CreateDirectory(savePath,NULL);

	//要保存的文件名
	savePath.Format("%s\\%d\\%d.png",pWnd->m_CustomFacePathUpload,dwUserID/1000,dwUserID);
	//如果是第一个数据包，要创建文件，否则只写
	UINT dwFlag=CFile::modeWrite;
	if(msg->nBlockIndex==0)
		dwFlag|=CFile::modeCreate;

	//要返回的消息，只返回数据块的索引和块数
	MSG_UL_S_UPLOADRESULT resultMsg;
	ZeroMemory(&resultMsg,sizeof(MSG_UL_S_UPLOADRESULT));
	resultMsg.dwUserID=dwUserID;
	resultMsg.nBlockCount=msg->nBlockCount;
	resultMsg.nBlockIndex=msg->nBlockIndex;
	resultMsg.bNeedCheck=pWnd->m_bCustomFaceNeedCheck;
	try
	{
		//写入文件
		CFile fw(savePath,dwFlag);
		fw.Seek(msg->nBlockIndex*512,CFile::begin);
		fw.Write(msg->szData,msg->nPackageSize);
		fw.Close();
	}
	catch (...) 
	{//写文件失败
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		pWnd->m_TCPSocket.SendData(uIndex,&resultMsg,sizeof(MSG_UL_S_UPLOADRESULT),MDM_GR_USER_LOGO,ASS_ULS_UPLOADFAILED,dwHandleID,0);
		return;
		
	}

	//返回消息
	pWnd->m_TCPSocket.SendData(uIndex,&resultMsg,sizeof(MSG_UL_S_UPLOADRESULT),MDM_GR_USER_LOGO,ASS_ULS_UPLOADSUCCEED,dwHandleID,0);
	TRACE("Receive Custom Logo File %d of %d at %s\n",msg->nBlockIndex,msg->nBlockCount,savePath);

	//文件上传完成的操作，注意，nBlockIndex是从开始的，所以nBlockCount-1才是最后一块数据
	if(msg->nBlockIndex==msg->nBlockCount-1)
	{
		//计算文件的MD5值
		MD5_CTX md5;
		CFile fr(savePath,CFile::modeRead);
		int len=fr.GetLength();
		byte *buf=new byte[len+1];
		ZeroMemory(buf,len+1);
		fr.Read(buf,len);
		fr.Close();
		md5.MD5Update(buf,len);
		byte *md5buf=new byte[16];
		ZeroMemory(md5buf,16);

		md5.MD5Final(md5buf);
		CString strmd5="",szChar;
		for(int i=0;i<16;i++)
		{
			szChar.Format("%2x",md5buf[i]);
			strmd5+=szChar;
		}
		delete md5buf;
		delete buf;

		//更新数据库
		UL_GP_I_UpdateUserLogo UL_Update;
		ZeroMemory(&UL_Update,sizeof(UL_GP_I_UpdateUserLogo));
		UL_Update.dwUserID=dwUserID;
		if(pWnd->m_bCustomFaceNeedCheck)
			UL_Update.nIconIndex=0x200+msg->dwUserSourceLogo;
		else
			UL_Update.nIconIndex=0x100+msg->dwUserSourceLogo;
		memcpy(UL_Update.szLogoMD5,strmd5,32);
		pWnd->m_SQLDataManage.PushLine(&UL_Update.DataBaseHead,sizeof(UL_GP_I_UpdateUserLogo),DTK_GP_USERLOGO_UPDATE,uIndex,0);
		TRACE("Receive Custom Logo Completed\n");
		//接收完成了
	}
}

/*
Function	:OnRequestLogoInformation
Notes		:请求某个用户的头像文件信息
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
	void *			:数据指针
	UINT			:数据包的大小
	UINT			:客户在服务器的SOCKET索引
	UINT			:未知
return		:void
*/
void CUserCustomLogo::OnRequestLogoInformation(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{

	if(sizeof(MSG_UL_C_INFORREQUEST)!=uSize)
		return;
	MSG_UL_C_INFORREQUEST * msg=(MSG_UL_C_INFORREQUEST*)pData;

	//需要向数据库查询，所以以下代码是建立一个数据库查询的请求，并加入队列中
	UL_GP_I_RequestInformationLogoMD5 UL_LogoMD5;
	ZeroMemory(&UL_LogoMD5,sizeof(UL_GP_I_RequestInformationLogoMD5));
	UL_LogoMD5.dwUserID=msg->dwUserID;
	UL_LogoMD5.dwRequestUserID=msg->dwRequestUserID;
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;
	pWnd->m_SQLDataManage.PushLine(&UL_LogoMD5.DataBaseHead,sizeof(UL_GP_I_RequestInformationLogoMD5),DTK_GP_USERLOGO_GETMD5,uIndex,0);
}

/*
Function	:OnDownloadRequest
Notes		:请求下载头像文件
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
	void *		:数据指针
	UINT		:数据包的大小
	UINT		:客户在服务器的SOCKET索引
	UINT		:未知
return		:void
*/
void CUserCustomLogo::OnDownloadRequest(void* pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	if(sizeof(MSG_UL_C_DOWNREQUEST)!=uSize)
		return;
	MSG_UL_C_DOWNREQUEST *msg=(MSG_UL_C_DOWNREQUEST *)pData;
	//开始下载第一块数据
	downloadCustomFace(msg->dwRequestUserID,0,uIndex,dwHandleID);
}

/*
Function	:downloadCustomFace
Notes		:读取头像文件并发给请求的客户端
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
	long		:用户的ID号，是被请求下载的用户ID号
	long		:数据块的索引
	UINT		:客户在服务器的SOCKET索引
	DWORD		:未知
return		:void
*/
void CUserCustomLogo::downloadCustomFace(long dwUserID, long nBlockIndex, UINT uIndex, DWORD dwHandleID)
{
	//取管理窗口指针
	CGameLogonManage *pWnd=(CGameLogonManage*)pParentWnd;
	//计算头像文件名
	CString logoFile;
	logoFile.Format("%s\\%d\\%d.png",pWnd->m_CustomFacePath,dwUserID/1000,dwUserID);
	//查询文件是否存在
	CFileFind ff;
	BOOL bFoundFile=ff.FindFile(logoFile);
	ff.Close();
	if(!bFoundFile)
		return;
	//读取文件块，每个块大小为
	char buf[513]={0};					//多一个字节
	CFile fr(logoFile,CFile::modeRead);
	int filelen=fr.GetLength();			//文件大小
	int packagesize=512;				//数据块大小
	if(nBlockIndex*512>filelen)			//是否超出了文件的结尾
	{
		fr.Close();
		return;
	}
	if(nBlockIndex*512+packagesize>filelen)		//最后一块大小是否超出了文件的结尾
		packagesize=filelen-nBlockIndex*512;	//最不足块大小的数据

	fr.Seek(nBlockIndex*512,CFile::begin);		//文件定位
	fr.Read(buf,packagesize);					//读数据
	fr.Close();									//关闭文件

	//填充数据结构
	MSG_UL_S_DOWN msg;
	ZeroMemory(&msg,sizeof(MSG_UL_S_DOWN));
	msg.dwUserID=dwUserID;							//用户ID
	msg.nBlockIndex=nBlockIndex;					//数据块索引
	msg.nBlockSize=512;								//数据块大小
	msg.nPackageSize=packagesize;					//数据包的大小
	msg.nFileSize=filelen;							//文件大小
	msg.nBlockCount=(filelen-1)/512+1;				//数据块数量
	memcpy(msg.szData,buf,packagesize);				//数据

	//发送数据
	pWnd->m_TCPSocket.SendData(uIndex,&msg,sizeof(MSG_UL_S_DOWN),MDM_GR_USER_LOGO,ASS_ULS_DOWN,dwHandleID,0);

}
/*
Function	:OnDownloadResult
Notes		:客户端返回了接收数据包的结果，事实上，客户端只有在接收成功了才会发过来
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
	void *		:数据指针
	UINT		:数据包的大小
	UINT		:客户在服务器的SOCKET索引
	UINT		:未知
return		:void
*/
void CUserCustomLogo::OnDownloadResult(void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	if(sizeof(MSG_UL_C_DOWNRESULT)!=uSize)
		return;
	MSG_UL_C_DOWNRESULT *msg=(MSG_UL_C_DOWNRESULT *)pData;
	//msg->nBlockIndex这个数据块已成功发送，所以发送下一个数据块
	msg->nBlockIndex++;
	downloadCustomFace(msg->dwUserID,msg->nBlockIndex,uIndex,dwHandleID);// msg->dwRequestUserID,0,uIndex,dwHandleID);
}

