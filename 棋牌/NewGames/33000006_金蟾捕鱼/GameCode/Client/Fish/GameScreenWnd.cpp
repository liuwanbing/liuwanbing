#include "StdAfx.h"
#include "gamescreenwnd.h"
#include "..\NewGameClient.h"

//定义
//发射炮弹
#define  TIMERIDSHOOT			1
//子弹运行
#define  TIMERIDBULLTE			2
//鱼儿游动运行
#define  TIMERIDFISHMOVE		3
#define  TIMERIDNETOPEN			4
#define  TIMERIDFISHROUND		5
#define  TIMERIDMONEYACTION		6
//玩家钱堆移动
#define  TIMERIDUSERMONEY		7
//切换场景动画
#define  TIMERIDCHANGESCREEN	8
//自动发射子弹
#define  TIMERIDSENDSHOOT		9
//
//自动发射子弹
#define  TIMERIDOUT				10
//定动作判定
#define  TIMERIDDING			11
//抖屏计时器
#define	 TIME_SHAKE				12
//显示广播
#define	 TIME_SHOW_MSG			13		//无用

/*---------------------------------------------------------------------------------------------------------------*/
//活图大小
////游动图
//const int GameScreenWnd::m_FishmoveCount[24] = {6,8,12,12,12,13,12,10,12,12,8,15,12,15,13,19,13,7,9,10,3,1,9,6};
////死亡图
//const int GameScreenWnd::m_FishdeadCount[24] = {2,2,2,3,3,3,4,3,2,4,6,4,3,3,3,3,3,0,2,2,0,0,3,0};
//活图大小
/*---------------------------------------------------------------------------------------------------------------*/
//游动图
const int GameScreenWnd::m_FishmoveCount[24] = {6, 8, 12, 12, 12, 13, 12, 10, 12, 12, 8,15,12,15,13,12,12,12,13,12,9,6,9,3};
//  const int GameScreenWnd::m_FishmoveCount[24] = {6, 8, 12, 12, 12, 13, 12, 10, 12, 12, 11, 15, 12, 15, 13, 12, 12, 12, 13, 12, 9, 6, 9, 3};
//死亡图
const int GameScreenWnd::m_FishdeadCount[24] = {2, 2,  2,  3,  3,  3,  4,  3,  2,  4,  6, 4,   3,  3,  3,  3,  3,  3,  0,  3, 4, 0, 3, 0};
/*---------------------------------------------------------------------------------------------------------------*/
const int GameScreenWnd::m_FishmoveRec[24][2] = {{32,128},{32,64},{64,128},{64,128},{64,128},{128,128},{64,256},{128,256},{128,256},{128,256},{256,256},{256,256},{128,256},{256,256},{512,256},{256,512},{256,512},/*{256,256}*/{512,512},{256,512},{256,512},{256,512},/*{64,512}*/{512,256},{512,1024},{256,256}/*{512,256}*/};
//死图大小
const int GameScreenWnd::m_FishDeadRec[24][2] = {{32,128},{32,64},{64,128},{64,128},{64,128},{128,128},{128,256},{128,256},{128,256},{256,256},{256,256},{256,256},{128,256},{256,256},{512,256},{256,512},{256,512},/*{256,256}*/{512,512},{256,512},{256,512},{256,512},/*{64,512}*/{512,256},{512,1024},{256,256}};
//
//const int GameScreenWnd::m_fishscore[24]={2,2,3,4,5,6,7,8,9,12,10,15,18,20,25,30,40,100,40,70,300,120,200,150};

/*---------------------------------------------------------------------------------------------------------------*/
//生成金币个数
const int GameScreenWnd::m_iFishMoneyCount[24]	={	1/*0*/,1/*1*/,2/*2*/,2/*3*/,3/*4*/,3/*5*/,3/*6*/,4/*7*/,4/*8*/,
													4/*9*/,4/*10*/,5/*11*/,5/*12*/,5/*13*/,5/*14*/,5/*15*/,6/*16*/,
													6/*17*/,6/*18*/,6/*19*/,6/*20*/,6/*21*/,7/*22*/,7/*23*/};
/*---------------------------------------------------------------------------------------------------------------*/
//抖屏位置
const int G_iShakePosition[5][2] = {{0,0},{-10,5},{-10,10},{10,15},{15,10}};
/*---------------------------------------------------------------------------------------------------------------*/
char G_szFishName[24][30] = {{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"乌龟"},
								{"小鱼"},{"小鱼"},{"小鱼"},{"剑鱼"},{"魔鬼鱼"},{"大银鲨"},{"大金鲨"},{"飞鱼"},{"龙虾"},{"蓝海豚"},
								{"金龙"},{"金蟾"},{"巨型黄金鲨"},{"深海怪物"}};
//char G_szFishName[24][30] = {	{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"小鱼"},{"乌龟"},
//								{"小鱼"},{"小鱼"},{"小鱼"},{"剑鱼"},{"魔鬼鱼"},{"炸弹银鲨"},{"炸弹黄金鲨"},{"黄金盒"},{"双头四脚怪"},{"黄金四脚怪"},
//								{"孙悟空"},{"金箍棒"},{"巨型黄金鲨"},{"金蟾"}};
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
	m_sUserName.Format("红鸟网络客服电话：4008819308");
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

	m_iMoveY = 0;
	m_iColorR = 255;
	m_iColorB = 10;
	m_iColorG = 10;

	//广播数据
	m_dqBroadCast.clear();
}

GameScreenWnd::~GameScreenWnd(void)
{
	CleanAllMemory();
}
/*---------------------------------------------------------*/
//获得客户端接口指针
void GameScreenWnd::SetGameClientP(CNewGameClient *p)
{
	m_pGameClient = p;
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
		pDxDialog->Render();
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

	//降低cpu使用率
	 pDxDialog->LoadImg();

	return;
}
/*---------------------------------------------------------*/
//载入资源
void GameScreenWnd::LoadImg()
{
   /*****开始加载资源******/	

	//载入背景资源
	CString bgImgPath;

	//载入背景
	char strPath[50] = {0};
	for(int i=0;i<3;i++)
	{
		sprintf(strPath,"Jcfish/Ui/BG/Bgimg%d.png",i);
		LoadImg(strPath,&m_bgImgTexture[i]);
		m_HaveLoadNum++;
		
	}
	//加载边框
	bgImgPath = "Jcfish/Ui/BG/Border.png";
    for(int n=0;n<10;n++)
	  if(!FAILED( (D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_BorderImgTexture))))
	      break;

	  m_HaveLoadNum++;
	//加载炮底座
	
	sprintf(strPath,"Jcfish/Ui/PAO/PaoTai.png");
	LoadImg(strPath,&m_PaoConImgTexture);
	m_HaveLoadNum++;

	  //加载炮底座
	sprintf(strPath,"Jcfish/Ui/PAO/PaoBG.png");
	LoadImg(strPath,&m_PaoBgImgTexture);
	m_HaveLoadNum++;

	  //加载炮底座
	sprintf(strPath,"Jcfish/Ui/PAO/PaoTwo.png");
	LoadImg(strPath,&m_PaoTwoImgTexture);
	m_HaveLoadNum++;
	//炮火图
	sprintf(strPath,"Jcfish/Ui/Pao/PaoTwoFire.png");
	LoadImg(strPath,&m_PaoTwoFireImgTexture);
	m_HaveLoadNum++;
	//
	sprintf(strPath,"Jcfish/Ui/Pao/PaoThreeFire.png");
	LoadImg(strPath,&m_PaoThreeFireImgTexture);
	m_HaveLoadNum++;
	//
	sprintf(strPath,"Jcfish/Ui/Pao/PaoThree.png");
	LoadImg(strPath,&m_PaoThreeImgTexture);
	m_HaveLoadNum++;

    for(int i=0;i<PLAY_COUNT;i++)
	{
	   m_HaveLoadNum++;
	   bgImgPath.Format("Jcfish/Ui/shotNumBack/shotNumBack%d.png",i);
	   for(int n=0;n<10;n++)
	     if(!FAILED( (D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_allscoreBgImgTexture[i]))))
	        break;
	}
	//数字图片
	for(int i=0;i<5;i++)
	{
		m_HaveLoadNum++;
	    bgImgPath.Format("Jcfish/Ui/Num/goldNum%d.png",i);
		for(int n=0;n<10;n++)
           if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_NumImgTexture[i])))
	          break;
	}
	//子弹图
	m_HaveLoadNum++;
	bgImgPath = "Jcfish/Ui/PAO/shot_0.png";
	for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_BulltTwoTexture[0])))
	        break;
	//超级状态图
	m_HaveLoadNum++;
	bgImgPath = "Jcfish/Ui/PAO/supPao.png";
	for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_SupStatTexture)))
	        break;

	bgImgPath = "Jcfish/Ui/PAO/shot_1.png";
	for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_BulltTwoTexture[1])))
	        break;
	
	bgImgPath = "Jcfish/Ui/PAO/net_0.png";
	for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_PaoTwoNetImgTexture)))
	        break;
	bgImgPath = "Jcfish/Ui/other/GAME_TIP.png";
	for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_OutTexture)))
	        break;
    bgImgPath = "Jcfish/Ui/Num/gold.png";
	for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_GoldImgTexture)))
	        break;
	
	bgImgPath = "Jcfish/Ui/Num/qianbi.png";
    for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_BiTexture)))
			 break;

	bgImgPath = "Jcfish/Ui/other/cursor.png";
    for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_CursorTexture)))
			 break;
	

	m_HaveLoadNum++;
	//载入撒钱动画图
	for(int i=0;i<22;i++)
	{
		char strPath[50] = {0};
		sprintf(strPath,"Jcfish/Ui/qian/%d.png",i);
		LoadImg(strPath,&m_MuchMoneyTexture[i]);
		m_HaveLoadNum++;
		
	}
	//定住动画图
	for(int i=0;i<10;i++)
	{
		char strPath[50] = {0};
		sprintf(strPath,"Jcfish/Ui/ding/%d.png",i);
		LoadImg(strPath,&m_DingTexture[i]);
		m_HaveLoadNum++;
		
	}

	//波浪图
	//浪潮图
	for(int i=0;i<2;i++)
	{
		m_HaveLoadNum++;
	    bgImgPath.Format("Jcfish/Ui/WaterWave/%d.png",i);
		for(int n=0;n<10;n++)
          if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xffffffff,NULL,NULL,&m_WaterWaveTexture[i])))
	          break;
	}
	//超级子弹图
	for(int i=0;i<2;i++)
	{
		m_HaveLoadNum++;
	    bgImgPath.Format("Jcfish/Ui/Pao/shot_%d_8.png",i);
		for(int n=0;n<10;n++)
          if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xffffffff,NULL,NULL,&m_SupButTexture[i])))
	          break;
	}
    //水面波浪
	for(int i=0;i<32;i++)
	{
	 m_HaveLoadNum++;
	 bgImgPath.Format("Jcfish/Ui/Water/%d.png",i);
	 for(int n=0;n<10;n++)
        if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_WaterTexture[i])))
			break;
	}

	//园环图
	for(int i=0;i<3;i++)
	{
		m_HaveLoadNum++;
	    bgImgPath.Format("Jcfish/Ui/quan/%d.png",i);
		for(int n=0;n<10;n++)
          if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_QuanTexture[i])))
			break;
	}
	//载入鱼游动图
   for(int i=0;i<24;i++)
	{
	    for(int j=0;j<m_FishmoveCount[i];j++)
		{
			    
			    sprintf(strPath,"Jcfish/Ui/fish/%d/%d.png",i,j);
				LoadImg(strPath,&m_FishMoveTexture[i][j]);
			    m_HaveLoadNum++;
		    
		}
		//死亡图
		for(int j=0;j<m_FishdeadCount[i];j++)
		{
			
  		    sprintf(strPath,"Jcfish/Ui/fish/%d/d%d.png",i,j);
			LoadImg(strPath,&m_FishDeadTexture[i][j]);
			m_HaveLoadNum++;
		    
		}
	}
	
	//子弹和鱼检测
	SetTimer(TIMERIDBULLTE,1,NULL);
	//鱼儿游动图序列
	SetTimer(TIMERIDFISHMOVE,150,NULL);
	//子弹撒网动画
	SetTimer(TIMERIDNETOPEN,20,NULL);
	//
	
	//掉钱动画
	SetTimer(TIMERIDMONEYACTION,100,NULL);
	//
	SetTimer(TIMERIDUSERMONEY,100,NULL);
	//
	//
	SetTimer(TIMERIDOUT,1000,NULL);
	
    //载入声音
    LoadSound();

    m_InitScreen = true;
	
	/*****加载资源结束******/	

}
//初始窗口
bool GameScreenWnd::InitDxDlog()
{
    m_hWnd=this->GetSafeHwnd();
	
	if(m_hWnd==NULL)AfxMessageBox("创建失败!");

	if( SUCCEEDED( InitD3D( m_hWnd ) ) )
    {
	   if( SUCCEEDED( TRUE ) )
        {
			 m_CursorInt = ShowCursor(TRUE);
			 m_RunRender = true;
			 m_hDxThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DxThreadRender,this,0,0);	
			 m_hWorkThread1=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DxThreadRender1,this,0,0);	
			 //SetTimer(100,100,NULL);
			 SetTimer(TIMERIDFISHROUND,1,NULL);
			 m_hWorkThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DxThreadLoad,this,0,0);
			 //LoadImg();
		}
	}  
	//按扭区域
	m_BtRect[0].top = 3;m_BtRect[0].left = 1092;m_BtRect[0].right = m_BtRect[0].left+ 58;m_BtRect[0].bottom = m_BtRect[0].top+24;
	m_BtRect[2].top = 3;m_BtRect[2].left = 1151;m_BtRect[2].right = m_BtRect[2].left+ 49;m_BtRect[2].bottom = m_BtRect[2].top+24;
	m_BtRect[1].top = 3;m_BtRect[1].left = 1200;m_BtRect[1].right = m_BtRect[1].left+ 58;m_BtRect[1].bottom = m_BtRect[1].top+24;
	m_BtRect[3].top = 710;m_BtRect[3].left = 500;m_BtRect[3].right = m_BtRect[3].left+ 72;m_BtRect[3].bottom = m_BtRect[3].top+27;
	m_BtRect[4].top = 714;m_BtRect[4].left = 600;m_BtRect[4].right = m_BtRect[4].left+ 72;m_BtRect[4].bottom = m_BtRect[4].top+20;

    return true;
}

//创建DX窗口及载入资源纹理图片
HRESULT GameScreenWnd::InitD3D( HWND hWnd )
{
    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
        return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//创建Direct3D设备对象
    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp, &m_pd3dDevice )))
    {
		AfxMessageBox("设备对象失败!");
        return E_FAIL;
    }
	// 创建点精灵
	if (FAILED(D3DXCreateSprite( m_pd3dDevice, &m_pSprite )))
	{
		AfxMessageBox("创建点精灵失败!");
		return E_FAIL;
	}

	//设置文字
	m_Dxfont = NULL;
    D3DXFONT_DESCA lf;
    ZeroMemory(&lf, sizeof(D3DXFONT_DESCA));
    lf.Height = 12; //字体高度
    lf.Width = 0; // 字体宽度
    lf.Weight = 400; 
    lf.Italic = false;
	_sntprintf(lf.FaceName,sizeof(lf.FaceName),TEXT("宋体"));
    D3D_OK!=D3DXCreateFontIndirect(m_pd3dDevice, &lf, &m_Dxfont); //创建字体对象


	m_DxCastFont = NULL;
	D3DXFONT_DESCA lf1;
	ZeroMemory(&lf1, sizeof(D3DXFONT_DESCA));
	lf1.Height = 40; //字体高度
	lf1.Width = 15; // 字体宽度
	lf1.Weight = 400; 
	lf1.Italic = false;
	_sntprintf(lf1.FaceName,sizeof(lf1.FaceName),TEXT("宋体"));
	D3D_OK!=D3DXCreateFontIndirect(m_pd3dDevice, &lf1, &m_DxCastFont); //创建字体对象

	CString bgImgPath;
	//按扭图
	for(int i=0;i<5;i++)
	{
	    bgImgPath.Format("Jcfish/Ui/bt/bt%d.png",i);
		for(int n=0;n<10;n++)
           if(!FAILED(D3DXCreateTextureFromFileEx(m_pd3dDevice,bgImgPath,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_BtTexture[i])))
			   break;
	}

    D3DXCreateTextureFromFileEx(m_pd3dDevice,("Jcfish/Ui/BG/jindu.png"),D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_LoadTexture);
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/bg/bg.jpg",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_LoadBgTexture);

	//初始化设置窗口
	m_configDlg.InitDlg(m_pd3dDevice,445,150);
	return S_OK;

}
//渲染界面
void GameScreenWnd::Render()
{
	//清理绘制设备
	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(100,100,100),1.0f,0);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, false );
	//准备绘制
	m_pd3dDevice->BeginScene() ;
	//设置精灵模式
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	//还载加载界面
	if(m_InitScreen==false || m_bHaveLinked == false)
	{
		m_pSprite->Draw(m_LoadBgTexture, NULL, NULL, &D3DXVECTOR3(0, 0, 0), D3DCOLOR_ARGB(255,255,255,255));
		//绘制进度
		if(m_HaveLoadNum>360)m_HaveLoadNum = 360;
		CRect rcClient;
		rcClient.top= 0;
		rcClient.left=0;
		rcClient.right =  m_HaveLoadNum;
		rcClient.bottom = 15;
		m_pSprite->Draw(m_LoadTexture, rcClient, NULL, &D3DXVECTOR3(462, 621-190, 0), D3DCOLOR_ARGB(255,255,255,255));
		//绘制按扭
		DrawBt();
		//清理缓存
		m_pSprite->Flush();
		//绘制结束,停止精灵
		m_pSprite->End();
		//停止设备
		m_pd3dDevice->EndScene();
		//更换设备到最前
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		return;

	}

	//绘制背景
	//如果在切换场景，显示浪潮图
	if(m_IsChangeScreen)
	{
		int pRreBgindex = 0;
		if(m_Bgindex==0)pRreBgindex=2;
		else if(m_Bgindex==1)pRreBgindex=0;
		else if(m_Bgindex==2)pRreBgindex=1;
		else pRreBgindex=0;

		m_pSprite->Draw(m_bgImgTexture[pRreBgindex], NULL, NULL, &D3DXVECTOR3(0, 0, 0), D3DCOLOR_ARGB(255,255,255,255));
		CRect rcClient;
		rcClient.top=0;
		rcClient.left=1280-(1280-m_ChanwaveSt.m_ptx-100);
		rcClient.right =  1280;
		rcClient.bottom =  740;
		int bgx = m_ChanwaveSt.m_ptx+100;
		if(bgx<0)bgx = 0;
		if( rcClient.left<0) rcClient.left = 0;
		m_pSprite->Draw(m_bgImgTexture[m_Bgindex], rcClient, NULL, &D3DXVECTOR3(bgx, 0, 0), D3DCOLOR_ARGB(255,255,255,255));
		//绘制波纹
		m_pSprite->Draw(m_WaterWaveTexture[m_ChanwaveSt.m_currImgIndex], NULL, NULL, &D3DXVECTOR3(m_ChanwaveSt.m_ptx, 0, 0), D3DCOLOR_ARGB(255,255,255,255));
	}
	else
	{
		m_pSprite->Draw(m_bgImgTexture[m_Bgindex], NULL, NULL, &D3DXVECTOR3(m_iShakex, m_iShakey, 0), D3DCOLOR_ARGB(255,255,255,255));
	}

	D3DXMATRIX mat;
	D3DXMATRIX mat1;
	D3DXMATRIX mat2;
	//绘制鱼儿
	try
	{
		Fish *tfish=NULL;
		for(int i=0;i<m_FishFactory.GetActiveCount();i++)
		{
			//枚举鱼群
			tfish=m_FishFactory.EnumActiveObject(i);
			if(tfish==NULL)continue;
			if(!tfish->m_Have)
			{
				m_FishFactory.FreeItem(tfish);continue;

			}
			if(GetTickCount()-tfish->m_creatTime>50000)
			{
				m_FishFactory.FreeItem(tfish);continue;

			}
			if(!tfish->m_Have)continue;
			int  fishindex = tfish->m_index;

			D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(m_FishmoveRec[fishindex][0]/2,m_FishmoveRec[fishindex][1]/2),tfish->m_Roation*(D3DX_PI/180),&D3DXVECTOR2(tfish->m_ptx+m_iShakex,tfish->m_pty+m_iShakey));
			D3DXMatrixTransformation2D(&mat1,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(m_FishmoveRec[fishindex][0]/2,m_FishmoveRec[fishindex][1]/2),tfish->m_Roation*(D3DX_PI/180),&D3DXVECTOR2(tfish->m_ptx-10+m_iShakex,tfish->m_pty-10+m_iShakey));
			D3DXMatrixTransformation2D(&mat2,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(256/2,256/2),m_ActionIndex1%360*(D3DX_PI/180),&D3DXVECTOR2(tfish->m_ptx+m_FishmoveRec[fishindex][0]/2-256/2+m_iShakex,tfish->m_pty+m_FishmoveRec[fishindex][1]/2-256/2+m_iShakey));
			m_pSprite->Flush();
			//如果有原换
			if(tfish->m_smallFish!=-1)
			{
				m_pSprite->SetTransform(&mat2);
				m_pSprite->Draw(m_QuanTexture[tfish->m_smallFish],NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
			}


			CRect rcClient;
			rcClient.top=0;
			rcClient.left=0; 
			if(tfish->m_isHit==false||m_FishdeadCount[fishindex]==0)
			{

				rcClient.right = rcClient.left+m_FishmoveRec[fishindex][0];
				rcClient.bottom = rcClient.top+m_FishmoveRec[fishindex][1];
				m_pSprite->SetTransform(&mat1);
				m_pSprite->Draw(m_FishMoveTexture[fishindex][tfish->m_actionindex%m_FishmoveCount[fishindex]],rcClient, NULL, NULL, D3DCOLOR_ARGB(80,0,0,0));
				m_pSprite->SetTransform(&mat);
				m_pSprite->Draw(m_FishMoveTexture[fishindex][tfish->m_actionindex%m_FishmoveCount[fishindex]],rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));

			}
			else
			{
				m_pSprite->SetTransform(&mat);
				rcClient.right = rcClient.left+m_FishDeadRec[fishindex][0];
				rcClient.bottom = rcClient.top+m_FishDeadRec[fishindex][1];
				m_pSprite->Draw(m_FishDeadTexture[fishindex][tfish->m_actionindex%m_FishdeadCount[fishindex]],rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
			}


		}
	}
	catch(...)
	{
		OutputDebugString("dwjjcby::渲染出错1");
		//AfxMessageBox(L"出错！");
	}

	//绘制自己子弹
	for(int i=0;i<200;i++)
	{
		if(!m_MeBullet[i].m_Have)continue;
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(32,31.0f),m_MeBullet[i].m_Roation*(D3DX_PI/180),&D3DXVECTOR2(m_MeBullet[i].m_ptx,m_MeBullet[i].m_pty));
		m_pSprite->Flush();
		m_pSprite->SetTransform(&mat);
		CRect rcClient;
		if(!m_MeBullet[i].m_Stop)
		{
			//如果超级炮发色
			if(m_MeBullet[i].m_issuper)
			{
				rcClient.top=0;
				rcClient.left=(m_ActionIndex%2)*64;
				rcClient.right = rcClient.left+128/2;
				rcClient.bottom = rcClient.top+64;
				m_pSprite->Draw(m_SupButTexture[m_MeBullet[i].m_isThree],rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));

			}
			else
			{
				rcClient.top=m_MeBullet[i].m_Senduser*384/6;
				rcClient.left=(m_ActionIndex%2)*64;
				rcClient.right = rcClient.left+128/2;
				rcClient.bottom = rcClient.top+384/6;
				m_pSprite->Draw(m_BulltTwoTexture[m_MeBullet[i].m_isThree],rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
			}
		}
		else
		{
			int  now_x = m_MeBullet[i].m_ptx - 161+32 ;
			int  now_y = m_MeBullet[i].m_pty - 161+32 ;

			D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(161,161.0f),m_MeBullet[i].m_Roation*(D3DX_PI/180),&D3DXVECTOR2(now_x,now_y));
			m_pSprite->SetTransform(&mat);
			rcClient.left=322*m_MeBullet[i].m_Netindex;
			rcClient.top=0;
			rcClient.right = rcClient.left+322;
			rcClient.bottom = rcClient.top+322;
			if(m_MeBullet[i].m_issuper)
			{
				m_pSprite->Draw(m_PaoTwoNetImgTexture,rcClient, NULL, NULL, D3DCOLOR_ARGB(255,230,140,0));
			}
			else
			{
				if(m_MeBullet[i].m_RealUser == m_MeRellayChairID)
				{
					m_pSprite->Draw(m_PaoTwoNetImgTexture,rcClient, NULL, NULL, D3DCOLOR_ARGB(240,0,250,250));
				}
				else
				{
					m_pSprite->Draw(m_PaoTwoNetImgTexture,rcClient, NULL, NULL, D3DCOLOR_ARGB(240,255,255,255));
				}
				
			}
		}
	}
	//绘制水面波浪，需要平铺15张图
	int BowenX = 0;
	int BowenY = 0;
	//还原角度为正常
	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&matRot);//矩阵
	m_pSprite->SetTransform(&matRot);
	for(int i=0;i<15;i++)
	{   
		BowenX = i * 300;
		BowenY = 0;
		if(i>4){BowenY=300;BowenX = (i-5) * 300;}
		if(i>9){BowenY=600;BowenX = (i-10) * 300;}
		m_pSprite->Draw(m_WaterTexture[m_ActionIndex%32], NULL, NULL, &D3DXVECTOR3(BowenX, BowenY, 0), D3DCOLOR_ARGB(200,255,255,255));

	}

	//绘制捕获中的鱼儿的分数
	for(int i=0;i<100;i++)
	{
		if(!m_NumArr[i].m_Have)continue;
		int nCellCount = 0;
		BYTE byCell[10];
		LONG lCellScore = m_NumArr[i].m_Numcount;
		if(lCellScore<=0)continue;

		if(lCellScore>0)
		{
			for(int i = 0;lCellScore>0;lCellScore=lCellScore/10,i++)
			{          
				byCell[i]=lCellScore%10;//从个位开始存每位数
				nCellCount++;
			}
		}
		int temppointx = 0;
		int m_jiaodu = 0;
		//如果是对面那么就要转换180度
		if(m_NumArr[i].m_HitUser>=3){m_jiaodu=180;}
		int  m_xPianyi = nCellCount*15*m_NumArr[i].m_beishu/2;
		int  m_xWidht = 24;
		if(m_NumArr[i].m_beishu>1.5)m_xWidht = 54;
		for( int j = nCellCount-1; j >=0; j-- )
		{
			CRect rcClient;
			rcClient.top=0;
			rcClient.left=(byCell[j])*m_xWidht;
			rcClient.bottom= rcClient.top + m_xWidht ;
			rcClient.right=  rcClient.left +m_xWidht;
			int now_x = m_NumArr[i].m_ptx+temppointx-m_xPianyi;
			int now_y = m_NumArr[i].m_pty;

			if(m_NumArr[i].m_Time<20)
			{
				if(m_NumArr[i].m_HitUser>=3)
					now_y=now_y+(m_NumArr[i].m_Time*4);
				else
					now_y=now_y-(m_NumArr[i].m_Time*4);
			}
			else if(m_NumArr[i].m_Time>=20&&m_NumArr[i].m_Time<30)
			{
				if(m_NumArr[i].m_HitUser>=3)
					now_y=now_y+80-((m_NumArr[i].m_Time-20)*4);
				else
					now_y=now_y-80+((m_NumArr[i].m_Time-20)*4);
			}
			else
			{
				if(m_NumArr[i].m_HitUser>=3)
					now_y=now_y+40;
				else
					now_y=now_y-40;

			}
			if(m_NumArr[i].m_beishu>1.5)
				D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(0.8f,0.8f),&D3DXVECTOR2(53.0f,12.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(now_x+m_iShakex,now_y+m_iShakey));
			else
				D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(m_NumArr[i].m_beishu,m_NumArr[i].m_beishu),&D3DXVECTOR2(53.0f,12.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(now_x+m_iShakex,now_y+m_iShakey));
			m_pSprite->SetTransform(&mat);
			if(m_NumArr[i].m_beishu>1.5)
				m_pSprite->Draw(m_NumImgTexture[4], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
			else
				m_pSprite->Draw(m_NumImgTexture[2], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	

			if(m_NumArr[i].m_HitUser>=3)
				temppointx = temppointx - 15*m_NumArr[i].m_beishu;
			else
				temppointx = temppointx + 15*m_NumArr[i].m_beishu;
		}

	}
	//定住
	if(m_DingStruc.m_have)
	{
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(1024/2,512/2),0*(D3DX_PI/180),&D3DXVECTOR2(m_DingStruc.m_ptx,m_DingStruc.m_pty));
		m_pSprite->Flush();
		m_pSprite->SetTransform(&mat);
		//如果于22就撒钱
		if(m_DingStruc.m_nowindex<10)
		{
			m_pSprite->Draw(m_DingTexture[m_DingStruc.m_nowindex],NULL, NULL, NULL, D3DCOLOR_ARGB(255,200,200,255));
		}
	}
	//掉钱动画
	for(int i=0;i<6;i++)
	{
		if(!m_MuchmoneyAct[i].m_have)continue;
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(1024/2,512/2),0*(D3DX_PI/180),&D3DXVECTOR2(m_MuchmoneyAct[i].m_ptx+m_iShakex,m_MuchmoneyAct[i].m_pty+m_iShakey));
		m_pSprite->Flush();
		m_pSprite->SetTransform(&mat);
		//如果于22就撒钱
		if(m_MuchmoneyAct[i].m_nowindex<22)
		{
			m_pSprite->Draw(m_MuchMoneyTexture[m_MuchmoneyAct[i].m_nowindex],NULL, NULL, NULL, D3DCOLOR_ARGB(255,200,200,255));
		}
		//否则就开始转换了
		else 
		{
			int ptx = m_UserPT[i].x-60;
			int pty = m_UserPT[i].y-255;
			if(i>=3)pty = pty +400;
			D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(310/2,310/2),m_MuchmoneyAct[i].m_Roation*(D3DX_PI/180),&D3DXVECTOR2(ptx+m_iShakex,pty+m_iShakey));
			m_pSprite->SetTransform(&mat);
			m_pSprite->Draw(m_QuanTexture[2],NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
			//绘制环上数字
			int nCellCount = 0;
			BYTE byCell[10];
			LONG lCellScore = m_MuchmoneyAct[i].m_DiuMoney;
			if(lCellScore>0)
			{
				for(int i = 0;lCellScore>0;lCellScore=lCellScore/10,i++)
				{          
					byCell[i]=lCellScore%10;//从个位开始存每位数
					nCellCount++;
				}
			}
			else
			{
				nCellCount =1;
				byCell[0]=0;
			}
			int temppointx = 0;
			ptx =  ptx  + 310/2-(nCellCount*45)/2;
			if(i<3)
			{
				for( int j = nCellCount-1; j >=0; j-- )
				{
					CRect rcClient;
					rcClient.top=0;
					rcClient.left=(byCell[j])*45;
					rcClient.bottom= rcClient.top + 51 ;
					rcClient.right=  rcClient.left +45;
					D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(22.5f,25.5f),0*(D3DX_PI/180),&D3DXVECTOR2(ptx+temppointx+m_iShakex,pty+140+m_iShakey));
					m_pSprite->SetTransform(&mat);
					m_pSprite->Draw(m_NumImgTexture[1], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
					temppointx = temppointx + 45;
				}
			}
			else
			{
				for( int j = 0; j <nCellCount; j++ )
				{
					CRect rcClient;
					rcClient.top=0;
					rcClient.left=(byCell[j])*45;
					rcClient.bottom= rcClient.top + 51 ;
					rcClient.right=  rcClient.left +45;
					D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(22.5f,25.5f),180*(D3DX_PI/180),&D3DXVECTOR2(ptx+temppointx+m_iShakex,pty+135+m_iShakey));
					m_pSprite->SetTransform(&mat);
					m_pSprite->Draw(m_NumImgTexture[1], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
					temppointx = temppointx + 45;
				}
			}

		}
	}

	//绘制玩家
	for(int i=0;i<PLAY_COUNT;i++)
	{  
		//绘制炮台
		if(!m_HaveUser[i])continue;
		int m_jiaodu = 0;
		int m_userpoajiaodu = int(m_UserPaoJiaodu[i]);
		int m_bgjuli = 60;
		int m_fenjuli  = 100;
		//如果是对面那么就要把炮转换180度
		if(i>=3){m_jiaodu=180;m_bgjuli =  40;m_fenjuli=77;}

		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(128.0f,128.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPT[i].x-28,m_UserPT[i].y-28));
		m_pSprite->SetTransform(&mat);
		m_pSprite->Draw(m_PaoConImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
		//炮台
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(128.0f,128.0f),m_userpoajiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPaoPT[i].x-28,m_UserPaoPT[i].y-28));
		m_pSprite->SetTransform(&mat);
		//炮
		if(m_UserSendstate[i])
		{
			if(m_BeiLv[i]>500) 
			{
				if(m_UserSuperPao[i])
				{
					D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(130.0f,130.0f),m_userpoajiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPaoPT[i].x-30,m_UserPaoPT[i].y-30));
					m_pSprite->SetTransform(&mat);
					m_pSprite->Draw(m_PaoThreeFireImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
				}
				else
				{
					m_pSprite->Draw(m_PaoThreeFireImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
				}
			}
			else
			{
				if(m_UserSuperPao[i])
				{
					D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(130.0f,130.0f),m_userpoajiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPaoPT[i].x-30,m_UserPaoPT[i].y-30));
					m_pSprite->SetTransform(&mat);
					m_pSprite->Draw(m_PaoThreeFireImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
				}
				else
				{
					m_pSprite->Draw(m_PaoThreeFireImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));

				}


			}
		}
		if(m_BeiLv[i]>500) 
		{
			if(m_UserSuperPao[i])
			{

				D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(130.0f,130.0f),m_userpoajiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPaoPT[i].x-30,m_UserPaoPT[i].y-30));
				m_pSprite->SetTransform(&mat);
				m_pSprite->Draw(m_PaoThreeImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,0,0));
			}
			else
				m_pSprite->Draw(m_PaoThreeImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
		}
		else
		{
			if(m_UserSuperPao[i])
			{
				D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(130.0f,130.0f),m_userpoajiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPaoPT[i].x-30,m_UserPaoPT[i].y-30));
				m_pSprite->SetTransform(&mat);
				m_pSprite->Draw(m_PaoTwoImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,0,0));
			}
			else
				m_pSprite->Draw(m_PaoTwoImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));


		}
		//倍率背景
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(128.0f,128.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPT[i].x-28,m_UserPT[i].y-28));
		m_pSprite->SetTransform(&mat);
		m_pSprite->Draw(m_PaoBgImgTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));
		//玩家倍率
		{
			int nCellCount = 0;
			BYTE byCell[10];
			LONG lCellScore = m_BeiLv[i];
			if(lCellScore>0)
			{
				for(int i = 0;lCellScore>0;lCellScore=lCellScore/10,i++)
				{          
					byCell[i]=lCellScore%10;//从个位开始存每位数
					nCellCount++;
				}
			}
			else
			{
				nCellCount =1;
				byCell[0]=0;
			}
			int temppointx = 0;
			int ajuli  = 12;
			if(i>=3)ajuli = -12;

			for( int j = nCellCount-1; j >=0; j-- )
			{
				int Thisx = m_UserPT[i].x + 100 - 16 * nCellCount/2 + temppointx - 8;
				if(i>=3)Thisx =  Thisx + nCellCount*16/2 + 16 ;
				CRect rcClient;
				rcClient.top=0;
				rcClient.left=(byCell[j])*24;
				rcClient.bottom= rcClient.top + 23 ;
				rcClient.right=  rcClient.left +24;
				D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(12.0f,12.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(Thisx,m_UserPT[i].y+m_fenjuli+ajuli));
				m_pSprite->SetTransform(&mat);
				m_pSprite->Draw(m_NumImgTexture[2], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
				if(i>=3)
					temppointx = temppointx - 16;
				else
					temppointx = temppointx + 16;
			}

		}

		//绘制分数背景
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(102.0f,51.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPT[i].x+150,m_UserPT[i].y+m_bgjuli));
		m_pSprite->SetTransform(&mat);
		m_pSprite->Draw(m_allscoreBgImgTexture[i], NULL, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));

		//玩家上分
		int nCellCount = 0;
		BYTE byCell[10];
		LONG lCellScore = m_UserScore[i];
		if(lCellScore>0)
		{
			for(int i = 0;lCellScore>0;lCellScore=lCellScore/10,i++)
			{          
				byCell[i]=lCellScore%10;//从个位开始存每位数
				nCellCount++;
			}
		}
		else
		{
			nCellCount =1;
			byCell[0]=0;
		}
		int temppointx = 0;
		for( int j = nCellCount-1; j >=0; j-- )
		{
			CRect rcClient;
			rcClient.top=0;
			rcClient.left=(byCell[j])*16;
			rcClient.bottom= rcClient.top + 23 ;
			rcClient.right=  rcClient.left +16;
			D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(53.0f,12.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPT[i].x+200+temppointx,m_UserPT[i].y+m_fenjuli));
			m_pSprite->SetTransform(&mat);
			m_pSprite->Draw(m_NumImgTexture[0], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
			if(i>=3)
				temppointx = temppointx - 16;
			else
				temppointx = temppointx + 16;
		}
		//当前中心点

		D3DXMATRIX matRot;
		D3DXMatrixIdentity(&matRot);//矩阵
		m_pSprite->SetTransform(&matRot);
		//玩家钱堆
		for(int j=0;j<3;j++)
		{
			m_pSprite->SetTransform(&matRot);
			if(!m_UserStruct[i][j].m_Have)continue;
			int m_count = m_UserStruct[i][j].m_Nowscore;
			//设置堆着的币最多50个 要不是就冲到对
			if(m_count>50)m_count = 50;
			int m_henx = j * 40;
			int m_heny = 0-m_count * 4 - 20;;
			for(int n=0;n<m_count;n++)
			{
				int m_ydx = 140;
				int m_hen = 0;
				if(i>=3)
				{
					m_hen = m_hen + n*4;
					m_ydx = 54;
				}
				else 
				{
					m_hen = m_hen - n*4;
				}

				m_pSprite->Draw(m_BiTexture, NULL, NULL, &D3DXVECTOR3(m_UserPT[i].x-30-m_henx, m_UserPT[i].y +m_ydx+m_hen , 0), D3DCOLOR_ARGB(255,255,255,255));	

			}

			//玩家上分
			int nCellCount = 0;
			BYTE byCell[10];
			LONG lCellScore =m_UserStruct[i][j].m_Money;
			if(lCellScore>0)
			{
				for(int n = 0;lCellScore>0;lCellScore=lCellScore/10,n++)
				{          
					byCell[n]=lCellScore%10;//从个位开始存每位数
					nCellCount++;
				}
			}
			int temppointx = 0;
			if(i>=3)m_heny = -m_heny-140;
			for( int n = nCellCount-1; n >=0; n-- )
			{
				CRect rcClient;
				rcClient.top=0;
				rcClient.left=(byCell[n])*12;
				rcClient.bottom= rcClient.top + 24 ;
				rcClient.right=  rcClient.left +12;
				int m_ydx = 30;
				int m_ydy = 140;
				if(i>=3)
				{
					m_ydx = 1;
					m_ydy = 180;
				}
				else 
				{
					m_ydx = -30;
					m_ydy = 140;
				}
				D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(6.0f,12.0f),m_jiaodu*(D3DX_PI/180),&D3DXVECTOR2(m_UserPT[i].x+m_ydx-m_henx+temppointx,m_UserPT[i].y +m_ydy+m_heny));
				m_pSprite->SetTransform(&mat);
				m_pSprite->Draw(m_NumImgTexture[3], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
				if(i>=3)
					temppointx = temppointx - 12;
				else
					temppointx = temppointx + 12;
			}		

		}

	}

	//绘制飘落金币
	for(int i=0;i<100;i++)
	{
		if(!m_GoldArr[i].m_Have)continue;
		D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(0.8f,0.8f),&D3DXVECTOR2(72,62.0f),0*(D3DX_PI/180),&D3DXVECTOR2(m_GoldArr[i].m_ptx+m_iShakex,m_GoldArr[i].m_pty+m_iShakey));
		m_pSprite->Flush();
		m_pSprite->SetTransform(&mat);
		CRect rcClient;
		rcClient.top=0;
		rcClient.left=1066/13*m_GoldArr[i].m_actionindex;
		rcClient.right = rcClient.left+1066/13;
		rcClient.bottom = rcClient.top+79;
		m_pSprite->Draw(m_GoldImgTexture,rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));


	}

	D3DXMatrixIdentity(&matRot);//矩阵
	m_pSprite->SetTransform(&matRot);
	if(m_IsMustOut)
	{
		m_pSprite->Draw(m_OutTexture, NULL, NULL, &D3DXVECTOR3(180, 300, 0), D3DCOLOR_ARGB(255,255,255,255));
		int nCellCount = 0;
		BYTE byCell[10];
		LONG lCellScore = m_OutTime;
		if(lCellScore>0)
		{
			for(int i = 0;lCellScore>0;lCellScore=lCellScore/10,i++)
			{          
				byCell[i]=lCellScore%10;//从个位开始存每位数
				nCellCount++;
			}
		}
		else
		{
			nCellCount =1;
			byCell[0]=0;
		}
		int temppointx = 0;
		for( int j = nCellCount-1; j >=0; j-- )
		{
			CRect rcClient;
			rcClient.top=0;
			rcClient.left=(byCell[j])*16;
			rcClient.bottom= rcClient.top + 23 ;
			rcClient.right=  rcClient.left +16;
			D3DXMatrixTransformation2D(&mat,NULL,0.0f,&D3DXVECTOR2(1.0f,1.0f),&D3DXVECTOR2(53.0f,12.0f),NULL,&D3DXVECTOR2(740+temppointx,320));
			m_pSprite->SetTransform(&mat);
			m_pSprite->Draw(m_NumImgTexture[0], rcClient, NULL, NULL, D3DCOLOR_ARGB(255,255,255,255));	
			temppointx = temppointx + 16;
		}
	}
	//还原角度为正常
	m_pSprite->SetTransform(&matRot);
	//绘制边框
	m_pSprite->Draw(m_BorderImgTexture, NULL, NULL, &D3DXVECTOR3(0, 0, 0), D3DCOLOR_ARGB(255,255,255,255));

	//绘制帐户及积分信息
	CRect rcClient;
	rcClient.top=719;
	rcClient.left=70;
	rcClient.bottom= rcClient.top + 24 ;
	rcClient.right=  rcClient.left +150;

	LPCWSTR strLpc = CA2W(m_sUserName);
	m_Dxfont->DrawTextW(m_pSprite,(strLpc),  wcslen(strLpc), &rcClient, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255,145,240,255));


	//积分
	rcClient.top=719;
	rcClient.left=256;
	rcClient.bottom= rcClient.top + 24 ;
	rcClient.right=  rcClient.left +150;
	TCHAR strLpc1[32];
	wsprintf(strLpc1,"%I64d",m_i64MyMoney);
	m_Dxfont->DrawTextW(m_pSprite,CA2W(strLpc1),  wcslen(CA2W(strLpc1)), &rcClient, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255,0,255,255));

	//绘制广播
	if(m_dqBroadCast.size()>0 && m_dqBroadCast.at(0).bHave)
	{
		rcClient.top=CLIENT_HEIGHT/2+m_iShakey - m_iMoveY;
		rcClient.left=10+m_iShakex;
		rcClient.bottom= rcClient.top + 100 ;
		rcClient.right=  CLIENT_WIDTH - 20;
		TCHAR strLpc2[MAX_PATH];
		wsprintf(strLpc2,"系统消息:%s%d桌的%s打中了%s,获得%I64d鱼币奖励!",m_dqBroadCast.at(0).szRoomName,(m_dqBroadCast.at(0).iDeskNo+1),m_dqBroadCast.at(0).szNickName,m_dqBroadCast.at(0).szFishName,m_dqBroadCast.at(0).i64FishScore);
		m_DxCastFont->DrawTextW(m_pSprite,CA2W(strLpc2),  wcslen(CA2W(strLpc2)), &rcClient, DT_TOP | DT_CENTER , D3DCOLOR_ARGB(255,m_iColorR,m_iColorG,m_iColorB));
	}
	

	if(m_cursor.m_have == true)
	{
		m_pSprite->Draw(m_CursorTexture, NULL, NULL, &D3DXVECTOR3(m_cursor.m_ptx, m_cursor.m_pty, 0), D3DCOLOR_ARGB(255,255,255,255));
	}

	//绘制设置
	m_configDlg.InitRender(m_pSprite);

	//绘制按扭
	DrawBt();
	//清理缓存
	m_pSprite->Flush();

	//绘制结束,停止精灵
	m_pSprite->End();
	//停止设备
	m_pd3dDevice->EndScene();
	//更换设备到最前
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

}
//绘制按扭
void GameScreenWnd::DrawBt()
{
	CRect rcClient;
	rcClient.top=0;
	rcClient.left=0;
	if(m_IsMouseOver[0])rcClient.left=58;
	rcClient.bottom= rcClient.top + 24 ;
	rcClient.right=  rcClient.left +58;
	m_pSprite->Draw(m_BtTexture[0], rcClient, NULL, &D3DXVECTOR3(1092, 3, 0), D3DCOLOR_ARGB(255,255,255,255));
	rcClient.top=0;
	rcClient.left=0;
	if(m_IsMouseOver[1])rcClient.left=58;
	rcClient.bottom= rcClient.top + 24 ;
	rcClient.right=  rcClient.left +58;
	m_pSprite->Draw(m_BtTexture[1], rcClient, NULL, &D3DXVECTOR3(1200, 3, 0), D3DCOLOR_ARGB(255,255,255,255));
	rcClient.top=0;
	rcClient.left=0;
	if(m_IsMouseOver[2])rcClient.left=49;
	rcClient.bottom= rcClient.top + 24 ;
	rcClient.right=  rcClient.left +49;
	m_pSprite->Draw(m_BtTexture[2], rcClient, NULL, &D3DXVECTOR3(1151, 3, 0), D3DCOLOR_ARGB(255,255,255,255));
	
	//如果还未加载完，就不显示下面2个按扭
	if(!m_InitScreen)return;
	rcClient.top=0;
	rcClient.left=0;
	if(m_IsMouseOver[3])rcClient.left=72;
	rcClient.bottom= rcClient.top + 27 ;
	rcClient.right=  rcClient.left +72;
	m_pSprite->Draw(m_BtTexture[3], rcClient, NULL, &D3DXVECTOR3(500, 708, 0), D3DCOLOR_ARGB(255,255,255,255));
	rcClient.top=0;
	rcClient.left=0;
	if(m_IsMouseOver[4])rcClient.left=72;
	rcClient.bottom= rcClient.top + 20 ;
	rcClient.right=  rcClient.left +72;
	m_pSprite->Draw(m_BtTexture[4], rcClient, NULL, &D3DXVECTOR3(600, 712, 0), D3DCOLOR_ARGB(255,255,255,255));
	

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
	if(m_UserSuperPao[PlayerID])
	{
		if(m_configDlg.m_ptx2>0)
			PlayGameSound(61);
	}
	else 
	{ 
		//if(CBassMusicManager::GetInstance()->IsPlaying(50+PlayerID))CBassMusicManager::GetInstance()->RePlay(50+PlayerID, false);
		if(m_configDlg.m_ptx2>0)
			PlayGameSound(51);
	}
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

	m_dqBroadCast.push_back(TBroadCast);

	if(1 == m_dqBroadCast.size())
	{
		m_iMoveY = 0;
		m_iColorR = 255;
		m_iColorB = 10;
		m_iColorG = 10;
		//SetTimer(TIME_SHOW_MSG,5000,NULL);
	}
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
void GameScreenWnd::UserAddMoney(int PlayerID,int Fishid,int Money,int FishKindScord,bool ishavaction ,bool bMoneyPile)
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

	//是否抖屏
	if(Deadfish->m_index >=16 && m_configDlg.m_bShakeWindow)
	{
		OutputDebugString("dwjdsnh::抖屏");
		SetTimer(TIME_SHAKE,15,NULL);
		PlayGameSound(35);
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
	if(musciindex<9)
	 {	
		if(m_configDlg.m_ptx2>0)
			PlayGameSound(20+musciindex);
	 }
	else
	 {
		 if(m_configDlg.m_ptx2>0)
			 PlayGameSound(29);
		    
	 }	
	 if(FishKindScord>=20&&ishavaction)
		 GiveMuchMoney(Fishid,PlayerID,Money,Deadfish,FishKindScord);
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
	 int iCount = 0;
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


	 if(bMoneyPile)
	 {
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
    
}
//掉落金币
void GameScreenWnd::GiveMuchMoney(int Fishindx,int PlyaerID,int Money,Fish *Deadfish,int fishscore)
{
	m_MuchmoneyAct[PlyaerID].m_have = true;
	m_MuchmoneyAct[PlyaerID].m_nowindex = 0;
	m_MuchmoneyAct[PlyaerID].m_DiuMoney = Money;
	m_MuchmoneyAct[PlyaerID].m_ptx = Deadfish->m_ptx+m_FishmoveRec[Deadfish->m_index][0]/2-1024/2;
	m_MuchmoneyAct[PlyaerID].m_pty = Deadfish->m_pty+m_FishmoveRec[Deadfish->m_index][1]/2-512/2;
	if(m_configDlg.m_ptx2>0)
		PlayGameSound(41);
  
}
//切换场景
void GameScreenWnd::ChangeScreen(int bgindex)
{

	if(m_configDlg.m_ptx2>0)
		PlayGameSound(40);
	//播放背景音乐
	PlayBgSound(bgindex);

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

	
	m_configDlg.LMouseMove(point.x,point.y);

	//SetCursor(LoadCursor((HINSTANCE)GetWindowLong(this->GetSafeHwnd(),GWL_HINSTANCE),MAKEINTRESOURCE(IDC_CURSOR1)));
	SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));

	/*if(LoadCursor(NULL,MAKEINTRESOURCE(IDC_CURSOR1)))
	{
		OutputDebugString("dwjjcby::--------");
	}	
	else
	{
		OutputDebugString("dwjjcby::-------2222-");
	}*/

	if(point.y>710||point.y<30||point.x<5||point.x>1275||m_configDlg.m_isvisable)
	{
		m_cursor.m_have = false;
		if(m_CursorInt<0)
		{
			m_CursorInt = ShowCursor(TRUE);
		}
	}
	else 
	{
		if(m_InitScreen)
		{
			if(m_CursorInt>=0)m_CursorInt = ShowCursor(FALSE);
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
	for(int i=0;i<5;i++)
	{
		if(m_BtRect[i].PtInRect(point))
			m_IsMouseOver[i]=true;
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
		m_pGameClient->OnMoveWindow(point);
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
			m_pGameClient->OnUserShot(false,m_UserPaoJiaodu[m_MeChariID]);
			UserShoot(m_MeChariID,m_UserPaoJiaodu[m_MeChariID],m_MeRellayChairID,false);
			m_NowTime = GetTickCount() ;

		}
		SetTimer(TIMERIDSENDSHOOT,400,NULL);
		//设置为鼠标按下状态
		m_MouseDown = true;
	}
	//按扭事件
	if(m_InitScreen&&m_BtRect[4].PtInRect(point))
	{
		//上分
		m_pGameClient->AddOrRemoveScore(true,m_BeiLv[m_MeChariID]);
		if(m_configDlg.m_ptx2>0)
			PlayGameSound(15);
	}
	if(m_InitScreen&&m_BtRect[3].PtInRect(point))
	{
		//下分
		m_pGameClient->AddOrRemoveScore(false,m_BeiLv[m_MeChariID]);
		if(m_configDlg.m_ptx2>0)
			PlayGameSound(16);
	}
	//最小化
	if(m_BtRect[0].PtInRect(point))
	{
		m_pGameClient->OnMinWondow();
	}
	//退出
	if(m_BtRect[1].PtInRect(point))
	{
		//还没加载完不让退出 因为会导致客户端奔溃
		if(m_InitScreen==false || m_bHaveLinked == false)
		{
			return;
		}
		m_pGameClient->OnCloseWindow();
	}
	//设置
	if(m_BtRect[2].PtInRect(point))
	{
		m_configDlg.m_isvisable = true;
	}
}
/*----------------------------------------------------------------------------------*/
void	GameScreenWnd::OnLButtonUp(UINT, CPoint ponit)
{
	m_configDlg.LMouseUp(ponit.x,ponit.y);
	//设置鼠标状态为松开
	m_MouseDown =  false;
	
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
	if(!m_InitScreen || !m_bHaveLinked)
	{
		return ;
	}
	switch( (WCHAR)nChar )
	{
	case VK_SPACE://空格发射
		{
			if(GetTickCount() - m_NowTime > 400 )
			{
				//如果没钱直接弹出去
				if(m_UserScore[m_MeChariID]<=0)break;
				if(m_BeiLv[m_MeChariID]>m_UserScore[m_MeChariID]) return ; //edit by guojm
				m_pGameClient->OnUserShot(false,m_UserPaoJiaodu[m_MeChariID]);
				UserShoot(m_MeChariID,m_UserPaoJiaodu[m_MeChariID],m_MeRellayChairID,false);
				m_NowTime = GetTickCount() ;
				KillTimer(TIMERIDSENDSHOOT);
			}
			//SetTimer(TIMERIDSENDSHOOT,400,NULL);
			break;
		}
	case VK_UP://上键加炮
		{
			m_pGameClient->OnUserAddFire(true);
			break;
		}
	case VK_DOWN://下减炮
		{
			m_pGameClient->OnUserAddFire(false);
			break;
		}
	case VK_LEFT://左上分
		{
			m_pGameClient->AddOrRemoveScore(true,m_BeiLv[m_MeChariID]);
			if(m_configDlg.m_ptx2>0)
			{
				PlayGameSound(15);
			}
			break;
		}
	case VK_RIGHT://左下分
		{
			m_pGameClient->AddOrRemoveScore(false,m_BeiLv[m_MeChariID]);
			if(m_configDlg.m_ptx2>0)
			{
				PlayGameSound(16);
			}
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
	OutputDebugString("dwjjcby::键盘弹起");
}
/*----------------------------------------------------------------------------------*/
//时间事件
void GameScreenWnd::OnTimer(UINT nIDEvent)
{
	if(TIME_SHOW_MSG == nIDEvent)	//显示广播
	{
		if(m_dqBroadCast.size() > 0)
		{
			m_dqBroadCast.pop_front();
		}
		else
		{
			KillTimer(TIME_SHOW_MSG);
		}
	}
	else if(TIME_SHAKE == nIDEvent)
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

			float m_x = 8*sin(2*3.1415926*(m_MeBullet[i].m_Roation)/360.0);
			float m_y = 8*cos(2*3.1415926*(m_MeBullet[i].m_Roation)/360.0);
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


		if(m_dqBroadCast.size()>0)
		{
			m_iMoveY += 10;
			if(m_iMoveY >= CLIENT_HEIGHT/2)
			{
				m_iMoveY = 0;

				m_dqBroadCast.pop_front();
			}
			else
			{
				if(0 == m_iColorB)
				{
					m_iColorR = 255;
					m_iColorB = 255;
					m_iColorG = 255;
				}
				else
				{
					m_iColorR = 255;
					m_iColorB = 10;
					m_iColorG = 10;
				}
			}
		}
	}
	//玩家钱堆移动，就是旁边打死鱼儿后的3堆钱
	else if(nIDEvent == TIMERIDUSERMONEY)
	{
		for(int i=0;i<PLAY_COUNT;i++)
		{
			//判断是否有玩家
			if(!m_HaveUser[i])continue;
			for(int j=0;j<3;j++)
			{
				if(!m_UserStruct[i][j].m_Have)continue;
				if(j==0&&m_UserStruct[i][1].m_Have&&m_UserStruct[i][j].m_Time>=50)
				{
					m_UserStruct[i][j].m_Have = false;
					m_UserStruct[i][j].m_Have =  m_UserStruct[i][j+1].m_Have;
					m_UserStruct[i][j].m_Money =  m_UserStruct[i][j+1].m_Money;
					m_UserStruct[i][j].m_Time =  0;
					m_UserStruct[i][j].m_FishScore = m_UserStruct[i][j+1].m_FishScore;
					m_UserStruct[i][j].m_Nowscore =  m_UserStruct[i][j+1].m_Nowscore;
					m_UserStruct[i][j].m_Have = true;
					m_UserStruct[i][1].m_Have = false;

					if(m_UserStruct[i][2].m_Have)
					{
						m_UserStruct[i][1].m_Have = true;
						m_UserStruct[i][1].m_Have =  m_UserStruct[i][2].m_Have;
						m_UserStruct[i][1].m_Money =  m_UserStruct[i][2].m_Money;
						m_UserStruct[i][1].m_FishScore = m_UserStruct[i][2].m_FishScore;
						m_UserStruct[i][1].m_Nowscore =  m_UserStruct[i][2].m_Nowscore;
						m_UserStruct[i][1].m_Time =  0;
						m_UserStruct[i][2].m_Have = false;

					}

				}
				else if(j==0&&!m_UserStruct[i][1].m_Have&&m_UserStruct[i][j].m_Time>=50)
				{
					m_UserStruct[i][j].m_Have = false;
				}
				m_UserStruct[i][j].m_Time++;
			}
		}
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
			m_pGameClient->OnUserShot(false,m_UserPaoJiaodu[m_MeChariID]);
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
			m_pGameClient->OnCloseWindow();
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
    m_sUserName		= sNickName;
	m_i64MyMoney	= i64money;
	m_MeChariID		= MePlayerid;
	m_MeRellayChairID = MeRellid;
}
/*----------------------------------------------------------------------------------*/
//清理所有数据内存，退出时候使用
void GameScreenWnd::CleanAllMemory()
{
	m_RunRender = false;
   //释放纹理
	if(m_SupStatTexture != NULL)
		m_SupStatTexture->Release();

	if(m_LoadTexture != NULL)
		m_LoadTexture->Release();

	if(m_LoadBgTexture != NULL)
		m_LoadBgTexture->Release();

	if(m_BiTexture != NULL)
		m_BiTexture->Release();

	if(m_PaoTwoNetImgTexture != NULL)
		m_PaoTwoNetImgTexture->Release();

	if(m_GoldImgTexture != NULL)
		m_GoldImgTexture->Release();

	if(m_PaoThreeFireImgTexture != NULL)
		m_PaoThreeFireImgTexture->Release();

	if(m_PaoThreeImgTexture != NULL)
		m_PaoThreeImgTexture->Release();

	if(m_PaoTwoFireImgTexture != NULL)
		m_PaoTwoFireImgTexture->Release();

	if(m_PaoTwoImgTexture != NULL)
		m_PaoTwoImgTexture->Release();

	if(m_PaoBgImgTexture != NULL)
		m_PaoBgImgTexture->Release();

	if(m_PaoConImgTexture != NULL)
		m_PaoConImgTexture->Release();

	if(m_BorderImgTexture != NULL)
		m_BorderImgTexture->Release();


	for(int i=0;i<2;i++)
		if(m_SupButTexture[i] != NULL)
		   m_SupButTexture[i]->Release();
	for(int i=0;i<32;i++)
		if(m_WaterTexture[i] != NULL)
		   m_WaterTexture[i]->Release();

	for(int i=0;i<2;i++)
		if(m_WaterWaveTexture[i] != NULL)
		   m_WaterWaveTexture[i]->Release();

	for(int i=0;i<5;i++)
		if(m_BtTexture[i] != NULL)
		   m_BtTexture[i]->Release();

	for(int i=0;i<3;i++)
		if(m_QuanTexture[i] != NULL)
		   m_QuanTexture[i]->Release();

	for(int i=0;i<22;i++)
		if(m_MuchMoneyTexture[i] != NULL)
		   m_MuchMoneyTexture[i]->Release();

	for(int i=0;i<23;i++)
		 for(int j=0;j<10;j++)
			 if(m_FishDeadTexture[i][j] != NULL)
		         m_FishDeadTexture[i][j]->Release();

	for(int i=0;i<23;i++)
		 for(int j=0;j<10;j++)
			 if(m_FishMoveTexture[i][j] != NULL)
		         m_FishMoveTexture[i][j]->Release();


	for(int i=0;i<2;i++)
		if(m_BulltTwoTexture[i] != NULL)
		   m_BulltTwoTexture[i]->Release();

	for(int i=0;i<2;i++)
		if(m_NumImgTexture[i] != NULL)
		   m_NumImgTexture[i]->Release();

	for(int i=0;i<2;i++)
		if(m_bgImgTexture[i] != NULL)
		   m_bgImgTexture[i]->Release();

	//释放点精灵
	if (m_pSprite != NULL)
		m_pSprite->Release();

	//释放Direct3D设备对象
    if( m_pd3dDevice != NULL ) 
        m_pd3dDevice->Release();

	//释放Direct3D对象
    if( m_pD3D != NULL )       
        m_pD3D->Release();

}
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

					if(m_configDlg.m_ptx2>0)
						PlayGameSound(13);
					//设置碰撞后子弹状态
					m_MeBullet[i].m_Netindex = 0;
					m_MeBullet[i].m_Stop = true;
					//设置被捕获鱼儿数据
					//如果这个子弹是我发射的
					if(m_MeBullet[i].m_Senduser==m_MeChariID)
					{
						//发送消息到服务端
						//							m_baseSink->HitFirsh(tfish->m_fishID,i,m_MeBullet[i].m_RealUser,m_MeBullet[i].m_isAndroid);
						m_pGameClient->OnUserHitFish(tfish->m_fishID,i,m_MeBullet[i].m_RealUser,m_MeBullet[i].m_isAndroid);
						bShoot=true;
						//中断判断
						break;
					}
					else if((m_MeBullet[i].m_isAndroid && m_bHitFlag && m_bHitDesk == m_MeRellayChairID))
					{
						//发送消息到服务端
						m_pGameClient->OnUserHitFish(tfish->m_fishID,i-100,m_MeBullet[i].m_RealUser,m_MeBullet[i].m_isAndroid);
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
void GameScreenWnd::LoadSound()
{
    CBassMusicManager::GetInstance()->Init(this->m_hWnd);
	//载入声音
	//背景声音
	CBassMusicManager::GetInstance()->LoadFromFile(0, "Jcfish/music/bg_01.mp3",true);
	CBassMusicManager::GetInstance()->LoadFromFile(1, "Jcfish/music/bg_02.mp3",true);
	CBassMusicManager::GetInstance()->LoadFromFile(2, "Jcfish/music/bg_03.mp3",true);
    //发送子弹声音
	CBassMusicManager::GetInstance()->LoadFromFile(51, "Jcfish/music/shot8.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(61, "Jcfish/music/shot_8.mp3",false);

	//开网声音
    CBassMusicManager::GetInstance()->LoadFromFile(13, "Jcfish/music/net_8.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(14, "Jcfish/music/net_1.mp3",false);
	//加钱声音
    CBassMusicManager::GetInstance()->LoadFromFile(15, "Jcfish/music/jia.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(16, "Jcfish/music/jia1.mp3",false);
	//死亡声音
	CBassMusicManager::GetInstance()->LoadFromFile(20, "Jcfish/music/die00.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(21, "Jcfish/music/die01.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(22, "Jcfish/music/die02.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(23, "Jcfish/music/die03.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(24, "Jcfish/music/die04.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(25, "Jcfish/music/die05.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(26, "Jcfish/music/die06.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(27, "Jcfish/music/die07.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(28, "Jcfish/music/die07.mp3",false);
	CBassMusicManager::GetInstance()->LoadFromFile(29, "Jcfish/music/fishdiescore.mp3",false);

	CBassMusicManager::GetInstance()->LoadFromFile(35, "Jcfish/music/bomb.ogg",false);	//爆炸声
	//
	CBassMusicManager::GetInstance()->LoadFromFile(71, "Jcfish/music/laserShot.mp3",false);
	//切换场景声音
    CBassMusicManager::GetInstance()->LoadFromFile(40, "Jcfish/music/senceConver.mp3",false);
	//大死声音
    CBassMusicManager::GetInstance()->LoadFromFile(41, "Jcfish/music/Bigdie01.mp3",false);
	CBassMusicManager::GetInstance()->SetVolumn(100);
	//播放背景音乐
	PlayBgSound(1);
}

/*----------------------------------------------------------------------------------------------*/
//播放背景音乐
void	GameScreenWnd::PlayBgSound(int iIndex)
{
	if(!m_configDlg.m_bBgSound)
	{
		return;
	}
	CBassMusicManager::GetInstance()->Stop(0);
	CBassMusicManager::GetInstance()->Stop(1);
	CBassMusicManager::GetInstance()->Stop(2);
	CBassMusicManager::GetInstance()->Play(iIndex, false);
}
/*----------------------------------------------------------------------------------------------*/
//停止背景音效
void	GameScreenWnd::StopBgSound()
{
	CBassMusicManager::GetInstance()->Stop(0);
	CBassMusicManager::GetInstance()->Stop(1);
	CBassMusicManager::GetInstance()->Stop(2);
}
/*----------------------------------------------------------------------------------------------*/
//音效是否正在播放
bool	GameScreenWnd::IsPlaying(int iIndex)
{
	return	CBassMusicManager::GetInstance()->IsPlaying(iIndex);
}
/*----------------------------------------------------------------------------------------------*/
//播放游戏音效
void	GameScreenWnd::PlayGameSound(int iIndex)
{
	if(!m_configDlg.m_bGameSound)
	{
		return;
	}
	CBassMusicManager::GetInstance()->Play(iIndex, false);
}
/*----------------------------------------------------------------------------------------------*/
//停止游戏音效
void	GameScreenWnd::StopGameSound()
{
	return;
}
/*----------------------------------------------------------------------------------------------*/

//载入图片纹理
void GameScreenWnd::LoadImg(const char* m_path,LPDIRECT3DTEXTURE9* ppTexture)
{
	unsigned long nSize = 0;
	FILE *fp = fopen(m_path, "rb");
	fseek(fp,0,SEEK_END);
	//获取大小
	nSize = ftell(fp);
	//创件缓存区域
	char * buffer = new char[4500000];
	//区域
	FilesParmesA * PngBuffdata = (FilesParmesA *)buffer;
	//读取数据
	fseek(fp,0,SEEK_SET);
	nSize = fread(PngBuffdata,sizeof(char), nSize,fp);
	fclose(fp);
	//拷贝图片信息到区域
	fseek(fp,0,SEEK_SET);
	//创建复制数据内存
	unsigned char* pBuffer = new unsigned char[PngBuffdata->m_PngSize];
	memcpy(pBuffer, PngBuffdata + 1, PngBuffdata->m_PngSize);
	nSize = PngBuffdata->m_PngSize;
	do{

		if(!FAILED(D3DXCreateTextureFromFileInMemory(m_pd3dDevice,pBuffer,nSize,ppTexture)))
			break;

	}while(true);
	delete pBuffer;
	delete buffer;
				
}

