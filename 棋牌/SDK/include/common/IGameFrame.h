#pragma once
#include "CommonDefine.h"

/// 游戏使用的奖励类型
struct AwardTypeForGame
{
    TCHAR PropName[32]; //道具名称
    int nPropCount;     //道具个数
    int nGold;          //金币数
    int nPropType;      //道具ID号
};

/// 房间信息
struct GameInfoForGame
{
    int iPower;     // 倍率
};

/// 用户信息
struct UserInfoForGame
{
    TCHAR			szNickName[61];										///<昵称  
    __int64			i64Money;												///用户金币								    						    
    long int		dwUserID;							                ///ID 号码
	ULONG			dwUserIP;							                ///登录IP地址
	UINT			bLogoID;							                ///头像 ID 号码
	bool			bBoy;								                ///性别
	bool            bIsRobot;                                           ///是否机器人
	BYTE			bGameMaster;						                ///管理等级

	BYTE						bDeskNO;							///游戏桌号
	BYTE						bDeskStation;						///桌子位置
	char						szCity[61];							///所在城市
};


class CGlobal;
class IGameFrame
{
public:
    /// 发消息
    virtual bool SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode) = 0;
    ///游戏从框架中获取的信息
    virtual int GetMyDeskStation()=0;
    ///获得自己的桌子号
    virtual int GetMyDeskNum()=0;
    ///这里传的是视图索引
    virtual int GetUserStation(int nIndex)=0;  
    ///将消息内容发送到游戏服务器
    virtual int SendGameData(WORD wSubId,void *buffer, int nLen)=0;
    ///只将消息发送到客户端大厅
    virtual int SendIpcData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)=0;
    ///视图转服务器坐位 因为所有游戏都要用到，所以在框架中处理
    virtual int ViewStation2DeskStation(int nViewIndex)=0;
    ///坐位号转视图号
    virtual int DeskStation2View(int nDeskStation)=0;
    ///获取玩家昵称
    virtual TCHAR * GetUserNickName(int nDeskStation)=0;
    ///关闭游戏客户端
    virtual void CloseClient()=0;
    ///点击继续
    virtual int ClickContinue() = 0;
    ///强制关闭游戏
    virtual void CloseClientforce()=0;
    ///设置时钟
    virtual void SetTimer(int nEventID, DWORD dwTime) = 0;
    ///关闭时钟
    virtual void KillTimer(int nEventID) = 0;
    ///插入一条系统消息
    virtual int InsertSystemMessage(TCHAR* szMessage)=0;
    /// 插入普通消息
    virtual int InsertNormalMessage(TCHAR* szMessage)=0;
    ///获取奖励类型
    virtual bool GetAwardType(AwardTypeForGame &Type)=0;	
    ///播放奖励动画
    virtual bool PlayAwardAnimation(const AwardTypeForGame &type)=0;
    ///获取倍率
    virtual bool GetGameBaseInfo(GameInfoForGame &Info)=0;
    ///获取用户信息
    virtual bool GetUserInfo(int nDeskStation, UserInfoForGame &Info)=0;
    /// 播放声音
    virtual bool PlaySound(const char *szFileName)=0;
    ///获取游戏音量
    virtual int GetVolume()=0; 
    /// 某方位是否使用视频
    virtual bool SetUseVideo(int nViewStation, bool bUse)=0;
    /// UI获取是否为旁观状态
    virtual bool GetIsWatching()=0;
    /// 获取是否允许旁观
    virtual bool GetIsEnableWatch()=0;
    /// 从游戏界面中打开银行界面
    virtual void OnVisitBank()=0;
    /// 获取全局信息
    virtual CGlobal* GetGlb()=0;
    /** @brief 使用小头像
     */
    virtual bool SetUseSmallLogo(int nViewStation, bool bUseSmall=true)=0;
    /** @brief 获取窗口大小
     */
    virtual RECT GetGameWindowRect()=0;

	/// 获取游戏信息2011-7-7
	virtual const GameInfoStructInExe* GetGameInfo()=0;

	 //状态改变
    virtual void SetStationParameter(BYTE bGameStation)=0;
};