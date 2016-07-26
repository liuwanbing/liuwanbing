#include"stdafx.h"
#include"WriteLog.h"
#include "afx.h"
#include "Shlwapi.h"
#include "ATLComTime.h"

extern CString GetAppPath(bool bFource=true);   // PengJiLin, 2010-6-7

//删除
void Dellog(int iRoomID,int iSmallRoomID)
{
	TCHAR szFileName	[50];
	wsprintf(szFileName,"%d-%d.txt",iRoomID,iSmallRoomID);
	try
	{
		::DeleteFile(szFileName);
	}
	catch(...)
	{TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		return ;
	}
	return ;
}
//写正常日志
void WriteStr(TCHAR *str,int iRoomID,int iSmallRoomID)
{
	try
	{	
		CFile file;
		TCHAR szFileName	[50];
		TCHAR szNewLine[]="\r\n";
		wsprintf(szFileName,"%d-%d.txt",iRoomID,iSmallRoomID);
		file.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);//建立新文件
		file.SeekToEnd();
		file.Write(szNewLine,strlen(szNewLine));			//换行	
		file.Write(str,strlen(str));						//写入游戏信息
		file.Close();
	}
	catch(CException *e)
	{		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		//AfxMessageBox("some exception!");
		e->Delete();
		return ; 
	}
}

void WriteStr(int iValue)
{
	TCHAR sz[200];
	wsprintf(sz,"iValue = %d",iValue);
	WriteStr(sz);
}

void WriteStr(long iValue)
{
	TCHAR sz[200];
	wsprintf(sz,"iValue = %ld",iValue);
	WriteStr(sz);
}
void  WriteError(TCHAR *pMessage ,int iRoomID,int iSmallRoomID )
{
	try
	{
		char FileName[50];
		TCHAR path[MAX_PATH];
		
        // PengJiLin, 2010-6-7, GetCurrentDirectory()函数全部替代
        //::GetCurrentDirectory (MAX_PATH,path);
        lstrcpy(path, GetAppPath());

		wsprintf(FileName,"%s\\Error%d-%d.txt\0",path,iRoomID,iSmallRoomID);//GetCurrentProcessId());
		CFile syslog;
		syslog.Open(FileName,CFile::modeNoTruncate| CFile::modeCreate |CFile::modeReadWrite);
		syslog.SeekToEnd();
		syslog.Write(pMessage,strlen(pMessage));

		syslog.SeekToEnd();
		TCHAR nextLine[] = "\r\n";
		syslog.Write(nextLine,strlen(nextLine));

		syslog.Close();
	}
	catch(CException *e)
	{		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		//AfxMessageBox("some exception!");
		e->Delete();
		return ; 
	}
}
