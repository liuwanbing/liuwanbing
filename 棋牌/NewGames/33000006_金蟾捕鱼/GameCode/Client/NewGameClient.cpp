#include "stdafx.h"
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
//#include "UI8_ControlsDefine.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"
#include "define.h"
#include <windows.h>

//#include "LoveSendClassInExe.h"


IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}
/*------------------------------------------------------------------------------*/
CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_GameScreen.SetGameClientP(this);
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_bGameStation = GS_WAIT_SETGAME;           //游戏状态
}
/*------------------------------------------------------------------------------*/
CNewGameClient::~CNewGameClient()
{

}
/*------------------------------------------------------------------------------*/
//游戏退出
bool CNewGameClient::OnGameQuiting()
{
	return true;
}

//加载配制文件

/*------------------------------------------------------------------------------*/
/// 初始化
int		CNewGameClient::Initial()
{
	LoadGameSet();
	//因为 gamelauncher 生成的窗体没这么大 所以这里强制生成 CLIENT_WIDTH*CLIENT_HEIGHT的窗体 

	RECT lpSrcWindow;
	if(GetWindowRect(m_pUI->GetWindowsHwnd(), &lpSrcWindow))
	{
		int iMoveX = (CLIENT_WIDTH-1024)/2;
		int iMoveY = (CLIENT_HEIGHT-738)/2;
		CWnd::FromHandle(m_pUI->GetWindowsHwnd())->MoveWindow(lpSrcWindow.left-iMoveX,lpSrcWindow.top-iMoveY,CLIENT_WIDTH,CLIENT_HEIGHT);
	}

	InitCommonControls();
	
    //载入创建游戏视图界面
	if(m_GameScreen.GetSafeHwnd()==NULL)
	{
		m_GameScreen.Create(AfxRegisterWndClass(0,NULL,NULL,NULL),NULL,WS_CHILD,CRect(0,0,CLIENT_WIDTH,CLIENT_HEIGHT),CWnd::FromHandle(m_pUI->GetWindowsHwnd()),0);
		
		if(!m_GameScreen.InitDxDlog())
		{
			AfxMessageBox("设备创建失败，程序即将退出。");
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
			return TRUE;  
		}
	    
		m_GameScreen.MoveWindow(0,0,CLIENT_WIDTH,CLIENT_HEIGHT);
		m_GameScreen.ShowWindow(SW_SHOW);
		m_GameScreen.SetWindowText("金蝉捕鱼客户端");
	}
	return 0;
}
/*------------------------------------------------------------------------------*/
/// 初始化UI
int CNewGameClient::InitUI()
{
	return 0;
}
/*------------------------------------------------------------------------------*/
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:           ///<设置状态
		{			    
			SetGameStation(buffer,nLen);
			break;
		}
	case ASS_S_USER_COME:		//玩家进入/离开消息
		{
			if(nLen != sizeof(GameUserComeData))
			{
				return TRUE;
			}

			GameUserComeData *pGameUserCome = (GameUserComeData *)buffer;
			if(NULL == pGameUserCome)
			{
				return TRUE;
			}
			//玩家进入
			m_GameScreen.UserComeInorLeave(SwitchChairID(pGameUserCome->byDeskStation),pGameUserCome->bComeOrLeave);
			//玩家的上分数
			m_GameScreen.SetUserScore(SwitchChairID(pGameUserCome->byDeskStation),pGameUserCome->iScore);
			//玩家的炮火倍数
			m_GameScreen.SetBeiLv(SwitchChairID(pGameUserCome->byDeskStation),pGameUserCome->iFireBase);

			break;
		}
	case ASS_BUILD_TRACE:	//生成鱼儿消息
		{
			if(nLen != sizeof(CMD_S_FishTrace))
			{
				return TRUE;
			}

			CMD_S_FishTrace *pFishTrace = (CMD_S_FishTrace *)buffer;
			if(NULL == pFishTrace)
			{
				return TRUE;
			}
			for(int i=0;i<5;i++)
			{
				//循环添加鱼儿轨迹
				m_GameScreen.AddFish(pFishTrace->m_fishtrace[i].x,pFishTrace->m_fishtrace[i].y,pFishTrace->m_fishtrace[i].rotation,pFishTrace->m_fishtrace[i].movetime,pFishTrace->m_fishtrace[i].changetime,pFishTrace->m_fishtrace[0].m_fishtype,pFishTrace->m_fishtrace[0].m_ptindex,i,pFishTrace->m_fishtrace[0].m_fishid,pFishTrace->m_fishtrace[0].m_fudaifishtype,pFishTrace->m_fishtrace[i].m_speed);
			}
			break;
		}
	case ASS_CHANGE_SCENCE:		//切换场景
		{
			if(nLen != sizeof(CMD_S_ChangeScene))
			{
				return TRUE;
			}
			CMD_S_ChangeScene *pChangeScene=(CMD_S_ChangeScene *)buffer;
			if(NULL == pChangeScene)
			{
				return TRUE;
			}
			m_GameScreen.ChangeScreen(pChangeScene->SceneIndex);
			break;
		}
	case ASS_FISH_REG:		//鱼阵
		{
			if(nLen != sizeof(CMD_S_FishTrace))
			{
				return TRUE;
			}

			CMD_S_FishTrace *pFishTrace = (CMD_S_FishTrace *)buffer;
			if(NULL == pFishTrace)
			{
				return TRUE;
			}
			for(int i=0;i<5;i++)
			{
				//循环添加鱼儿轨迹
				m_GameScreen.AddFish(pFishTrace->m_fishtrace[i].x,pFishTrace->m_fishtrace[i].y,pFishTrace->m_fishtrace[i].rotation,pFishTrace->m_fishtrace[i].movetime,pFishTrace->m_fishtrace[i].changetime,pFishTrace->m_fishtrace[0].m_fishtype,pFishTrace->m_fishtrace[0].m_ptindex,i,pFishTrace->m_fishtrace[0].m_fishid,pFishTrace->m_fishtrace[0].m_fudaifishtype,pFishTrace->m_fishtrace[i].m_speed);
			}
			break;
		}
	case ASS_S_BUY_BULLET:		//玩家上下分结果
		{
			CDealUserBuyBullet(buffer, nLen);
			break;
		}
	case ASS_S_USER_SHOT:		//玩家发射子弹消息
		{
			CDealUserShot(buffer, nLen);
			break;
		}
	case ASS_S_USER_SHOT_ERROR:	//子弹发射错误
		{
			CDealUserShotError(buffer, nLen);
			break;
		}
	case ASS_S_ADD_FIRE:		//玩家加减炮火倍数
		{
			CDealUserAddFire(buffer, nLen);
			break;
		}
	case ASS_S_HIT_FISH:		//击中鱼儿
		{
			CDealUserHitFish(buffer, nLen);
			break;
		}
	case ASS_S_CANCEL_SUPER:	//取消超级炮弹
		{
			CDealCanCelSuperPao(buffer, nLen);
			break;
		}
	case ASS_S_CATCH_FISH_BROAD:	//捕获大鱼广播
		{
			CDealCatchBroadCast(buffer, nLen);
			break;
		}
	
	}
	return TRUE;
}
/*------------------------------------------------------------------------------*/
///设置游戏不同状态时的数据内容，主要用于断线重连
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	if(nLen != sizeof(GameStation_Base))
	{
		return ;
	}

	GameStation_Base *pGameStation = (GameStation_Base *) pBuffer;
	if(NULL == pGameStation)
	{
		return;
	}
	//拷贝游戏数据
	memcpy(&m_GameData, pGameStation ,sizeof(m_GameData));

	//自己的位置
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation();

	//炮火单位
	m_GameScreen.SetCellScore(m_GameData.iFireBase);
	//玩家进入
	m_GameScreen.UserComeInorLeave(SwitchChairID(m_iMyDeskStation),true);
	//个人信息
	CString sNickName = "";
	UserInfoForGame sUser;
	if(m_pGameFrame->GetUserInfo(m_iMyDeskStation,sUser))
	{
		sNickName.Format("%s",sUser.szNickName);
	}
	else
	{
		sNickName.Format("获取玩家信息错误");
		sUser.i64Money = 0;
	}
	m_GameScreen.SetMeInformation(SwitchChairID(m_iMyDeskStation),m_iMyDeskStation,sNickName,sUser.i64Money);
	//自己上下分
	m_GameScreen.SetUserScore(SwitchChairID(m_iMyDeskStation),m_GameData.iUserScore[m_iMyDeskStation]);
	//自己炮火倍率
	m_GameScreen.SetBeiLv(SwitchChairID(m_iMyDeskStation),m_GameData.iUserFireBase[m_iMyDeskStation]);


	//设置炮火倍率
	for(int i=0;i<PLAY_COUNT;i++)
	{   
		if(i == m_iMyDeskStation)
		{
			continue;
		}
		memset(&sUser,0,sizeof(sUser));
		if(m_pGameFrame->GetUserInfo(i,sUser))
		{
			//设置该位置有人
			m_GameScreen.UserComeInorLeave(SwitchChairID(i),true);
			//设置玩家上下分
			m_GameScreen.SetUserScore(SwitchChairID(i),m_GameData.iUserScore[i]);
			//玩家倍率
			m_GameScreen.SetBeiLv(SwitchChairID(i),m_GameData.iUserFireBase[i]);
		}
	}

	//设置是否作为机器人击中鱼的逻辑判断客户端
	m_GameScreen.SetRobotLogic(m_GameData.bHitFlag, m_GameData.bHitDesk);


	//标识收到设置游戏状态消息
	m_GameScreen.m_bHaveLinked = true;
}
/*------------------------------------------------------------------------------*/
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	return 0;
}
/// 
int CNewGameClient::ResetGameFrame(void)
{
	
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome()
{
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	return false;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{

	switch(nTimerID)
	{
	
	case ID_TIME_CLOCK:
		{
			

			break;
		}
	case ID_PLAY_MOVE_VSCARD:
		{
			m_pGameFrame->KillTimer(ID_PLAY_MOVE_VSCARD);
	
		}
		break;
	case ID_TIME_LOOKCARD:
		{
	
		}
		break;
	}
}

/// 设置游戏置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation = bGameStation;
	if (m_pGameFrame)
		m_pGameFrame->SetStationParameter(bGameStation);
}














//-----------------------------------提示相关操作--------------------------------
/*-------------------------------------------------------------------*/
//玩家上下分
void	CNewGameClient::AddOrRemoveScore(bool bAdd,int iScore)
{
	CMD_C_BuyBullet  TBuyBullet;

	TBuyBullet.bAddOrMove = bAdd;
	TBuyBullet.iScore = iScore;
	m_pGameFrame->SendGameData(&TBuyBullet,sizeof(TBuyBullet),MDM_GM_GAME_NOTIFY,ASS_C_BUY_BULLET,0);
}
/*-------------------------------------------------------------------*/
//玩家上下分结果
void	CNewGameClient::CDealUserBuyBullet(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(CMD_S_BulletCount))
	{
		return;
	}

	CMD_S_BulletCount *pBulletCount = (CMD_S_BulletCount *)buffer;
	if(NULL == pBulletCount)
	{
		return;
	}
	//设置玩家上下分
	m_GameScreen.SetUserScore(SwitchChairID(pBulletCount->byDeskStation),pBulletCount->i64UserScore);
  
	//如果是自己上下分 就要刷新自己的金币数
	if(pBulletCount->byDeskStation == m_iMyDeskStation)
	{
		m_GameScreen.SetBaseScore(pBulletCount->i64UserMoney);
	}
}
/*-------------------------------------------------------------------*/
//玩家发射子弹
void	CNewGameClient::OnUserShot(bool bIsSuper,float fRoalation)
{
	CMD_C_UserShoot TUserShoot;
	TUserShoot.fAngle=fRoalation;
	m_pGameFrame->SendGameData(&TUserShoot,sizeof(TUserShoot),MDM_GM_GAME_NOTIFY,ASS_C_USER_SHOT,0);
}
/*-------------------------------------------------------------------*/
//玩家发射子弹消息
void	CNewGameClient::CDealUserShot(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(CMD_S_UserShoot))
	{
		return;
	}

	CMD_S_UserShoot *pUserShoot = (CMD_S_UserShoot *)buffer;
	if(NULL == pUserShoot)
	{
		return;
	}

	//发射炮弹
	if(pUserShoot->byStation != m_iMyDeskStation)
	{
		m_GameScreen.UserShoot(SwitchChairID(pUserShoot->byStation),pUserShoot->fAngle,pUserShoot->byStation,pUserShoot->bIsRobot);
	}
	//设置发射玩家的分数
	m_GameScreen.SetUserScore(SwitchChairID(pUserShoot->byStation),pUserShoot->i64UserScore);
}
/*-------------------------------------------------------------------*/
//玩家发射子弹错误
void	CNewGameClient::CDealUserShotError(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(CMD_S_UserShootError))
	{
		return;
	}

	CMD_S_UserShootError *pUserShootError = (CMD_S_UserShootError *)buffer;
	if(NULL == pUserShootError)
	{
		return;
	}

	//发射炮弹
	if(pUserShootError->byStation == m_iMyDeskStation)
	{
		//设置发射玩家的分数
		m_GameScreen.SetUserScore(SwitchChairID(pUserShootError->byStation),pUserShootError->i64UserScore);
		m_GameScreen.SetBeiLv(SwitchChairID(pUserShootError->byStation),pUserShootError->iFireBase);
	}
	
}
/*-------------------------------------------------------------------*/
//玩家加减火力
void	CNewGameClient::OnUserAddFire(bool bAdd)
{
	CMD_C_AddFireBase TAddFire;
	TAddFire.bAdd = bAdd;
	TAddFire.byDeskStation = m_iMyDeskStation;

	m_pGameFrame->SendGameData(&TAddFire,sizeof(TAddFire),MDM_GM_GAME_NOTIFY,ASS_C_ADD_FIRE,0);
}
/*-------------------------------------------------------------------*/
//玩家加减炮火
void	CNewGameClient::CDealUserAddFire(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(CMD_S_AddFireBase))
	{
		return;
	}

	CMD_S_AddFireBase *pAddFire = (CMD_S_AddFireBase *)buffer;
	if(NULL == pAddFire)
	{
		return;
	}

	m_GameScreen.SetBeiLv(SwitchChairID(pAddFire->byDeskStation),pAddFire->iUserFireBase);
}
/*-------------------------------------------------------------------*/
//击中鱼儿
void	CNewGameClient::OnUserHitFish(int iFishID,int iBulletID,int iShootUSER,bool bIsAndroid)
{
	CMD_C_HitFish THitFish;
	THitFish.iBulletID	= iBulletID;
	THitFish.iFishID	= iFishID;
	THitFish.bIsAndroid = bIsAndroid;
	THitFish.iBulletUserId = iShootUSER;

	m_pGameFrame->SendGameData(&THitFish,sizeof(THitFish),MDM_GM_GAME_NOTIFY,ASS_C_HIT_FISH,0);

}
/*-------------------------------------------------------------------*/
//玩家击中了鱼儿
void	CNewGameClient::CDealUserHitFish(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(CMD_S_CaptureFish))
	{
		return;
	}

	CMD_S_CaptureFish *pHitFish = (CMD_S_CaptureFish *)buffer;
	if(NULL == pHitFish)
	{
		return;
	}


	//设置超级炮弹
	if(pHitFish->bCanSuperPao)
	{
		m_GameScreen.SetSuperPao(SwitchChairID(pHitFish->byStation),pHitFish->iFishID);
	}
	else if(pHitFish->bCanSuperPao==false && pHitFish->lFishScore==0)
	{
		m_GameScreen.CancelSuperPao(SwitchChairID(pHitFish->byStation));
	}

	if(pHitFish->iFishKindScore ==-1)
	{
		m_GameScreen.UserAddMoney(SwitchChairID(pHitFish->byStation),pHitFish->iFishID,pHitFish->lFishScore,1000,true);
		m_GameScreen.SetBomAction(SwitchChairID(pHitFish->byStation),pHitFish->iFishID,pHitFish->lFishScore);
	}
	else
	{
		if(pHitFish->lFishScore==0)
		{
			//m_GameClientView.m_Cocos2dXWin.runclass->AddMoney(SwitchChairID(phitFish->wChairID),phitFish->dwFishID,phitFish->lFishScore,phitFish->FishKindscore,true);
			m_GameScreen.UserAddMoney(SwitchChairID(pHitFish->byStation),pHitFish->iFishID,pHitFish->lFishScore,pHitFish->iFishKindScore,true);
			//分数
		}
		else if(pHitFish->lFishScore>0)
		{
			m_GameScreen.UserAddMoney(SwitchChairID(pHitFish->byStation),pHitFish->iFishID,pHitFish->lFishScore,pHitFish->iFishKindScore,true);
			//分数
			m_GameScreen.SetUserScore(SwitchChairID(pHitFish->byStation),pHitFish->i64UserScore);

			//是否一网打尽
			if(pHitFish->iCatchCount > 0)
			{
				for(int i=0; i<pHitFish->iCatchCount; i++)
				{
					m_GameScreen.UserAddMoney(SwitchChairID(pHitFish->byStation),pHitFish->iCatchFishID[i],pHitFish->iCatchScore[i],pHitFish->iFishKindScore,false,false);
				}
			}
		}
	}

}

/*-------------------------------------------------------------------*/
//取消超级炮弹
void	CNewGameClient::CDealCanCelSuperPao(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(CMD_S_CanCelSuperPao))
	{
		return;
	}

	CMD_S_CanCelSuperPao *pCanCelSuper = (CMD_S_CanCelSuperPao *)buffer;
	if(NULL == pCanCelSuper)
	{
		return;
	}

	//设置超级炮弹
	if(!pCanCelSuper->bCanSuperPao)
	{
		m_GameScreen.CancelSuperPao(SwitchChairID(pCanCelSuper->byStation));
	}
}
/*-------------------------------------------------------------------*/
//捕获大鱼广播
void	CNewGameClient::CDealCatchBroadCast(BYTE * buffer,int nLen)
{
	if(!m_GameScreen.m_configDlg.m_bTipMsg)
	{
		return;
	}
	if(nLen != sizeof(CMD_S_CaptureFishBroadcast))
	{
		return;
	}

	CMD_S_CaptureFishBroadcast *pCaptureBroadcast = (CMD_S_CaptureFishBroadcast *)buffer;
	if(NULL == pCaptureBroadcast)
	{
		return;
	}
	
	m_GameScreen.CatchBroadCast(pCaptureBroadcast->iDeskNo,pCaptureBroadcast->szNickName,pCaptureBroadcast->szRoomName,pCaptureBroadcast->iFishType,pCaptureBroadcast->i64FishScore);


}













/*-------------------------------------------------------------------*/
//转换座位
int		CNewGameClient::SwitchChairID(int m_Currchairid)
{
	int changChairID = m_Currchairid;
	switch(m_pGameFrame->GetMyDeskStation())
	{
	case 0:
	case 1:
	case 2:
		{
			switch(m_Currchairid)
			{
			case 0:return 0;
			case 1:return 1;
			case 2:return 2;
			case 3:return 3;
			case 4:return 4;
			case 5:return 5;
			}

		}
	case 3:
	case 4:
	case 5:
		{
			switch(m_Currchairid)
			{
			case 0:return 3;
			case 1:return 4;
			case 2:return 5;
			case 3:return 0;
			case 4:return 1;
			case 5:return 2;
			}

		}
	}
	return changChairID;
}
/*-------------------------------------------------------------------*/
void	CNewGameClient::OnMoveWindow(CPoint point)
{
	CWnd::FromHandle(m_pUI->GetWindowsHwnd())->PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));
}
/*-------------------------------------------------------------------*/
//最小化窗口
void	CNewGameClient::OnMinWondow()
{
	CWnd::FromHandle(m_pUI->GetWindowsHwnd())->ShowWindow(SW_MINIMIZE);
}
/*-------------------------------------------------------------------*/
//退出关闭游戏
void	CNewGameClient::OnCloseWindow()
{
	m_pGameFrame->CloseClient();
}
/*-------------------------------------------------------------------*/
//读取配置文件
void	CNewGameClient::LoadGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	
	//背景音效
	m_GameScreen.m_configDlg.m_bBgSound	= ("1" == f.GetKeyVal(key,"BgSound","1"));
	//游戏音效
	m_GameScreen.m_configDlg.m_bGameSound	= ("1" == f.GetKeyVal(key,"GameSound","1"));
	//抖屏
	m_GameScreen.m_configDlg.m_bShakeWindow = ("1" == f.GetKeyVal(key,"ShakeWindow","0"));
	//打中了鱼儿播放广播提示
	m_GameScreen.m_configDlg.m_bTipMsg = ("1" == f.GetKeyVal(key,"TipMsg","0"));
	
}
/*-------------------------------------------------------------------*/
//保存游戏配置
void	CNewGameClient::SaveGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	


	//背景音效
	if (m_GameScreen.m_configDlg.m_bBgSound)
	{
		//本来没有播放 那么这里就要开始播放背景音乐了
		f.SetKeyValString(key,"BgSound","1");
		if(!m_GameScreen.IsPlaying(m_GameScreen.m_Bgindex))
		{
			m_GameScreen.PlayBgSound(m_GameScreen.m_Bgindex);
		}
	}
	else
	{
		//如果正在播放 那么就要停止了
		f.SetKeyValString(key,"BgSound","0");
		m_GameScreen.StopBgSound();
	}

	//游戏音效
	if (m_GameScreen.m_configDlg.m_bGameSound)
	{
		f.SetKeyValString(key,"GameSound","1");
	}
	else
	{
		//如果正在播放音效 此时就要停止
		f.SetKeyValString(key,"GameSound","0");
	}

}