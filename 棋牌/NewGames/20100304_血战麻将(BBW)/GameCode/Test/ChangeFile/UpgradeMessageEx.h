#pragma once
#include "../GameMessage/UpgradeMessage.h"
#pragma pack(push,1)


#define MAIN_VERSION					710		// 主版本号,?年?月
#define ASS_VERSION						15		// 辅助版本号,?号
         
#ifdef HAOHUA //豪华版
//文件名字定义
#define GAMENAME						TEXT("推倒胡2人麻将") 
#define NAME_ID						    20607200  		// 名字 ID
#else
//文件名字定义
#define GAMENAME						TEXT("推倒胡2人麻将") 
#define NAME_ID						    20607200  		// 名字 ID
#endif

static TCHAR szTempStr[MAX_PATH] =		{0};			/** 暂存字符串，用来获取 dll_name 等的时候使用 */
#define GET_CLIENT_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".ico"))
#define GET_SERVER_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".dll"))
#define GET_SKIN_FOLDER(A)				(sprintf(A,"%d",NAME_ID),strcat(A,""))
///////////////////////////////////////////////////////////////////////////////////////////////////


#define ROOM_ID                         1

#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)
#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")  // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))          // 客户端目录
#define SKIN_MJ							TEXT("MjSkin")					   // 麻将共用图片文件夹

// 版本定义
#define GAME_MAX_VER					1				// 现在最高版本
#define GAME_LESS_VER					1				// 现在最低版本
#define GAME_CHANGE_VER					0				// 修改版本

#define PLAY_COUNT						2				// 游戏人数

#define MAX_HAND_PAI					14				//手牌数


#define EVENT_MAKE_DIFEN				70				//确定底分事件
#define EVENT_YA_ZHU					71				//押注事件



enum	BUTTON_TYPE			// 按钮类型
{
	BUTTON_HE = 0,		//胡牌按钮
	BUTTON_GANG,		//杠牌
	BUTTON_PENG,		//碰牌
	BUTTON_CHI,			//吃牌
	BUTTON_GUO,			//放弃
	BUTTON_TING			//听牌
};

enum	USER_STATE_TYPE			// 出牌类型
{
	USER_STATE_TING = 0,	//听牌状态
	USER_STATE_AUTO,		//托管状态
	USER_STATE_GANG,		//杠状态
	USER_STATE_CUT,			//断线状态
	USER_STATE_HAVE_OUT		//已经出牌状态
};

enum HUPAI_TYPE_EX//LU胡牌类型枚举结构
{	
	HUPAI_HU_PAI			=150,//胡牌
	HUPAI_QING_YI_SE,			//清一色
	HUPAI_PENG_PENG_HU,			//碰碰胡
	HUPAI_QI_DUI,				//七对
	HUPAI_HAO_HUA_QI_DUI,		//豪华七对
	HUPAI_TIAN_HU,				//天糊
	HUPAI_DI_HU,				//地糊
	HUPAI_QIANG_GANG,			//抢杠
	HUPAI_GANG_KAI,				//杠开
	HUPAI_GANG_PAO,				//杠后炮
	HUPAI_ZI_MO					//自摸
};

enum	GAME_STATE			// 游戏状态
{
	STATE_BEGIN_OUT,	//出牌前状态：不处理，出牌，拦牌消息
	STATE_OUT_CARD,		//处理出牌消息，抓牌玩家的杠胡消息
	STATE_BLOCK_CARD,	//处理拦牌消息，和放弃消息
	STATE_QIANG_GANG,	//处理拦牌，抢杠消息
	STATE_FINISH		//游戏结束庄：不出来任何消息
};

///冒泡排序(从大到小排序)
static void MaoPaoSort(int a[], int count,bool big)
{
	if(a == NULL)
        return ;
	for (int i =0; i < count - 1; i++ )
	{
	    for(int j = 0; j < count - i -1; j++)
		{
			if(big)
			{
				if(a[j] < a[j+1])
				{
					int iTempBig = a[j];
					a[j] = a[j+1];
					a[j+1] = iTempBig;		
				}
			}
			else
			{
				if(a[j] > a[j+1])
				{
					int iTempBig = a[j];
					a[j] = a[j+1];
					a[j+1] = iTempBig;		
				}
			}
		}	
	}
}
///冒泡排序(从大到小排序)
static void MaoPaoSort(BYTE a[], int count,bool big)
{
	if(a == NULL)
        return ;
	for (int i =0; i < count - 1; ++i )
	{
	    for(int j = i+1; j < count; ++j)
		{
			if(big)
			{
				if(a[j] > a[i])
				{
					BYTE iTempBig = a[j];
					a[j] = a[i];
					a[i] = iTempBig;		
				}
			}
			else
			{
				if(a[j] < a[i])
				{
					BYTE iTempBig = a[j];
					a[j] = a[i];
					a[i] = iTempBig;		
				}
			}
		}	
	}
}
//冒泡排序(从大到小)
static void MaoPaoSort(PaiStruct a[], int count)
{
	if(a == NULL)
        return ;
	PaiStruct iTempBig ;
	for (int i =0; i < count - 1; i++ )
	{
	    for(int j = i; j < count; j++)
		{
			if(a[i].count < a[j].count)
			{
				iTempBig.count= a[i].count;
				iTempBig.pai= a[i].pai;

				a[i].count = a[j].count;
				a[i].pai = a[j].pai;

				a[j].count = iTempBig.count;	
				a[j].pai = iTempBig.pai;	
			}
		}	
	}
}

///吃碰杠牌数据结构
struct GCPStructEx:GCPStruct
{
	GCPStructEx()
	{
		Init();
	}
	void Init()
	{
		GCPStruct::Init();
	};
};

///事件数据包,重载得来，可以添加自己的特殊数据
///游戏开始事件
struct tagBeginEx:tagBegin
{
	BYTE	m_byMenFeng[PLAY_COUNT];
	void Clear()
	{
		tagBegin::Clear();
		memset(m_byMenFeng,255,sizeof(m_byMenFeng));
	}
};

///以东为庄事件
struct tagDongNtEx:tagDongNt
{
	void Clear()
	{
		tagDongNt::Clear();
	}
};

///掷2颗色子的点数和为庄事件
struct tagTwoSeziNtEx:tagTwoSeziNt
{
	void Clear()
	{
		tagTwoSeziNt::Clear();
	}
};

///掷2颗色子确定庄家和起牌点位置事件
struct tagTwoSeziNtAndGetPaiEx:tagTwoSeziNtAndGetPai
{
	void Clear()
	{
		tagTwoSeziNtAndGetPai::Clear();
	}
};

///轮流掷2颗色子确定庄家
struct tagTurnSeziNtEx:tagTurnSeziNt
{
	void Clear()
	{
		tagTurnSeziNt::Clear();
	}
};

///掷2颗色子确定起牌位置事件
struct tagTwoSeziDirEx:tagTwoSeziDir
{
	void Clear()
	{
		tagTwoSeziDir::Clear();
	}
};

///掷2颗色子确定起牌位置（点数和）和起牌点（最小点）事件
struct tagTwoSeziDirAndGetPaiEx:tagTwoSeziDirAndGetPai
{
	void Clear()
	{
		tagTwoSeziDirAndGetPai::Clear();
	}
};

///掷2颗色子确定起牌敦数事件
struct tagTwoSeziGetPaiEx:tagTwoSeziGetPai
{
	void Clear()
	{
		tagTwoSeziGetPai::Clear();
	}
};

///发牌事件
struct tagSendPaiEx:tagSendPai
{
	void Clear()
	{
		tagSendPai::Clear();
	}
};

///掷色子2颗色子定精牌事件
struct tagTwoSeziJingEx:tagTwoSeziJing
{
	void Clear()
	{
		tagTwoSeziJing::Clear();
	}
};

///所有玩家补花事件
struct tagAllBuHuaEx:tagAllBuHua
{
	void Clear()
	{
		tagAllBuHua::Clear();
	}
};

///单个玩家补花事件
struct tagOneBuHuaEx:tagOneBuHua
{
	void Clear()
	{
		tagOneBuHua::Clear();
	}
};
///开始出牌通知事件
struct tagBeginOutPaiEx:tagBeginOutPai
{

	JingStruct  m_StrJing;//财神
	void Clear()
	{
		tagBeginOutPai::Clear();
		m_StrJing.Init();
	}
};
///出牌事件
struct tagOutPaiEx:tagOutPai
{
	void Clear()
	{
		tagOutPai::Clear();
	}
};
///抓牌牌事件
struct tagZhuaPaiEx:tagZhuaPai
{
	JingStruct  m_StrJing;//财神
	void Clear()
	{
		tagZhuaPai::Clear();
		m_StrJing.Init();
	}
};
///吃碰杠糊通知事件事件
struct tagCPGNotifyEx:tagCPGNotify
{
	BYTE	m_byFan;//检测到的糊牌番数
	void Clear()
	{
		tagCPGNotify::Clear();
		m_byFan = 0;//检测到的糊牌番数
	}
};
///吃牌牌事件
struct tagChiPaiEx:tagChiPai
{
	GCPStructEx m_UserGCPData[4][5];     //吃碰杠杠牌
	void Clear()
	{
		tagChiPai::Clear();
		memset(m_UserGCPData, 255, sizeof(m_UserGCPData));//吃碰杠杠牌
	}
};
///碰牌牌事件
struct tagPengPaiEx:tagPengPai
{
	GCPStructEx m_UserGCPData[4][5];     //吃碰杠杠牌
	void Clear()
	{
		tagPengPai::Clear();
		memset(m_UserGCPData, 255, sizeof(m_UserGCPData));//吃碰杠杠牌
	}
};

///杠牌牌事件
struct tagGangPaiEx:tagGangPai
{
	GCPStructEx m_UserGCPData[4][5];     //吃碰杠杠牌
	void Clear()
	{
		tagGangPai::Clear();
		memset(m_UserGCPData, 255, sizeof(m_UserGCPData));//吃碰杠杠牌
	}
};
///听牌牌事件
struct tagTingPaiEx:tagTingPai
{
	void Clear()
	{
		tagTingPai::Clear();
	}
};
///糊牌牌事件
struct tagHuPaiEx:tagHuPai
{
	GCPStructEx m_UserGCPData[4][5];     //吃碰杠杠牌
	void Clear()
	{
		tagHuPai::Clear();
		memset(m_UserGCPData, 255, sizeof(m_UserGCPData));//吃碰杠杠牌
	}
};
///算分事件
struct tagCountFenEx:tagCountFen
{
	int 	iGangFen[4];//各个玩家的杠分
	GCPStructEx m_UserGCPData[4][5];     //吃碰杠杠牌
	int		iYaZhu[4];//押注得分数
	//闲家同意押注情况
	bool	m_bYaZhu[PLAY_COUNT];
	void Clear()
	{
		memset(iGangFen, 0, sizeof(iGangFen));//各个玩家的杠分
		tagCountFen::Clear();
		memset(m_UserGCPData, 255, sizeof(m_UserGCPData));//吃碰杠杠牌
		memset(iYaZhu, 0, sizeof(iYaZhu));//押注得分数
		memset(m_bYaZhu, 0, sizeof(m_bYaZhu));//闲家同意押注情况

	}
};
///游戏结束处理事件
struct tagEndHandleEx:tagEndHandle
{
	void Clear()
	{
		tagEndHandle::Clear();
	}
};
///非正常结束处理事件
struct tagUnnormalEndHandleEx:tagUnnormalEndHandle
{
	void Clear()
	{
		tagUnnormalEndHandle::Clear();
	}
};
///////////////////////////////////////////////////

///确定底分事件数据包
struct tagFanDiFenEx
{
	int     iBaseFen;//底分
	BYTE	byMaster;//桌主位置
	BYTE	byType;//0设置底分消息，1同意底分消息
	BYTE	byNo;	//事件的代号
	BYTE	byNext;	//下一个事件的代号
	void Clear()
	{
		byNo = EVENT_MAKE_DIFEN;//事件的代号
		byNext = 255;
	}
};
///押注事件数据包
struct tagYaZhuEx
{
	BYTE	byNote;//当前押注状态：255没有回应，0,不同意，1同意
	BYTE	byNotify;//通知类型：0通知庄家押注，1通知闲家押注，2押注结果
	BYTE	byNtNote;//庄家押注状态：255没有回应，0不压，1一半，2全额
	BYTE	byUserNote[4];//各个玩家押注状态：255没有回应，0,不同意，1同意
	BYTE	byNo;		//事件的代号
	BYTE	byNext;	//下一个事件的代号
	void Clear()
	{
		byNote = 255;//当前玩家的押注状态
		byNotify = 255;
		byNtNote = 255;
		memset(byUserNote, 255, sizeof(byUserNote));//各个玩家押注状态：255没有回应，0,不同意，1同意
		byNo = EVENT_YA_ZHU;//事件的代号
		byNext = 255;
	}
};


//////////////////新增数据包/////////
///麻将参数设置
struct MJ_SettingEx:MJ_Setting
{
	BYTE		byYaZhuTime;				//押注等待时间
    MJ_SettingEx()
    {
		Clear();
    }
	void Clear()
	{		
		MJ_Setting::Clear();
		byYaZhuTime = 10;//押注等待时间
	}
};

///麻将时间等待：时间到了服务器代为处理
struct MJ_WaitTimeEx:MJ_WaitTime
{
    MJ_WaitTimeEx()
    {
		Clear();
    }
	void Clear()
	{	
		MJ_WaitTime::Clear();
	}
};

///断线重连数据包(放在最后)
struct GameStationDataEx:GameStationData
{
    int                 m_nWhichTask;
	bool				bShowUserId;					// 是否显示所有玩家的id
	BYTE				m_iArMenPai[4][MEN_CARD_NUM];	// 门牌不超过40张
	JingStruct			m_StrJing;						// 财神信息
	GCPStructEx			m_UserGCPData[4][5];			// 玩家吃碰杠牌
	MJ_SettingEx        m_MjSet;
	BYTE				m_byMenPaiCount[4];		//各个方向门牌数量
	//游戏底分
	int					m_iBaseFen;//游戏底分
	BYTE				m_byMaster;//桌主
	bool				m_bReBese[PLAY_COUNT];//是否回应了基础分设置
	GameStationDataEx()
	{		
		Init();
	};
	void Init()
	{
		memset(this, 0, sizeof(GameStationDataEx));	
		m_StrJing.Init();
		memset(m_byArHandPai,255,sizeof(m_byArHandPai));
		memset(m_byArOutPai,255,sizeof(m_byArOutPai));
		memset(m_iArMenPai,255,sizeof(m_iArMenPai));
		memset(m_UserGCPData,255,sizeof(m_UserGCPData));
		m_MjSet.Clear();
		memset(m_byMenPaiCount,26,sizeof(m_byMenPaiCount));
		m_iBaseFen = 0;//游戏底分
		m_byMaster = 255;//桌主
		memset(m_bReBese,0,sizeof(m_bReBese));//是否回应了基础分设置
	}
};


#pragma pack(pop)
