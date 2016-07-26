#pragma once

#include "StdAfx.h"

/// 操作掩码
#define	UG_HUA_MASK					0xF0			///< 1111 0000
#define	UG_VALUE_MASK				0x0F			///< 0000 1111

/// 扑克花色
#define UG_FANG_KUAI				0x00			///< 方块	0000 0000
#define UG_MEI_HUA					0x10			///< 梅花	0001 0000
#define UG_HONG_TAO					0x20			///< 红桃	0010 0000
#define UG_HEI_TAO					0x30			///< 黑桃	0011 0000
#define UG_NT_CARD					0x40			///< 主牌	0100 0000
#define UG_ERROR_HUA				0xF0			///< 错误   1111 0000

/// 扑克出牌类型
#define UG_ERROR_KIND				0				///< 错误
#define UG_ONLY_ONE					1				///< 单牌
#define UG_DOUBLE					2				///< 对牌
#define UG_THREE					3				///< 三条
#define UG_DOUBLE_TRACKOR			4				///< 两条拖拉机
#define UG_THREE_TARCKOR			5				///< 三条拖拉机
#define	UG_SHOW_CARD				6				///< 甩牌

///////////////////////////////////////////////////////////////

enum XQ_PLAY_FLAG
{
	XQ_PLAY_FLAG_RED = 0,
	XQ_PLAY_FLAG_BLACK = 1
};


/// 象棋
#define XQ_FLAG_NONE				0
#define XQ_FLAG_RED					1
#define XQ_FLAG_BLACK				-1

#define XQ_QIZI_NONE				0
#define XQ_CROSS					8

/// 红方
#define XQ_RED_JU					1
#define XQ_RED_MA					2
#define XQ_RED_PAO					3
#define XQ_RED_JIANG				6
#define XQ_RED_SHI					4
#define XQ_RED_XIANG				5
#define XQ_RED_BIN					7

/// 黑方
#define XQ_BLACK_JU					-1
#define XQ_BLACK_MA					-2
#define XQ_BLACK_PAO				-3
#define XQ_BLACK_JIANG				-6
#define XQ_BLACK_SHI				-4
#define XQ_BLACK_XIANG				-5
#define XQ_BLACK_BIN				-7

#define XQ_MAX_I					(10)
#define XQ_MAX_J					(9)


/// 棋盘
#define XQ_BOARD_ORG_X				(266)			///< 302)	//棋盘左上角在客户区中的坐标
#define XQ_BOARD_ORG_Y				(81)			///< 114)	//棋盘左上角在客户区中的坐标
#define XQ_BOARD_X					(248)			///< (223)	//棋盘左上角在整个棋盘图的坐标
#define XQ_BOARD_Y					(74)			///< (64)//(78)	//棋盘左上角在整个棋盘图的坐标
#define XQ_QIZI_RECT_A				(57)			///< 棋盘中每个格子的宽度
#define XQ_QIZI_RECT_R				(50)			///< 棋子

/// 显示时间设置坐标
/// 中间显示
#define XQ_INIT_SET_JU_TIME_X		(85)			///< (63)
#define XQ_INIT_SET_JU_TIME_Y		(327)			///< (337)
#define XQ_INIT_SET_BU_TIME_X		(85)			///< (63)
#define XQ_INIT_SET_BU_TIME_Y		(349)			///< (359)
#define XQ_INIT_SET_DU_TIME_X		(85)			///< (63)
#define XQ_INIT_SET_DU_TIME_Y		(371)			///< (381)

/// 上方显示用时
#define XQ_SET_TOP_JU_TIME_X		(85)			///< (63)
#define XQ_SET_TOP_JU_TIME_Y		(248)			///< (258)
#define XQ_SET_TOP_BU_TIME_X		(85)			///< (63)
#define XQ_SET_TOP_BU_TIME_Y		(267)			///< (277)
#define XQ_SET_TOP_DU_TIME_X		(85)			///< (63)
#define XQ_SET_TOP_DU_TIME_Y		(285)			///< (295)

/// 下方显示用时
#define XQ_SET_BOT_JU_TIME_X		(85)			///< (63)
#define XQ_SET_BOT_JU_TIME_Y		(438)			///< (448)
#define XQ_SET_BOT_BU_TIME_X		(85)			///< (63)
#define XQ_SET_BOT_BU_TIME_Y		(455)			///< (465)
#define XQ_SET_BOT_DU_TIME_X		(85)			///< (63)
#define XQ_SET_BOT_DU_TIME_Y		(471)			///< (481)

/// 用户昵称
#define XQ_UPPEOPLE_X				(85)			///< (63)
#define XQ_UPPEOPLE_Y				(228)			///< (238)
#define XQ_DOWNPEOPLE_X				(85)			///< (63)
#define XQ_DOWNPEOPLE_Y				(420)			///< (430)

/// 用户头像
#define XQ_UPPEOPLE_LOG_X			(47)			///< (27)
#define XQ_UPPEOPLE_LOG_Y			(96)
#define XQ_UPPEOPLE_VIDEO_X			(10)
#define XQ_UPPEOPLE_VIDEO_Y			(76)

#define XQ_DOWNPEOPLE_LOG_X			(47)			///< (27)
#define XQ_DOWNPEOPLE_LOG_Y			(522)
#define XQ_DOWNPEOPLE_VIDEO_X		(47)
#define XQ_DOWNPEOPLE_VIDEO_Y		(502)
/////////////////////////////////////////////////////



/// 升级逻辑类 （支持 2 副和 3 副扑克）
class CUpGradeGameLogic
{
	/// 变量定义
private:
	int							m_iQiZi[10][9];		///< 棋子
	int							m_iSideFlag;		///< 本方,还是敌方
	/// 函数定义
public:
	/// 构造函数
	CUpGradeGameLogic(void);

public:
	/// 查找棋子是什么棋
	int  GetChess(int xChess,int yChess,int chessArray[][9]);	
	/// 是否可以出牌
	// BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, BYTE iHandCard[], int iHandCount);
	/// 初始化棋盘
	void InitBoard(int iMeFlag, int qi[][9]);
	/// 本方,还是敌方棋子
	int IsMeSideBin(){return m_iSideFlag;}
	/// 设置是否为本方
	int SetMeSide(int iSide){m_iSideFlag=iSide;}
	/// 获取某一个坐标棋子值
	int GetQiZiValue(int fx, int fy);
	/// 能否走子(仅只测试,不做具体实施)
	BOOL CanMoveQiZi(int fromi, int fromj, int toi, int toj, int qi[][9],bool check);
	/// 走子(前面只是测试是否可以走)
	BOOL MoveQiZi(int fromi, int fromj, int toi, int toj, int qi[][9]);
	/// 设置棋子(用于回退)
	BOOL SetQiZi(int fx,int fy,int value);
	/// 是否敵方将军本方將軍
	int CheckOtherOrMeJiang(int Startx, int Starty,int Endx,int Endy,int meflag,int MeBoard[][9]);
	/// 是否本方将军
	int CheckMeJiangJun(int Startx, int Starty,int Endx,int Endy,int meflag,int MeBoard[][9]);
	/// 是否敌方将军
	int CheckOtherJiangJun(int Startx, int Starty,int Endx,int Endy,int meflag,int MeBoard[][9],BOOL bTest=false);
	/// 本找王的位置
	CPoint GetQiZiKing(int meflag,int meboard[][9]);
	/// 查找某所有棋子
	int GetMeQiZi(CPoint point[],int meflag,int MeBoard[][9]);
	/// 确认是否有棋可走
	BOOL HaveQiZiMove(int meflag,int meboard[][9]);
	/// 确认是否胜负
	BOOL IsLose(int iMeFlag, int qi[][9]);
	/// 确认是否有活动棋
	BOOL IsLive(int meflag,int MeBoard[][9]);
	/// 能够吃掉对方王
	BOOL CanEatKing(int meflag,int MeBoard[][9]);
	/// 遍历
	BOOL IsDieCheck(int meflag,int MeBoard[][9]);	
	void SetBoard(int iMeFlag, int qi[][9]);

	/// 内部函数
private:
	
	/// 走车
	BOOL MoveZhu(int fx, int fy, int tx, int ty);
	/// 走马
	BOOL MoveMa(int fx, int fy, int tx, int ty);
	/// 走炮
	BOOL MovePao(int fx, int fy, int tx, int ty, int flag);
	/// 走相
	BOOL MoveXiang(int fx, int fy, int tx, int ty);
	/// 走士
	BOOL MoveShi(int fx, int fy, int tx, int ty);
	/// 走将
	BOOL MoveShuai(int fx, int fy, int tx, int ty);
	/// 走兵
	BOOL MoveBin(int fx, int fy, int tx, int ty);
};
