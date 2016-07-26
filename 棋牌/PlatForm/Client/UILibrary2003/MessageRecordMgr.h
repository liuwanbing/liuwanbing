
/*****************************************************************************
*File name    :		RecordMessage.h
*Description   :	记录消息内容类
*Author       :		任德
*Version       :	欢乐围屋3.0.2.0
*Date         :		2012.1.30
*Others       :		
*History 	  :		
*Date         Version       Author               Modify
*******************************************************************************/ 
#ifndef _RECORDMESSAGE_H_
#define	_RECORDMESSAGE_H_

#define SAVE_CHAR_RECORD_TIME  7    //保存聊天记录时间
const __time64_t ONE_DAY_HAVE_SECOND = 60*60*24;
#include "PublicDefine.h"
#include <vector>

//聊天消息结构体
class CMessage
{
public:
	//当前玩家ID
	int m_nOwnerID;

	//当前玩家名称
	CString m_strOwnerName;

	//聊友ID
	int m_nChaterID;

	//聊友名称
	CString m_strChaterName;

	//单条记录长度
	int m_nMsgLen;

	//单条聊天内容
	CString m_strMsg;

    //聊天时间 日期: 年 月 日
    CString m_strChatDay;

	//聊天时间 时:分:秒
	CString m_strChatTime;

	//聊天方向 true 代表玩家发送消息给聊友 false 则相反之。
	bool m_bSendMsg;

	//消息索引(用于读取消息)
	int m_nMsgIndex;

    //当前时间
    SYSTEMTIME m_curTime;

	//初始化消息体
	CMessage()
	{
		m_nOwnerID = 0;
		m_strOwnerName = "";
		m_nChaterID = 0;
		m_strChaterName = "";
		m_nMsgLen = 0;
		m_strMsg = "";
        m_strChatDay = "";
		m_strChatTime = "";
		m_bSendMsg = true;
		m_nMsgIndex = 0;
	}
};

//消息记录管理类
class EXT_CLASS CMessageRecordMgr  : public CMessage
{
public:

	CMessageRecordMgr();

	~CMessageRecordMgr();

public:
    //初始化文件名称 消息头等
    void Init();

	//向消息框写入一条消息 [索引][玩家ID][聊友ID][消息开始位置][消息长度]
	bool WriteOneLineMsg();

	//一次性读取所有消息
	bool ReadMsg(vector<CString>& vecContentData);

    //关闭文件
    void CloseFile();

private:
    //清空自定义路径等字符串
    void Clear();

    //是否打开文件
    bool IsOpenFile(const CString& strFilePath);

    //文件夹是否存在
    bool CMessageRecordMgr::IsDirExist(const CString& strDir);

    //文件是否存在
    bool IsFileExist(CString& strFile,const CString& strMsgFileName);

    //获取文件路径
    void GenerateMsgFilePath();

    //获取文件名称
    void InitPartMsgFileName();

    //获取存在的文件名称,若不存在就组装新文件名
    void GenerateMsgFileName();

    //设置目录文件内容
    void SetCatalogData();

    //打开文件
    bool OpenMsgFile(bool bCreateFile);

    //打开目录文件
    bool OpenCatalogfile(const bool bCreateFile,const bool bOpenCatalogFile);

    //打开内容文件
    bool OpenContentfile(const bool bCreateFile,const bool bOpenContentFile);

    //写入文件
    bool Write();

    //读取目录文件
    bool ReadCatalogFile(vector<CString>& vecCatalogData);

    //读取内容文件
    bool ReadContentFile(const vector<CString>& vecCatalog,vector<CString>& vecContentData);

    //是否有效的聊天时间
    bool IsValidChatTime(SYSTEMTIME sysRecordTime);
private:
    //管理聊天文件,bool为是否打开聊文件
    std::map<CString,bool> m_mapMsgFileOpen;

	//消息目录文件名称 当前玩家某某_聊友某某_catalog_聊天开始时间.txt
	CString m_strCatalogFileName;

	//消息内容文件名称 当前玩家某某_聊友某某_content_聊天开始时间_.txt
	CString m_strContentFileName;

	//消息目录文件中消息头结构
	CString m_strCatalogData;

	//目录文件存放路径
	CString m_strCatalogFilePath;

	//内容文件存放路径
	CString m_strContentFilePath;

    //目录文件对象
    CFile m_fCatalogFile;

    //内容文件对象
    CFile m_fContentFile;

    //目录文件是否存在
    bool m_bCatalogFileExist;

    //内容文件是否存在
    bool m_bContentFileExist;

    //存放目录文件中内容
    vector<CString>m_vecCatalogData;

    //聊天内容
    vector<CString> m_vecContentData;

    //消息总长度
    ULONG m_ulMsgLen;
private: 
    //存放文件是否存在的容器迭代器
    typedef std::map<CString,bool>::const_iterator MAPMSGFILEOPENITER;

    //存放目录文件中内容的容器迭代器
    typedef std::vector<CString>::const_iterator VECCATALOGDATAITER;
};
#endif