#ifndef CMESSAGEHANDLE_HEAD_FILE
#define CMESSAGEHANDLE_HEAD_FILE

#include "PublicDefine.h"
#include "AFCEdit.h"
#include "AFCExpressionWnd.h"

//using namespace AFC::AFCEdit;

//消息处理类
class EXT_CLASS CLookingClass//C-MessageHandle
{
	//变量定义
public:
	CNewMiniDlg						* m_pRichEcit;
	
	//函数定义
public:
	//构造函数
	CLookingClass(CNewMiniDlg * pRichEdit);
	//析构函数
	virtual ~CLookingClass(void);
	CAFCColorResource					Color_Struct;
	void SetColor(CAFCColorResource	Color_Struct);
	//功能函数
public:
	//新闻
	void InsertNews(TCHAR * szCharString, UINT uSize=0, TCHAR * szFontName=NULL);
	void InsertNews(CString szCharString, UINT uSize=0, TCHAR * szFontName=NULL);
	//用户名字
	void InsertUserName(TCHAR * szUserName);
	void InsertSuperUserName(TCHAR * szUserName);
	//简单信息
	void InsertNormalMessage(TCHAR * szCharString, bool bEnter=true, UINT uSize=0, TCHAR * szFontName=NULL);
	//系统消息
	void InsertSystemMessage(TCHAR * szCharString, UINT uSize=0, TCHAR * szFontName=NULL);
	//系统消息
	void InsertSystemMessageM(TCHAR * szCharString, UINT uSize=0, TCHAR * szFontName=NULL);
	//系统公告
	void InsertSystemBoard(TCHAR * szCharString, UINT uSize=0, TCHAR * szFontName=NULL);
	//自定义头消息
	void InsertTitleMessage(TCHAR * szMsg, BYTE bNewOrSys=0,UINT uSize=0, TCHAR * szFontName=NULL,TCHAR *Title=NULL);
	//用户聊天信息
	void InsertUserTalk(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL);
    //插入聊天记录
    void InsertChatRecord(TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL);
    //嘟嘟
	void InsertDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL,int iDuduType = 0);

	//内部函数
private:
	//解释插入
	bool TranslateInsertChar(TCHAR * szCharString, COLORREF crTextColor, UINT uSize, TCHAR * szFontName);
	//解释字符
	CTrueItem * TranslateChar(TCHAR * szCharString, int iCharLength, int & iPassCharCount);
};

#endif