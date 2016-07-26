#pragma once

//#include "StdAfx.h"

//操作掩码
#define	UG_HUA_MASK					0xF0			//1111 0000
#define	UG_VALUE_MASK				0x0F			//0000 1111

//扑克花色
#define UG_FANG_KUAI				0x00			//方块	0000 0000
#define UG_MEI_HUA					0x10			//梅花	0001 0000
#define UG_HONG_TAO					0x20			//红桃	0010 0000
#define UG_HEI_TAO					0x30			//黑桃	0011 0000
#define UG_NT_CARD					0x40			//主牌	0100 0000
#define UG_ERROR_HUA				0xF0			//错误  1111 0000

//扑克出牌类型
#define SH_THREE					7				//三条
#define SH_SAME_HUA_CONTINUE		6				//同花顺
#ifdef  ZJH_CONTINUE_MORE
#define SH_SAME_HUA					4				//同花
#define SH_CONTINUE					5				//顺子
#else
#define SH_SAME_HUA					5				//同花
#define SH_CONTINUE					4				//顺子
#endif
#define SH_DOUBLE					3				//对子
#define SH_OTHER					2				//单牌
#define SH_SPECIAL					1				//特殊235
#define UG_ERROR_KIND				0				//错误

//升级逻辑类 
class CUpGradeGameLogic
{
	//变量定义
private:

	struct PSTypeNode
	{
		int mps,mcount;

		PSTypeNode()
		{
			mps = -1;
			mcount = 0;
		}
	};


	//函数定义
public:
	//构造函数
	CUpGradeGameLogic(void);

	void Msg(const char *lpszFormat, ...)
	{
		va_list pArg;
		va_start(pArg, lpszFormat);
		char szMessage[1024] = { 0 };
#ifdef WIN32
		_vsnprintf(szMessage, 1023, lpszFormat, pArg);
#else
		vsnprintf(szMessage, 1023, lpszFormat, pArg);
#endif
		va_end(pArg);
		OutputDebugString(szMessage);
	}

public:
	//获取扑克数字
	int GetCardNum(BYTE iCard);
	//混乱扑克
	BYTE RandCard(BYTE iCard[], int iCardCount,int iIndex = 0);

};