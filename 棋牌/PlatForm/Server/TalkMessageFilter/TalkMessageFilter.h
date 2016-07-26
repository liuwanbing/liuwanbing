#pragma once
#include <list>
using namespace std;
/********************************************************************
	created:	2009/03/16
	created:	16:3:2009   18:05
	filename: 	\客户端组件\TalkMessageFilter\TalkMessageFilter.h
	file path:	\客户端组件\TalkMessageFilter
	file base:	TalkMessageFilter
	file ext:	h
	author:		Administrator
	
	purpose:  添加聊天过滤功能	
    TalkMessageFilter   version:  1.0   ·  date: 03/16/2009  
    Copyright (C) 2009 - All Rights Reserved

*********************************************************************/



class CTalkMessageFilter
{
public:
	CTalkMessageFilter(void);
	~CTalkMessageFilter(void);
	//加载不需要过滤的语言
    void LoadFilterMessage(TCHAR* pstrAppPath);
	//判断是否包含需要过滤的语言
	bool IsAllowableTalk(TCHAR* pTalkContent,UINT uSize);
	const CHAR* GetReplaceContent() const {return m_strReplaceContent;}
private:
	list<TCHAR*> m_lsUnAllowableWord;
	DWORD m_dwFileHandle;

	CHAR   m_strReplaceContent[1024];

};
