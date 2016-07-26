#pragma once

#include "mj.h"
///开发库版本
#define DEV_LIB_VER				1									///开发库版本
//支持类型
///支持类型定义
#define SUP_NORMAL_GAME			0x01								///普通游戏
#define SUP_MATCH_GAME			0x02								///比赛游戏
#define SUP_MONEY_GAME			0x04								///金币游戏
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME

////////////////////////////////////////////////////////////////////////////////
#define GFF_FORCE_FINISH			0										///< 强行解除
#define GF_NORMAL               10                                  /// 游戏正常结束
#define GF_SAFE                 11                                  /// 游戏安全结束

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE		// 不知什么意思
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

#define AI_FAST							// 机器人快速出牌
#ifndef AI_MACHINE						// 取消快速机器人
#undef AI_FAST
#endif

// 支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
#define GAME_TABLE_NAME					TEXT("MatchTable_BSMJ")

// 版本定义
#define GAME_MAX_VER					2									// 现在最高版本
#define GAME_LESS_VER					2									// 现在最低版本
#define GAME_CHANGE_VER					0									// 修改版本		
#define PLAY_COUNT						4									// 游戏人数	

// 游戏状态定义
#define GS_WAIT_SETGAME					0				// 等待东家设置状态
#define GS_WAIT_ARGEE					1				// 等待同意设置
#define GS_SEND_CARD					20				// 发牌状态
#define GS_PLAY_GAME					22				// 游戏中状态
#define GS_WAIT_NEXT					23				// 等待下一盘开始 



// 抓牌状态
#define GAME_STATUS_FAPAI				0				// 游戏状态为0时,表示是补牌
#define GAME_STATUS_CHUPAI				1				// 为1,表示是正常发牌


//////组牌类型及动作类型////////////////////
/********************************************************************************/
#define ACTION_NO				0				//没有动作
#define ACTION_CHI				1				//吃牌动作
#define ACTION_PENG				2				//碰牌动作
#define ACTION_KAN				3				//坎牌牌动作
#define ACTION_AN_GANG			4				//暗杠动作
#define ACTION_BU_GANG			5				//补杠动作
#define ACTION_MING_GANG		6				//明杠动作
#define ACTION_CHI_TNG			7				//抢吃（吃后听牌）
#define ACTION_TING				8				//听动作
#define ACTION_HU				10				//糊动作

#define TYPE_JIANG_PAI			15				//将牌类型
#define TYPE_JINGDIAO_JIANG		16				//精钓将牌类型
#define TYPE_SHUN_ZI			17				//顺子类型
#define TYPE_AN_KE				18				//暗刻类型
#define TYPE_JING_KE			19				//财神刻子类型

///各种牌的数量
#define HAND_CARD_NUM			17				//最大手牌张数
#define OUT_CARD_NUM			60				//出牌张数
#define HUA_CARD_NUM			20				//花牌张数
#define MEN_CARD_NUM			40				//门牌张数
#define MAX_HUPAI_TYPE			20				//糊牌类型
#define MAX_HAND_PAI			14				//手牌数

///糊牌检测临时数据
struct CheckHuStruct
{
	BYTE  data[HAND_CARD_NUM][2];   //牌数据，0牌值，1张数
	BYTE  conut;
	void Add(BYTE pai)
	{
		if(GetPaiCount(pai)>0)
		{
			for(int i=0;i<HAND_CARD_NUM;i++)
			{
				if(data[i][0] == pai && pai != 255)
					data[i][1]++;
			}
		}
		else
		{
			data[conut][0] = pai;
			data[conut][1] = 1;
			conut++;
		}
	};
	BYTE GetPaiCount(BYTE pai)
	{
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			if(data[i][0] == pai && pai != 255 && data[i][1] != 255)
				return data[i][1];
		}
		return 0;
	};
	BYTE GetDataCount()
	{
		conut = 0;
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			if(data[i][0] != 255)
				conut++;
		}
		return conut;
	};
	BYTE GetAllPaiCount()
	{
		BYTE num = 0;
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			if(data[i][0] != 255)
				num += data[i][1];
		}
		return num;
	};
	void SetPaiCount(BYTE pai,BYTE num)
	{
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			if(data[i][0] == pai && pai != 255)
			{
				data[i][1] = num;
			}
		}
	}
	void Init()
	{
		memset(data,255,sizeof(data));
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			data[i][1] = 0;
		}
		conut=0;
	};
	CheckHuStruct()
	{
		Init();
	}
};
///平糊组牌数据结构
struct PingHuStruct
{
	BYTE  byType[6];    //组牌类型
	BYTE  data[6][4];   //数据
	bool  isjing[6][4];//是否财神替代
	BYTE  count;        //节点数
	PingHuStruct()
	{
		Init();
	};
	void AddData(BYTE type,BYTE pai[],bool jing[])
	{
		for(int i=0;i<6;i++)
		{
			if(byType[i] == 255)
			{
				byType[i] = type;
				data[i][0] = pai[0];data[i][1] = pai[1];data[i][2] = pai[2];data[i][3] = pai[3];
				isjing[i][0] = jing[0];isjing[i][1] = jing[1];isjing[i][2] = jing[2];isjing[i][3] = jing[3];
				count++;
				break;
			}
		}
	};
	void DeleteData(BYTE type,BYTE pai[],bool jing[])//删除元素
	{
		for(int i=0;i<6;i++)
		{
			if(byType[i] == type && data[i][0] == pai[0 ]&& data[i][1] == pai[1] && isjing[i][0] == jing[0] && isjing[i][1] == jing[1])
			{
				byType[i] = 255;
				memset(data[i],255,sizeof(data[i]));
				memset(isjing[i],0,sizeof(isjing[i]));
				count--;
				break;
			}
		}
	};
	void SortData()
	{
		for(int i=0;i<5;i++)
		{
			if(byType[i] != 255)
				continue;
			for(int j=i;j<5;j++)
			{
				byType[j] = byType[j+1];
				memcpy(data[j],data[j+1],sizeof(data[j]));//数据
				memcpy(isjing[j],isjing[j+1],sizeof(isjing[j]));//是否财神替代
			}
		}
		count = 0;
		for(int i=0;i<5;i++)
		{
			if(byType[i] != 255)
				count++;
		}
	};
	void Init()
	{
		memset(this,255,sizeof(PingHuStruct));
		memset(isjing,0,sizeof(isjing));
		count = 0;
	};
};

/////////////////糊牌类型结构//////////////////////////////////////
///糊牌类型
enum HUPAI_TYPE//LU胡牌类型枚举结构
{		
	HUPAI_TYPE_None						=0,		////没糊	
	HUPAI_TYPE_HuPai					=1,		////成牌
	HUPAI_TYPE_ZiMo						=2,		////自摸
	HUPAI_TYPE_PingHu					=3,		////平糊
	HUPAI_TYPE_TianHu					=4,		////天糊
	HUPAI_TYPE_DiHu						=5,		////地糊
	HUPAI_TYPE_GangKai					=6,		//杠上花
	HUPAI_TYPE_GangHaoPao				=7,		//杠后炮
	HUPAI_TYPE_YiBanGao					=8,		//一般高
	HUPAI_TYPE_XiXiangFeng				=9,		//喜相逢
	HUPAI_TYPE_LianLiu					=10,	//连六
	HUPAI_TYPE_LaoShaoFu				=11,	//老少副
	HUPAI_TYPE_YaoJiuKe					=12,	//幺九刻
	HUPAI_TYPE_MingGang					=13,	//明杠
	HUPAI_TYPE_QueYiMen					=14,	//缺一门
	HUPAI_TYPE_WuZi						=15,	//无字
	HUPAI_TYPE_BianZhang				=16,	//边张
	HUPAI_TYPE_KanZhang					=17,	//坎张
	HUPAI_TYPE_DanDiaoJiang				=18,	//单钓将
	HUPAI_TYPE_JianKe					=19,	//箭刻
	HUPAI_TYPE_QuanFengKe				=20,	//圈风刻
	HUPAI_TYPE_MenFengKe				=21,	//门风刻
	HUPAI_TYPE_MenQianQing				=22,	//门前清
	HUPAI_TYPE_SiGuiYi					=23,	//四归一
	HUPAI_TYPE_ShuangTongKe				=24,	//双同刻
	HUPAI_TYPE_ShuangAnKe				=25,	//双暗刻
	HUPAI_TYPE_AnGang					=26,	//暗杠
	HUPAI_TYPE_DuanYao					=27,	//断幺
	HUPAI_TYPE_QuanDaiYao				=28,	//全带幺
	HUPAI_TYPE_BuQiuRen					=29,	//不求人
	HUPAI_TYPE_ShuangMingGang			=30,	//双明杠
	HUPAI_TYPE_HuJueZhang				=31,	//和绝张
	HUPAI_TYPE_PengPengHu				=32,	//碰碰和
	HUPAI_TYPE_HunYiSe					=33,	//混一色
	HUPAI_TYPE_SanSeSanBuGao			=34,	//三色三步高
	HUPAI_TYPE_WuMenQi					=35,	//五门齐
	HUPAI_TYPE_QuanQiuRen				=36,	//全求人
	HUPAI_TYPE_ShuangAnGang				=37,	//双暗杠
	HUPAI_TYPE_ShuangJianKe				=38,	//双箭刻
	HUPAI_TYPE_HuaLong					=39,	//花龙
	HUPAI_TYPE_TuiBuDao					=40,	//推不倒
	HUPAI_TYPE_SanSeSanTongShun			=41,	//三色三同顺
	HUPAI_TYPE_SanSeSanJieGao			=42,	//三色三节高
	HUPAI_TYPE_WuFanHu					=43,	//无番和
	HUPAI_TYPE_MiaoShouHuiChun			=44,	//妙手回春
	HUPAI_TYPE_HaiDiLaoYue				=45,	//海底捞月
	HUPAI_TYPE_QiangGang				=46,	//抢杠和
	HUPAI_TYPE_QuanBuKao				=47,	//全不靠(十三烂)
	HUPAI_TYPE_ZuHeLong					=48,	//组合龙
	HUPAI_TYPE_DaYuWu					=49,	//大于五
	HUPAI_TYPE_XiaoYuWu					=50,	//小于五
	HUPAI_TYPE_SanFengKe				=51,	//三风刻
	HUPAI_TYPE_QingLong					=52,	//清龙
	HUPAI_TYPE_SanSeShuangLongHui		=53,	//三色双龙会
	HUPAI_TYPE_YiSeSanBuGao				=54,	//一色三步高
	HUPAI_TYPE_QuanDaiWu				=55,	//全带五
	HUPAI_TYPE_SanTongKe				=56,	//三同刻
	HUPAI_TYPE_SanAnKe					=57,	//三暗刻
	HUPAI_TYPE_QiDui					=58,	//七对
	HUPAI_TYPE_QiXingBuKao				=59,	//七星不靠
	HUPAI_TYPE_QuanShuangKe				=60,	//全双刻
	HUPAI_TYPE_QingYiSe					=61,	//清一色
	HUPAI_TYPE_YiSeSanTongShun			=62,	//一色三同顺
	HUPAI_TYPE_YiSeSanJieGao			=63,	//一色三节高
	HUPAI_TYPE_QuanDa					=64,	//全大
	HUPAI_TYPE_QuanZhong				=65,	//全中
	HUPAI_TYPE_QuanXiao					=66,	//全小
	HUPAI_TYPE_YiSeSiBuGao				=67,	//一色四步高
	HUPAI_TYPE_SanGang					=68,	//三杠
	HUPAI_TYPE_HunYaoJiu				=69,	//混幺九
	HUPAI_TYPE_YiSeSiTongShun			=70,	//一色四同顺
	HUPAI_TYPE_YiSeSiJieGao				=71,	//一色四节高
	HUPAI_TYPE_QingYaoJiu				=72,	//清幺九
	HUPAI_TYPE_XiaoSiXi					=73,	//小四喜
	HUPAI_TYPE_XiaoSanYuan				=74,	//小三元
	HUPAI_TYPE_ZiYiSe					=75,	//字一色
	HUPAI_TYPE_SiAnKe					=76,	//四暗刻
	HUPAI_TYPE_YiSeShuangLongHui		=77,	//一色双龙会
	HUPAI_TYPE_DaSiXi					=78,	//大四喜
	HUPAI_TYPE_DaSanYuan				=79,	//大三元
	HUPAI_TYPE_LvYiSe					=80,	//绿一色
	HUPAI_TYPE_JiuLianBaoDeng			=81,	//九莲宝灯
	HUPAI_TYPE_SiGang					=82,	//四杠（十八罗汉）
	HUPAI_TYPE_LianQiDui				=83,	//连七对
	HUPAI_TYPE_ShiSanYao				=84		//十三幺
};
///////////////////////////////////////////////////////////////

///财神数据结构
struct JingStruct
{
	BYTE  byPai[8];    //每张精牌的牌值
	BYTE  byJingNum;   //精牌种类张数
	JingStruct()
	{
		Init();
	}
	void Init()
	{
		memset(this,255,sizeof(JingStruct));
		byJingNum = 0;
	};
	bool CheckIsJing(BYTE pai)
	{
		for(int i=0;i<8;i++)
		{
			if(byPai[i] == pai && pai!=255)
				return true;
		}
		return false;
	};
	bool AddJing(BYTE pai)
	{
		for(int i=0;i<8;i++)
		{
			if(byPai[i] == 255 && pai !=255)
			{

				byPai[i] = pai;
				byJingNum++;
				return true;
			}
		}
		return false;
	};
	void sort()
	{
		BYTE pai = 255;
		for(int i=0;i<7;i++)
		{
			
			for(int j=i+1;j<8;++j)
			{
				if(byPai[j] < byPai[i])
				{
					pai = byPai[j];
					byPai[j] = byPai[i];
					byPai[i] = pai;
				}
			}
		}
	}
};

///吃碰杠牌数据结构
struct GCPStruct
{
	BYTE  byType;    //吃碰杠类型
	BYTE  iOutpai;   //别人出的牌
	BYTE  byData[4]; //吃碰杠牌数据
	BYTE  iStation;  //吃碰杠玩家位置
	BYTE  iBeStation;//被吃碰杠玩家位置
	GCPStruct()
	{
		Init();
	}
	void Init()
	{
		memset(byData,255,sizeof(byData));
		byType = 255;    //吃碰杠类型
		iOutpai = 255;   //别人出的牌
		iStation = 255;  //吃碰杠玩家位置
		iBeStation = 255;//被吃碰杠玩家位置
	};
};
//
///牌数据结构（保存牌值和张数）
struct PaiStruct
{
	BYTE  pai;   //牌数据
	BYTE  count; //牌张数
	PaiStruct()
	{
		Init();
	};
	void Init()
	{
		memset(this,255,sizeof(PaiStruct));
		count = 0;
	};
};


// 游戏数据包
/********************************************************************************/
//只用一个数据包，多余的数据可以不使用,断线重连使用
struct GameStationData
{
	//基本信息
	int					iVersion;						// 正副版本
	int					iVersion2;
	int					uRunPublish;					//逃跑要扣的罚分
	int					basepoint;						//房间的倍率
	bool				bShowUserId;					// 是否显示所有玩家的id
	//游戏信息
	BYTE				byPlayCount;					// 游戏盘数
	BYTE				byNtStation;					// 庄家位置
	BYTE				byBeenPlayGame;					// 已经游戏的局数
	BYTE				byQuanFeng;						// 圈风设置
	bool				m_bistuoguan[4];				// 是否托管
	BYTE				m_byNowOutStation;				// 现在出牌用户
	BYTE				m_byOtherOutPai;				// 现在出的牌
	bool				m_bTing[4];						// 是否听牌
	BYTE				m_byThingDoing;					// 正在发生的事件号 

    int					Stationpara;
    BYTE                ucAgree;                        //低四位表是否同意
    int                 m_nWhichTask;
	//牌数据
	BYTE				m_byArHandPai[4][HAND_CARD_NUM];//手牌不超过20张，0为结束标志
	BYTE				m_byArOutPai[4][OUT_CARD_NUM];	//出牌不超过160张，0为结束标志
	BYTE				m_byArHuaPai[4][HUA_CARD_NUM];	//花牌不超过10张，0为结束标志
	BYTE				m_iArMenPai[4][MEN_CARD_NUM];	//门牌不超过40张
	//游戏时间
	BYTE				byAutoOutTime;		//自动出牌时间
	BYTE				byOutTime;			//出牌时间
	BYTE				byThinkTime;		//思考时间
	BYTE				byBeginTime;		//开始等待时间
	bool				bAutoBegin;			//时间到了是否自动开始

	GameStationData()
	{
		Init();
	};
	void Init()
	{
		memset(this, 0, sizeof(GameStationData));	
		memset(m_byArHandPai,255,sizeof(m_byArHandPai));
		memset(m_byArOutPai,255,sizeof(m_byArOutPai));
		memset(m_byArHuaPai,255,sizeof(m_byArHuaPai));
		memset(m_iArMenPai,255,sizeof(m_iArMenPai));

		byAutoOutTime = 1;		//自动出牌时间
		byOutTime = 20;			//出牌时间
		byThinkTime = 15;		//思考时间
		byBeginTime = 30;		//开始等待时间
		bAutoBegin = 0;			//时间到了是否自动开始
	}
};

//用户托管数据包
struct Usertouguan
{						
	BYTE			desk;				// 谁托管
	bool			touguang;			// 托管状态
	bool			bTuoGuan[4];		// 各个玩家托管状态
};
/////用户同意结构
//struct MSG_GR_R_UserAgree
//{
//    BYTE			bDeskNO;							///游戏桌号
//    BYTE			bDeskStation;						///位置号码
//    BYTE			bAgreeGame;							///同意标志
//};
//用户同意游戏
struct UserArgeeGame
{
	BYTE			byPlayCount;				// 游戏盘数
	BYTE			byCardCount;				// 牌数目
};

// 游戏结束统计数据包
struct GameCutStruct
{
	BYTE				bDeskStation;					// 退出位置
	int					iTurePoint[4];					// 庄家得分
	int					iChangeMoney[8];				// 玩家银子
};

struct HaveThingStruct		// 有事离开
{
	BYTE	pos;
	TCHAR	szMessage[61];
};

struct LeaveResultStruct	// 请求离开结果
{
	BYTE	bLeaveStation; //想要离开玩家的位置
	BYTE	bDeskStation;
	int		bArgeeLeave;
};

struct MessageStruct		// 客户端到服务器,消息
{
	TCHAR Message[200];
};

///////////////////////////////////////////////////////////////////////////

#define MAX_HEPAI_TYPES     256 ///最大糊牌类型

///麻将牌：各家门牌，翻精点，当前拿牌方向
struct MjMenPaiData 
{
	BYTE    byMenPai[300];				//麻将的数量，玩家个数，门牌最大敦数，每敦上下2张牌 
	BYTE    byNt;						//庄家位置
	BYTE	byStation;					//当前抓牌玩家的位置
	BYTE	byDir;						//当前抓牌方向(抓那个玩家门前的牌)
	BYTE	byGetPaiDir;				//本局起牌方向
	BYTE	byRemainDun;				//剩余敦数
	BYTE	byGetPai;				    //起牌敦数
	BYTE    byMenPaiDun[4];             //每个玩家门前的敦数
	BYTE    byStartIndex;				//门牌起始索引		
	BYTE    byEndIndex;				    //门牌结束索引		
	BYTE    byCaiShenIndex;				//财神位置索引		
	BYTE    byAllPaiNum;				//所有牌的数量		

	MjMenPaiData()
	{
		Init();
	};
	void Init()
	{
		memset(byMenPai,255,sizeof(byMenPai));
		memset(byMenPaiDun,255,sizeof(byMenPaiDun));//玩家门前的敦数
		byNt = 255;						//庄家位置
		byStation = 255;					//当前抓牌玩家的位置
		byDir = 255;						//当前抓牌方向(抓那个玩家门前的牌)
		byGetPaiDir = 255;				//本局起牌方向
		byRemainDun = 255;				//剩余敦数
		byGetPai = 255;				    //起牌敦数
		byStartIndex = 255;				//门牌起始索引
		byEndIndex = 255;				    //门牌结束索引
		byAllPaiNum = 255;				    //所有牌的数量
		byCaiShenIndex = 255;				//财神位置索引
	};
	BYTE GetMenPaiNum()
	{
		BYTE num = 0;
		for(int i=0;i<300;i++)
		{
			if(byMenPai[i] != 255)
			{
				num++;
			}
			if(num >= 254)
				break;
		}
		return num;
	};
	BYTE GetOnePaiNum(BYTE pai)
	{
		BYTE num = 0;
		for(int i=0;i<300;i++)
		{
			if(byMenPai[i] == pai)
			{
				num++;
			}
		}
		return num;
	};
	bool DeleteAnMenPai(BYTE pai)
	{
		if(pai == 255)
			return false;
		for(int i=0;i<300;++i)
		{
			if(byMenPai[i] != 255)
			{
				byMenPai[i] = 255;
				return true;
			}
		}
		return false;
	};
	bool ChangAnMenPai(BYTE oldpai,BYTE newpai)// 门牌中要被换的牌 oldpai,换来的新牌 newpai
	{
		if(oldpai == 255 || newpai == 255)
			return false;
		for(int i=0;i<300;++i)
		{
			if(byMenPai[i] == oldpai)
			{
				byMenPai[i] = newpai;//换牌成功
				return true;
			}
		}
		return false;
	};
};

///麻将常规属性
struct MjRuleSet 
{
	BYTE    byGameUser;			//游戏人数
	BYTE    bySeziNum;			//筛子的个数
	BYTE    byAllMjCount;		//麻将的数量 
	BYTE	byHandCardNum;		//手牌张数
	BYTE	byGamePassType;		//流局剩牌留下
	BYTE	byGamePassNum;		//流局牌数
	BYTE	byNextNtType;		//下局庄家的确定类型
	BYTE    byMenPaiNum[4];		//各家门牌数
	BYTE	byOtherJing;		//其他财神
	BYTE    byAutoOutTime;		//自动出牌时间
	BYTE    byOutTime;			//出牌时间
	BYTE    byThinkTime;		//思考时间
	BYTE    byBeginTime;		//开始等待时间
	BYTE    byGoodPaiChance;	//发好牌的几率
	bool    bHaveFlower;		//是否有花牌
	bool    bGangZhong;			//是否杠红中
	bool    bGangFa;			//是否杠发财
	bool    bGangBai;			//是否杠白板
	bool    bHaveHongZhong;		//是否有红中
	bool    bHaveFaCai;			//是否有发财
	bool    bHaveBaiBan;		//是否有白板
	bool    bHaveFengPai;		//是否有东南西北
	bool	bHaveJing;			//有无财神
	bool    bRandJing;          //随机数定精
	bool	bMainJing;			//正精
	bool	bLessJing;			//副精
	bool    bHaveQFeng;			//是否有圈风
	bool    bNetCutTuoGuan;		//是否断线托管
	bool	bForceTuoGuan;		//是否强退托管
	bool	bAutoBegin;			//时间到了是否自动开始
	bool	bGetGoodPai;		//是否随机发好牌
	MjRuleSet()
	{
		memset(this, 0, sizeof(MjRuleSet));		
	}

};
///动作属性
struct ActionRuleSet 
{
	bool bChi;			//吃
	bool bChiFeng;		//吃风牌
	bool bChiJian;		//吃中发白
	bool bChiJing;		//吃财神
	bool bPeng;			//碰
	bool bKan;			//坎
	bool bSaoHu;		//扫虎（查虎麻将）
	bool bGang;			//杠
	bool bBuGang;		//补杠
	bool bTing;			//听
	bool bQiangChi;		//抢吃(吃后听牌)
	bool bGangAfterTing;//听牌后还能杠牌
	bool bSeziAfterGang;//杠后打色子抓牌
	bool bQxHh;			//不能弃先糊后（一圈内）
	bool bQxHhBut;		//不能弃先糊后（一圈内），糊不同的牌可以，糊得更大可以
	bool bOutLianZhang;	//吃牌后不能出连张（出的牌不能与吃牌中自己的两张牌组成顺子）
	ActionRuleSet()
	{
		memset(this, 0, sizeof(ActionRuleSet));
	}
};
///算分属性
struct CountFenRuleSet 
{
	CountFenRuleSet()
	{
		memset(this, 0, sizeof(CountFenRuleSet));
	}
};
///糊牌属性
struct HuPaiRuleSet 
{
	bool bDuoXiang;		//一炮多响
	bool bJieHu;		//截糊,非一炮多响中，多人可以糊牌，糊特殊牌型的玩家可以截糊
	bool bZimoHu;		//自摸
	bool bDianPaoHu;	//点炮
	bool bNoLimite;		//没有条件，只要牌型符合记录（4+1 或者 7对，十三烂，十三幺）
	bool bSanMQ;		//三门齐
	bool bHaveYJ;		//有幺九
	bool bHaveZi;		//有字牌
	bool bHaveZorYJ;	//有字牌或有幺九
	bool b258Jiang;		//258做将
	bool bKaiMen;		//已开门
	bool bShouBaYi;		//不能手把一
	bool bPiaoCan;		//飘糊条件不限
	bool bQiangGHu;		//抢杠糊
	bool bGangKai;		//杠上开花
	bool bGangPao;		//杠后炮
	bool bHaiDiLaoYue;  //海底捞月
	bool bTianHu;		//天糊
	bool bDiHu;			//地糊
	BYTE byPingHuType;	//平糊限制类型,0没有限制，1，2，3
	HuPaiRuleSet()
	{
		memset(this, 0, sizeof(HuPaiRuleSet));
	}
};

///糊牌类型设置
struct HuPaiTypeSet 
{
	BYTE byZiMo;			//自摸
	BYTE byTianHu;			//天糊
	BYTE byDiHu;			//地糊
	BYTE byGSH;				//杠上花
	BYTE byGHP;				//杠后炮
	BYTE byWuHu;			//乌糊类型
	BYTE byQiXing;			//七星
	BYTE byLongQiDui;		//龙七对
	BYTE byYiBanGao;		//一般高
	BYTE byXiXiangFeng;		//喜相逢
	BYTE byLianLiu;			//连六
	BYTE byLaoShaoFu;		//老少副
	BYTE byYaoJiuKe;		//幺九刻
	BYTE byMingGang;		//明杠
	BYTE byQueYiMen;		//缺一门
	BYTE byWuZi;			//无字
	BYTE byBianZhang;		//边张
	BYTE byKanZhang;		//坎张
	BYTE byDanDiao;			//单钓将
	BYTE byJianKe;			//箭刻
	BYTE byQuanFengKe;		//圈风刻
	BYTE byMenFengKe;		//门风刻
	BYTE byMenQianQing;		//门前清
	BYTE byPingHu;			//平和
	BYTE bySiGuiYi;			//四归一
	BYTE byShuangTongKe;	//双同刻
	BYTE byShuangAnKe;		//双暗刻
	BYTE byAnGang;			//暗杠
	BYTE byDuanYao;			//断幺
	BYTE byQuanDaiYao;		//全带幺
	BYTE byBuQiuRen;		//不求人
	BYTE byShuangMingGang;	//双明杠
	BYTE byHuJueZhang;		//和绝张
	BYTE byPengPengHu;		//碰碰和
	BYTE byHunYiSe;			//混一色
	BYTE bySanSeSanBuGao;	//三色三步高
	BYTE byWuMenQi;			//五门齐
	BYTE byQuanQiuRen;		//全求人
	BYTE byShuangAnGang;	//双暗杠
	BYTE byShuangJianKe;	//双箭刻
	BYTE byHuaLong;			//花龙
	BYTE byTuiBuDao;		//推不倒
	BYTE bySSSTongShun;		//三色三同顺
	BYTE bySSSJieGao;		//三色三节高
	BYTE byWuFanHu;			//无番和
	BYTE byMiaoShouHuiChun;	//妙手回春
	BYTE byHaiDiLaoYue;		//海底捞月
	BYTE byQiangGangHu;		//抢杠和
	BYTE byQuanBuKao;		//全不靠
	BYTE byZuHeLong;		//组合龙
	BYTE byDaYuWu;			//大于五
	BYTE byXiaoYuWu;		//小于五
	BYTE bySanFengKe;		//三风刻
	BYTE byQiangLong;		//清龙
	BYTE bySSShuangLongHui;	//三色双龙会
	BYTE byYiSeSanBuGao;	//一色三步高
	BYTE byQuanDaiWu;		//全带五
	BYTE bySanTongKe;		//三同刻
	BYTE bySanAnKe;			//三暗刻
	BYTE byQiDui;			//七对
	BYTE byQiXingBuKao;		//七星不靠
	BYTE byQuanShuangKe;	//全双刻
	BYTE byQingYiSe;		//清一色
	BYTE byYiSSTongShun;	//一色三同顺
	BYTE byYiSSJieGao;		//一色三节高
	BYTE byQuanDa;			//全大
	BYTE byQuanZhong;		//全中
	BYTE byQuanXiao;		//全小
	BYTE byYiSeSiBuGao;		//一色四步高
	BYTE bySanGang;			//三杠
	BYTE byHunYaoJiu;		//混幺九
	BYTE byYSSiTongShun;	//一色四同顺
	BYTE byYSSiJieGao;		//一色四节高
	BYTE byQingYaoJiu;		//清幺九
	BYTE byXiaoSiXi;		//小四喜
	BYTE byXiaoSanYuan;		//小三元
	BYTE byZiYeSe;			//字一色
	BYTE bySiAnKe;			//四暗刻
	BYTE byYSShuangLongHui;	//一色双龙会
	BYTE byDaSiXi;			//大四喜
	BYTE byDaSanYuan;		//大三元
	BYTE byLvYiSe;			//绿一色
	BYTE byJiuLianBaoDeng;	//九莲宝灯
	BYTE bySiGang;			//四杠（十八罗汉）
	BYTE byLianQiDui;		//连七对
	BYTE byShiSanYao;		//十三幺

	HuPaiTypeSet()
	{
		memset(this, 255, sizeof(HuPaiTypeSet));
	}
};

///麻将事件数据结构
struct MjThing
{
	BYTE byBeginNext1;			//	第一局前，游戏开始事件后接事件号
	BYTE byBeginNext2;			//	第一局后，游戏开始事件后接事件号
	BYTE byDongNtNext;			//	以东为庄事件的后接事件号
	BYTE by2SeziNtNext;			//	掷2颗色子定庄事件的后接事件号
	BYTE by2SeziNtAndGpNext;	//	掷2颗色子定庄和起牌点事件的后接事件号
	BYTE byTurnSeziNtNext;		//	轮流掷2颗色子定庄事件的后接事件号
	BYTE by2SeziDirNext;		//	掷2颗色子定方向事件的后接事件号
	BYTE by2SeziDirAndGpNext;	//	掷2颗色子定方向和起牌点事件的后接事件号
	BYTE by2SeziGpNext;			//	掷2颗色子定起牌点事件的后接事件号
	BYTE bySendPaiNext;			//	发牌事件的后接事件号
	BYTE byTiaoPaiNext;			//	跳牌事件的后接事件号
	BYTE by2SeziJingNext;		//	掷2颗色子定精牌事件的后接事件号
	BYTE by1SeziJinNext;		//	掷1颗色子定金牌事件的后接事件号
	BYTE byAllBuHuaNext;		//	出牌前所有玩家补花事件的后接事件号
	BYTE byOneBuHuaNext;		//	游戏中单人补花事件的后接事件号
	BYTE byBeginOutNext;		//	开始出牌事件的后接事件号
	BYTE byOutPaiNext;			//	出牌事件的后接事件号
	BYTE byZhuaPaiNext;			//	抓牌事件的后接事件号
	BYTE byChiPaiNext;			//	吃牌事件的后接事件号
	BYTE byPengPaiNext;			//	碰牌事件的后接事件号
	BYTE byKanPaiNext;			//	坎牌事件的后接事件号
	BYTE bySaoHuNext;			//	扫虎事件的后接事件号
	BYTE byGangPaiNext;			//	杠牌事件的后接事件号
	BYTE byTingPaiNext;			//	听牌事件的后接事件号
	BYTE byHuPaiNext;			//	糊牌事件的后接事件号
	BYTE byCountFenNext;		//	算分事件的后接事件号
	BYTE byEndHandleNext;		//	结束处理事件的后接事件号
	BYTE byUnnormalEngNext;		//	非正常结束事件的后接事件号

	MjThing()
	{
		memset(this, 255, sizeof(MjThing));
	}
};

//麻将规则数据包，通知客户端该麻将的数据
struct MjRuleStruct 
{	
	MjRuleSet       m_mjRule;					// 麻将常规属性
	ActionRuleSet   m_mjAction;                 // 麻将动作属性
	CountFenRuleSet m_mjFenRule;				// 麻将算分属性
	HuPaiRuleSet    m_mjHuRule;					// 麻将糊牌属性
	HuPaiTypeSet	m_mjHuType;					// 麻将糊牌类型
};

//提牌器设置牌数据包
struct SuperStruct
{
	BYTE  bySetType;	//设置类型：0 换所有牌，1换选定牌，2设置自己的下一张牌，3设置牌强的下一张牌
	BYTE  mjcount;		//设置牌的个数
	BYTE  mjdata[HAND_CARD_NUM];	//设置牌数据
	BYTE  handdata[HAND_CARD_NUM];	//设置牌数据

	SuperStruct()
	{
		Init();
	}
	void Init()
	{	 
		bySetType = 255;
		mjcount = 0;
		for( int i = 0; i < 20; i++)
		{  mjdata[i] = 255; }
	}
};
/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/
#define  MDM_GM_GAME_NOTIFY				180				///消息类型：游戏通知消息
#define ASS_GM_GAME_STATION				2				///游戏状态
#define ASS_GM_AGREE_GAME               1               ///用户同意

#define ASS_UG_USER_SET					70				//用户设置游戏
#define ASS_SEZIFINISH					72				//色子结束
#define ASS_AUTOCARD					74				//托管
#define ASS_PASS_CHECK					75				//"过"
#define ASS_HAVE_THING					76				//有事离开
#define ASS_LEFT_RESULT					77				//离开结果

#define ASS_SUPER						78				//超级客户端，发牌器
#define ASS_HUPAI_TEST					79				//超级客户端，糊牌测试

#define ASS_MJ_RULE						89				//麻将规则数据
#define ASS_EXIT						90				//退出
#define ASS_MESSAGE						91				//消息
#define	ASS_SET_STATE					92				//正在出牌人掉线重连后发给4个客户端的计时器信号
#define ASS_GM_SHORT_STOP               93              //暂停消息
#define ASS_NET_CAUSE_END               94              //网络原因导致游戏结束

//#define ASS_HUPAI_TEST					159			//超级客户端，糊牌测试
#define ASS_CHANGE_PAI					160				//用户换牌，测试用
#define ASS_SET_MOPAI					161				//用户设置准备要摸的牌
#define ASS_HUANGZHUANG					162				//强制荒庄

/********************************************************************************/

//////////////////////事件号////////////////////////////////////////////////////////////////////
//发牌前事件
enum MJTHINGS
{
    THING_BEGIN				=10,			//游戏开始事件					        
    THING_DONG_NT			,			//首局以东为庄事件					         
    THING_2SEZI_NT			,			//首局掷2颗色子的点数和为庄事件		   
    THING_2SEZI_NT_GP		,			//首局掷2颗色子定庄家和起牌点事件	
    THING_TURN_2SEZI_NT		,			//首局轮流掷2颗色子定庄家事件		
    THING_2SEZI_DIR			,			//掷2颗色子事件定起牌方向事件		
    THING_2SEZI_DIR_GP		,			//掷2颗色子事件定起牌方向和起牌点事件
    THING_2SEZI_GP			,			//掷2颗色子事件定起牌点事件		
    THING_2SEZI_NT_DIR_GP	,			//掷2颗色子事件定起牌点事件	
    //出牌前事件            
    THING_SEND_PAI			,			//发牌事件							
    THING_TIAO_PAI			,			//跳牌事件							
    THING_2SEZI_JING		,			//掷2颗色子定精牌事件			
    THING_1SEZI_JIN			,			//掷1颗色子定金牌事件
    THING_ALL_BUHUA		    ,			//为所有玩家补花事件				
    THING_ONE_BUHUA		    ,			//单个玩家补花事件					
    //正常动作事件          
    THING_BEGIN_OUT			,			//庄家开始发牌通知事件
    THING_DING_QUE          ,           //定缺事件
    THING_OUT_PAI			,			//出牌事件
    THING_ZHUA_PAI			,			//抓牌事件
    THING_CPG_NOTIFY		,			//杠吃碰通知事件事件
    THING_CHI_PAI			,			//吃牌事件
    THING_PENG_PAI			,			//碰牌事件
    THING_KAN_PAI			,			//坎牌事件
    THING_SAO_HU			,			//扫虎事件
    THING_GANG_PAI			,			//杠牌事件
    THING_TING_PAI			,			//听牌事件
    //其他动作事件          

    //糊牌及糊牌后事件
    THING_HU_PAI			,			//糊事件	
    THING_COUNT_FEN			,			//算分事件	
    THING_ENG_HANDLE		,			//结束处理事件
    THING_ENG_UN_NORMAL		,			//非正常结束处理事件
                               
    THING_MAX				                //最大事件id
};                              
                            
//////////////////////事件数据结构/////////////

///游戏开始事件
struct tagBegin
{
	BYTE	byDo;		//0未发生，10已结束，中间状态为正在发生
	BYTE	byNt;		//庄家位置
	BYTE	byPlayCount;// 游戏回合
	BYTE	byQuanFeng;	// 圈风设置
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	byNo;		//事件的代号
	BYTE	byNext;	//下一个事件的代号
	BYTE	byNext2;	//下一个事件的代号（已经有庄时）
	void Clear()
	{
		byDo	=255;		//0未发生，10已结束，中间状态为正在发生
		byNt	=255;		//庄家位置
		byPlayCount	=255;// 游戏回合
		byQuanFeng	=255;// 圈风设置
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		byNo = THING_DONG_NT;//事件的代号
	}
};

///以东为庄事件
struct tagDongNt
{
	BYTE   byDo;		//0未发生，10已结束，中间状态为正在发生
	BYTE   byNt;		//庄家位置
	BYTE   byNo;		//事件的代号
	BYTE   byNext;	//下一个事件的代号
	void Clear()
	{	
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byNt	=255;//庄家位置
		byNo = THING_BEGIN;//事件的代号
	}
};

///掷2颗色子的点数和为庄事件
struct tagTwoSeziNt
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//掷色子玩家
	BYTE	byNt;//庄家位置
	BYTE	bySezi0;//色子0数据
	BYTE	bySezi1;//色子1数据
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE	byNo;//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//掷色子玩家
		byNt	=255;//庄家位置
		bySezi0	=255;//色子0数据
		bySezi1	=255;//色子1数据	
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_2SEZI_NT;//事件的代号
	}
};

///掷2颗色子确定庄家和起牌点位置事件
struct tagTwoSeziNtAndGetPai
{
	BYTE   byDo;	//0未发生，10已结束，中间状态为正在发生
	BYTE   byUser;	//掷色子玩家
	BYTE   byNt;	//庄家位置
	BYTE   bySezi0;	//色子0数据
	BYTE   bySezi1;	//色子1数据
	BYTE   byGetPai;//起牌位置
	bool   bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE   byNo;//事件的代号
	BYTE   byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//掷色子玩家
		byNt	=255;//庄家位置
		bySezi0	=255;//色子0数据
		bySezi1	=255;//色子1数据
		byGetPai=255;//起牌位置
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_2SEZI_NT_GP;//事件的代号
	}
};

///轮流掷2颗色子确定庄家
struct tagTurnSeziNt
{

	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byNt;//庄家位置
	BYTE	byUser;//当前掷色子的玩家
	BYTE	bySezi0;//色子0数据
	BYTE	bySezi1;//色子1数据
	BYTE	bySezi[4];//玩家0色子数据
	BYTE	byMaxUser;//最大点数的玩家个数
	BYTE	byMaxDian;//最大点数值
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	bool	bDoFinish[4];//记录已经掷完色子玩家
	bool	bDoSezi[4];//本轮要掷色子的玩家
	bool	bFirst;//是否第一轮掷色子
	BYTE	byNo;//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byNt	=255;//庄家位置
		byUser	=255;//当前掷色子的玩家
		bySezi0	=255;//色子0数据
		bySezi1	=255;//色子1数据
		memset(bySezi,255,sizeof(bySezi));//玩家0色子数据
		byMaxUser	=255;//最大点数的玩家个数
		byMaxDian	=255;//最大点数值
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		memset(bDoFinish,0,sizeof(bDoFinish));
		memset(bDoSezi,0,sizeof(bDoSezi));
		bFirst = true;//是否第一轮掷色子
		byNo = THING_TURN_2SEZI_NT;//事件的代号
	}
};

///掷2颗色子确定起牌位置事件
struct tagTwoSeziDir
{
	BYTE	byDo;			//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;			//掷色子玩家
	BYTE	byNt;			//庄家
	BYTE	bySezi0;		//色子0数据
	BYTE	bySezi1;		//色子1数据
	BYTE	byDirection;	//起牌方位
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE	byNo;			//事件的代号
	BYTE	byNext;		//下一个事件的代号
	void Clear()
	{
		byDo	=255;			//0未发生，10已结束，中间状态为正在发生
		byUser	=255;		//掷色子玩家
		bySezi0	=255;		//色子0数据
		bySezi1	=255;		//色子1数据
		byNt = 255;			//庄家
		byDirection	=255;	//起牌方位
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_2SEZI_DIR;//事件的代号
	}
};

///掷2颗色子确定起牌位置（点数和）和起牌点（最小点）事件
struct tagTwoSeziDirAndGetPai
{
	BYTE	byDo;		//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;	//掷色子玩家
	BYTE	bySezi0;	//色子0数据
	BYTE	bySezi1;	//色子1数据
	BYTE	byDirection;//起牌方位
	BYTE	byGetPai;	//起牌点
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE	byNo;		//事件的代号
	BYTE	byNext;	//下一个事件的代号
	void Clear()
	{
		byDo	=255;		//0未发生，10已结束，中间状态为正在发生
		byUser	=255;	//掷色子玩家
		bySezi0	=255;	//色子0数据
		bySezi1	=255;	//色子1数据
		byDirection	=255;//起牌方位
		byGetPai	=255;	//起牌点
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_2SEZI_DIR_GP;//事件的代号
	}
};

///掷2颗色子确定起牌敦数事件
struct tagTwoSeziGetPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//掷色子玩家
	BYTE	bySezi0;//色子0数据
	BYTE	bySezi1;//色子1数据
	BYTE	byGetPai;//起牌点
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;		//0未发生，10已结束，中间状态为正在发生
		byUser	=255;	//掷色子玩家
		bySezi0	=255;	//色子0数据
		bySezi1	=255;	//色子1数据
		byGetPai	=255;	//起牌点
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_2SEZI_GP;//事件的代号
	}
};

///发牌事件
struct tagSendPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byPaiNum;//每个玩家发牌张数
	BYTE	byRemainPai;//剩余牌数
	BYTE	bySendPai;//发出牌数
	BYTE	byGetPai;//起牌点
	BYTE	byAllPaiNum;//总牌数
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	byStartIndex;///开始索引
	BYTE	byEndIndex;  ///结束索引

	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo		=255;//0未发生，10已结束，中间状态为正在发生
		byPaiNum	=255;//每个玩家发牌张数
		byRemainPai	=255;//剩余牌数
		bySendPai	=255;//发出牌数
		byGetPai	=255;//起牌点
		byAllPaiNum	=255;//总牌数
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		byStartIndex	=255;///开始索引
		byEndIndex	=255;///结束索引
		byNo = THING_SEND_PAI;//事件的代号
	}
};
///跳牌事件
struct tagTiaoPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//跳牌玩家位置
	BYTE	byNt;//庄家位置
	BYTE	byIndex0;//跳牌位置0
	BYTE	byIndex1;//跳牌位置1
	BYTE	byRemainPai;//剩余牌数
	BYTE	bySendPai;//发出牌数
	BYTE	byGetPai;//起牌点
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo		=255;//0未发生，10已结束，中间状态为正在发生
		byUser		=255;//跳牌玩家位置
		byNt		=255;//庄家位置
		byIndex0	=255;//跳牌位置0
		byIndex1	=255;//跳牌位置1
		byRemainPai	=255;//剩余牌数
		bySendPai	=255;//发出牌数
		byGetPai	=255;//起牌点
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		byNo = THING_TIAO_PAI;//事件的代号
	}
};
///掷色子2颗色子定精牌事件
struct tagTwoSeziJing
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//掷色子玩家位置
	BYTE	byNt;//庄家位置
	BYTE	byFanPai;//翻牌点
	BYTE	bySezi0;//色子0数据
	BYTE	bySezi1;//色子1数据
	BYTE	byMainJing;//正精牌值
	BYTE	byLessJing;//副精的牌值
	BYTE	byStartIndex;///开始索引
	BYTE	byEndIndex;  ///结束索引
	BYTE	byCaiShenIndex; ///财神索引
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo		=255;//0未发生，10已结束，中间状态为正在发生
		byUser		=255;//掷色子玩家位置
		byNt		=255;//庄家位置
		byFanPai	=255;//翻牌点
		bySezi0		=255;//色子0数据
		bySezi1		=255;//色子1数据
		byMainJing	=255;//正精牌值
		byLessJing	=255;//副精的牌值
		byStartIndex	=255;///开始索引
		byEndIndex	=255;  ///结束索引
		byCaiShenIndex=255; ///财神索引
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_2SEZI_JING;//事件的代号
	}
};
///掷色子1颗色子定金牌事件
struct tagOneSeziJin
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//掷色子玩家位置
	BYTE	byNt;//庄家位置
	BYTE	byFanPai;//翻牌点
	BYTE	bySezi0;//色子0数据
	BYTE	byMainJing;//正精牌值
	BYTE	byLessJing;//副精的牌值
	BYTE	byStartIndex;///开始索引
	BYTE	byEndIndex;  ///结束索引
	BYTE	byCaiShenIndex; ///财神索引
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	bool	bSeziFinish[4];//记录已经播放完色子动画的玩家
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo		=255;//0未发生，10已结束，中间状态为正在发生
		byUser		=255;//掷色子玩家位置
		byNt		=255;//庄家位置
		byFanPai	=255;//翻牌点
		bySezi0		=255;//色子0数据
		byMainJing	=255;//正精牌值
		byLessJing	=255;//副精的牌值
		byStartIndex	=255;///开始索引
		byEndIndex	=255;  ///结束索引
		byCaiShenIndex=255; ///财神索引
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		memset(bSeziFinish,0,sizeof(bSeziFinish));
		byNo = THING_1SEZI_JIN;//事件的代号
	}
};

///所有玩家补花事件
struct tagAllBuHua
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser[4];//需要补花玩家位置
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	m_byArHuaPai[4][HUA_CARD_NUM];//花牌数据
	bool	bBuHuaFinish[4];//记录已经已经补花完成的玩家
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo = 255;//0未发生，10已结束，中间状态为正在发生
		memset(byUser, 255, sizeof(byUser));	//需要补花玩家位置
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArHuaPai, 255, sizeof(m_byArHuaPai));//花牌数据	
		memset(bBuHuaFinish,0,sizeof(bBuHuaFinish));
		byNo = THING_ALL_BUHUA;//事件的代号
	}
};

///单个玩家补花事件
struct tagOneBuHua
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//补花玩家位置
	BYTE	byHua;	  //抓到得花牌
	BYTE	byPs;	  //补花的牌
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	m_byArHuaPai[4][HUA_CARD_NUM];//
	bool	bFinish;//是否补花结束
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//补花玩家位置
		byHua	=255;	  //抓到得花牌
		byPs	=255;	  //补花的牌
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArHuaPai, 255, sizeof(m_byArHuaPai));//花牌数据
		bFinish=false;
		byNo = THING_ONE_BUHUA;//事件的代号
	}
};
///开始出牌通知事件
struct tagBeginOutPai
{
	BYTE	byDo;	//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;	//首次出牌玩家位置
	BYTE	byNt;	//庄家位置
	BYTE	byMainJing;//正精牌值
	BYTE	byLessJing;//副精的牌值
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//首次出牌玩家位置
		byNt	=255;//庄家位置
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		byMainJing	=255;//正精牌值
		byLessJing	=255;//副精的牌值
		byNo = THING_BEGIN_OUT;//事件的代号
	}
};
///出牌事件
struct tagOutPai
{
	BYTE	byDo;				//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;				//出牌玩家位置
	BYTE	byPs;				//出牌的值
	BYTE	byMaxAction;		//玩家最大可以做的动作
	BYTE	byMaxStation;		//最大可以做的动作，的玩家位置
	BYTE	byCanAction[4][5];	//玩家可以做的动作
	BYTE	byWantAction[4];	//玩家想做的动作
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[4][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	bool	bOtherUse;			//牌打出后其他人要了这张牌
	bool	bTing;				//是否听牌出牌
	bool    bUserTing[4];		//玩家听牌状态
	BYTE	byNo;				//事件的代号
	BYTE	byNext;				//下一个事件的代号(0代表是循环事件，下一个事件由用户触发)
	void Clear()
	{
		byDo	=255;				//0未发生，10已结束，中间状态为正在发生
		byUser	=255;				//出牌玩家位置
		byPs		=255;				//出牌的值
		byMaxAction	=255;		//玩家最大可以做的动作
		byMaxStation = 255;		//最大可以做的动作，的玩家位置
		memset(byCanAction, 255, sizeof(byCanAction));//玩家最大可以做的动作	
		memset(byWantAction, 255, sizeof(byWantAction));//玩家想做的动作
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据	
		memset(bUserTing, 0, sizeof(bUserTing));//玩家听牌状态	
		bOtherUse = false;    //牌打出后其他人要了这张牌
		bTing = false;//是否听牌出牌
		byNo = THING_OUT_PAI;//事件的代号
	}
};
///抓牌牌事件
struct tagZhuaPai
{
	BYTE	byDo;		//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;		//抓牌玩家位置
	BYTE	byPs;		//抓牌的值
	BYTE	byStartIndex;///开始索引
	BYTE	byEndIndex;  ///结束索引
	BYTE	byCaiShenIndex; ///财神索引
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[4][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	BYTE	byMenPai[4][MEN_CARD_NUM];//门牌
	BYTE	m_byArHuaPai[4][HUA_CARD_NUM];//玩家花牌
	BYTE	byGetPaiIndex;//抓牌的索引
	bool	bHead;		//抓牌的方向
	bool	bCanOutPai;	//抓牌后是否可以出牌
	bool    m_bGangKai;	//是否杠开	
	bool    m_bBuHua;	//是否要补花	
	bool    bUserTing[4];		//玩家听牌状态
	BYTE	byNo;		//事件的代号
	BYTE	byNext;		//下一个事件的代号
	void Clear()
	{
		byDo	=255;		//0未发生，10已结束，中间状态为正在发生
		byUser	=255;		//抓牌玩家位置
		byPs		=255;		//抓牌的值
		byStartIndex=255;///开始索引
		byEndIndex	=255;  ///结束索引
		byCaiShenIndex=255; ///财神索引
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据
		memset(byMenPai, 255, sizeof(byMenPai));	//门牌
		memset(m_byArHuaPai, 255, sizeof(m_byArHuaPai));	//花牌
		byGetPaiIndex = 255;//抓牌的索引
		bHead = true;
		bCanOutPai = false;//抓牌后是否可以出牌
		m_bGangKai = false;//是否杠开	
		m_bBuHua = false;	//是否要补花	
		memset(bUserTing, 0, sizeof(bUserTing));//玩家听牌状态	
		byNo = THING_ZHUA_PAI;//事件的代号
	}
};
///吃碰杠糊通知事件事件
struct tagCPGNotify
{
	BYTE	byDo;				//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;				//出牌玩家位置
	BYTE	byPs;				//出牌的值
	bool	bZhuaPai;			//是否自己抓牌
	bool	bChi;				//是否能吃
	bool	bPeng;				//是否能碰
	bool	bGang;				//是否能杠
	bool	bKan;				//是否能坎牌
	bool	bSaoHu;				//当前能否扫虎
	bool	bTing;				//当前能否听牌
	bool	bHu;				//是否能糊
	bool	bQiangGang;			//是否抢杠糊
	bool    bHaveReturn;		//客户端是否返回的用户选择
	bool    bCanAction;			//玩家是否有动作可做
	BYTE	m_iChi[3][3];		//能吃的牌
	BYTE	m_iGangData[4][2];	//能杠的牌	
	BYTE	m_byTingCanOut[HAND_CARD_NUM];//听牌能打的牌
	BYTE    m_byMaxThing;		//玩家能做的最大动作
	BYTE	byHuType[MAX_HUPAI_TYPE];	//糊牌类型
	BYTE	m_byKan[5];			//当前能坎的牌
	BYTE    m_iHuPs;			//玩家胡的牌
	BYTE	m_byDianPao;		//点炮玩家位置
	BYTE	byNo;				//事件的代号
	BYTE	byNext;				//下一个事件的代号(0代表是循环事件，下一个事件由用户触发)
	void Clear()
	{
		byDo	=255;			//0未发生，10已结束，中间状态为正在发生
		byUser	=255;			//出牌玩家位置
		byPs	=255;			//出牌的值
		bZhuaPai=0;				//是否自己抓牌
		bChi	=0;				//是否能吃
		bPeng	=0;				//是否能碰
		bGang	=0;				//是否能杠
		bKan	=0;				//是否能坎牌
		bSaoHu  =0;				//当前能否扫虎
		bTing	=0;				//当前能否听牌
		bHu		=0;				//是否能糊
		bQiangGang = 0;			//是否抢杠糊
		bCanAction = false;			//玩家是否有动作可做
		m_byMaxThing = 0;		//玩家能做的最大动作
		bHaveReturn = 0;		//客户端是否返回的用户选择
		memset(m_iChi,255,sizeof(m_iChi));//能吃的牌
		memset(m_iGangData,255,sizeof(m_iGangData));//能杠的牌	
		memset(m_byTingCanOut,255,sizeof(m_byTingCanOut));///听牌能打的牌
		memset(byHuType,255,sizeof(byHuType));///糊牌类型
		memset(m_byKan,255,sizeof(m_byKan));///当前能坎的牌
		m_iHuPs = 255;///玩家胡的牌
		m_byDianPao = 255;		//点炮玩家位置
		byNo = THING_OUT_PAI;//事件的代号
	}
};
///吃牌牌事件
struct tagChiPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//吃牌玩家位置
	BYTE	byBeChi;//被吃牌玩家位置
	BYTE	byType;//吃牌的类型（0吃大，1吃中间，2吃小，吃字牌）
	BYTE	byPs;	//吃的牌值
	BYTE	byChiPs[3];//吃牌的数组
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[4][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//吃牌玩家位置
		byBeChi	=255;//被吃牌玩家位置
		byType	=255;//吃牌的类型（0吃大，1吃中间，2吃小，吃字牌）
		byPs		=255;	//吃的牌值
		memset(byChiPs, 255, sizeof(byChiPs));//吃牌的数组
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据
		byNo = THING_CHI_PAI;//事件的代号
	}
};
///碰牌牌事件
struct tagPengPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//碰牌玩家位置
	BYTE	byBePeng;//被碰牌玩家位置
	BYTE	byPs;	//碰的牌值
	BYTE	m_byArHandPai[PLAY_COUNT][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[PLAY_COUNT][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//吃牌玩家位置
		byBePeng	=255;//被碰牌玩家位置
		byPs		=255;//碰的牌值
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据
		byNo = THING_PENG_PAI;//事件的代号
	}
};
///坎牌事件
struct tagKanPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//碰牌玩家位置
	BYTE	byPs;	//碰的牌值
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//吃牌玩家位置
		byPs		=255;//碰的牌值
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		byNo = THING_KAN_PAI;//事件的代号
	}
};
///扫虎事件（查虎麻将专用）
struct tagSaoHu
{
	BYTE	byDo;		//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;		//玩家位置
	BYTE	byPs;		//碰的牌值
	bool	bSaohu[4];	//扫虎的玩家
	bool	bHitSaohu[4];	//刚点击了扫虎
	BYTE    byOutData[4];//单钓扫虎玩家出的牌
	BYTE    byHandData[4];//单钓扫虎玩家留在手中的牌
	BYTE    byBaoPai;	//包牌玩家位置
	BYTE	byNo;		//事件的代号
	BYTE	byNext;		//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//吃牌玩家位置
		byPs	=255;//碰的牌值
		byBaoPai=255;	//包牌玩家位置
		memset(bSaohu, 0, sizeof(bSaohu));//	
		memset(bHitSaohu, 0, sizeof(bHitSaohu));//	
		memset(byOutData, 255, sizeof(byOutData));//	
		memset(byHandData, 255, sizeof(byHandData));//	
		byNo = THING_SAO_HU;//事件的代号
	}
};
///杠牌牌事件
struct tagGangPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//杠牌玩家位置
	BYTE	byBeGang;//被杠牌玩家位置
	BYTE	byType;//杠牌的类型
	BYTE	byPs;	//杠的牌值
	BYTE	byGangPai[4];//要杠的四张牌
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[4][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	bool    bHe[4];//玩家能否抢杠糊
	bool    bChenkQ;//检测抢杠
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//杠牌玩家位置
		byBeGang	=255;//被杠牌玩家位置
		byType	=255;//杠牌的类型
		byPs		=255;	//杠的牌值
		memset(byGangPai, 255, sizeof(byGangPai));//要杠的四张牌
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据
		memset(bHe, 0, sizeof(bHe));
		bChenkQ = false;
		byNo = THING_GANG_PAI;//事件的代号
	}
};
///听牌牌事件
struct tagTingPai
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//听牌玩家位置
	BYTE	byTing;//出牌玩家位置
	BYTE	byPs;	//听牌出的牌值
	BYTE	byNo;	//事件的代号
	bool	bHitTing[4];//玩家点击了听牌
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//听牌玩家位置
		byTing	=255;//出牌玩家位置
		byPs		=255;	//听牌出的牌值	
		memset(bHitTing,0,sizeof(bHitTing));//玩家点击了听牌
		byNo = THING_TING_PAI;//事件的代号
	}
};
///糊牌牌事件
struct tagHuPai
{
	BYTE	byDo;				//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;				//糊牌玩家位置
	BYTE	byDianPao;			//点炮玩家位置
	BYTE	byNt;				//庄家位置
	BYTE	byPs;				//点炮的牌值
	BYTE	byMainJing;//正精牌值
	BYTE	byLessJing;//副精的牌值
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[4][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	BYTE    byHuType[4][MAX_HUPAI_TYPE];//各个玩家的糊牌类型
	BYTE	byUserNum;			//糊牌玩家的个数
	BYTE	byBaoPaiUser;		//包牌玩家
	bool	bCanH[4];			//位置0-3那些玩家点击了和
	bool	bDuoXiang;			//是否一炮多响
	bool    bZimo;				//是否自摸
	bool    bQiangGang;			//是否抢杠
	BYTE	byNo;				//事件的代号
	BYTE	byNext;				//下一个事件的代号
	void Clear()
	{
		byDo		=255;//0未发生，10已结束，中间状态为正在发生
		byUser		=255;//糊牌玩家位置
		byDianPao	=255;//点炮玩家位置
		byNt		=255;//庄家位置
		byPs		=255;	//点炮的牌值
		byMainJing	=255;//正精牌值
		byLessJing	=255;//副精的牌值
		byBaoPaiUser=255;		//包牌玩家
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据
		memset(byHuType, 255, sizeof(byHuType));//各个玩家的糊牌类型
		byUserNum = 0;//糊牌玩家的个数
		memset(bCanH,0,sizeof(bCanH));
		bDuoXiang = false;//是否一炮多响
		bZimo = false;   //是否自摸
		bQiangGang = false;   //是否抢杠
		byNo = THING_HU_PAI;//事件的代号
	}
};
///算分事件
struct tagCountFen
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//糊牌玩家位置
	BYTE	byDian;//点炮玩家位置
	BYTE	byNt;//庄家位置
	BYTE	byNextNt;//下局庄家位置
	BYTE	byLianZhuang;//连庄数
	bool	bHu[4];//那些玩家糊牌
	bool	bCanHu[4];//本次能能胡的玩家
	bool	bZiMo;//是否自摸
	bool    bTest;//是否测试数据包
	__int64 		iZongFen[8];//各个玩家的总分
	__int64 		iMoney[8];//各个玩家的总分
	int		iHuShu[4];//各个玩家的糊数
	int 	iHuFen[4];//各个玩家的糊牌分
	BYTE	m_byArHandPai[4][HAND_CARD_NUM];//手牌数据
	BYTE	m_byArOutPai[4][OUT_CARD_NUM];//出牌不超过160张，0为结束标志
	BYTE    byHuType[4][MAX_HUPAI_TYPE];//各个玩家的糊牌类型	
	bool	bDH;//一炮多响
	BYTE	byPs;	//点炮的牌值
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	void Clear()
	{
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//糊牌玩家位置
		byDian	=255;//点炮玩家位置
		byNt	=255;//庄家位置
		byNextNt=255;//下局庄家位置
		byLianZhuang = 0;//连庄数
		bZiMo   = false;
		bTest   = false;//是否测试数据包
		memset(bHu,0,sizeof(bHu));
		memset(bCanHu,0,sizeof(bCanHu));
		memset(iZongFen,0,sizeof(iZongFen));
		memset(iHuShu,0,sizeof(iHuShu));
		memset(iMoney,0,sizeof(iMoney));
		memset(iHuFen,0,sizeof(iHuFen));
		memset(m_byArHandPai, 255, sizeof(m_byArHandPai));//手牌数据	
		memset(m_byArOutPai, 255, sizeof(m_byArOutPai));//出牌数据
		memset(byHuType, 255, sizeof(byHuType));//各个玩家的糊牌类型
		bDH = false;	
		byPs = 255;	//点炮的牌值
		byNo = THING_COUNT_FEN;//事件的代号
	}
};
///游戏结束处理事件
struct tagEndHandle
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//糊牌玩家位置
	BYTE	byHu;//点炮玩家位置
	BYTE	byNextNt;//下局庄家位置
	BYTE	byPs;	//点炮的牌值
	bool	bDH;//一炮多响
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	tagEndHandle()
	{
		Clear();
	}
	void Clear()
	{		
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//糊牌玩家位置
		byHu	=255;//点炮玩家位置
		byNextNt=255;//下局庄家位置
		byPs		=255;	//点炮的牌值
		bDH = 0;	
		byNo = THING_ENG_HANDLE;//事件的代号
	}
};
///非正常结束处理事件
struct tagUnnormalEndHandle
{
	BYTE	byDo;//0未发生，10已结束，中间状态为正在发生
	BYTE	byUser;//糊牌玩家位置
	int		iFen[8];//得分
	int		iMoney[8];//金币
	bool    bForce;//是否强退结束
	BYTE	byNo;	//事件的代号
	BYTE	byNext;//下一个事件的代号
	tagUnnormalEndHandle()
	{
		Clear();
	}
	void Clear()
	{		
		byDo	=255;//0未发生，10已结束，中间状态为正在发生
		byUser	=255;//糊牌玩家位置
		memset(iFen, 0, sizeof(iFen));
		memset(iMoney, 0, sizeof(iMoney));
		bForce = false;//是否安全结束
		byNo = THING_ENG_UN_NORMAL;//事件的代号
	}
};
///////////////////////////////////////////////////


#endif