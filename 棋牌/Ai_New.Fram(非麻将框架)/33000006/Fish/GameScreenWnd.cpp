#include "StdAfx.h"
#include "gamescreenwnd.h"
//#include "..\NewGameClient.h"

//定义
//发射炮弹
#define  TIMERIDSHOOT          1
//子弹运行
#define  TIMERIDBULLTE         2
//鱼儿游动运行
#define  TIMERIDFISHMOVE       3
#define  TIMERIDNETOPEN        4
#define  TIMERIDFISHROUND      5
#define  TIMERIDMONEYACTION    6
//玩家钱堆移动
#define  TIMERIDUSERMONEY      7
//切换场景动画
#define  TIMERIDCHANGESCREEN   8
//自动发射子弹
#define  TIMERIDSENDSHOOT      9
//自动发射子弹
#define  TIMERIDOUT            10
//定动作判定
#define  TIMERIDDING           11
//抖屏计时器
#define	 TIME_SHAKE				12
//显示广播
#define	 TIME_SHOW_MSG			13

/*---------------------------------------------------------------------------------------------------------------*/
//活图大小
////游动图
const int GameScreenWnd::m_FishmoveCount[24] = {6,8,12,12,12,13,12,10,12,12,11,15,12,15,13,19,13,7,9,10,3,1,9,6};
////死亡图
const int GameScreenWnd::m_FishdeadCount[24] = {2,2,2,3,3,3,4,3,2,4,6,4,3,3,3,3,3,0,2,2,0,0,3,0};
/*---------------------------------------------------------------------------------------------------------------*/
const int GameScreenWnd::m_FishmoveRec[24][2] = {{32,128},{32,64},{64,128},{64,128},{64,128},{128,128},{64,256},{128,256},{128,256},{128,256},{256,256},{256,256},{128,256},{256,256},{512,256},{256,512},{256,512},/*{256,256}*/{256,256},{256,512},{256,512},{512,512},{64,512}/*{512,256}*/,{512,1024},{512,256}/*{512,256}*/};
//死图大小
const int GameScreenWnd::m_FishDeadRec[24][2] = {{32,128},{32,64},{64,128},{64,128},{64,128},{128,128},{128,256},{128,256},{128,256},{256,256},{256,256},{256,256},{128,256},{256,256},{512,256},{256,512},{256,512},/*{256,256}*/{512,512},{256,512},{256,512},{512,512},{64,512}/*{512,256}*/,{512,1024},{512,256}};
/*---------------------------------------------------------------------------------------------------------------*/
//生成金币个数
const int GameScreenWnd::m_iFishMoneyCount[24]	={	1/*0*/,1/*1*/,2/*2*/,2/*3*/,3/*4*/,3/*5*/,3/*6*/,4/*7*/,4/*8*/,
													4/*9*/,4/*10*/,5/*11*/,5/*12*/,5/*13*/,5/*14*/,5/*15*/,6/*16*/,
													6/*17*/,6/*18*/,6/*19*/,6/*20*/,6/*21*/,7/*22*/,7/*23*/};
/*---------------------------------------------------------------------------------------------------------------*/
//抖屏位置
const int G_iShakePosition[5][2] = {{0,0},{-10,5},{-10,10},{10,15},{15,10}};
/*---------------------------------------------------------------------------------------------------------------*/
//char G_szFishName[24][30] = {{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"乌龟"},
//								{"小鱼"},{"小鱼"},{"小鱼"},{"剑鱼"},{"魔鬼鱼"},{"大银鲨"},{"大金鲨"},{"飞鱼"},{"龙虾"},{"蓝海豚"},
//								{"金龙"},{"金蟾"},{"巨型黄金鲨"},{"深海怪物"}};
char G_szFishName[24][30] = {	{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"乌龟"},
								{"小鱼"},{"小鱼"},{"小鱼"},{"剑鱼"},{"魔鬼鱼"},{"炸弹银鲨"},{"炸弹黄金鲨"},{"黄金盒"},{"双头四脚怪"},{"黄金四脚怪"},
								{"孙悟空"},{"金箍棒"},{"巨型黄金鲨"},{"金蟾"}};
/*---------------------------------------------------------------------------------------------------------------*/

GameScreenWnd::GameScreenWnd()
{
	_bMouseTrack=TRUE;
	//初始数据
	ZeroMemory(m_HaveUser,sizeof(m_HaveUser));
	//ZeroMemory(m_UserPaoJiaodu,sizeof(m_UserPaoJiaodu));
	ZeroMemory(m_UserSendstate,sizeof(m_UserSendstate));
	ZeroMemory(m_UserScore,sizeof(m_UserScore));
	ZeroMemory(m_MuchmoneyAct,sizeof(m_MuchmoneyAct));
	ZeroMemory(m_IsMouseOver,sizeof(m_IsMouseOver));
	ZeroMemory(m_BeiLv,sizeof(m_BeiLv));
	ZeroMemory(m_UserScore,sizeof(m_UserScore));
	ZeroMemory(m_UserSuperPao,sizeof(m_UserSuperPao));
	m_bHaveLinked = false;
	m_DingStruc.m_have = false;
	//已经载入数量
	m_HaveLoadNum = 0;
	m_InitScreen= false;
	m_Bgindex = 0;
	m_ActionIndex = 0;
	m_ActionIndex1 = 0;
	m_SoundIndex =0;
	m_IsMustOut = false;
	m_IsDingzhu = false;
	//获取当前系统启动时间
	m_NowTime =  GetTickCount();
	//是否切换场景
	m_IsChangeScreen  =  false;

    //测试数据
	for(int i=0;i<6;i++)
	{
	   m_UserPaoJiaodu[i]=0;
	   if(i>=3)m_UserPaoJiaodu[i]=180;
       m_UserScore[i]=0;
	   m_BeiLv[i] =  0;
	   for(int j=0;j<3;j++)
	   {
		   m_UserStruct[i][j].m_Have = false;
		   m_UserStruct[i][j].m_Money = 0;
		   m_UserStruct[i][j].m_Time = 0;
	   }
	}
   
	//玩家坐标
	m_UserPT[0].x  =  110;
	m_UserPT[0].y  =  560;
	m_UserPT[1].x  =  530;
	m_UserPT[1].y  =  560;
	m_UserPT[2].x  =  950;
	m_UserPT[2].y  =  560;
	m_UserPT[5].x  =  110;
	m_UserPT[5].y  =  -20;
	m_UserPT[4].x  =  530;
	m_UserPT[4].y  =  -20;
	m_UserPT[3].x  =  950;
	m_UserPT[3].y  =  -20;

   //拷贝坐标
	CopyMemory(m_UserPaoPT,m_UserPT,sizeof(m_UserPaoPT));

	//自己座位好,经过切换的
	m_MeChariID =2;
	m_i64MyMoney = 888888888;
	m_OutTime = 120;
	
	//初始数据
	for(int i=0;i<100;i++)
	{
	    m_MeBullet[i].m_Have=false;
		m_OterBullet[i].m_Have=false;
		m_NumArr[i].m_Have = false;
		m_SoundArr[i].m_Have = false;
		m_GoldArr[i].m_Have=false;
	}

	//抖屏参数
	m_iShakex = 0;
	m_iShakey = 0;
	m_iShakeCount = 0;

	m_iMoveY  = 0;
	m_iColorR = 255;
	m_iColorB = 10;
	m_iColorG = 10;


}

GameScreenWnd::~GameScreenWnd(void)
{
}
/*---------------------------------------------------------*/
//获得客户端接口指针
void GameScreenWnd::SetGameClientP(CClientGameDlg *p)
{
//	m_pGameClient = p;
}
/*---------------------------------------------------------*/
//渲染线程
void DxThreadRender(LPVOID pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	if(pThreadData==NULL) return;

	//类型转换
	GameScreenWnd *pDxDialog=(GameScreenWnd *)pThreadData;

	//降低cpu使用率
	while(NULL!=pDxDialog && pDxDialog->GetSafeHwnd()!=NULL&&pDxDialog->m_RunRender)// && pDxDialog->m_bD3DSucess)
	{
		Sleep(10);
	}
	return;
}
/*---------------------------------------------------------*/
//渲染线程
void DxThreadRender1(LPVOID pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	if(pThreadData==NULL) return;

	//类型转换
	GameScreenWnd *pDxDialog=(GameScreenWnd *)pThreadData;

	//降低cpu使用率
	while(NULL!=pDxDialog && pDxDialog->GetSafeHwnd()!=NULL&&pDxDialog->m_RunRender)// && pDxDialog->m_bD3DSucess)
	{
		
		pDxDialog->CheckHit();
		Sleep(10);
	}

	return;
}
/*---------------------------------------------------------*/
//载入线程
void DxThreadLoad(LPVOID pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	if(pThreadData==NULL) return;

	//类型转换
	GameScreenWnd *pDxDialog=(GameScreenWnd *)pThreadData;


	return;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------------------*/
//初始窗口
bool	GameScreenWnd::InitDxDlog()
{
	OutputDebugString("dwjdsnh::InitDxDlog-0");
	m_RunRender = true;

	//SetTimer(TIMERIDOUT,1000,NULL);

	return true;
}
/*---------------------------------------------------------------------*/


//创建DX窗口及载入资源纹理图片
HRESULT GameScreenWnd::InitD3D( HWND hWnd )
{
	return S_OK;

}

//发射子弹
void GameScreenWnd::UserShoot(int PlayerID,float Roation,int RealChairID,bool isAndroid)
{
	if(m_InitScreen==false)return;

	if(PlayerID == m_MeChariID)
	{
		if(m_UserScore[m_MeChariID]- (__int64)m_BeiLv[m_MeChariID]<0)
			return;
	}

	SetTimer(TIMERIDSHOOT,100,NULL);
	
	
	//设置玩家为发射状态
	m_UserSendstate[PlayerID] = true;
	//设置炮位置
	m_UserPaoPT[PlayerID] = m_UserPT[PlayerID];
	float jiaodu =  (int(Roation)+360)%360;
	if(PlayerID>=3)
	{
	  if(jiaodu>=270){jiaodu = 90+jiaodu-270;}
	  else if(jiaodu<=90){jiaodu = 180+jiaodu;}
	}
	m_UserPaoJiaodu[PlayerID] = jiaodu;

    //判断发射玩家m,如果是自己
	if(PlayerID == m_MeChariID)
	{
		if(m_UserScore[m_MeChariID]-m_BeiLv[m_MeChariID]<0)return;
		m_OutTime = 120;
	   //设置子弹
		for(int i=0;i<100;i++)
		{
		  if(m_MeBullet[i].m_Have==true)continue;
		  m_MeBullet[i].m_isThree = false;
		  m_MeBullet[i].m_issuper = false;
		  //如果是超级炮就设置子弹为超级子弹
		  if(m_UserSuperPao[PlayerID])m_MeBullet[i].m_issuper = true;
		  //如果大于500倍率就是3发子弹
		  if(m_BeiLv[PlayerID]>500)m_MeBullet[i].m_isThree=true;
	  	  m_MeBullet[i].m_ptx = m_UserPT[PlayerID].x+70;
		  m_MeBullet[i].m_pty = m_UserPT[PlayerID].y+70;
		  m_MeBullet[i].m_Have = true;
		  m_MeBullet[i].m_Stop = false;
		  m_MeBullet[i].m_Netindex = 0;
		  m_MeBullet[i].m_Senduser = PlayerID;
		  m_MeBullet[i].m_Roation = jiaodu;
		  m_MeBullet[i].m_isAndroid = false;
		  m_MeBullet[i].m_RealUser = RealChairID;

		  break;
		}
	}
	else
	{
	  
	   //设置子弹
		for(int i=100;i<200;i++)
		{
		  if(m_MeBullet[i].m_Have==true)continue;
		  m_MeBullet[i].m_isThree = false;
		  m_MeBullet[i].m_issuper = false;
		  //如果是超级炮就设置子弹为超级子弹
		  if(m_UserSuperPao[PlayerID])m_MeBullet[i].m_issuper = true;
		  //如果大于500倍率就是3发子弹
		  if(m_BeiLv[PlayerID]>500)m_MeBullet[i].m_isThree=true;
	  	  m_MeBullet[i].m_ptx = m_UserPT[PlayerID].x+70;
		  m_MeBullet[i].m_pty = m_UserPT[PlayerID].y+70;
		  m_MeBullet[i].m_Have = true;
		  m_MeBullet[i].m_Stop = false;
		  m_MeBullet[i].m_Netindex = 0;
		  m_MeBullet[i].m_Senduser = PlayerID;
		  m_MeBullet[i].m_Roation = jiaodu;
		  m_MeBullet[i].m_isAndroid = isAndroid;
		  m_MeBullet[i].m_RealUser = RealChairID;
		  
		 // if()
		  //if(PlayerID>=3)m_MeBullet[i].m_Roation= Roation+180;
		  break;
		}
	
	}

}

/*-----------------------------------------------------------------------------*/
//捕获鱼广播
void	GameScreenWnd::CatchBroadCast(int iDeskNo,char szNickName[],char szRoomName[],int iFishType,__int64 i64FishScore)
{

	BroadCastData TBroadCast;
	TBroadCast.bHave = true;
	TBroadCast.iDeskNo = iDeskNo;
	TBroadCast.i64FishScore = i64FishScore;
	memcpy(TBroadCast.szNickName,szNickName,sizeof(TBroadCast.szNickName));
	memcpy(TBroadCast.szRoomName,szRoomName,sizeof(TBroadCast.szRoomName));

	//鱼的名字
	memcpy(TBroadCast.szFishName,G_szFishName[iFishType],sizeof(TBroadCast.szFishName));

	
}
/*-----------------------------------------------------------------------------*/
//定住
void GameScreenWnd::SetDingAction(Fish *tfish)
{
    m_IsDingzhu =  true;
	m_DingStruc.m_have =  true;
	m_DingStruc.m_nowindex = 0;
	m_DingStruc.m_ptx = tfish->m_ptx + m_FishmoveRec[tfish->m_index][0]/2-1024/2;
	m_DingStruc.m_pty = tfish->m_pty + m_FishmoveRec[tfish->m_index][1]/2-512/2;

	SetTimer(TIMERIDDING,5000,NULL);

}
//炸弹爆炸
void GameScreenWnd::SetBomAction(int PlayerId,int Fishid,int fishscore)
{
	INT_PTR nEnumIndex=0;
	Fish *tfish=NULL;
    do{
		tfish=m_FishFactory.EnumActiveObject(nEnumIndex++);
		if (NULL==tfish) break;
		if(GetTickCount()-tfish->m_creatTime<10000)
		   UserAddMoney(PlayerId,tfish->m_fishID,fishscore,0,false);

	}while(true);
}
//加钱用户
void GameScreenWnd::UserAddMoney(int PlayerID,int Fishid,int Money,int FishKindScord,bool ishavaction )
{
	if(m_InitScreen==false)return;
	int Fishindx = -1;
	INT_PTR nEnumIndex=0;
	Fish *Deadfish=NULL;
	Fish *tfish=NULL;
	do{
		tfish=m_FishFactory.EnumActiveObject(nEnumIndex++);
		if (NULL==tfish) break;
		if(tfish->m_fishID==Fishid)
		{
		  Deadfish = tfish;
		  break;
		}
	}while(true);
	
	if(Deadfish==NULL)return;
	//是否是棒子需要定住
	if(Deadfish->m_index==21) 
	{
	   SetDingAction(Deadfish);
	}



	//设置这条鱼的死亡状态
	//m_FishArr[j].m_DeadMoney = 1000;
	Deadfish->m_HitUser = PlayerID;
	Deadfish->m_isHit = true;
	Deadfish->m_actionindex = 0;
	Deadfish->m_Nowindex = 0;
	//如果只是让鱼死亡 就直接返回
	if(FishKindScord<=0)return;
	//设置完毕
    //播放声音
    char musicPath[32]={0};
	int musciindex= rand()%20;
	

	 //显示鱼的分数
	 for(int ci=0;ci<100;ci++)
		 {
			 if(m_NumArr[ci].m_Have==true)continue;
			 m_NumArr[ci].m_Have = true;
			 m_NumArr[ci].m_ptx =  Deadfish->m_ptx + m_FishmoveRec[Deadfish->m_index][0]/2;
			 m_NumArr[ci].m_pty =  Deadfish->m_pty + m_FishmoveRec[Deadfish->m_index][1]/2;
			 m_NumArr[ci].m_Time = 0;
			 m_NumArr[ci].m_Numcount = Money;
			 m_NumArr[ci].m_HitUser=PlayerID;
			 m_NumArr[ci].m_beishu = 1.4f;
			 if(Money>=10000)m_NumArr[ci].m_beishu = 2.0f;
			 break;

		 }
						
	//增加飘落金币
	 int iCount =0;
	 for(int ci=0;ci<100;ci++)
	 {   
		 //如果已经占用循环下去
		 if(m_GoldArr[ci].m_Have==true)continue;
		 m_GoldArr[ci].m_Have = true;
		 m_GoldArr[ci].m_Player = PlayerID;
		 m_GoldArr[ci].m_actionindex =0;
		 m_GoldArr[ci].m_ptx =  Deadfish->m_ptx + m_FishmoveRec[Deadfish->m_index][0]/2 + (iCount*82.0);
		 m_GoldArr[ci].m_pty =  Deadfish->m_pty + m_FishmoveRec[Deadfish->m_index][1]/2;

		 //求角度
		 float px = m_GoldArr[ci].m_ptx-m_UserPT[PlayerID].x-100;
		 float py = m_UserPT[PlayerID].y-m_GoldArr[ci].m_pty+100;
		 float bili = float(px/py);
		 float jiaodu=atan2(px,py)*180.0/3.1415926;
		 m_GoldArr[ci].m_Roation=jiaodu;

		 iCount++;
		 if(iCount >= m_iFishMoneyCount[Deadfish->m_index])
		 {
			 break;
		 }
	 }


    m_UserScore[PlayerID] =m_UserScore[PlayerID] + Money;
	if(!m_UserStruct[PlayerID][0].m_Have)
	{
		m_UserStruct[PlayerID][0].m_Have=true;
		m_UserStruct[PlayerID][0].m_Money = Money;
		m_UserStruct[PlayerID][0].m_Time = 0;
		m_UserStruct[PlayerID][0].m_FishScore = FishKindScord;
		m_UserStruct[PlayerID][0].m_Nowscore = 0;


	}
	else if(m_UserStruct[PlayerID][0].m_Have&&!m_UserStruct[PlayerID][1].m_Have)
	{
		m_UserStruct[PlayerID][1].m_Have=true;
		m_UserStruct[PlayerID][1].m_Money = Money;
		m_UserStruct[PlayerID][1].m_Time = 0;
		m_UserStruct[PlayerID][1].m_FishScore = FishKindScord;
		m_UserStruct[PlayerID][1].m_Nowscore = 0;
	}
	else if(m_UserStruct[PlayerID][0].m_Have&&m_UserStruct[PlayerID][1].m_Have&&!m_UserStruct[PlayerID][2].m_Have)
	{
		m_UserStruct[PlayerID][2].m_Have=true;
		m_UserStruct[PlayerID][2].m_Money = Money;
		m_UserStruct[PlayerID][2].m_Time = 0;
		m_UserStruct[PlayerID][2].m_FishScore = FishKindScord;
		m_UserStruct[PlayerID][2].m_Nowscore = 0;
	}
	else
	{
		m_UserStruct[PlayerID][0].m_Have =  m_UserStruct[PlayerID][1].m_Have;
		m_UserStruct[PlayerID][0].m_Money =  m_UserStruct[PlayerID][1].m_Money;
		m_UserStruct[PlayerID][0].m_Time =  0;
		m_UserStruct[PlayerID][0].m_FishScore = m_UserStruct[PlayerID][1].m_FishScore;
		m_UserStruct[PlayerID][0].m_Nowscore = m_UserStruct[PlayerID][1].m_Nowscore;
        m_UserStruct[PlayerID][1].m_Have =  m_UserStruct[PlayerID][2].m_Have;
		m_UserStruct[PlayerID][1].m_Money =  m_UserStruct[PlayerID][2].m_Money;
		m_UserStruct[PlayerID][1].m_FishScore = m_UserStruct[PlayerID][2].m_FishScore;
		m_UserStruct[PlayerID][1].m_Nowscore = m_UserStruct[PlayerID][2].m_Nowscore;
		m_UserStruct[PlayerID][1].m_Time =  0;
		m_UserStruct[PlayerID][2].m_Have=true;
		m_UserStruct[PlayerID][2].m_Money = Money;
		m_UserStruct[PlayerID][2].m_Time = 0;
		m_UserStruct[PlayerID][2].m_FishScore = FishKindScord;
		m_UserStruct[PlayerID][2].m_Nowscore = 0;
	}

	
}
//切换场景
void GameScreenWnd::ChangeScreen(int bgindex)
{


	m_ChanwaveSt.m_currImgIndex = 0;
	m_ChanwaveSt.m_ptx = 1280;
	m_ChanwaveSt.m_pty = 0;
	m_ChanwaveSt.m_Time = 0;
	m_IsChangeScreen = true;
	m_Bgindex = bgindex;
	if(m_Bgindex>=3)m_Bgindex=0;
	//开启动动画
	SetTimer(TIMERIDCHANGESCREEN,50,NULL);
	//把鱼消失
	Fish *tfish=NULL;
	for(int i=0;i<m_FishFactory.GetActiveCount();i++)
	{
		//枚举鱼群
		tfish=m_FishFactory.EnumActiveObject(i);
		if (NULL==tfish) break;
		//激活判断
		if(tfish->m_Have==false)continue;
		if(tfish->m_isHit==true)continue;
		tfish->m_speed  = 100;
	}  
}


/*----------------------------------------------------------------------------------*/
//消息映射
BEGIN_MESSAGE_MAP(GameScreenWnd, CWnd)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()
/*----------------------------------------------------------------------------------*/
//鼠标进入 获得焦点
void	GameScreenWnd::OnMouseHover(UINT, CPoint)
{
	this->SetFocus();
}
/*----------------------------------------------------------------------------------*/
//鼠标离开 失去焦点
void	GameScreenWnd::OnMouseLeave()
{
	_bMouseTrack = TRUE;
	m_bMoveWindow = false;
	KillTimer(TIMERIDSENDSHOOT);
}
/*----------------------------------------------------------------------------------*/
void	GameScreenWnd::OnMouseMove(UINT, CPoint point)
{
	this->SetFocus();
	//设置鼠标追踪
	if (_bMouseTrack)     // 若允许 追踪，则。 
	{
		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof (csTME);
		csTME.dwFlags = TME_LEAVE|TME_HOVER;
		csTME.hwndTrack = m_hWnd ;		// 指定要 追踪 的窗口 
		csTME.dwHoverTime = 5;			// 鼠标在按钮上停留超过 10ms ，才认为状态为 HOVER
		::_TrackMouseEvent (&csTME);	// 开启 Windows 的 WM_MOUSELEAVE ， WM_MOUSEHOVER 事件支持
		_bMouseTrack=FALSE ;			// 若已经 追踪 ，则停止 追踪 
	}

	if(m_bMoveWindow)
	{
//		m_pGameClient->OnMoveWindow(point);
		return;
	}
	m_configDlg.LMouseMove(point.x,point.y);

	//SetCursor(LoadCursor((HINSTANCE)GetWindowLong(this->GetSafeHwnd(),GWL_HINSTANCE),MAKEINTRESOURCE(IDC_CURSOR1)));
	SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));


	if(point.y>710||point.y<30||point.x<5||point.x>1275||m_configDlg.m_isvisable)
	{
		
	}
	else 
	{
		if(m_InitScreen)
		{
			m_cursor.m_have = true;
			m_cursor.m_ptx = point.x-28;
			m_cursor.m_pty = point.y-28;
		}
	}


	//处理自己炮的移动角度
	float px=point.x-m_UserPT[m_MeChariID].x-100;
	float py=m_UserPT[m_MeChariID].y-point.y+100;
	float bili = float(px/py);
	float jiaodu=atan2(px,py)*180.0/3.1415926;
	if(jiaodu<-90)jiaodu=-90;
	if(jiaodu>90)jiaodu=90;
	if(jiaodu<0)jiaodu=360+jiaodu;
	m_UserPaoJiaodu[m_MeChariID]=jiaodu;

	for(int i=0;i<5;i++)
	{
		m_IsMouseOver[i]=false;
	}

}
/*----------------------------------------------------------------------------------*/
void	GameScreenWnd::OnLButtonDown(UINT, CPoint point)
{
	m_configDlg.LMouseDown(point.x,point.y);
	if(m_configDlg.m_isvisable)
	{
		return ;
	}
	//移动窗口
	if(point.y<30)
	{
		/*POINT pMouse;
		CPoint cpoint;
		if(GetCursorPos(&pMouse))
		{
			cpoint.x = pMouse.x;
			cpoint.y = pMouse.y;
		}
		else
		{
			cpoint = point;
		}*/
//		m_pGameClient->OnLeftButtonDown(point);
		m_bMoveWindow = true;
	}

	if(point.y>30&&point.y<710&&m_InitScreen)
	{
		//发射
		if(GetTickCount() - m_NowTime > 400 )
		{
			//如果没钱直接弹出去
			if(m_UserScore[m_MeChariID]<=0)
			{
				return ;
			}
			if(m_BeiLv[m_MeChariID]>m_UserScore[m_MeChariID])
			{
				return ; 
			}
//			m_pGameClient->OnUserShot(false,m_UserPaoJiaodu[m_MeChariID]);
			UserShoot(m_MeChariID,m_UserPaoJiaodu[m_MeChariID],m_MeRellayChairID,false);
			m_NowTime = GetTickCount() ;

		}
		SetTimer(TIMERIDSENDSHOOT,200,NULL);
		//设置为鼠标按下状态
		m_MouseDown = true;
	}
	//按扭事件

	{
		//上分
//		m_pGameClient->AddOrRemoveScore(true,m_BeiLv[m_MeChariID]);
	
	}

	{
		//下分
//		m_pGameClient->AddOrRemoveScore(false,m_BeiLv[m_MeChariID]);
			
	}
	
}
/*----------------------------------------------------------------------------------*/
void	GameScreenWnd::OnLButtonUp(UINT, CPoint ponit)
{
	m_configDlg.LMouseUp(ponit.x,ponit.y);
	//设置鼠标状态为松开
	m_MouseDown =  false;
	m_bMoveWindow = false;
	//停止子弹发射
	KillTimer(TIMERIDSENDSHOOT);
}
/*----------------------------------------------------------------------------------*/
void	GameScreenWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_configDlg.m_isvisable)
	{
		return ;
	}
	
	switch( (WCHAR)nChar )
	{
	case VK_SPACE://空格发射
		{
			if(GetTickCount() - m_NowTime > 190 )
			{
				//如果没钱直接弹出去
				if(m_UserScore[m_MeChariID]<=0)break;
				if(m_BeiLv[m_MeChariID]>m_UserScore[m_MeChariID]) return ; //edit by guojm
//				m_pGameClient->OnUserShot(false,m_UserPaoJiaodu[m_MeChariID]);
				UserShoot(m_MeChariID,m_UserPaoJiaodu[m_MeChariID],m_MeRellayChairID,false);
				m_NowTime = GetTickCount() ;
				KillTimer(TIMERIDSENDSHOOT);
			}
			break;
		}
	case VK_UP://上键加炮
		{
//			m_pGameClient->OnUserAddFire(true);
			break;
		}
	case VK_DOWN://下减炮
		{
//			m_pGameClient->OnUserAddFire(false);
			break;
		}
	case VK_LEFT://左上分
		{
//			m_pGameClient->AddOrRemoveScore(true,m_BeiLv[m_MeChariID]);
		
			break;
		}
	case VK_RIGHT://左下分
		{
//			m_pGameClient->AddOrRemoveScore(false,m_BeiLv[m_MeChariID]);
			
			break;
		}
	case VK_ESCAPE:
		{
			return ;
		}
	}		
}
/*----------------------------------------------------------------------------------*/
//键盘弹起
void	GameScreenWnd::OnkeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
/*----------------------------------------------------------------------------------*/
//时间事件
void GameScreenWnd::OnGameTimer(UINT nIDEvent)
{
	
	if(TIME_SHAKE == nIDEvent)
	{
		m_iShakeCount++;
		if(m_iShakeCount > 15)
		{
			m_iShakex = 0;
			m_iShakey = 0;
			m_iShakeCount = 0;
			KillTimer(TIME_SHAKE);
		}
		else
		{
			m_iShakex = G_iShakePosition[m_iShakeCount%5][0];
			m_iShakey = G_iShakePosition[m_iShakeCount%5][1];
		}
	}
	else if(TIMERIDSHOOT==nIDEvent)
	{   
		for(int i= 0 ;i < 6;i++)
		{
			if(m_UserSendstate[i])
			{
				float m_x = 7*sin(2*3.1415926*(m_UserPaoJiaodu[i])/360.0);
				float m_y = 7*cos(2*3.1415926*(m_UserPaoJiaodu[i])/360.0);
				m_UserPaoPT[i].y =m_UserPaoPT[i].y+m_y;
				m_UserPaoPT[i].x =m_UserPaoPT[i].x-m_x;
				m_UserSendstate[i]=false;

			}
			else 
			{
				m_UserPaoPT[i].y =m_UserPT[i].y;
				m_UserPaoPT[i].x =m_UserPT[i].x;
				//KillTimer(TIMERIDSHOOT);
			}
		}
	}
	else if(TIMERIDBULLTE==nIDEvent)
	{
		//移动自己的子弹
		for(int i=0;i<200;i++)
		{
			if(m_MeBullet[i].m_Have==false)continue;
			if(m_MeBullet[i].m_Stop==true)continue;		 

			//判断边缘
			if(m_MeBullet[i].m_ptx<0)//左边超出
			{
				m_MeBullet[i].m_Roation = 360-m_MeBullet[i].m_Roation;
			}
			if(m_MeBullet[i].m_ptx+64>CLIENT_WIDTH)//右边超出
			{
				m_MeBullet[i].m_Roation = 360-m_MeBullet[i].m_Roation;
			}
			if(m_MeBullet[i].m_pty+64>CLIENT_HEIGHT)//底部超出
			{
				m_MeBullet[i].m_Roation = 180-m_MeBullet[i].m_Roation;
			}
			if(m_MeBullet[i].m_pty<0)//顶部超出
			{
				m_MeBullet[i].m_Roation = 180-m_MeBullet[i].m_Roation;
			}

			if(m_MeBullet[i].m_ptx<0&&m_MeBullet[i].m_pty<0)
			{
				m_MeBullet[i].m_Roation = 135;
				m_MeBullet[i].m_ptx = 20;
				m_MeBullet[i].m_pty = 20;
			}
			else if(m_MeBullet[i].m_ptx<0&&m_MeBullet[i].m_pty>CLIENT_HEIGHT)
			{
				m_MeBullet[i].m_Roation = 45;
				m_MeBullet[i].m_ptx = 20;
				m_MeBullet[i].m_pty = 720;
			}
			else if(m_MeBullet[i].m_ptx>CLIENT_WIDTH&&m_MeBullet[i].m_pty>CLIENT_HEIGHT)
			{
				m_MeBullet[i].m_Roation = 315;
				m_MeBullet[i].m_ptx = 1260;
				m_MeBullet[i].m_pty = 720;
			}
			else if(m_MeBullet[i].m_ptx>CLIENT_WIDTH&&m_MeBullet[i].m_pty<0)
			{
				m_MeBullet[i].m_Roation = 225;
				m_MeBullet[i].m_ptx = 1260;
				m_MeBullet[i].m_pty = 20;
			}

			float m_x = 0.0;
			float m_y = 0.0;
			if(m_MeBullet[i].m_issuper)
			{
				m_x = 14*sin(2*3.1415926*(m_MeBullet[i].m_Roation)/360.0);
				m_y = 14*cos(2*3.1415926*(m_MeBullet[i].m_Roation)/360.0);
			}
			else
			{
				m_x = 10*sin(2*3.1415926*(m_MeBullet[i].m_Roation)/360.0);
				m_y = 10*cos(2*3.1415926*(m_MeBullet[i].m_Roation)/360.0);
			}
			
			m_MeBullet[i].m_ptx = m_MeBullet[i].m_ptx + m_x;
			m_MeBullet[i].m_pty = m_MeBullet[i].m_pty - m_y;

		}
		//移动鱼儿坐标
		Fish *tfish=NULL;
		for(int i=0;i<m_FishFactory.GetActiveCount();i++)
		{
			//枚举鱼群
			if(m_IsDingzhu)break;//如果目前定住直接弹出去
			tfish=m_FishFactory.EnumActiveObject(i);
			if (NULL==tfish) break;
			//激活判断
			if(tfish->m_Have==false)continue;
			if(tfish->m_isHit==true)continue;
			float m_x = tfish->m_speed*sin(2*3.1415926*(tfish->m_Roation)/360.0)/3.5;
			float m_y = tfish->m_speed*cos(2*3.1415926*(tfish->m_Roation)/360.0)/3.5;
			tfish->m_ptx = tfish->m_ptx + m_x;
			tfish->m_pty = tfish->m_pty - m_y;
		}
		//飘落金币坐标移动
		for(int i=0;i<100;i++)
		{
			if(m_GoldArr[i].m_Have==false)continue;
			float m_x = 12*sin(2*3.1415926*(m_GoldArr[i].m_Roation)/360.0)/4;
			float m_y = 12*cos(2*3.1415926*(m_GoldArr[i].m_Roation)/360.0)/4;
			m_GoldArr[i].m_ptx = m_GoldArr[i].m_ptx - m_x;
			m_GoldArr[i].m_pty = m_GoldArr[i].m_pty + m_y;

			//判定
			CRect m_rct;
			m_rct.top = m_UserPT[m_GoldArr[i].m_Player].y+50;
			m_rct.left = m_UserPT[m_GoldArr[i].m_Player].x+50;
			m_rct.right =  m_rct.left + 100;
			m_rct.bottom =  m_rct.top + 100;
			m_GoldArr[i].m_Point.x = m_GoldArr[i].m_ptx;
			m_GoldArr[i].m_Point.y = m_GoldArr[i].m_pty;
			bool is_hit =  m_rct.PtInRect(m_GoldArr[i].m_Point);
			if(is_hit||m_GoldArr[i].m_ptx<0||m_GoldArr[i].m_ptx>1280||m_GoldArr[i].m_pty<0||m_GoldArr[i].m_pty>740)
				m_GoldArr[i].m_Have = false;
		}
	}
	//鱼儿动画序列切换
	else if(TIMERIDFISHMOVE==nIDEvent)
	{
		OutputDebugString("dwjdsnh::TIMERIDFISHMOVE-0");
		//这里处理全局动画序列
		m_ActionIndex ++ ;
		if(m_ActionIndex>100000000)m_ActionIndex = 0;
		INT_PTR nEnumIndex=0;
		Fish *tfish=NULL;
		do
		{
			tfish=m_FishFactory.EnumActiveObject(nEnumIndex++);
			//枚举鱼群
			if (NULL==tfish) break;
			if(tfish->m_Have==false)continue;
			if(tfish->m_isHit)
			{
				tfish->m_Nowindex++;
				if(tfish->m_Nowindex>=5)
				{
					tfish->m_actionindex++;
					if(tfish->m_actionindex>=m_FishdeadCount[tfish->m_index]*3)tfish->m_Have = false;
				}
			}
			else
			{
				if(!m_IsDingzhu)
				{
					tfish->m_actionindex++;
					if(tfish->m_actionindex>=m_FishmoveCount[tfish->m_index])tfish->m_actionindex=0;
				}
			}
		}while(true);
	}
	//展开子弹网子
	else if(nIDEvent == TIMERIDNETOPEN)
	{
		//移动自己的子弹
		for(int i=0;i<200;i++)
		{
			if(m_MeBullet[i].m_Have==false)continue;
			if(m_MeBullet[i].m_Stop==false)continue;
			m_MeBullet[i].m_Netindex++;
			if(m_MeBullet[i].m_Netindex>=19){m_MeBullet[i].m_Have = false;}
		}
		//翻滚飘落金币
		for(int i=0;i<100;i++)
		{
			if(m_GoldArr[i].m_Have==false)continue;
			m_GoldArr[i].m_actionindex++;
			if(m_GoldArr[i].m_actionindex >= 13){ m_GoldArr[i].m_actionindex = 0;}
		}
	}
	//鱼儿的转换角度动画
	else if(nIDEvent == TIMERIDFISHROUND)
	{
		m_ActionIndex1 ++;
		if(m_ActionIndex1>100000000)m_ActionIndex1 = 0;
		//循环吧
		Fish *tfish=NULL;
		for(int i=0;i<m_FishFactory.GetActiveCount();i++)
		{
			//枚举鱼群
			if(m_IsDingzhu)break;//如果目前定住直接弹出去
			tfish=m_FishFactory.EnumActiveObject(i);
			if (NULL==tfish) break;
			if(tfish->m_Have==false)continue;
			if(tfish->m_isHit==true)continue;

			tfish->m_Currtime = tfish->m_Currtime+ 1;
			if(tfish->m_Currtime>=m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].movetime)
			{
				tfish->m_PtIndex++;
				tfish->m_Currtime = 0;
				tfish->m_speed = m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].m_speed;
			}
			if(tfish->m_Roation!=m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].rotation)
			{
				if(tfish->m_Roation>270&&m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].rotation<90)
				{
					m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].rotation = 360 + m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].rotation;
				}

				float tempr = (m_FishTrace[tfish->m_traceIndex][tfish->m_PtIndex].rotation - tfish->m_Roation) / 50;
				tfish->m_Roation = tfish->m_Roation +tempr;
			}
			//如果是有附加鱼就是转圈的鱼那么就执行下面的操作		     
		}
		for(int ci=0;ci<100;ci++)
		{   
			//如果已经占用循环下去
			if(m_NumArr[ci].m_Have==false)continue;
			m_NumArr[ci].m_Time++;
			if(m_NumArr[ci].m_Time>=150)
			{
				m_NumArr[ci].m_Have=false;
			}
		}
		for(int i=0;i<PLAY_COUNT;i++)
		{
			//判断是否有玩家
			if(!m_HaveUser[i])continue;
			for(int j=0;j<3;j++)
			{
				if(!m_UserStruct[i][j].m_Have)continue;
				if(m_UserStruct[i][j].m_Nowscore<m_UserStruct[i][j].m_FishScore)
					m_UserStruct[i][j].m_Nowscore++;
			}
		}
	}
	//大鱼死后转圈和撒钱动画
	else if(nIDEvent == TIMERIDMONEYACTION)
	{
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_MuchmoneyAct[i].m_have==false)continue;
			m_MuchmoneyAct[i].m_nowindex++;
			//如果序列大于21就转换成更改图片角度
			if(m_MuchmoneyAct[i].m_nowindex>21)
			{
				m_MuchmoneyAct[i].m_Roation = m_MuchmoneyAct[i].m_Roation + 30 ;
			}
			//如果序列大于了100那么就切换为不显示了
			if(m_MuchmoneyAct[i].m_nowindex>90)
			{
				m_MuchmoneyAct[i].m_have=false;
			}			
		}
		if(m_DingStruc.m_have == true)
		{
			m_DingStruc.m_nowindex++;
			if(m_DingStruc.m_nowindex>60)m_DingStruc.m_have=false;
		}


		
	}
	//玩家钱堆移动，就是旁边打死鱼儿后的3堆钱
	else if(nIDEvent == TIMERIDUSERMONEY)
	{
		
	}
	//场景切换动画
	else if(TIMERIDCHANGESCREEN==nIDEvent)
	{

		m_ChanwaveSt.m_currImgIndex = 1-m_ChanwaveSt.m_currImgIndex;
		m_ChanwaveSt.m_ptx = m_ChanwaveSt.m_ptx-20;
		m_ChanwaveSt.m_pty = 0;
		m_ChanwaveSt.m_Time++;
		if(m_ChanwaveSt.m_ptx<-500)
		{
			//如果已经超出边缘那么停止动画
			m_IsChangeScreen = false;
			KillTimer(TIMERIDCHANGESCREEN);
		}
	}
	//发送炮弹
	else if(TIMERIDSENDSHOOT==nIDEvent)
	{
		//发射子弹
		if(m_UserScore[m_MeChariID]>0&&m_BeiLv[m_MeChariID]<m_UserScore[m_MeChariID])  //edit by guojm)
		{
//			m_pGameClient->OnUserShot(false,m_UserPaoJiaodu[m_MeChariID]);
			UserShoot(m_MeChariID,m_UserPaoJiaodu[m_MeChariID],m_MeRellayChairID,false);
		}
		else
		{
			KillTimer(TIMERIDSENDSHOOT);
		}
	}
	else if(TIMERIDOUT==nIDEvent)
	{
		m_OutTime--;
		if(m_OutTime<=60)
		{
			m_IsMustOut =  true;
		}
		else
		{
			m_IsMustOut =  false;
		}
		//退出
		if( m_OutTime<=0)
		{
//			m_pGameClient->OnCloseWindow();
		}
	}
	else if(TIMERIDDING==nIDEvent)
	{
		m_IsDingzhu =  false;
		KillTimer(TIMERIDDING);
	}

	CWnd::OnTimer(nIDEvent);
}
/*----------------------------------------------------------------------------------*/
//玩家进入或者出去
void	GameScreenWnd::UserComeInorLeave(int PlayerID,bool ComeOrLeave)
{
	//设置桌位是否有玩家
	m_HaveUser[PlayerID] = ComeOrLeave; 
}
/*----------------------------------------------------------------------------------*/
//自己信息
void	GameScreenWnd::SetMeInformation(int MePlayerid,int MeRellid ,CString sNickName , __int64 i64money)
{
	//赋值信息
	m_i64MyMoney	= i64money;
	m_MeChariID		= MePlayerid;
	m_MeRellayChairID = MeRellid;
}
/*----------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------*/
void GameScreenWnd::AddFish(int traceX,int traceY,float roation,float movetime,float changetime,int fishtype,int ptindex,int nowindex,int fishid,int smallfishtype,int fishspeed)
{
	//清除这鱼
	m_FishArr[ptindex].m_Have = false;

	//清理轨迹数组
	if(m_MeRellayChairID>=3)
	{
		float temp = roation+180;
		if(temp>360)temp=temp-360;
		//继续判定
		if(temp<0)temp=360-abs(temp);
		int tx,ty;
		if(traceX<0&&traceY<=0)
		{
			tx = CLIENT_WIDTH +abs(traceX);
			ty = CLIENT_HEIGHT +abs(traceY);

		}
		else  if(traceX<0&&traceY>0)
		{
			tx = CLIENT_WIDTH +abs(traceX);
			ty = CLIENT_HEIGHT -abs(traceY);

		}
		else  if(traceX>0&&traceY>0)
		{
			tx = CLIENT_WIDTH -abs(traceX);
			ty = CLIENT_HEIGHT -abs(traceY);

		}
		else  if(traceX>0&&traceY<0)
		{
			tx = CLIENT_WIDTH -abs(traceX);
			ty = CLIENT_HEIGHT +abs(traceY);

		}

		//赋予值
		m_FishTrace[ptindex][nowindex].x = tx;
		m_FishTrace[ptindex][nowindex].y = ty;
		m_FishTrace[ptindex][nowindex].movetime = movetime;
		m_FishTrace[ptindex][nowindex].rotation = temp;
		m_FishTrace[ptindex][nowindex].changetime = changetime;
		m_FishTrace[ptindex][nowindex].m_fudaifishtype = smallfishtype;
		m_FishTrace[ptindex][nowindex].m_speed = fishspeed;
		m_FishTrace[ptindex][nowindex].m_fishid = fishid;
		m_FishTrace[ptindex][nowindex].m_fishtype = fishtype;
		m_FishTrace[ptindex][nowindex].m_fudaifishtype = smallfishtype;

	}
	else
	{
		//赋予值
		m_FishTrace[ptindex][nowindex].x = traceX;
		m_FishTrace[ptindex][nowindex].y = traceY;
		m_FishTrace[ptindex][nowindex].movetime = movetime;
		m_FishTrace[ptindex][nowindex].rotation = roation;
		m_FishTrace[ptindex][nowindex].changetime = changetime;
		m_FishTrace[ptindex][nowindex].m_fudaifishtype = smallfishtype;
		m_FishTrace[ptindex][nowindex].m_speed = fishspeed;
		m_FishTrace[ptindex][nowindex].m_fishid = fishid;
		m_FishTrace[ptindex][nowindex].m_fishtype = fishtype;
		m_FishTrace[ptindex][nowindex].m_fudaifishtype = smallfishtype;

	}
	//如果已经到了第4个轨迹坐标鱼就活了
	if(nowindex==4)
	{

		bool isHace = false;
		if(!isHace)
		{
			//获取鱼类
			Fish *tempFish=m_FishFactory.ActiveItem();
			if (NULL==tempFish) return ;
			tempFish->m_actionindex = 0;
			tempFish->m_Nowindex = 0;
			tempFish->m_HitUser = -1;
			tempFish->m_Currtime = 0;
			tempFish->m_fishID = m_FishTrace[ptindex][0].m_fishid;
			tempFish->m_Have = true;
			tempFish->m_index = m_FishTrace[ptindex][0].m_fishtype;
			tempFish->m_PtIndex = 0;
			tempFish->m_isHit = false;
			tempFish->m_ptx = m_FishTrace[ptindex][0].x - m_FishmoveRec[m_FishTrace[ptindex][0].m_fishtype][0]/2 ;
			tempFish->m_pty = m_FishTrace[ptindex][0].y - m_FishmoveRec[m_FishTrace[ptindex][0].m_fishtype][1]/2;
			tempFish->m_Roation = m_FishTrace[ptindex][0].rotation;
			tempFish->m_speed = m_FishTrace[ptindex][0].m_speed;
			tempFish->m_smallFish = 0;
			tempFish->m_smallFish = m_FishTrace[ptindex][0].m_fudaifishtype;
			tempFish->m_traceIndex = ptindex;
			tempFish->m_creatTime = GetTickCount();

		}
	}  
}
/*----------------------------------------------------------------------------------*/
void GameScreenWnd::CheckHit()
{
	if(m_InitScreen==false)return;
	//判断子弹是否击中了鱼
	try{
		Fish *tfish=NULL;
		for(int i=0;i<200;i++)
		{
			if(m_MeBullet[i].m_Have==false)continue;
			INT_PTR nEnumIndex=0;
			Fish *tfish=NULL;
			bool bShoot=false;
			do
			{
				tfish=m_FishFactory.EnumActiveObject(nEnumIndex++);
				//枚举鱼群

				if (NULL==tfish) break;
				if(tfish->m_Have==false)continue;
				if(tfish->m_isHit==true)continue;
				//if(GetTickCount()-tfish->m_creatTime>50000)continue;
				//创建不规则区域
				CRgn  nit;
				//定义四个顶点坐标
				CPoint VertexPoint[4];
				//算出第一个坐标点
				long x2;
				float co;
				float si;
				//根据鱼儿返回比例
				int m_BliCountx = 3;
				int m_BliCounty = 2;
				if(tfish->m_index>=7&&tfish->m_index<12)
				{
					m_BliCountx = 5;
					m_BliCounty = 4;
				}
				else if(tfish->m_index>12&&tfish->m_index<13)
				{
					m_BliCountx = 3;
					m_BliCounty = 2;
				}
				else if(tfish->m_index>=13&&tfish->m_index<14)
				{
					m_BliCountx = 4;
					m_BliCounty = 3;
				}
				else if(tfish->m_index>=14&&tfish->m_index<=16)
				{
					m_BliCountx = 6;
					m_BliCounty = 5;
				}

				else if(tfish->m_index>=17&&tfish->m_index<18)
				{
					m_BliCountx = 4;
					m_BliCounty = 4;
				}
				else if(tfish->m_index>=18&&tfish->m_index<19)
				{
					m_BliCountx = 6;
					m_BliCounty = 5;
				}
				else if(tfish->m_index>=19&&tfish->m_index<20)
				{
					m_BliCountx = 6;
					m_BliCounty = 3;
				}
				else if(tfish->m_index>=20&&tfish->m_index<21)
				{
					m_BliCountx = 4;
					m_BliCounty = 4;
				}
				else if(tfish->m_index>=21&&tfish->m_index<22)
				{
					m_BliCountx = 2;
					m_BliCounty = 4;
				}
				else if(tfish->m_index>=22&&tfish->m_index<23)
				{
					m_BliCountx = 7;
					m_BliCounty = 6;
				}
				else if(tfish->m_index>=23&&tfish->m_index<24)
				{
					m_BliCountx = 6;
					m_BliCounty = 2;
				}
				//
				co=cos(2*3.1415926*(tfish->m_Roation)/360.0);
				si=sin(2*3.1415926*(tfish->m_Roation)/360.0);
				//旋转的中心点
				CPoint Center;
				//中心点就是这个鱼X坐标+这条鱼的宽度的一半
				Center.x= tfish->m_ptx + m_FishmoveRec[tfish->m_index][0]/2;
				Center.y= tfish->m_pty + m_FishmoveRec[tfish->m_index][1]/2;
				//即将要旋转的点
				CPoint Center1;
				Center1.x= Center.x - (m_FishmoveRec[tfish->m_index][0]/m_BliCountx);
				Center1.y= Center.y - m_FishmoveRec[tfish->m_index][1]/m_BliCounty;
				x2=Center1.x;
				VertexPoint[0].x=(long)((double)(Center1.x-Center.x)*co-(double)(Center1.y-Center.y)*si+Center.x);
				VertexPoint[0].y=(long)((double)(x2-Center.x)*si+(double)(Center1.y-Center.y)*co+Center.y);

				Center1.x= Center.x - m_FishmoveRec[tfish->m_index][0]/m_BliCountx;
				Center1.y= Center.y + m_FishmoveRec[tfish->m_index][1]/m_BliCounty;
				x2=Center1.x;
				VertexPoint[1].x=(long)((double)(Center1.x-Center.x)*co-(double)(Center1.y-Center.y)*si+Center.x);
				VertexPoint[1].y=(long)((double)(x2-Center.x)*si+(double)(Center1.y-Center.y)*co+Center.y);

				Center1.x= Center.x + m_FishmoveRec[tfish->m_index][0]/m_BliCountx;
				Center1.y= Center.y + m_FishmoveRec[tfish->m_index][1]/m_BliCounty;
				x2=Center1.x;
				VertexPoint[2].x=(long)((double)(Center1.x-Center.x)*co-(double)(Center1.y-Center.y)*si+Center.x);
				VertexPoint[2].y=(long)((double)(x2-Center.x)*si+(double)(Center1.y-Center.y)*co+Center.y);

				Center1.x= Center.x + m_FishmoveRec[tfish->m_index][0]/m_BliCountx;
				Center1.y= Center.y - m_FishmoveRec[tfish->m_index][1]/m_BliCounty;
				x2=Center1.x;
				VertexPoint[3].x=(long)((double)(Center1.x-Center.x)*co-(double)(Center1.y-Center.y)*si+Center.x);
				VertexPoint[3].y=(long)((double)(x2-Center.x)*si+(double)(Center1.y-Center.y)*co+Center.y);
				nit.CreatePolygonRgn(VertexPoint, 4, ALTERNATE);

				m_MeBullet[i].m_Point.x = m_MeBullet[i].m_ptx+32;
				m_MeBullet[i].m_Point.y = m_MeBullet[i].m_pty+32;

				//判断子弹是否在枪口后面
				CRect m_rect;
				m_rect.left = m_UserPT[m_MeBullet[i].m_Senduser].x+30;
				m_rect.top =  m_UserPT[m_MeBullet[i].m_Senduser].y+10;
				m_rect.right = m_rect.left + 100;
				m_rect.bottom = m_rect.top + 190;
				bool is_No =m_rect.PtInRect(m_MeBullet[i].m_Point);


				//获取鱼儿区域完毕
				//开始获取子弹区域       
				bool is_hit =  nit.PtInRegion(m_MeBullet[i].m_Point);	
				nit.DeleteObject();
				if(is_hit&&m_MeBullet[i].m_Stop==false&&!is_No)
				{
					//播放声音


					//设置碰撞后子弹状态
					m_MeBullet[i].m_Netindex = 0;
					m_MeBullet[i].m_Stop = true;
					//设置被捕获鱼儿数据
					//如果这个子弹是我发射的
					if(m_MeBullet[i].m_Senduser==m_MeChariID||m_MeBullet[i].m_isAndroid)
					{
						//发送消息到服务端
						bShoot=true;
						//中断判断
						break;
					}
				}
			}while(true);
		}
	}
	catch(...)
	{
		//  AfxMessageBox(L"游戏出错，可能影响游戏运行，请联系开发者QQ：157480296");
	}
}

/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------*/


