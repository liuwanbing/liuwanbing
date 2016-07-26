
/*****************************************************************************
*File name    :		RecordMessage.h
*Description   :	记录消息内容类
*Author       :		任德
*Version       :	欢乐围屋3.0.2.0
*Date         :		2013.1.30
*Others       :		
*History 	  :		
*Date         Version       Author               Modify
*******************************************************************************/ 
#include "StdAfx.h"
#include "MessageRecordMgr.h"
#include "AFCFunction.h"
/*************************************************
Function          :  CMessageRecordMgr
Description    : CMessageRecordMgr构造函数
Input                :  无
Output             :  无
Return              :  无
Author              :  任德
Date                :  2013-01-30
*************************************************/
CMessageRecordMgr::CMessageRecordMgr()
	: CMessage()
{
    m_bCatalogFileExist = false;
    m_bContentFileExist = false;
    m_mapMsgFileOpen.clear();
}

/*************************************************
Function      :  ~CMessageRecordMgr
Description   :  CMessageRecordMgr析构函数
Input         :  无
Output        :  无
Return        :  无
Author        :  任德
Date          :  2013-01-30
*************************************************/
CMessageRecordMgr::~CMessageRecordMgr()
{
    m_fCatalogFile.Close();
    m_fContentFile.Close();
}

/*************************************************
Function      :  Init
Description   :  初始化 文件名称 消息头等
Input         :  无
Output        :  无
Return        :  无
Author        :  任德
Date          :  2013-01-30
*************************************************/
void CMessageRecordMgr::Init()
{
	//1.清空玩家和聊友字符串
    Clear();

    //2.初始化文件名称部分
    InitPartMsgFileName();

    //3.产生文件名
    GenerateMsgFileName();

	//4.产生文件路径
    GenerateMsgFilePath();
}
/*************************************************
Function      :  WriteOneLineMsg
Description   :  向消息框写入一条消息
Input         :  const CString& strMsg
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-01-30
*************************************************/
bool CMessageRecordMgr::WriteOneLineMsg()
{
    //1.创建并打开两个文件
    if (!OpenMsgFile(true))
        return false;

    //2.设置目录文件内容
    SetCatalogData();

	//3.写目录文件 内容文件
    if (!Write())
        return false;

	return true;
}
/*************************************************
Function      :  ReadMsg
Description   :  一次性读取所有消息
Input         :  无
Output        :  无
Return        :  CString
Author        :  任德
Date          :  2013-01-30
*************************************************/
bool CMessageRecordMgr::ReadMsg(vector<CString>& vecContentData)
{
    vector<CString> vecCatalogData;

    //1.打开两个文件
	if (!OpenMsgFile(false))
        return false;

    //2.读取目录文件
    if (!ReadCatalogFile(vecCatalogData))
        return false;

    //3.读取内容文件
    if (!ReadContentFile(vecCatalogData,vecContentData))
        return false;

    return true;
}

/*************************************************
Function      :  Clear
Description   :  清空路径等字符串
Input         :  无
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-30
*************************************************/
void CMessageRecordMgr::Clear()
{
    m_ulMsgLen = 0;
    m_strCatalogData = "";
    m_strCatalogFilePath = "";
    m_strContentFilePath = "";
    m_strCatalogFileName = "";
    m_strContentFileName = "";
    m_vecCatalogData.clear();
    m_vecContentData.clear();
}
/*************************************************
Function      :  IsOpenCatalogFile
Description   : 是否打开目录文件
Input         :  无
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-01-31
*************************************************/
bool CMessageRecordMgr::IsOpenFile(const CString& strFilePath)
{
    //是否打开目录文件
    MAPMSGFILEOPENITER mapFileIter = m_mapMsgFileOpen.find(strFilePath);
    if (m_mapMsgFileOpen.end() != mapFileIter)
    {
        if (mapFileIter->second)
            return true;
        else
        {
            return false;
        }
    }
    return false;
}
/*************************************************
Function      :  CloseFile
Description   :  关闭文件并置文件关闭的状态标志
Input         :  无
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-01-31
*************************************************/
void CMessageRecordMgr::CloseFile()
{
    m_mapMsgFileOpen[m_strCatalogFilePath] = false;
    m_mapMsgFileOpen[m_strContentFilePath] = false;

    CFileStatus status;
    if(m_fCatalogFile.GetStatus(m_strCatalogFilePath,status)) //判断是否打开
        m_fCatalogFile.Close();

    if(m_fContentFile.GetStatus(m_strContentFilePath,status)) //判断是否打开
        m_fContentFile.Close();
}
/*************************************************
Function      :  IsFileExist
Description :  文件是否存在
Input           :  无
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-01-31
*************************************************/
bool CMessageRecordMgr::IsFileExist(CString& strFile,const CString& strMsgFileName)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hListFile = NULL;
    CString strFindFile("");

    TCHAR curDir[MAX_PATH] = {0};
    ::GetCurrentDirectory(MAX_PATH,curDir);

    strFindFile.Format("%s\\MsgRecord\\*",curDir);
    hListFile = FindFirstFile(strFindFile.GetBuffer(),&FindFileData);
    if (INVALID_HANDLE_VALUE == hListFile)
        return false;

    do
    {
        if (0 == lstrcmp(FindFileData.cFileName,TEXT("."))
            || 0 == lstrcmp(FindFileData.cFileName,TEXT("..")))
            continue;

        bool bFind = true;
        int nFindLen = strMsgFileName.GetLength();
        if (strlen(FindFileData.cFileName) > nFindLen)
        {
            for (int i=0; i<nFindLen; i++)
            {
                if (FindFileData.cFileName[i] != strMsgFileName[i])
                {
                    bFind = false;
                    break;
                }
            }
            if (bFind)
            {
                strFile.Format("%s",FindFileData.cFileName);
                return true;
            }
        }
    } while (FindNextFile(hListFile,&FindFileData));

    return false;
}
/*************************************************
Function      :  GetMsgFilePath
Description :  获取文件路径
Input           :  无
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-31
*************************************************/
void CMessageRecordMgr::GenerateMsgFilePath()
{
    //1.获取当前目录
    TCHAR curDir[MAX_PATH] = {0};
    ::GetCurrentDirectory(MAX_PATH,curDir);

    //2.判断存放消息记录文件夹是否存在，如不存在则增加文件夹
    CString strPath("");
    strPath.Append(curDir);
    strPath.Append(_T("\\"));
    if (!IsDirExist(strPath))
    {
        strPath.Append(_T("MsgRecord\\"));
        CreateDirectory(strPath,NULL);
    }
    else
        strPath.Append(_T("MsgRecord\\"));

    //3.目录文件存放路径
    m_strCatalogFilePath.Append(strPath);
    m_strCatalogFilePath.Append(m_strCatalogFileName);

    //4.内容文件存放路径
    m_strContentFilePath.Append(strPath);
    m_strContentFilePath.Append(m_strContentFileName);
}
/*************************************************
Function      :  IsDirExist
Description :  文件夹是否存在
Input           :  const CString& strDir
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-01-31
*************************************************/
bool CMessageRecordMgr::IsDirExist(const CString& strDir)
{
    CFileFind cfFinder;
    BOOL bWorking = cfFinder.FindFile(strDir);
    while (bWorking)
    {
        bWorking = cfFinder.FindNextFile();
        if (0 == (cfFinder.GetFileName()).Compare(_T("MsgRecord")))
            return true;
    }
    cfFinder.Close();
    return false;
}
/*************************************************
Function      :  GetMsgFileName
Description :  获取文件名
Input           :  无
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-31
*************************************************/
void CMessageRecordMgr::InitPartMsgFileName()
{
    //1.初始玩家和聊友字符串
    CString strUserID("");
    CString strChatID("");
    strUserID.Format("[%d]",m_nOwnerID);
    strChatID.Format("[%d]",m_nChaterID);

    //2.组装目录文件的名称字段
    m_strCatalogFileName = _T("玩家");
    m_strCatalogFileName = m_strCatalogFileName + strUserID + _T("_") + _T("聊友")
        + strChatID + _T("_catalog_");

    //3.组装内容文件的名称字段
    m_strContentFileName = _T("玩家");
    m_strContentFileName = m_strContentFileName + strUserID + _T("_") + _T("聊友")
        + strChatID + _T("_content_");
}
/*************************************************
Function      :  JudgeExistFileName
Description :  获取存在的文件名称,若不存在就组装新文件名
Input           :  无
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-31
*************************************************/
void CMessageRecordMgr::GenerateMsgFileName()
{
    //1.获取系统时间
    GetLocalTime(&m_curTime);
    CString strTime("");
    strTime.Format(TEXT("%02d年_%02d月_%02d日"),m_curTime.wYear,m_curTime.wMonth,m_curTime.wDay);

    //2.消息目录文件名称 当前玩家某某_聊友某某_catalog_聊天开始时间.txt
    CString strFilePath("");
    m_bCatalogFileExist = IsFileExist(strFilePath,m_strCatalogFileName);
    if (!m_bCatalogFileExist)
        m_strCatalogFileName = m_strCatalogFileName + strTime + ".txt";
    else
        m_strCatalogFileName = strFilePath;

    //3.消息内容文件名称 当前玩家某某_聊友某某_content_聊天开始时间_.txt
    m_bContentFileExist = IsFileExist(strFilePath,m_strContentFileName);
    if (!m_bContentFileExist)
        m_strContentFileName = m_strContentFileName + strTime + ".txt";
    else
        m_strContentFileName = strFilePath;
}
/*************************************************
Function      :  OpenFile
Description :  打开目录和内容文件
Input           :  无
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-31
*************************************************/
bool CMessageRecordMgr::OpenMsgFile(bool bCreateFile)
{
    //1.文件是否打开
    bool bOpenCatalogFile = false;
    bool bOpenContentFile = false;

    if (!m_bCatalogFileExist)
        bOpenCatalogFile = true;
    else if (!IsOpenFile(m_strCatalogFilePath))
        bOpenCatalogFile = true;

    if (!m_bContentFileExist)
        bOpenContentFile = true;
    else if (!IsOpenFile(m_strContentFilePath))
        bOpenContentFile = true;
    
    //1.打开目录文件
    if (!OpenCatalogfile(bCreateFile,bOpenCatalogFile))
        return false;

    //2.打开内容文件
    if (!OpenContentfile(bCreateFile,bOpenContentFile))
        return false;

    return true;
}
/*************************************************
Function      :  OpenCatalogfile
Description :  打开目录文件
Input           :  const bool bCreateFile,const bool bOpenCatalogFile
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-31
*************************************************/
bool CMessageRecordMgr::OpenCatalogfile(const bool bCreateFile,const bool bOpenCatalogFile)
{
    CFileException e;
    if (bOpenCatalogFile)
    {
        if (bCreateFile)
        {
            if (FALSE == m_fCatalogFile.Open(m_strCatalogFilePath.GetBuffer(),CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary | CFile::modeNoTruncate,&e))
            {
                CString strCatalog("");
                strCatalog.Format("创建%s文件失败!",m_strCatalogFilePath);
                DUIOkeyMsgBox(NULL,strCatalog);
                return false;
            }
        }
        else
        {
            if (FALSE == m_fCatalogFile.Open(m_strCatalogFilePath.GetBuffer(),CFile::modeReadWrite | CFile::typeBinary,&e))
            {
                return false;
            }
        }
        m_mapMsgFileOpen[m_strCatalogFilePath] = true;
    }
    return true;
}
/*************************************************
Function      :  OpenContentfile
Description :  打开内容文件
Input           :  bool bCreateFile,bool bOpenContentFile
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-01-31
*************************************************/
bool CMessageRecordMgr::OpenContentfile(const bool bCreateFile,const bool bOpenContentFile)
{
    CFileException e;
    if (bOpenContentFile)
    {
        if (bCreateFile)
        {
            if (FALSE == m_fContentFile.Open(m_strContentFilePath.GetBuffer(),CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary | CFile::modeNoTruncate,&e))
            {
                CString strContent("");
                strContent.Format("创建%s文件失败!",m_strContentFilePath);
                DUIOkeyMsgBox(NULL,strContent);
                return false;
            }
        }
        else
        {
            if (FALSE == m_fContentFile.Open(m_strContentFilePath.GetBuffer(),CFile::modeReadWrite | CFile::typeBinary,&e))
            {
                return false;
            }
        }
        m_mapMsgFileOpen[m_strContentFilePath] = true;
    }
    return true;
}

/*************************************************
Function      :  SetCatalogData
Description :  设置目录文件内容
Input           :  无
Output        :  无
Return        :  void
Author        :  任德
Date          :  2013-02-01
*************************************************/
void CMessageRecordMgr::SetCatalogData()
{
    //2.写目录文件
    CString strUserID("");
    CString strChatID("");
    CString strField("");
    strUserID.Format("[%d]",m_nOwnerID);
    strChatID.Format("[%d]",m_nChaterID);

    //3.组装目录消息 [玩家ID][聊友ID][消息开始位置][消息长度]
    if (m_bSendMsg)
    {
        m_strCatalogData.Append(strUserID);	//玩家ID
        strField.Format("{%s}",m_strOwnerName);
        m_strCatalogData.Append(strField);
        m_strCatalogData.Append(strChatID);	//聊友ID
        strField.Format("{%s}",m_strChaterName);
        m_strCatalogData.Append(strField);
    }
    else
    {
        m_strCatalogData.Append(strChatID);
        strField.Format("{%s}",m_strChaterName);
        m_strCatalogData.Append(strField);
        m_strCatalogData.Append(strUserID);
        strField.Format("{%s}",m_strOwnerName);
        m_strCatalogData.Append(strField);
    }
    strField.Format("<%d>",m_nMsgLen);	//单条消息长度
    m_strCatalogData.Append(strField);
    strField.Format("(%s)",m_strChatDay);
    m_strCatalogData.Append(strField);
    strField.Format("(%s)",m_strChatTime);
    m_strCatalogData.Append(strField);
    m_strCatalogData.Append(_T("\r\n"));
}
/*************************************************
Function      :  Write
Description   :  写入目录和内容文件
Input         :  无
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-02-01
*************************************************/
bool CMessageRecordMgr::Write()
{
    try
    {
        //1.初始待计算位置数据
        int nRetPos = 0;
        int nCurPos = 0;
        CString strCatalogData("");
        std::string strChatMsg(m_strMsg);

        //2.计算位置并统计
        while (-1 != nRetPos)
        {
            CString strLen("");
            CString strValidMsg("");

            nRetPos = strChatMsg.find(_T("\r\n"),nCurPos);
            if (-1 != nRetPos)
            {
                //到回车为止的一句话
                strValidMsg = m_strMsg.Mid(nCurPos,nRetPos - nCurPos);

                //实际长度
                strLen.Format("%d",strValidMsg.GetLength());

                //去掉回车后的位置
                nCurPos = nRetPos + 2;
            }
            else
            {
                //剩余内容
                strValidMsg = m_strMsg.Mid(nCurPos);

                //实际长度
                if (nCurPos != 0 && strValidMsg != "")
                    strLen.Format("%d",strValidMsg.GetLength());
            }

            //写入回车符为止的实际长度
            if (strLen != "")
            {
                int nBeginPos = m_strCatalogData.Find("<");
                int nEndPos = m_strCatalogData.Find(">");
                strCatalogData = m_strCatalogData.Mid(0,nBeginPos+1) + strLen + m_strCatalogData.Mid(nEndPos,m_strCatalogData.GetLength());
            }

            if (strValidMsg != "")
            {
                m_fCatalogFile.SeekToEnd();
                strCatalogData = ("" == strCatalogData) ? m_strCatalogData : strCatalogData;
                m_fCatalogFile.Write(strCatalogData.GetBuffer(),strCatalogData.GetLength());

                m_fContentFile.SeekToEnd();
                m_fContentFile.Write(strValidMsg.GetBuffer(),strValidMsg.GetLength());
            }
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}
/*************************************************
Function      :  ReadCatalogFile
Description   :  读取目录文件中一行
Input         :  CString& strCatalogData
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-02-01
*************************************************/
bool CMessageRecordMgr::ReadCatalogFile(vector<CString>& vecCatalogData)
{
     CString strCatalogData("");

     //1.读取文件内容
     m_fCatalogFile.SeekToBegin();
     CArchive ar(&m_fCatalogFile,CArchive::load);
     while(NULL != ar.ReadString(strCatalogData))    //循环读取文件，直到文件结束
     {
         if ("" == strCatalogData)
             continue;
         vecCatalogData.push_back(strCatalogData);
         int nBeginPos = strCatalogData.Find("<");
         int nEndPos = strCatalogData.Find(">");
         CString strLen(strCatalogData.Mid(nBeginPos+1,nEndPos-(nBeginPos+1)));
         m_ulMsgLen += atoi(strLen);
     }

     //2.判断内容是否为空
     if (vecCatalogData.empty())
     {
             return false;
     }

    return true;
}
/*************************************************
Function      :  ReadContentFile
Description   :  读取内容文件
Input         :  无
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-02-03
*************************************************/
bool CMessageRecordMgr::ReadContentFile(const vector<CString>& vecCatalogData,vector<CString>& vecContentData)
{
    int curPos = 0; 
    CString strCatalog("");
    CString strContent("");
    CString strContentTmp("");
    CString strSameName("");

    m_fContentFile.Seek(curPos,CFile::begin);
    CArchive ar(&m_fContentFile,CArchive::load);
    while(NULL != ar.ReadString(strContentTmp))    //循环读取文件，直到文件结束
     {
         if ("" == strContentTmp)
             continue;
         strContent += strContentTmp;
     }

    VECCATALOGDATAITER vecCatalogIter = vecCatalogData.begin();
    while (vecCatalogIter != vecCatalogData.end())
    {
        strCatalog.Format("%s",*vecCatalogIter);

        //1.获取玩家昵称
        int nBeginPos = strCatalog.Find("{");
        int nEndPos = strCatalog.Find("}");
        CString strNameTmp(strCatalog.Mid(nBeginPos + 1,nEndPos - (nBeginPos + 1)));
        strNameTmp.Append(_T("说:"));

        //2.记录的时间
        SYSTEMTIME sysRecordTime;
        nBeginPos = strCatalog.Find("(");
        CString strTime(strCatalog.Mid(nBeginPos,nBeginPos+11));
        sysRecordTime.wYear = atoi(strTime.Mid(1,4));
        sysRecordTime.wMonth = atoi(strTime.Mid(6,7));
        sysRecordTime.wDay = atoi(strTime.Mid(9,10));
        sysRecordTime.wHour = atoi(strTime.Mid(13,14));
        sysRecordTime.wMinute = atoi(strTime.Mid(16,17));
        sysRecordTime.wSecond = atoi(strTime.Mid(19,20));

        //3.获取玩家聊天时间
        strNameTmp.Append(_T("      "));
        strNameTmp.Append(strTime.Mid(0,11));
        nBeginPos = strCatalog.Find(")");
        strTime = strCatalog.Mid(nBeginPos+2,10);
        strNameTmp.Append(_T("   "));
        strNameTmp.Append(strTime);

        //4.获取聊天内容长度
        nBeginPos = strCatalog.Find("<");
        nEndPos = strCatalog.Find(">");
        CString strContentLen(strCatalog.Mid(nBeginPos+1,nEndPos - (nBeginPos+1)));
        int ContentLen = atoi(strContentLen.GetBuffer());

        //5.判断是否规定时间内的记录
        if (!IsValidChatTime(sysRecordTime))
        {
            curPos += ContentLen;
            ++vecCatalogIter;
            continue;
        }

        //6.重复的目录及时间代表是同一人说话
        if (0 != strSameName.Compare(strNameTmp))
            vecContentData.push_back(strNameTmp);
        strSameName = strNameTmp;

        //7.根据聊天内容长度读取内容文件获取聊天内容
        CString strContentData(strContent.Mid(curPos,ContentLen));
        vecContentData.push_back(strContentData);
        curPos += ContentLen;
        ++vecCatalogIter;
    }
    return true;
}
/*************************************************
Function      :  IsValidChatTime
Description   : 聊天信息是否有效(显示7天内的聊天信息)
Input         :  无
Output        :  无
Return        :  bool
Author        :  任德
Date          :  2013-02-03
*************************************************/
bool CMessageRecordMgr::IsValidChatTime(SYSTEMTIME sysRecordTime)
{
    CTime curTime = CTime::GetCurrentTime();
    __time64_t nCurTime = curTime.GetTime();
    CTime sysRecTime(sysRecordTime,-1);
    __time64_t nDif = nCurTime - sysRecTime.GetTime();

    if (nDif / ONE_DAY_HAVE_SECOND > SAVE_CHAR_RECORD_TIME)
        return false;
    else
        return true;
}