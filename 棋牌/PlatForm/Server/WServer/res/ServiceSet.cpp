#include "Stdafx.h"
#include "ServiceSet.h"

BEGIN_MESSAGE_MAP(CServiceSet, CDialog)
END_MESSAGE_MAP()

//构造函数
CServiceSet::CServiceSet(SC_ServiceInfoStruct & ServiceInfo, CServiceManage * pServiceManage) : CDialog(IDD_DLL_SET)
{
	m_KindID=0;
	m_szServiceIP[0]=0;
	m_ServiceInfo=ServiceInfo;
	m_pServiceManage=pServiceManage;
	memset(&m_GameRoomInit,0,sizeof(m_GameRoomInit));
	m_GameRoomInit.ServiceInfo=ServiceInfo.ServiceInfo;
}

//析构函数
CServiceSet::~CServiceSet()
{
}

//DDX/DDV 支持
void CServiceSet::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_KIND, m_GameKind);
	DDX_Control(pDX, IDC_GAME_TYPE, m_ComType);
	DDX_Control(pDX, IDC_GAME_SQL_IP, m_CenterSQLIP);
	DDX_Control(pDX, IDC_USER_SQL_IP, m_LogonSQLIP);
	DDX_Control(pDX, IDC_NATIVE_SQL_IP, m_NativeSQLIP);
}

//初始化函数
BOOL CServiceSet::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	((CEdit *)GetDlgItem(IDC_SOCKET_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_ROOM_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_DESK_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_BASE_POINT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_LESS_POINT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(4);
	((CEdit *)GetDlgItem(IDC_ROOM_NAME))->LimitText(30);
	((CEdit *)GetDlgItem(IDC_ROOM_NOTE))->LimitText(50);

	//设置游戏类型
	if (m_ServiceInfo.ServiceInfo.uSupportType&SUP_NORMAL_GAME) m_ComType.AddString(TEXT("点值游戏"));
	if (m_ServiceInfo.ServiceInfo.uSupportType&SUP_MATCH_GAME) m_ComType.AddString(TEXT("比赛游戏"));
	if (m_ServiceInfo.ServiceInfo.uSupportType&SUP_MONEY_GAME) m_ComType.AddString(TEXT("金币游戏"));

	//设置房间名字
	CString strRoomName=m_ServiceInfo.ServiceInfo.szGameName;
	strRoomName+=TEXT("游戏大厅");
	SetDlgItemText(IDC_ROOM_NAME,strRoomName);

	//备注信息
	SetDlgItemText(IDC_ROOM_NOTE,m_ServiceInfo.ServiceInfo.szDllNote);

	return TRUE;
}

//确定函数
void CServiceSet::OnOK()
{
	//变量定义
	DWORD dwSQLIP=0;
	TCHAR szBuffer[2000];

	//挂接类型
	if (m_GameKind.GetCurSel()==CB_ERR)
	{
		AfxMessageBox(TEXT("请选择挂接类型"),MB_ICONQUESTION);
		m_GameKind.SetFocus();
		return;
	}
	m_GameKind.GetLBText(m_GameKind.GetCurSel(),szBuffer);
	if (lstrcmp(TEXT("金币类"),szBuffer)==0) m_GameRoomInit.uKindID=1;
	else if (lstrcmp(TEXT("扑克类"),szBuffer)==0) m_GameRoomInit.uKindID=2;
	else if (lstrcmp(TEXT("棋类"),szBuffer)==0) m_GameRoomInit.uKindID=3;
	else if (lstrcmp(TEXT("对战类"),szBuffer)==0) m_GameRoomInit.uKindID=4;
	else if (lstrcmp(TEXT("比赛类"),szBuffer)==0) m_GameRoomInit.uKindID=5;
	else
	{
		AfxMessageBox(TEXT("请选择正确的挂接类型"),MB_ICONQUESTION);
		m_GameKind.SetFocus();
		return;
	}

	//游戏类型
	if (m_ComType.GetCurSel()==CB_ERR)
	{
		AfxMessageBox(TEXT("请选择游戏类型"),MB_ICONQUESTION);
		m_ComType.SetFocus();
		return;
	}
	m_ComType.GetLBText(m_ComType.GetCurSel(),szBuffer);
	if (lstrcmp(TEXT("点值游戏"),szBuffer)==0) m_GameRoomInit.InitInfo.uComType=TY_NORMAL_GAME;
	else if (lstrcmp(TEXT("比赛游戏"),szBuffer)==0) m_GameRoomInit.InitInfo.uComType=TY_MATCH_GAME;
	else if (lstrcmp(TEXT("金币游戏"),szBuffer)==0) m_GameRoomInit.InitInfo.uComType=TY_MONEY_GAME;
	else
	{
		AfxMessageBox(TEXT("请选择正确的游戏类型"),MB_ICONQUESTION);
		m_ComType.SetFocus();
		return;
	}

	//监听端口
	m_GameRoomInit.InitInfo.uListenPort=GetDlgItemInt(IDC_SOCKET_PORT);
	if ((m_GameRoomInit.InitInfo.uListenPort<3000L)||((m_GameRoomInit.InitInfo.uListenPort>30000L)))
	{
		AfxMessageBox(TEXT("请正确填写监听端口，有效范围 3000 到 30000"),MB_ICONQUESTION);
		GetDlgItem(IDC_SOCKET_PORT)->SetFocus();
		return;
	}

	//房间 ID
	m_GameRoomInit.InitInfo.uRoomID=GetDlgItemInt(IDC_ROOM_ID);
	if ((m_GameRoomInit.InitInfo.uRoomID<1L)||((m_GameRoomInit.InitInfo.uRoomID>30000L)))
	{
		AfxMessageBox(TEXT("请正确填写房间 ID，有效范围 1 到 30000"),MB_ICONQUESTION);
		GetDlgItem(IDC_ROOM_ID)->SetFocus();
		return;
	}

	//桌子数目
	m_GameRoomInit.InitInfo.uDeskCount=GetDlgItemInt(IDC_DESK_COUNT);
	if ((m_GameRoomInit.InitInfo.uDeskCount<10L)||((m_GameRoomInit.InitInfo.uDeskCount>200L)))
	{
		AfxMessageBox(TEXT("请正确填写桌子数目，有效范围 10 到 200"),MB_ICONQUESTION);
		GetDlgItem(IDC_DESK_COUNT)->SetFocus();
		return;
	}

	//基础分数
	m_GameRoomInit.InitInfo.uBasePoint=GetDlgItemInt(IDC_BASE_POINT);
	if (m_GameRoomInit.InitInfo.uBasePoint>10000L)
	{
		AfxMessageBox(TEXT("请正确填写基础分数，有效范围 0 到 10000"),MB_ICONQUESTION);
		GetDlgItem(IDC_BASE_POINT)->SetFocus();
		return;
	}
	
	//最少分数
	m_GameRoomInit.InitInfo.uLessPoint=GetDlgItemInt(IDC_LESS_POINT);
	if (m_GameRoomInit.InitInfo.uLessPoint>30000L)
	{
		AfxMessageBox(TEXT("请正确填写最少分数，有效范围 0 到 30000"),MB_ICONQUESTION);
		GetDlgItem(IDC_LESS_POINT)->SetFocus();
		return;
	}

	//最少分数
	m_GameRoomInit.InitInfo.uLessPoint=GetDlgItemInt(IDC_LESS_POINT);
	if (m_GameRoomInit.InitInfo.uLessPoint>30000L)
	{
		AfxMessageBox(TEXT("请正确填写最少分数，有效范围 0 到 30000"),MB_ICONQUESTION);
		GetDlgItem(IDC_LESS_POINT)->SetFocus();
		return;
	}

	//最大人数
	m_GameRoomInit.InitInfo.uMaxPeople=GetDlgItemInt(IDC_MAX_CONNECT);
	if (m_GameRoomInit.InitInfo.uLessPoint>1000L)
	{
		AfxMessageBox(TEXT("请正确填写最大人数，有效范围 0 到 1000"),MB_ICONQUESTION);
		GetDlgItem(IDC_MAX_CONNECT)->SetFocus();
		return;
	}

	//游戏服务器 IP
	if (m_CenterSQLIP.GetAddress(dwSQLIP)!=4)
	{
		AfxMessageBox(TEXT("请正确填写游戏数据库 IP"),MB_ICONQUESTION);
		m_CenterSQLIP.SetFocus();
		return;
	}
	sprintf(m_GameRoomInit.InitInfo.szSQLIP,TEXT("%d.%d.%d.%d"),FIRST_IPADDRESS(dwSQLIP),SECOND_IPADDRESS(dwSQLIP),THIRD_IPADDRESS(dwSQLIP),FOURTH_IPADDRESS(dwSQLIP));

	//登陆服务器 IP
	if (m_LogonSQLIP.GetAddress(dwSQLIP)!=4)
	{
		AfxMessageBox(TEXT("请正确填写用户数据库 IP"),MB_ICONQUESTION);
		m_LogonSQLIP.SetFocus();
		return;
	}
	sprintf(m_GameRoomInit.InitInfo.szLogonSQLIP,TEXT("%d.%d.%d.%d"),FIRST_IPADDRESS(dwSQLIP),SECOND_IPADDRESS(dwSQLIP),THIRD_IPADDRESS(dwSQLIP),FOURTH_IPADDRESS(dwSQLIP));

	//本地服务器 IP
	if (m_NativeSQLIP.GetAddress(dwSQLIP)!=4)
	{
		AfxMessageBox(TEXT("请正确填写本地数据库 IP"),MB_ICONQUESTION);
		m_NativeSQLIP.SetFocus();
		return;
	}
	sprintf(m_GameRoomInit.InitInfo.szNativeSQLIP,TEXT("%d.%d.%d.%d"),FIRST_IPADDRESS(dwSQLIP),SECOND_IPADDRESS(dwSQLIP),THIRD_IPADDRESS(dwSQLIP),FOURTH_IPADDRESS(dwSQLIP));

	//房间名字
	GetDlgItemText(IDC_ROOM_NAME,m_GameRoomInit.InitInfo.szGameRoomName,sizeof(m_GameRoomInit.InitInfo.szGameRoomName));

	//备注信息
	GetDlgItemText(IDC_ROOM_NOTE,m_GameRoomInit.szRoomNote,sizeof(m_GameRoomInit.szRoomNote));

	//调整参数
	if (m_GameRoomInit.uKindID==1)
	{
		if ((m_ServiceInfo.ServiceInfo.uSupportType&SUP_MONEY_GAME)==0)
		{
			AfxMessageBox(TEXT("此游戏不支持金币游戏模式，请重新选择挂接类型"),MB_ICONQUESTION);
			m_GameKind.SetFocus();
			return;
		}
		m_GameRoomInit.InitInfo.uComType=TY_MONEY_GAME;
	}
	if (m_GameRoomInit.uKindID==5)
	{
		if ((m_ServiceInfo.ServiceInfo.uSupportType&SUP_MATCH_GAME)==0)
		{
			AfxMessageBox(TEXT("此游戏不支持比赛游戏模式，请重新选择挂接类型"),MB_ICONQUESTION);
			m_GameKind.SetFocus();
			return;
		}
		m_GameRoomInit.InitInfo.uComType=TY_MATCH_GAME;
	}
	if (m_GameRoomInit.InitInfo.szGameTable[0]==0) lstrcpy(m_GameRoomInit.InitInfo.szGameTable,m_ServiceInfo.ServiceInfo.szGameTable);
	if (m_GameRoomInit.InitInfo.uComType==TY_MONEY_GAME)
	{
		m_GameRoomInit.uKindID=1;
		lstrcpy(m_GameRoomInit.InitInfo.szGameTable,TEXT("MoneyGameEx"));
		if (m_GameRoomInit.InitInfo.uMoneyPoint==0) m_GameRoomInit.InitInfo.uMoneyPoint=1;
	}
	else if (m_GameRoomInit.InitInfo.uComType==TY_MATCH_GAME)
	{
		m_GameRoomInit.uKindID=5;
		m_GameRoomInit.InitInfo.uMoneyPoint=0;
	}
	UINT uLessMaxPeople=m_GameRoomInit.InitInfo.uDeskCount*m_ServiceInfo.ServiceInfo.uDeskPeople+50;
	if (m_GameRoomInit.InitInfo.uMaxPeople<uLessMaxPeople) m_GameRoomInit.InitInfo.uMaxPeople=uLessMaxPeople;

	//生成连接字符
	CString strConnect;
#ifdef _DEBUG
	strConnect.Format(TEXT("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s;"),
		m_pServiceManage->m_Info.m_szSQLPass,m_pServiceManage->m_Info.m_szSQLName,szLogonDataBaseName,m_pServiceManage->m_Info.m_szSQLAddr);
#else
	strConnect.Format(TEXT("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s,3000;"),
		m_pServiceManage->m_Info.m_szSQLPass,m_pServiceManage->m_Info.m_szSQLName,szLogonDataBaseName,m_pServiceManage->m_Info.m_szSQLAddr);
#endif
	if(stc_bUseAccessDB)
		strConnect=stc_sLogonDataConnect;


	//连接数据库
	CAFCDataBase DataBase;
	DataBase.ShowError(true);
	if (DataBase.Open(strConnect)==false) return;

	//获取数据库密码
	if (m_pServiceManage->GetSQLDataInfo(m_GameRoomInit.InitInfo.szSQLIP,m_GameRoomInit.InitInfo.szSQLName,m_GameRoomInit.InitInfo.szSQLPass,DataBase)==false)
	{
		AfxMessageBox(TEXT("获取游戏数据库信息错误，请重新设置游戏数据库 IP 信息"));
		m_CenterSQLIP.SetFocus();
		return;
	}

	//获取数据库密码
	if (m_pServiceManage->GetSQLDataInfo(m_GameRoomInit.InitInfo.szLogonSQLIP,m_GameRoomInit.InitInfo.szLogonSQLName,m_GameRoomInit.InitInfo.szLogonSQLPass,DataBase)==false)
	{
		AfxMessageBox(TEXT("获取用户数据库信息错误，请重新设置游戏数据库 IP 信息"));
		m_LogonSQLIP.SetFocus();
		return;
	}

	//获取数据库密码
	if (m_pServiceManage->GetSQLDataInfo(m_GameRoomInit.InitInfo.szNativeSQLIP,m_GameRoomInit.InitInfo.szNativeSQLName,m_GameRoomInit.InitInfo.szNativeSQLPass,DataBase)==false)
	{
		AfxMessageBox(TEXT("获取本地数据库信息错误，请重新设置游戏数据库 IP 信息"));
		m_NativeSQLIP.SetFocus();
		return;
	}

	//执行语句资料
	sprintf(szBuffer,TEXT("INSERT INTO ComRoomInfo (RoomID,RoomName,ServiceIP,CenterSQLIP,LogonSQLIP,NativaSQLIP,ServerInfoID,")
		TEXT("ComType,SocketPort,KindID,NameID,MatchInfoID,DeskCount,MaxPeople,BasePoint,LessPoint,MoneyPoint,RoomRule,UserPower,")
		TEXT("LockTable,IPRuleTable,MatchTable,Note) ")
		TEXT("VALUES (%ld,'%s','%s','%s','%s','%s',%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,'%s','%s','%s','%s')"),
		m_GameRoomInit.InitInfo.uRoomID,m_GameRoomInit.InitInfo.szGameRoomName,m_pServiceManage->m_Info.m_szServiceIP,m_GameRoomInit.InitInfo.szSQLIP,
		m_GameRoomInit.InitInfo.szLogonSQLIP,m_GameRoomInit.InitInfo.szNativeSQLIP,m_ServiceInfo.uServiceID,m_GameRoomInit.InitInfo.uComType,
		m_GameRoomInit.InitInfo.uListenPort,m_GameRoomInit.uKindID,m_ServiceInfo.ServiceInfo.uNameID,m_GameRoomInit.InitInfo.dwMatchInfoID,
		m_GameRoomInit.InitInfo.uDeskCount,m_GameRoomInit.InitInfo.uMaxPeople,m_GameRoomInit.InitInfo.uBasePoint,m_GameRoomInit.InitInfo.uLessPoint,
		m_GameRoomInit.InitInfo.uMoneyPoint,m_GameRoomInit.InitInfo.dwRoomRule,m_GameRoomInit.InitInfo.dwUserPower,m_GameRoomInit.InitInfo.szLockTable,
		m_GameRoomInit.InitInfo.szIPRuleTable,m_GameRoomInit.InitInfo.szGameTable,m_GameRoomInit.szRoomNote);
	if (DataBase.Execute(szBuffer)==false) return;

	__super::OnOK();
}
