#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
#include "CommonDefine.h"
#include "../common/IGameFrame.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "UI_ForLogic/iuserinterface.h"
#include "Common/UIMessageDefines.h"
using namespace std;
using namespace irrklang;

#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\

#define SOUND_GAME_BG               0xff                 //背景音效

/////定时器ID/////////////////////////////////////////////////////
#define TIMER_TIMETOREADY			101			//准备定时器ID
#define TIMER_SHOWSINGLECARD		102			//显示单张牌定时器ID
#define TIMER_SHOWFAILINFO			103			//显示出牌失败信息
#define TIMER_TRUN_HIDE			    104			//查看轮牌数据定时器
#define TIMER_AUTO					105         //托管出牌

interface IGameFrame;
interface IUserInterface;
class CNewGameClient : public IGameImpl
{
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);
public: 
	CUpGradeGameLogic   m_cLogic ;             ///客户端逻辑
private:
	///框架游戏指针
	IGameFrame         *m_pGameFrame;
	///界面游戏指针
	IUserInterface     *m_pUI;
	ISoundEngine       *m_SoundEngine;				 ///游戏音效
	ISoundEngine       *m_SoundEnginBg ;             ///背景音效
	BYTE				m_bGameStation;							///< 游戏状态
	int					m_timeToGetReady;			//准备时间
	int					m_timeToThink;				//思考时间
	int					m_iSendCardSpeed;			//发牌速度
	bool				m_bIsSuper;					//是否为超端
	bool				m_isAuto;					//是否托管
	bool				m_isFirstOutCard;			//是否第一个出牌
	int					m_tipIndex;					//提示计数
	BYTE				m_byCards[33];				//牌数据
	vector<BYTE>		m_vbyCards;					//牌数据
	vector<BYTE>		m_vbyBigJor;				//大王
	vector<BYTE>		m_vbySmallJor;				//小王
	vector<BYTE>		m_vbyLevelCard;				//级牌
	BYTE				m_byBackCard[8];			//底牌
	BYTE				m_byGameLevel[2];			//级牌
	BYTE				m_byLevel;				//游戏级别
	BYTE				m_byGameLevelColor;			//级牌花色
	BYTE				m_byOldCard[33];			//提示临时存储
	TFirstData			m_tFirstPlayerCard;			//一圈中第一个出牌的玩家出的牌
	T_CS_OutCard		m_tCurrentOutCard;			//当前出的牌
	BYTE				m_byCurrentOutCardPlayer;	//当前出牌的玩家座位号
	int					m_CardCounts;				//25张要显示的牌
	IOperateCard	   *m_pIOperateCard;			//可操作牌指针
	INoMoveCard		   *m_pINoMoveCard;				//不可操作牌指针
	IText			   *m_pITextFailInfo;			//出牌失败文字
	IText			   *m_PITextScore;				//得分
	//超端牌数据
	TOtherCards			m_tOtherCard;				//其他玩家的牌
	bool				m_bIsGetOtherCard;			//是否已获得其他玩家的牌
	bool				m_bSortFuc;					//排序
 public:
    /// 初始化
    virtual int Initial();
    /// 初始化UI
    virtual int InitUI();
    /// 游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
    /// 界面消息
    virtual int OnUIMessage(TUIMessage* pMessage);
    /// 重新设置状态
    virtual int ResetGameFrame(void);
    /// 玩家进入
    virtual int GameUserCome(void);
    /// 玩家离开
    virtual int GameUserLeft(int nIndex);
    /// 玩家断线
    virtual int GameUserOffLine(void);
    // 是否正在游戏，由游戏实现
    // 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
    virtual bool IsPlayingGame();          
    /// 定时器消息
    virtual void OnTimer(int nTimerID);
    /// 置状态
    virtual void SetStationParameter(BYTE bGameStation);
    ///设置游戏状态
	virtual BYTE GetStationParameter();
	///游戏结束
	virtual bool OnGameQuiting();
public:
	//处理发牌消息
	void OnReceiveSendCard(void *buffer);
	//处理出牌消息
	void OnReceiveOutCard(void *buffer);
	//处理配置消息
	void OnReceiveConfig(void *buffer);
	//处理结束消息
	void OnReceiveFinish(void *buffer);
	//处理准备消息
	void OnReceiveReady(void *buffer);
	//处理埋底牌完成消息
	void OnReceiveBackCardFinish(void *buffer);
	//接受新的一轮消息
	void OnReceiveNewTurn(void *buffer);
	//接受发牌完成消息
	void OnReceiveSendFinish(void *buffer);
	//接受埋牌消息
	void OnReceiveBackCard(void *buffer);
	//接受得分消息
	void OnReceiveScore(void *buffer);
	//提示出牌
	void OnTipOutCard();
	//游戏状态处理
	void GameStationProc(TGameStation& tGameStation);
	//界面鼠标点击事件
	void OnUIClicked(UINT32 ControlID);
	//显示准备图片的座位号转视图号显示
	void ShowGetReadyPic(BYTE byDeskStaion,bool bFlag);
	//发送准备,或退出游戏消息消息	 , isReadyOrQuit - false： 退出游戏； true:准备好
	void ReadyOrQuit(bool bReadyOrQuit);
	//播放定时器
	void SetTimerVisibleAndPlay(const BYTE byDeskStation,bool bFlag,BYTE byMaxTime,BYTE byMinTime);
	//显示现在的等级
	void ShowCurrentGameLevel(TGameLevel *pTGameLevel);
	//亮主
	void ShowLevelCard(BYTE *pbyCards,int iCount);
	//叫主
	void CallLevelCard(BYTE byLevelCardColor);
	//获得升起牌的数量
	int GetUpNums(const int iControlID);
	//获取升起的牌
	void GetUpCard(const int iControlID,BYTE bCardList[]);
	//设置静态文本控件的文本内容
	void SetTextContent(const int iControlID,WCHAR *pwText);
	//打印牌数据
	void PirntCards();
	//重置游戏状态
	void ResetGameStatus();
protected:
	void SetControlVisible(const int iControlID,bool bFlag)
	{
		IBCInterFace *pControl = nullptr;
		pControl = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(iControlID));
		if(pControl != nullptr)
		{
			pControl->SetControlVisible(bFlag);
		}
	}
	void SetBtEnable(const int iButtonID, bool bFlag);
};