#include "IGameImpl.h"
#include "UI_ForLogic/IUserInterface.h"

#include "..\ChangeFile\UserDataEx.h"
#include "..\ChangeFile\GameDataEx.h"
#include "sounddefines.h"

#include <irrKlang.h>
using namespace irrklang;
//#include "SoundsDef.h"
#include <queue>;

interface IGameFrame;
interface IUserInterface;

const BYTE MY_VIEWSTATION = 4;				// 自己的视图位置总是4

#define     BN_VOLUM_UP            13      ///< 音量加
#define     BN_VOLUM_DOWN          14      ///< 音量减
#define     BN_VOLUM_OFF           15      ///< 音乐关
#define     BN_VOLUM_ON            16      ///< 音乐开



#define TIME_OUT_CARD			10		//出牌计时器
#define TIME_BLOCK_CARD			11		//拦牌计时器
#define TIME_ZHUAPAI			12		// 抓牌计时器
#define TIME_HIDE_SEZI_ANI		13      // 色子动画播放后，隐藏它的定时器
#define TIME_BEGIN				14      // 自动开始
#define TIME_FAPAI				15      // 发牌定时器
#define TIME_DING_QUE			16      // 定缺门定时器
#define ID_WARMING_TIME			17		// 警告定时器

#define TIME_FENG_YU			20      // 刮风下雨对话框停留时间
#define TIME_SHOW_HU			21      // 显示糊牌对话框定时器


#define TIME_TALKING_0			25		// 玩家0聊天显示计时器
#define TIME_TALKING_1			26		// 玩家1聊天显示计时器
#define TIME_TALKING_2			27		// 玩家2聊天显示计时器
#define TIME_TALKING_3			28		// 玩家3聊天显示计时器


struct MjUiInfo
{
    int         m_nUserHandInfo[20];
    int         m_nHandCardCount;

    int         m_nUserOutInfo[OUT_CARD_NUM];
    int         m_nOutCardCount;

	bool        m_bIsBigGang[5];//是否大杠
    BYTE        m_ucCPGData[5][4];
    BYTE        m_ucSpeCPGData[5][4];//特殊杠数据
    BYTE        m_ucSpeCPGNum[5][4];//特殊杠牌的数量
    BYTE        m_ucChair[5];
    BYTE        m_ucCPGTile[5];
    MJ_LOGIC_TYPE m_CPGtype[5];
    int         m_nCPGCount;
    
    MjUiInfo()
    {
        memset(this, 0, sizeof(MjUiInfo));
    }
    void ClearHand()
    {
        memset(m_nUserHandInfo, 0, sizeof(m_nUserHandInfo));
        m_nHandCardCount = 0;
    }
    void GetFromByte(BYTE ucByteInfo[])
    {
        ClearHand();
        for (int i=0; i<17; ++i)
        {
            if (0==ucByteInfo[i] || 255==ucByteInfo[i])
            {
                break;
            }
            m_nHandCardCount++;
            m_nUserHandInfo[i] = ucByteInfo[i];
        }
    }
    void FillWithBack(int nNum)
    {
        memset(m_nUserHandInfo, 0, sizeof(m_nUserHandInfo));
        m_nHandCardCount = nNum;
    }
    void ClearOut()
    {
        memset(m_nUserOutInfo, 0, sizeof(m_nUserOutInfo));
        m_nOutCardCount = 0;
    }
    void GetOutFromByte(BYTE ucByteInfo[])
    {
        ClearOut();
        for (int i=0; i<OUT_CARD_NUM; ++i)
        {
            if (0==ucByteInfo[i] || 255==ucByteInfo[i])
            {
                break;
            }
            m_nUserOutInfo[i] = ucByteInfo[i];
			m_nOutCardCount++;
        }
    }
    void ClearCPG()
    {
        memset(m_bIsBigGang, 0, sizeof(m_bIsBigGang));//是否大杠
        memset(m_ucSpeCPGData, 0, sizeof(m_ucSpeCPGData));//特殊杠数据
        memset(m_ucSpeCPGNum, 0, sizeof(m_ucSpeCPGNum));//特殊杠牌的数量
        memset(m_ucCPGData, 0, sizeof(m_ucCPGData));
        memset(m_ucChair, 0, sizeof(m_ucChair));
        memset(m_ucCPGTile, 0, sizeof(m_ucCPGTile));
        memset(m_CPGtype, LT_EMPTY, sizeof(m_CPGtype));
        m_nCPGCount = 0;
    }
    void GetFromCPGData(GCPStructEx cpgstr[],bool show=true)
    {
        ClearCPG();
		int index = 0;
        for (int i=0; i<5; ++i)
        {
			if(cpgstr[i].byType == 255)
				continue;
			if(cpgstr[i].bIsBigGang)
			{
				if (-1==cpgstr[i].byBigGang[0][0] || 255==cpgstr[i].byBigGang[0][0] || 0==cpgstr[i].byBigGang[0][0])
				{
					break;
				}
				for (int j=0; j<4; ++j)
				{
					m_ucSpeCPGData[i][j] = cpgstr[i].byBigGang[j][0];	
					m_ucSpeCPGNum[i][j]	 = cpgstr[i].byBigGang[j][1];	//每张牌的数量
				} 
				m_bIsBigGang[i] = true;//标志大杠
			}
			else
			{
				if (-1==cpgstr[i].byData[0] || 255==cpgstr[i].byData[0] || 0==cpgstr[i].byData[0])
				{
					break;
				}
				for (int j=0; j<4; ++j)
				{
					m_ucCPGData[i][j] = cpgstr[i].byData[j];                
				} 
			}
            m_ucChair[i] = cpgstr[i].iBeStation;
            m_ucCPGTile[i] = cpgstr[i].iOutpai;
            m_nCPGCount ++;

            if (ACTION_CHI==cpgstr[i].byType)
            {
                m_CPGtype[i] = LT_COLLECT;
            }
            else if (ACTION_PENG==cpgstr[i].byType)
            {
                m_CPGtype[i] = LT_TRIPLET;
            }
            else if (ACTION_AN_GANG==cpgstr[i].byType)
            {
                for (int j=0; j<3; ++j)
                {
                    m_ucCPGData[i][j] = 0;
                }
				if(!show)
					m_ucCPGData[i][3] = 0;
                m_CPGtype[i] = LT_QUADRUPLET_CONCEALED;
            }
            else if (ACTION_BU_GANG==cpgstr[i].byType || ACTION_MING_GANG==cpgstr[i].byType)
            {
                m_CPGtype[i] = LT_QUADRUPLET_REVEALED;
            }
        }
    }
};


class CNewGameClient : public IGameImpl
{

public:

    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);

    virtual ~CNewGameClient();

private:

    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);

public:

#define SAFE_CTRL_OPERATE(CTRL_TYPE,CTRL_ID, OPERATE)\
	{\
	CTRL_TYPE* PRE=NULL; \
	PRE = dynamic_cast<CTRL_TYPE *>(m_pUI->GetIControlbyID(CTRL_ID));\
	if (PRE!=NULL) PRE->OPERATE; \
	}\

/*-----------------------------------------------------------------------------------------------------------
	一系列平台提供的需要重载来驱动游戏的函数，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
  ----------------------------------------------------------------------------------------------------------*/
public:
    //初始化
    virtual int Initial();
    //初始化UI
    virtual int InitUI();
    //游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
    //界面消息
    virtual int OnUIMessage(TUIMessage* pMessage);
    //
    virtual int ResetGameFrame(void);
    //玩家进入
    virtual int GameUserCome(void);
    //玩家离开
    virtual int GameUserLeft(int nIndex);
    //玩家断线
    virtual int GameUserOffLine(void);
    //是否正在游戏，由游戏实现
    //旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
    virtual bool IsPlayingGame();
    //定时器消息
    virtual void OnTimer(int nTimerID);
    //置状态
    virtual void SetStationParameter(BYTE bGameStation);
	//设置游戏状态，主要是断线重连来后处理的消息
    virtual void SetGameStation(void * pBuffer,int nLen);
	//退出游戏
	virtual bool OnGameQuiting();
	//获取游戏状态
	BYTE GetStationParameter();

	///重新开局时的初始化
	void ReStartGameInitUI();
/*-----------------------------------------------------------------------------------------------------------
	所有逻辑处理模块
  ----------------------------------------------------------------------------------------------------------*/

public:

	BYTE		 m_byShowFaPai;


private:

	//游戏框架
    IGameFrame* m_pGameFrame;
	//游戏UI
    IUserInterface* m_pUI;
	//背景音乐引擎
	ISoundEngine* m_pUIBkMusic;
	//游戏音效引擎
	ISoundEngine* m_pUIGameSound;


	//游戏状态
	BYTE m_bGameStation;
	//游戏背景音乐声效开关
	bool m_bBgMusic;
	//游戏背景音乐
	bool m_bBkMusic;
	//游戏音效开关
	bool m_bGameSound;
	//该客户端是否允许被旁观
	bool m_bAllowWatch;


    ///游戏设置
    MJ_Setting      m_MJSetting;
	///玩家的数据：手牌，出牌，吃碰杠牌等
	UserDataEx      m_UserData;	
	GameDataEx      m_GameData;	

    BYTE            m_byNtLogicPos; //庄家
    BYTE            m_ucJinPai[2];  //精牌
    BYTE            m_ucCurrentPlayer;  //当值玩家
    BYTE            m_ucLastTile;       //上次出的牌
    BYTE            m_ucHandState;      //手牌状态 0表无状态 1表吃牌选择状态
	BYTE			m_nEnableTiles[MAX_TILES_INCTRL];//可以杠的牌

    MjUiInfo        m_UserDataCard[PLAY_COUNT];
	
	///一些游戏中可能需要用到的个人信息
	int             m_nDeskStation;    ///<自己在服务器上的坐位号
    int             m_nWhichTask;       /// 每局任务

    tagTwoSeziJingEx  m_tagJinPai;		
    tagTwoSeziGetPaiEx m_tagSeziGetPai;	
    tagTwoSeziNtEx    m_tagSeziNt;		
    tagTwoSeziDirEx   m_tagSeziDir;		
    tagSendPaiEx      m_tagSendpai;		
    tagCountFenEx     m_tagCountFen;	
	
	
    int               m_nTimeOutCnt;
    bool              m_bIsTuoGuan[PLAY_COUNT];

    ////////////////////////////////////////////////////////////////////////////////
    // 发牌动画用
    BYTE            m_ucOpenDoor;
    BYTE            m_ucOpenDun;
    int             m_nStep;
    ////////////////////////////////////////////////////////////////////////////////
    // 抓牌动画用
    int             m_nZhuaViewStaiton;
    ////////////////////////////////////////////////////////////////////////////////
    // 翻精动画用
    int             m_nFanJinState;     // 0上精从牌墙到中间，1上精从中间到左上角，2下精从牌墙到中间，3下精从中间回牌牌，4下上精从左上回牌墙，5翻精的小手
    POINT           m_ptShangJin;
    POINT           m_ptXiaJin;
    POINT           m_ptCenter;
    POINT           m_ptJinLeftTop;
    int             m_nHandIndex;       // 小手在哪一墩
    int             m_nHandPos;         // 小手在哪一家
    int             m_nJinWallIndex;    // 精牌位置（墩） = -1;
    int             m_nJinDoor;         // 精牌位置= -1;

	int				m_GangPai[2][4];
	BYTE			m_GangType[2];// 杠牌的种类
	bool			m_bIsBigGang[2];//是否大杠
	BYTE			m_byCheckPai;//选择的杠牌

    int				m_nInvalidTiles[MAX_TILES_INCTRL];//当前不能出的牌
	bool			m_bStopSendPai;//发牌是否结束
	int				m_iFinshiX;//结算框初始位置
	int				m_iFinshiY;//结算框初始位置

	BOOL			m_bSound;                   ///是否开启音效
	int				m_TimeClockCount;           ///闹钟倒计时时长
	BYTE			m_bTimeOutCount;            ///超时次数
	int				m_iSendCardCount;//当前发牌数
	int				m_iTimeWarm;	//警告计时器时间
	wchar_t			m_wszNiCheng[PLAY_COUNT][256];//游戏中保存玩家的昵称

	
public:


    // 用户托管
    void OnUserTuoguan(void *buffer, int nLen);
    // 用户同意
    void OnUserAgree(void *buffer, int len);

    // 游戏开始
    void OnGameStart(void *buffer, int len);
	/// 第0位玩家为庄家消息:第一局
	void OnDongNt(void *buffer, int nLen);
    // 定起牌点数
    void OnSeziSureOpenPoint(void *buffer, int nLen);
    // 发送手牌数据
    // 注意此时没有牌数据，跳牌后才有
    void OnSendAllCard(void *buffer, int nLen);
    // 游戏开始打牌了
    void OnGamePlay(void *buffer, int nLen);


    // 出牌
    void OnNotifyOutCard(void *buffer, int nLen);
    // 抓牌
    void OnNotifyZhuaPai(void *buffer, int nLen);
    // 拦牌
    void OnNotifyBlock(void *buffer, int nLen);
    // 吃牌
    void OnNotifyChiPai(void *buffer, int nLen);
    // 碰牌
    void OnNotifyPengPai(void *buffer, int nLen);
    // 杠牌
    void OnNotifyGangPai(void *buffer, int nLen);


    // 胡牌
    void OnNotifyHuPai(void *buffer, int nLen);    
    // 放弃
    void OnNotifyCancelBlock(void *buffer, int nLen);
    // 结算
    void OnNotifyFinish(void *buffer, int nLen);
	 /// 强退结束游戏
	void OnForceFinish( void *buffer, int nLen);

	/// 定缺事件
    void OnNotifyDingQue(void *buffer, int nLen);  
	/// 发牌器发牌
	void OnNotifySuperSetPai(void *buffer, int nLen);			
    // 显示结算框
    void UIShowResultDlg(tagCountFenEx * fen , tagHuPaiEx * hu ,BYTE type);

    // 请求出牌
    void RequestOutCard(BYTE ucTile);
    // 请求吃牌
    void RequestBlockChi();
    // 请求碰牌
    void RequestBlockPeng();
    // 请求杠牌
    void RequestBlockGang();
    // 请求胡牌
    void RequestBlockHu();
    // 放弃拦牌
    void RequestCancelBlock();
    // 请求托管
    void RequestTuoGuan(bool bTuoguan = true);
	//吃杠请求
	void RequestBlockChiGang(UINT32 id);
//////////////////////////////////////////////////////////////////////

    // 显示城墙
    void UIShowWall(BYTE bViewStation, bool bShow = true);
    // 显示城墙数据
    // 数据格式：255表没有牌， 0表牌背，其它与手牌一样
    void UIShowWall(BYTE station, BYTE ucArrMenPai1[]);
    // 显示庄家标志
    void UIShowZhuang(BYTE bViewStation, bool bShow = true);
    // 设置手牌数据
    void UISetHandCard(BYTE bViewStation, int nHandInfo[], int nCount, bool bShowGive = false);
    // 设置出牌
    void UISetOutCard(BYTE bViewStation, int nOutInfo[], int nCount, bool bShowGive = false);
///////

    // 骰子动画
    void UIShowsaizi(BYTE bViewStation, int nPoint1, int nPoint2);
    // 出牌提示
    void UIShowLastTileTip(BYTE bViewStation, bool bShow,BYTE pai=255);
    // 显示杠牌控件
    void UIShowGangCtrl(bool bShow,int pai[] = NULL,int pai2[]=NULL);
	// 显示吃杠牌控件
	void UIShowChiGangCtrl(bool bShow,bool chi);
////////

   // @brief 显示准备
    void UIShowReady(BYTE bViewStation, bool bShow );
    //@brief 显示托管
    void UIShowTrustee(bool bShow);
    //@brief 显示等待动画
    void UIShowWaiting(bool bShow);
	///显示封顶
	void UIShowFengDing();
    // @brief 显示移动动画控件
    void UIShowZhuaMoveAction(POINT ptStart, POINT ptEnd, BYTE ucViewStatin, bool bShow);
    // 显示缺门
    void UIShowQueMen(bool show,BYTE station = 255);
	//显示刮风下雨对话框
	void ShowGFXY(bool show,BYTE station = 255,BYTE bestation = 255,BYTE type = 255);

    // 请求出牌                
    void OnUIRequestOutCard(UINT32 uControlID,bool outtime = false);
    // 请求拦牌
    void OnRequestBlock(UINT32 uControlID);
    // 当麻将控件不可用时，鼠标移动到控件上
    int OnMoveOnMjCtrl(TUIMessage* pMessage);
    // 当麻将控件不可用时，鼠标单击控件
    int OnLButtonOnMjCtrl(TUIMessage* pMessage);
    // 动画控件消息
    int OnCartoonFinish(TUIMessage *pMessage);



public:


	//显示开始出牌状态界面
	void UIshowBeginOut();
	//播放动画
	void PlayAni(int Id);
	//保存玩家的的昵称
	void GetUserName();

	//设置计时器
	void OnSetTimer(BYTE station,int type);
	//删除所有时间
	void OnKillAllTimer();
	/// 消除定时器
	BOOL KillGameTimer(int TimerID);
    // 设置定时器
    void UIShowTimer(BYTE bViewStation, int nTime, bool bShow = true);



	//发送游戏数据:发送通知消息，不包含数据
	void SendGameDataEx(BYTE id);
	//发送游戏数据
	void SendGameDataEx(BYTE id,void *pData,UINT uSize);
	//闪烁窗口
	void FlashWindow();
	//玩家聊天发话UI渲染
	void ShowTalk(BYTE bDeskstation,wchar_t* pwszText,int iDispMillSeconds/* = 5000*/);
	//玩家聊天发话UI渲染结束
	void ShowTalkFinish(BYTE bViewstation);


	//将字符串转换成牌数据
	void GetPaiFormChar(int &count ,BYTE data[],const wchar_t pszPai[],int type);
	//将两个字符转换成牌
	BYTE ChangeChar(wchar_t data1,wchar_t data2);
	//获取控件数据,并发送配牌数据
	void SendSuperData();
	//显示或者隐藏超级客户端发牌
	void ShowSuperSetPai(bool show);


	//获取游戏状态
	BYTE GetGameStationEx(){return m_bGameStation;};
	//设置游戏状态
	void SetGameStationEx(BYTE state)
	{
		m_bGameStation = state;
	SetStationParameter(state);};

	//点击“房间配置”后的响应
	void OnClickedConfig();
	//点击“房间配置”确定按钮后的响应
	void OnClickedConfigOK();
	//点击“房间配置”取消按钮后的响应
	void OnClickedConfigCancel();
	//加载游戏配置
	void LoadGameConfig();
	//保存游戏配置
	void SaveGameConfig();
	//初始化数据
	void InitGameDate();

// 声音
private:
#define MAX_SOUND_RAND 4    // 有几种不一样的声音
    struct SOUND_INFO
    {
        TCHAR szFileName[MAX_SOUND_RAND][MAX_PATH];
        int nCount;       
        SOUND_INFO()
        {
            memset(szFileName, 0, sizeof(szFileName));
            nCount = 0;
        }
        SOUND_INFO(const SOUND_INFO &other)
        {
            memcpy(szFileName, other.szFileName, sizeof(szFileName));
            nCount = other.nCount;
        }
        SOUND_INFO &operator = (const SOUND_INFO &other)
        {
            if (&other==this)
            {
                return *this;
            }
            memcpy(szFileName, other.szFileName, sizeof(szFileName));
            nCount = other.nCount;
            return *this;
        }
    };
    std::map<BYTE,SOUND_INFO>           m_SoundMap;
private:
    /** @brief 从配置文件中读取声音文件名
     */
    void GetSoundMapFromCfg();
    /** @brief 播放出牌声音
    */
    void PlayTileSound(int nDeskStation,BYTE ucTile);
    ///**
    //* @brief 播放普通声音（不含方言）
    //*/
    void PlayNormalSound(const char *pszFileName);
	//播放游戏背景
	void PlayBkMusic(bool bOn);


};