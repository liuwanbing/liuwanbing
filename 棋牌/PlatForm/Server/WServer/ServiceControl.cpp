#include "Stdafx.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ServiceManage.h"
#include "ServiceControl.h"
#include "SetupDialog.h"
#include <SYS/Stat.h>
//宏定义
#define BT_WIDTH						80							//按钮宽度
#define BT_HEIGHT						23							//按钮高度

BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CServiceControl, CBaseDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_SET_COM, OnBnClickedSetCom)
/*	ON_BN_CLICKED(IDC_INSTANCELL, OnBnClickedInstancell)
	ON_BN_CLICKED(IDC_UNINSTANLL, OnBnClickedUninstanll)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)*/
	ON_BN_CLICKED(IDC_RE_LOAD, &CServiceControl::OnBnClickedReLoad)
	ON_BN_CLICKED(IDC_UPDATE_COM, &CServiceControl::OnBnClickedUpdateCom)
	ON_BN_CLICKED(IDC_SETUP, &CServiceControl::OnBnClickedSetup)
	ON_BN_CLICKED(IDC_MOTIF, &CServiceControl::OnBnClickedMotif)
	ON_BN_CLICKED(IDC_REMOVE, &CServiceControl::OnBnClickedRemove)
	ON_BN_CLICKED(IDC_START_ALL, &CServiceControl::OnBnClickedStartAll)
	ON_BN_CLICKED(IDC_STOP_ALL, &CServiceControl::OnBnClickedStopAll)
	ON_BN_CLICKED(IDC_DELETE, &CServiceControl::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CHANGE, &CServiceControl::OnBnClickedChange)
	ON_BN_CLICKED(IDC_INFO, &CServiceControl::OnBnClickedInfo)
	ON_BN_CLICKED(IDC_MANAGE, &CServiceControl::OnBnClickedManage)
	ON_BN_CLICKED(IDC_RELOAD_ROOMLIST, &CServiceControl::OnBnClickedReloadRoomlist)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/********************************************************************************************************/

//构造函数
CBaseDialog::CBaseDialog(UINT uTemplate, CWnd * pParent) : CDialog(uTemplate,pParent)
{
}

//析构函数
CBaseDialog::~CBaseDialog()
{
}

//获取窗口
bool CBaseDialog::SafeGetClientRect(UINT uID, CRect & ClientRect)
{
	CWnd * pControlWnd=GetDlgItem(uID);
	if ((pControlWnd!=NULL)&&(pControlWnd->GetSafeHwnd())) 
	{
		pControlWnd->GetClientRect(&ClientRect);
		return true;
	}
	ClientRect.SetRectEmpty();
	return false;
}

//获取窗口
bool CBaseDialog::SafeGetClientRect(CWnd * pControlWnd, CRect & ClientRect)
{
	if ((pControlWnd!=NULL)&&(pControlWnd->GetSafeHwnd())) 
	{
		pControlWnd->GetClientRect(&ClientRect);
		return true;
	}
	ClientRect.SetRectEmpty();
	return false;
}

//移动窗口
bool CBaseDialog::SafeMoveWindow(UINT uID, int x, int y, int iWidth, int iHeight)
{
	CWnd * pControlWnd=GetDlgItem(uID);
	if ((pControlWnd!=NULL)&&(pControlWnd->GetSafeHwnd())) 
	{
		if ((iWidth<=0)||(iHeight<=0)) iWidth=0,iHeight=0;
		pControlWnd->MoveWindow(x,y,iWidth,iHeight);
		return true;
	}
	return false;
}

//移动窗口
bool CBaseDialog::SafeMoveWindow(CWnd * pControlWnd, int x, int y, int iWidth, int iHeight)
{
	if ((pControlWnd!=NULL)&&(pControlWnd->GetSafeHwnd())) 
	{
		if ((iWidth<=0)||(iHeight<=0)) iWidth=0,iHeight=0;
		pControlWnd->MoveWindow(x,y,iWidth,iHeight);
		return true;
	}
	return false;
}

/********************************************************************************************************/

//构造函数
CServiceControl::CServiceControl() : CBaseDialog(IDD_SERVICE_CONTROL)
{
}

//析构函数
CServiceControl::~CServiceControl()
{
}

//DDX/DDV 支持
void CServiceControl::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START_LIST, m_StartList);
	DDX_Control(pDX, IDC_INSTANLL_LIST, m_InstallList);
}

//初始化函数
BOOL CServiceControl::OnInitDialog()
{
	__super::OnInitDialog();
//隐藏列表
//GetDlgItem(IDC_INSTANLL_LIST)->ShowWindow(FALSE);
//GetDlgItem(IDC_START_LIST)->ShowWindow(FALSE);
	 //////版权版权版权版权版权版权版权版权
//CStatic* pStatic1=new CStatic();
//pStatic1->Create(TMLcopyright,WS_CHILD|WS_VISIBLE,CRect(100,0,450,10),this,10101);
//SetWindowTitle(TMLcopyright);

	//初始化组件列表
	CListCtrl * pList=(CListCtrl *)GetDlgItem(IDC_INSTANLL_LIST);
	pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
	pList->InsertColumn(0,TEXT("组件 ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(1,TEXT("组件名字"),LVCFMT_LEFT,100);
	pList->InsertColumn(2,TEXT("组件状态"),LVCFMT_LEFT,80);
	pList->InsertColumn(3,TEXT("版本号码"),LVCFMT_LEFT,60);
	pList->InsertColumn(4,TEXT("游戏人数"),LVCFMT_LEFT,60);
	pList->InsertColumn(5,TEXT("默认信息表"),LVCFMT_LEFT,100);
	pList->InsertColumn(6,TEXT("模块名字"),LVCFMT_LEFT,150);
//	pList->InsertColumn(7,TEXT("备注信息"),LVCFMT_LEFT,300);
//	pList->InsertColumn(7,TEXT("在线人数"),LVCFMT_LEFT,300);

	//初始化房间列表
	pList=(CListCtrl *)GetDlgItem(IDC_START_LIST);
	pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
	pList->InsertColumn(0,TEXT("组件的子实例ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(1,TEXT("组件的子实例名字"),LVCFMT_LEFT,120);
	pList->InsertColumn(2,TEXT("组件名字"),LVCFMT_LEFT,100);
	pList->InsertColumn(3,TEXT("运行状态"),LVCFMT_LEFT,60);
	pList->InsertColumn(4,TEXT("组件类型"),LVCFMT_LEFT,60);
	pList->InsertColumn(5,TEXT("是否启用"),LVCFMT_LEFT,60);
	pList->InsertColumn(6,TEXT("服务地址"),LVCFMT_LEFT,100);
	pList->InsertColumn(7,TEXT("监听端口"),LVCFMT_LEFT,60);
	pList->InsertColumn(8,TEXT("实例中子业务数目"),LVCFMT_LEFT,60);
	pList->InsertColumn(9,TEXT("在线人数"),LVCFMT_LEFT,60);
	pList->InsertColumn(10,TEXT("最大人数"),LVCFMT_LEFT,60);
	pList->InsertColumn(11,TEXT("最少分数"),LVCFMT_LEFT,60);
	pList->InsertColumn(12,TEXT("基础分数"),LVCFMT_LEFT,60);
	pList->InsertColumn(13,TEXT("最大分数"),LVCFMT_LEFT,60);
	pList->InsertColumn(14,TEXT("房间规则"),LVCFMT_LEFT,60);
	pList->InsertColumn(15,TEXT("附加权限"),LVCFMT_LEFT,60);
	pList->InsertColumn(16,TEXT("中心数据库 IP"),LVCFMT_LEFT,0);
	pList->InsertColumn(17,TEXT("登陆数据库 IP"),LVCFMT_LEFT,0);
	pList->InsertColumn(18,TEXT("本地数据库 IP"),LVCFMT_LEFT,0);
	pList->InsertColumn(19,TEXT("类型 ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(20,TEXT("名字 ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(21,TEXT("比赛 ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(22,TEXT("游戏信息表"),LVCFMT_LEFT,100);
	pList->InsertColumn(23,TEXT("同步锁定表名"),LVCFMT_LEFT,100);
	pList->InsertColumn(24,TEXT("限制地址表名"),LVCFMT_LEFT,100);
	pList->InsertColumn(25,TEXT("限制用户登陆表名"),LVCFMT_LEFT,100);
	pList->InsertColumn(26,TEXT("排列 ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(27,TEXT("组件 ID"),LVCFMT_LEFT,60);
	pList->InsertColumn(28,TEXT("模块名字"),LVCFMT_LEFT,0);
	pList->InsertColumn(29,TEXT("备注信息"),LVCFMT_LEFT,0);
	pList->InsertColumn(30,TEXT("游戏人数"),LVCFMT_LEFT,60);
	pList->InsertColumn(31,TEXT("停止登陆"),LVCFMT_LEFT,60);
	pList->InsertColumn(32,TEXT("停止服务时间"),LVCFMT_LEFT,100);
	pList->InsertColumn(33,TEXT("税收比例/100"),LVCFMT_LEFT,60);
	pList->InsertColumn(34,TEXT("罚分"),LVCFMT_LEFT,60);


	//(CButton *)GetDlgItem(IDC_START)->EnableWindow(false);
	//(CButton *)GetDlgItem(IDC_STOP)->EnableWindow(false);
	//(CButton *)GetDlgItem(IDC_SET_COM)->EnableWindow(false);


	///"建立房间",IDC_SET_COM

	//"启动房间",IDC_START,359,119,56,16
   // PUSHBUTTON      "停止房间",IDC_STOP
    SetTimer(1,1000*60*30,NULL);
	SetTimer(10,1000*60*60*3,NULL);
	return TRUE;
}

//位置变化函数
void CServiceControl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	
	//移动控件
	SafeMoveWindow(IDC_INSTANLLED_FRAME,5,2,cx-10,cy/3-7);
	SafeMoveWindow(IDC_START_FRAME,5,cy/3,cx-10,cy*2/3-5);
	SafeMoveWindow(IDC_INSTANLL_LIST,15,18,cx-120,cy/3-30);
	SafeMoveWindow(IDC_START_LIST,15,cy/3+18,cx-120,cy*2/3-30);
	
	//移动按钮
	SafeMoveWindow(IDC_SET_COM,cx-95,18,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_UPDATE_COM,cx-95,18+BT_HEIGHT+5,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_SETUP,cx-95,18+BT_HEIGHT*2+10,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_RE_LOAD,cx-95,18+BT_HEIGHT*3+15,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_MOTIF,cx-95,18+BT_HEIGHT*4+20,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_DELDLL,cx-95,18+BT_HEIGHT*5+25,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_REMOVE,cx-95,18+BT_HEIGHT*6+30,BT_WIDTH,BT_HEIGHT);

	SafeMoveWindow(IDC_START,cx-95,cy/3+88,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_STOP,cx-95,cy/3+88+BT_HEIGHT+5,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_START_ALL,cx-95,cy/3+88+BT_HEIGHT*2+10,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_STOP_ALL,cx-95,cy/3+88+BT_HEIGHT*3+15,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_DELETE,cx-95,cy/3+88+BT_HEIGHT*4+20,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_CHANGE,cx-95,cy/3+88+BT_HEIGHT*5+25,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_INFO,cx-95,cy/3+88+BT_HEIGHT*6+30,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_MANAGE,cx-95,cy/3+88+BT_HEIGHT*7+35,BT_WIDTH,BT_HEIGHT);
	SafeMoveWindow(IDC_RELOAD_ROOMLIST,cx-95,cy/3+88+BT_HEIGHT*8+40,BT_WIDTH,BT_HEIGHT);

	return;
}

//停止游戏房间
void CServiceControl::OnBnClickedStop()
{
	//效验数据
	POSITION SelectPos;
	SelectPos=m_StartList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL)
	{
		AfxMessageBox(TEXT("请先在右边的组件的子实例列表中选择要停止的组件的子实例名称项！"),MB_ICONQUESTION);
		return;
	}
	int iItem=m_StartList.GetNextSelectedItem(SelectPos);
	SC_GameRoomInitStruct * pComRoomInfo=(SC_GameRoomInitStruct *)m_StartList.GetItemData(iItem);
	if (pComRoomInfo==NULL)
	{
		AfxMessageBox(TEXT("组件的子实例信息读取错误，组件的子实例停止失败！"),MB_ICONQUESTION);
		return;
	}

	//停止游戏房间
	if (g_Service.StopGameRoom(pComRoomInfo)==false)
	{
		CString strMessage;
		strMessage.Format(TEXT("停止失败：%s"),g_Service.GetLastErrorMsg());
		AfxMessageBox(strMessage,MB_ICONSTOP);
		return;
	}

	//修改提示
	//g_Service.UpdateRoomListInfo(&m_StartList);
	m_StartList.SetFocus();
	m_StartList.SetItemText(iItem,3,TEXT("停止"));

	return;
}

//启动游戏房间
void CServiceControl::OnBnClickedStart()
{
	//效验数据
	POSITION SelectPos;
	SelectPos=m_StartList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL)
	{
		AfxMessageBox(TEXT("请先在右边的组件的子实例列表中选择要启动的组件的子实例名称项！"),MB_ICONQUESTION);
		return;
	}
	int iItem=m_StartList.GetNextSelectedItem(SelectPos);
	SC_GameRoomInitStruct * pComRoomInfo=(SC_GameRoomInitStruct *)m_StartList.GetItemData(iItem);
	if (pComRoomInfo==NULL)
	{
		AfxMessageBox(TEXT("组件的子实例信息读取错误，组件的子实例启动失败！"),MB_ICONQUESTION);
		return;
	}

#ifdef BZ_LICENSE
	if(coreGetGameLicense(pComRoomInfo->ServiceInfo.uNameID)==pComRoomInfo->ServiceInfo.uNameID)
#else
	if (true)
#endif // BZ_LICENSE
	{
	//启动房间
	if(!g_Service.bGetServiceInfo(pComRoomInfo))
	{
		CString strMessage;
		strMessage.Format(TEXT("房间对应的库%s不存在：%s"),pComRoomInfo->ServiceInfo.szDLLFileName,g_Service.GetLastErrorMsg());
		AfxMessageBox(strMessage,MB_ICONSTOP);
		return;
	}
	if (!g_Service.StartGameRoom(pComRoomInfo))
	{
		CString strMessage;
		strMessage.Format(TEXT("启动新组件房间失败：%s"),g_Service.GetLastErrorMsg());
		AfxMessageBox(strMessage,MB_ICONSTOP);
		return;
	}
	//修改提示
	m_StartList.SetFocus();
	m_StartList.SetItemText(iItem,3,TEXT("启动"));
	}
	else
	{
		CString s=pComRoomInfo->ServiceInfo.szGameName;
		if(s.Left(7)!="未认证_")
		{
			s="未认证_"+s;
			if(s.GetLength()>60)
				s=s.Left(60);
		}	
		strcpy(pComRoomInfo->ServiceInfo.szGameName,s);
		m_StartList.SetItemText(iItem,3,TEXT("无认证"));
	}

	

	return;
}

//增加游戏组件
void CServiceControl::OnBnClickedSetCom()
{

	//增加组件对话框 
	CSetupDialog sd;
	if(sd.DoModal()==IDOK)
	{
		SC_ServiceInfoStruct p;
		lstrcpy(p.ServiceInfo.szDLLFileName,sd.dllFileName);
		lstrcpy(p.ServiceInfo.szDllNote,sd.dllNote);
		lstrcpy(p.ServiceInfo.szGameName,sd.gameName);
		lstrcpy(p.ServiceInfo.szGameTable, sd.tableDbName);
		lstrcpy(p.ServiceInfo.szWriter,sd.writer);

		p.ServiceInfo.uDeskPeople=sd.deskPeople;
		p.ServiceInfo.uNameID = sd.nameID;
		p.ServiceInfo.uServiceVer = sd.sver;
		p.ServiceInfo.uSupportType = sd.supportType;
		p.uServiceID = sd.serInfoID;
		p.uServiceStation = 0;
		
		/*测试
		wsprintf(a,"%d,%d,%d,%d,%d,%d,%s,%s,%s,%s",p.ServiceInfo.uDeskPeople,p.
		ServiceInfo.uNameID,p.ServiceInfo.uServiceVer,p.ServiceInfo.uSupportType ,p.uServiceID,	p.uServiceStation,p.ServiceInfo.szDLLFileName,p.ServiceInfo.szDllNote,p.ServiceInfo.szGameName,p.ServiceInfo.szGameTable);
		AfxMessageBox(a);
		*/

		if(!g_Service.SetupCom(&m_InstallList,p))
			AfxMessageBox(TEXT("写数据库失败，请检查你填写的内容！"),MB_ICONQUESTION);
	}
	return;
}

/********************************************************************************************************/

//刷新组件列表
void CServiceControl::OnBnClickedReLoad()
{
	// TODO: Add your control notification handler code here
	g_Service.UpdateComDLLList(&m_InstallList);
	return;
}

//更新的游戏组件
void CServiceControl::OnBnClickedUpdateCom()
{
	// TODO: Add your control notification handler code here
	//选择要更新的游戏组件
	POSITION SelectPos;
	SelectPos=m_InstallList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL) 
	{
		AfxMessageBox(TEXT("请先在右边的组件列表中选择要更新的组件名称项！"),MB_ICONQUESTION);
		return;
	}
	if(AfxMessageBox("更新游戏组件将自动停止此游戏的所有房间，建议在更新前通知相关房间的所有在线玩家并备份要更新的动态库文件\n准备好了更新吗？",IDOK)==IDOK)
	{
		//得到要更新的游戏组件信息结构体
		SC_ServiceInfoStruct * pServiceInfo=(SC_ServiceInfoStruct *)m_InstallList.GetItemData(m_InstallList.GetNextSelectedItem(SelectPos));
		//stop 组件的房间
		for(int i=0;i<g_Service.m_ComRoomInfo.GetSize();i++)
		{
			SC_GameRoomInitStruct * p=(SC_GameRoomInitStruct *)g_Service.m_ComRoomInfo.GetAt(i);
			if(!p)continue;
			if(p->uServiceID==pServiceInfo->uServiceID) 
			{
				if (!g_Service.StopGameRoom(p))
				{
					CString strMessage;
					strMessage.Format(TEXT("停止房间失败：%s"),g_Service.GetLastErrorMsg());
					AfxMessageBox(strMessage,MB_ICONSTOP);
					continue;
				}
				//修改提示
				g_Service.UpdateRoomListInfo(&m_StartList);
			}
		}

		if(!g_Service.CopyNewCom(pServiceInfo))//安装组件
		{
			AfxMessageBox("请启动对应的房间！");
			return;
		}
		OnBnClickedReLoad();	
		
		//start 新组件房间
		for(int i=0;i<g_Service.m_ComRoomInfo.GetSize();i++)
		{
			SC_GameRoomInitStruct * p=(SC_GameRoomInitStruct *)g_Service.m_ComRoomInfo.GetAt(i);
			if(!p)continue;
			if(p->uServiceID==pServiceInfo->uServiceID) 
			{
				if(!g_Service.bGetServiceInfo(p))
				{
					CString strMessage;
					strMessage.Format(TEXT("房间对应的库%s不存在：%s"),p->ServiceInfo.szDLLFileName,g_Service.GetLastErrorMsg());
					AfxMessageBox(strMessage,MB_ICONSTOP);
					continue;
				}
				if (!g_Service.StartGameRoom(p))
				{
					CString strMessage;
					strMessage.Format(TEXT("启动新组件房间失败：%s"),g_Service.GetLastErrorMsg());
					AfxMessageBox(strMessage,MB_ICONSTOP);
					continue;
				}
				//修改提示
				g_Service.UpdateRoomListInfo(&m_StartList);
			}
		}
	};
	return;
}

//安装未安装组件
void CServiceControl::OnBnClickedSetup()
{
	POSITION SelectPos;
	SelectPos=m_InstallList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL) 
	{
		AfxMessageBox(TEXT("请先在右边的组件列表中选择要更新的组件名称项！"),MB_ICONQUESTION);
		return;
	}
	SC_ServiceInfoStruct * pServiceInfo=(SC_ServiceInfoStruct *)m_InstallList.GetItemData(m_InstallList.GetNextSelectedItem(SelectPos));
	if(pServiceInfo->uServiceStation!=0)//安装
	{
		AfxMessageBox("只能安装未安装的组件，重新选择！");
		return;
	}
	if(g_Service.CopyNewCom(pServiceInfo))//拷贝要安装组件到工作目录
	{
			AfxMessageBox("安装组件失败");
			OnBnClickedReLoad();
	}
	return;
	// TODO: Add your control notification handler code here
}

//修改组件信息
void CServiceControl::OnBnClickedMotif()
{
	// TODO: Add your control notification handler code here
	POSITION SelectPos;
	SelectPos=m_InstallList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL) 
	{
		AfxMessageBox(TEXT("请先在右边的组件列表中选择要更新的组件名称项！"),MB_ICONQUESTION);
		return;
	}
	SC_ServiceInfoStruct * avantp=(SC_ServiceInfoStruct *)m_InstallList.GetItemData(m_InstallList.GetNextSelectedItem(SelectPos));
	//拷贝信息,方便填写
	CSetupDialog sd;
	sd.serInfoID = avantp->uServiceID;
	sd.supportType =avantp->ServiceInfo.uSupportType;
	sd.sver = avantp->ServiceInfo.uServiceVer;
	sd.deskPeople = avantp->ServiceInfo.uDeskPeople;
	sd.dllFileName=avantp->ServiceInfo.szDLLFileName;
	sd.dllNote=avantp->ServiceInfo.szDllNote;
	sd.gameName=avantp->ServiceInfo.szGameName;
	sd.tableDbName=avantp->ServiceInfo.szGameTable;
	sd.writer="szbzw";

	AfxMessageBox("点击确定将弹出修改对话框,,注意serInfoID不允许修改.强行修改无效!");
	if(sd.DoModal()==IDOK)
	{
		if(AfxMessageBox("将确定要修改吗?",IDOK)!=IDOK)
			return;
		//TCHAR a[111];
		SC_ServiceInfoStruct p;
		lstrcpy(p.ServiceInfo.szDLLFileName,sd.dllFileName);
		lstrcpy(p.ServiceInfo.szDllNote,sd.dllNote);
		lstrcpy(p.ServiceInfo.szGameName,sd.gameName);
		lstrcpy(p.ServiceInfo.szGameTable, sd.tableDbName);
		lstrcpy(p.ServiceInfo.szWriter,sd.writer);

		p.ServiceInfo.uDeskPeople=sd.deskPeople;
		p.ServiceInfo.uNameID = sd.nameID;
		p.ServiceInfo.uServiceVer = sd.sver;
		p.ServiceInfo.uSupportType = sd.supportType;
		p.uServiceStation = 0;
		
		/*测试
		wsprintf(a,"%d,%d,%d,%d,%d,%d,%s,%s,%s,%s",p.ServiceInfo.uDeskPeople,p.ServiceInfo.uNameID,
		p.ServiceInfo.uServiceVer,p.ServiceInfo.uSupportType ,p.uServiceID,	p.uServiceStation,
		p.ServiceInfo.szDLLFileName,p.ServiceInfo.szDllNote,p.ServiceInfo.szGameName,p.ServiceInfo.szGameTable);
		AfxMessageBox(a);
		*/
		if(!g_Service.ModifCom(&m_InstallList,avantp,p))//avantp是修改前的资料,p是修改后的资料
			AfxMessageBox(TEXT("写数据库失败，请检查你填写的内容！"),MB_ICONQUESTION);
	}
	return;
}

void CServiceControl::OnBnClickedRemove()
{
	// TODO: Add your control notification handler code here
	POSITION SelectPos;
	SelectPos=m_InstallList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL) 
	{
		AfxMessageBox(TEXT("请先在右边的组件列表中选择要更新的组件名称项！"),MB_ICONQUESTION);
		return;
	}
	SC_ServiceInfoStruct * pServiceInfo=(SC_ServiceInfoStruct *)m_InstallList.GetItemData(m_InstallList.GetNextSelectedItem(SelectPos));
	if(AfxMessageBox("确定要清除当前记录吗?将同时删除其数据库ServerDllInfo表里的对应项!\n注:此操作不删除任何*Srv.DLL文件,要删除某个*Srv.DLL请按<删除动态库>按钮",IDOK)==IDOK)
		g_Service.RemoveDLLCom(&m_InstallList,pServiceInfo);
}

//启动所有房间
void CServiceControl::OnBnClickedStartAll()
{
	// TODO: Add your control notification handler code here
	//启动房间
	for(int i=0;i<g_Service.m_ComRoomInfo.GetSize();i++)
	{
		SC_GameRoomInitStruct *p;
		p = (SC_GameRoomInitStruct*) g_Service.m_ComRoomInfo.GetAt(i);
		if(p->hDllInstance!=NULL||p->pIManageService!=NULL)
			continue;
	#ifdef BZ_LICENSE
		if(coreGetGameLicense(p->ServiceInfo.uNameID)==p->ServiceInfo.uNameID)
	#else
		if (true)
	#endif // BZ_LICENSE
		{
		if(!g_Service.bGetServiceInfo(p))
		{
			CString strMessage;
			strMessage.Format(TEXT("房间对应的库%s不存在：%s"),p->ServiceInfo.szDLLFileName,g_Service.GetLastErrorMsg());
			AfxMessageBox(strMessage,MB_ICONSTOP);
			continue;
		}
		if (!g_Service.StartGameRoom(p))
		{
			CString strMessage;
			strMessage.Format(TEXT("启动组件房间失败：%s"),g_Service.GetLastErrorMsg());
			AfxMessageBox(strMessage,MB_ICONSTOP);
			}
		}
		else
		{
			CString s=p->ServiceInfo.szGameName;
			if(s.Left(7)!="未认证_")
			{
				s="未认证_"+s;
				if(s.GetLength()>60)
					s=s.Left(60);
			}	
			strcpy(p->ServiceInfo.szGameName,s);
		}
	}
	
	//修改提示
	g_Service.UpdateRoomListInfo(&m_StartList);
	
	return;
}


//停止所有房间
void CServiceControl::OnBnClickedStopAll()
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("是否停止所有游戏？",IDOK)==IDOK)
	{
		for(int i=0;i<=g_Service.m_ComRoomInfo.GetUpperBound();i++)
		{
			SC_GameRoomInitStruct *p;
			p = (SC_GameRoomInitStruct*) g_Service.m_ComRoomInfo.GetAt(i);
			if(p->hDllInstance==NULL||p->pIManageService==NULL)
				continue;
			if (!g_Service.StopGameRoom(p))
			{
				CString strMessage;
				strMessage.Format(TEXT("停止组件房间失败：%s"),g_Service.GetLastErrorMsg());
				AfxMessageBox(strMessage,MB_ICONSTOP);
			}
		}
		//修改提示
		g_Service.UpdateRoomListInfo(&m_StartList);
	}
}

void CServiceControl::OnBnClickedDelete()
{
	// TODO: Add your control notification handler code here
	POSITION SelectPos;
	SelectPos=m_StartList.GetFirstSelectedItemPosition();
	if (SelectPos==NULL) 
	{
		AfxMessageBox(TEXT("请先在右边的组件列表中选择要更新的组件名称项！"),MB_ICONQUESTION);
		return;
	}
	SC_GameRoomInitStruct  * p=(SC_GameRoomInitStruct *)m_StartList.GetItemData(m_StartList.GetNextSelectedItem(SelectPos));
	if(p->hDllInstance!=NULL||p->pIManageService!=NULL)
	{
		AfxMessageBox("先停止房间");
		return;
	}
	if(AfxMessageBox("确定要删除吗",IDOK)==IDOK)
	{
		g_Service.DeleteGameRoomInfo(p);
		g_Service.UpdateRoomListInfo(&m_StartList);
	}
}

//增加房间
void CServiceControl::OnBnClickedChange()
{

}

//修改房间信息
void CServiceControl::OnBnClickedInfo()
{
	// TODO: Add your control notification handler code here
}


//管理房间
void CServiceControl::OnBnClickedManage()
{
	// TODO: Add your control notification handler code here
}

//刷新房间列表
void CServiceControl::OnBnClickedReloadRoomlist()
{
	g_Service.GetRoomListInfo();
	g_Service.UpdateRoomListInfo(&m_StartList);
	// TODO: Add your control notification handler code here
}

void CServiceControl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//AfxMessageBox("ok");
    if(nIDEvent == 1)
		g_Service.CountOnLinePeople(&m_InstallList,&m_StartList);

	if(nIDEvent==10)
	{

#ifdef BZ_LICENSE

		long timeStamp=coreGetTimeStamp();
		long licExpires=coreGetLicenseExpires();
		if(timeStamp>licExpires)
		{
			KillTimer(10);
			for(int i=0;i<=g_Service.m_ComRoomInfo.GetUpperBound();i++)
			{
				SC_GameRoomInitStruct *p;
				p = (SC_GameRoomInitStruct*) g_Service.m_ComRoomInfo.GetAt(i);
				if(p->hDllInstance==NULL||p->pIManageService==NULL)
					continue;
				g_Service.StopGameRoom(p);
			}
			//修改提示
			g_Service.UpdateRoomListInfo(&m_StartList);

			CString s,code=coreGetCode();
			s.Format("您的服务器未注册或已过期，请与服务商联系。\n\n请将以下机器码发送给服务商，获取注册码文件：\n\n%s\n\n",code);
			MessageBox(s,"提示",MB_ICONERROR);
			s="机器码已复制到您的剪贴板中，直接Ctrl+V粘贴机器码！";
			MessageBox(s,"提示",MB_ICONINFORMATION);
			OpenClipboard();
			EmptyClipboard();
			HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,code.GetLength()+5); 
			if (hData==NULL)  
			{ 
				CloseClipboard(); 
				return; 
			}
			LPTSTR szMemName=(LPTSTR)GlobalLock(hData); 
			lstrcpy(szMemName,code); 
			SetClipboardData(CF_TEXT,hData); 
			GlobalUnlock(hData);  
			GlobalFree(hData); 
			CloseClipboard(); 
			this->GetParentFrame()->DestroyWindow();
			return;

		}

#endif // BZ_LICENSE

	}

	CBaseDialog::OnTimer(nIDEvent);
}

BOOL CServiceControl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE)
			return TRUE;
	}

	return CBaseDialog::PreTranslateMessage(pMsg);
}
