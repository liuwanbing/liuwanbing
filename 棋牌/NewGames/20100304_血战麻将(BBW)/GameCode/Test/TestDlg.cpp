// TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include ".\testdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CTestDlg 对话框

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUT_HUPAI, OnBnClickedButHupai)
	ON_BN_CLICKED(IDC_BUT_TINGPAI, OnBnClickedButTingpai)
	ON_CBN_SELCHANGE(IDC_CO_ST_CH, OnCbnSelchangeCoStCh)
	ON_CBN_SELCHANGE(IDC_CO_MAINJING, OnCbnSelchangeCoMainjing)
	ON_CBN_SELCHANGE(IDC_CO_LESSJING, OnCbnSelchangeCoLessjing)
	ON_EN_CHANGE(IDC_EDIT_DATA, OnEnChangeEditData)
	ON_BN_CLICKED(IDC_BUT_MORE_TEST, OnBnClickedButMoreTest)
	ON_BN_CLICKED(IDC_CHECK_DIAOPAO, OnBnClickedCheckDiaopao)
END_MESSAGE_MAP()

// CTestDlg 消息处理程序

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//((CButton *)GetDlgItem(IDC_CHECK_DIAOPAO))->SetSetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_CHECK_DIAOPAO))->SetCheck(BST_UNCHECKED);	///点炮糊牌 （默认无）

	m_SrvLogic.SetDeskPoint(this);//吃碰杠检测方法(获取父类指针)


	CComboBox *pCtrl = NULL;
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_MAINJING);		///正精
	AddPaiStrToCom(pCtrl);

	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_LESSJING);		///副精
	AddPaiStrToCom(pCtrl);


	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}


void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

///////////////////////////////////////////////////////////////////////////////////////

///将牌信息添加到列表控件中
void CTestDlg::AddPaiStrToCom(CComboBox *pCtrl)
{
	pCtrl->InsertString(0,"无");
	pCtrl->InsertString(1,"1万");
	pCtrl->InsertString(2,"2万");
	pCtrl->InsertString(3,"3万");
	pCtrl->InsertString(4,"4万");
	pCtrl->InsertString(5,"5万");
	pCtrl->InsertString(6,"6万");
	pCtrl->InsertString(7,"7万");
	pCtrl->InsertString(8,"8万");
	pCtrl->InsertString(9,"9万");

	pCtrl->InsertString(10,"1同");
	pCtrl->InsertString(11,"2同");
	pCtrl->InsertString(12,"3同");
	pCtrl->InsertString(13,"4同");
	pCtrl->InsertString(14,"5同");
	pCtrl->InsertString(15,"6同");
	pCtrl->InsertString(16,"7同");
	pCtrl->InsertString(17,"8同");
	pCtrl->InsertString(18,"9同");

	pCtrl->InsertString(19,"1条");
	pCtrl->InsertString(20,"2条");
	pCtrl->InsertString(21,"3条");
	pCtrl->InsertString(22,"4条");
	pCtrl->InsertString(23,"5条");
	pCtrl->InsertString(24,"6条");
	pCtrl->InsertString(25,"7条");
	pCtrl->InsertString(26,"8条");
	pCtrl->InsertString(27,"9条");

	pCtrl->InsertString(28,"东");
	pCtrl->InsertString(29,"南");
	pCtrl->InsertString(30,"西");
	pCtrl->InsertString(31,"北");
	pCtrl->InsertString(32,"中");
	pCtrl->InsertString(33,"发");
	pCtrl->InsertString(34,"白");
}

///将位置信息添加到列表控件中
void CTestDlg::AddStationStrToCom(CComboBox *pCtrl)
{
	pCtrl->InsertString(0,"-1");
	pCtrl->InsertString(1,"位置0");
	pCtrl->InsertString(2,"位置1");
	pCtrl->InsertString(3,"位置2");
	pCtrl->InsertString(4,"位置3");

}

///将吃碰杠类型信息添加到列表控件中
void CTestDlg::AddCPGTypeStrToCom(CComboBox *pCtrl)
{
	pCtrl->InsertString(0,"无");
	pCtrl->InsertString(1,"吃");
	pCtrl->InsertString(2,"碰");
	pCtrl->InsertString(3,"坎");
	pCtrl->InsertString(4,"暗杠");
	pCtrl->InsertString(5,"补杠");
	pCtrl->InsertString(6,"明杠");

}

///获取下拉列表牌的数据(测试用)
int CTestDlg::GetComboPaiData(CComboBox * pCtrl)
{
	int mj =255;
	int index=0;
	index = pCtrl->GetCurSel();
	if(index>0 && index <10)//万
	{
		mj = index;
	}
	else if(index >= 10 && index <= 18)//同
	{
		mj = index+1;
	}
	else if(index >= 19 && index <= 27)//条
	{
		mj = index+2;
	}
	else if(index>=28 && index<=34)//字牌
	{
		mj = index+3;
	}
	return mj;
}

///获取下拉列表位置的数据(测试用)
int CTestDlg::GetComboStationData(CComboBox * pCtrl)
{
	int station = -1;
	int index=0;
	index = pCtrl->GetCurSel();
	station = index -1;
	return station;
}

///获取下拉列表吃碰杠牌类型的数据(测试用)
int CTestDlg::GetComboCPGTypeData(CComboBox * pCtrl)
{
	int type = 0;
	int index=0;
	index = pCtrl->GetCurSel();
	switch(index)
	{
	case 0:
		type = ACTION_NO;//无
		break;
	case 1:
		type = ACTION_CHI;//吃牌动作
		break;
	case 2:
		type = ACTION_PENG;//碰牌动作
		break;
	case 3:
		type = ACTION_KAN;//坎牌牌动作
		break;
	case 4:
		type = ACTION_AN_GANG;//暗杠动作
		break;
	case 5:
		type = ACTION_BU_GANG;//补杠动作
		break;
	case 6:
		type = ACTION_MING_GANG;//明杠动作
		break;
	}			
	return type;
}

///根据当前数据设置控件状态
void CTestDlg::SetCtrlStatue()
{
	CComboBox * pCtrl = NULL;
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_ST_CH);		///选牌位置
	if(m_byNowStation==255)
	{	
		pCtrl->SetCurSel(0);
	}
	else
	{
		pCtrl->SetCurSel(m_byNowStation+1);
	}

	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_MAINJING);		///正精
	if(m_byMainJing==255)
	{	
		pCtrl->SetCurSel(0);
	}
	else
	{
		pCtrl->SetCurSel(m_byMainJing - (m_byMainJing-1)/9);
	}
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_LESSJING);		///副精
	if(m_byLessJing==255)
	{	
		pCtrl->SetCurSel(0);
	}
	else
	{
		pCtrl->SetCurSel(m_byLessJing - (m_byLessJing-1)/9);
	}
	if(m_byNowStation<4)//玩家吃碰杠数据
	{
		int index=0;
		for(int i=0;i<5;i++)
		{
			if(m_stCPGData[m_byNowStation][i].byType == 255)
				break;
			
			
			index++;
		}
		for(int i=index;i<5;i++)
		{
			m_stCPGData[m_byNowStation][i].Init();
		}
	}
	//bool m_bZimo;//是否自摸糊牌
}


//将字符串解析成牌数据
BYTE CTestDlg::ChangeStrToData(CString st)
{
	CString str="";
	BYTE pai = 0;
	if(st == "df")
	{
		pai = 31;
	}
	else if(st == "nf")
	{
		pai = 32;
	}
	else if(st == "xf")
	{
		pai = 33;
	}
	else if(st == "bf")
	{
		pai = 34;
	}
	else if(st == "hz")
	{
		pai = 35;
	}
	else if(st == "fc")
	{
		pai = 36;
	}
	else if(st == "bb")
	{
		pai = 37;
	}
	else
	{
		if(st == "1w")
		{
			pai = 1;
		}
		else if(st == "2w")
		{
			pai = 2;
		}
		else if(st == "3w")
		{
			pai = 3;
		}
		else if(st == "4w")
		{
			pai = 4;
		}
		else if(st == "5w")
		{
			pai = 5;
		}
		else if(st == "6w")
		{
			pai = 6;
		}
		else if(st == "7w")
		{
			pai = 7;
		}
		else if(st == "8w")
		{
			pai = 8;
		}
		else if(st == "9w")
		{
			pai = 9;
		}
		else if(st == "1b")
		{
			pai = 11;
		}
		else if(st == "2b")
		{
			pai = 12;
		}
		else if(st == "3b")
		{
			pai = 13;
		}
		else if(st == "4b")
		{
			pai = 14;
		}
		else if(st == "5b")
		{
			pai = 15;
		}
		else if(st == "6b")
		{
			pai = 16;
		}
		else if(st == "7b")
		{
			pai = 17;
		}
		else if(st == "8b")
		{
			pai = 18;
		}
		else if(st == "9b")
		{
			pai = 19;
		}
		else if(st == "1t")
		{
			pai = 21;
		}
		else if(st == "2t")
		{
			pai = 22;
		}
		else if(st == "3t")
		{
			pai = 23;
		}
		else if(st == "4t")
		{
			pai = 24;
		}
		else if(st == "5t")
		{
			pai = 25;
		}
		else if(st == "6t")
		{
			pai = 26;
		}
		else if(st == "7t")
		{
			pai = 27;
		}
		else if(st == "8t")
		{
			pai = 28;
		}
		else if(st == "9t")
		{
			pai = 29;
		}
	}

	if(pai==0)
		pai = 255;
	return pai;
}

//选择测试位置
void CTestDlg::OnCbnSelchangeCoStCh()
{
	// TODO: 在此添加控件通知处理程序代码
}

//选择正精
void CTestDlg::OnCbnSelchangeCoMainjing()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox * pCtrl = NULL;
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_MAINJING);		///正精
	m_byMainJing = GetComboPaiData(pCtrl);//正精
}

//选择副精
void CTestDlg::OnCbnSelchangeCoLessjing()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox * pCtrl = NULL;
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_LESSJING);		///副精
	m_byLessJing = GetComboPaiData(pCtrl);//副精
}


//牌数据输入
void CTestDlg::OnEnChangeEditData()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit * pCtrl = NULL;
	pCtrl = (CEdit *)GetDlgItem(IDC_EDIT_DATA);		///牌数据字符串
	pCtrl->GetWindowTextA(m_sData);

}


//胡牌测试按钮
void CTestDlg::OnBnClickedButHupai()
{
	// TODO: 在此添加控件通知处理程序代码

	BYTE hupaitype[20];//糊牌类型
	BYTE data[14];
	memset(data,255,sizeof(data));
	memset(hupaitype,255,sizeof(hupaitype));

	m_bZimo = false;
	if(((CButton *)GetDlgItem(IDC_CHECK_DIAOPAO))->GetCheck() == BST_UNCHECKED)	///点炮糊牌 （默认无）
	{
		m_bZimo = true;
	}

	m_SrvLogic.m_UserData.InitData();
	//加入吃碰杠牌
	CString str="";
	int index = 0;
	for(int i=0;i<28;i+=2)
	{
		str = m_sData.Mid(i,2);
		data[index] = ChangeStrToData(str);
		index++;
	}
	for(int i=0;i<14;i++)
	{
		m_SrvLogic.m_UserData.m_byArHandPai[0][i] = data[i];
	}

	CComboBox * pCtrl = NULL;
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_MAINJING);		///正精
	m_byMainJing = GetComboPaiData(pCtrl);//正精
	pCtrl = (CComboBox *)GetDlgItem(IDC_CO_LESSJING);		///副精
	m_byLessJing = GetComboPaiData(pCtrl);//副精

	m_SrvLogic.m_UserData.m_StrJing.AddJing(m_byMainJing);//正财神
	m_SrvLogic.m_UserData.m_StrJing.AddJing(m_byLessJing);//副财神

	str.Format("糊牌测试：pai 数据 %d %d %d %d %d %d %d %d %d %d %d %d %d %d",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13]);
	OutputDebugString(str);
	
	if(m_SrvLogic.m_CheckHuPai.CanHu(0,m_SrvLogic.m_UserData.GetLastHandPai(0),m_SrvLogic.m_GameData.T_CPGNotify[0],m_bZimo,0))
	{
		str = "";
		GetHuPaiType(str,m_SrvLogic.m_GameData.T_CPGNotify[0].byHuType);
		AfxMessageBox(str);

	}
	else
	{
		AfxMessageBox("无法糊牌");
	}

}

//听牌测试
void CTestDlg::OnBnClickedButTingpai()
{
	// TODO: 在此添加控件通知处理程序代码

	BYTE hupaitype[20];
	BYTE data[14];
	BYTE CanOutData[14];//听后能出的牌
	memset(data,255,sizeof(data));
	memset(CanOutData,255,sizeof(CanOutData));
	memset(hupaitype,255,sizeof(hupaitype));

	m_SrvLogic.m_UserData.InitData();
	//加入吃碰杠牌

	CString str="";
	int index = 0;
	for(int i=0;i<28;i+=2)
	{
		str = m_sData.Mid(i,2);
		data[index] = ChangeStrToData(str);
		index++;
	}
	for(int i=0;i<14;i++)
	{
		m_SrvLogic.m_UserData.m_byArHandPai[0][i] = data[i];
	}

	m_SrvLogic.m_UserData.m_StrJing.Init();//没有财神

	if(m_SrvLogic.m_CheckHuPai.CanTing(0,m_SrvLogic.m_GameData.T_CPGNotify[0]))
	{
		str.Format("听牌测试：能出的牌： %d %d %d %d %d %d %d",CanOutData[0],CanOutData[1],CanOutData[2],CanOutData[3],CanOutData[4],CanOutData[5],CanOutData[6]);
		AfxMessageBox(str);
	}
	else
	{
		AfxMessageBox("无法听牌");
	}
}

//糊牌批测试
void CTestDlg::OnBnClickedButMoreTest()
{
	// TODO: 在此添加控件通知处理程序代码
// TODO: 在此添加控件通知处理程序代码
	CString s = CINIFile::GetAppPath ();//本地路径   
	CINIFile f(s  + _T("hipaitest.ini"));
	CString key="count";
	int count = f.GetKeyVal(key,"count",0);//测试牌型数
	BYTE handpai[17],lastpai=255 ,mainjing = 255,lessjing = 255;
	memset(handpai,255,sizeof(handpai));
	CString strdata="",sHuType="",schecktype0="",schecktype1="",schecktype2="",str="";
	int index = 0;
	bool zimo = false;
	BYTE hupaitype[20];
	memset(hupaitype,255,sizeof(hupaitype));

	for(int i=0;i<count;++i)
	{
		m_SrvLogic.m_UserData.InitData();
		memset(hupaitype,255,sizeof(hupaitype));

		key.Format("kind_%d",i);
		//牌数据
		strdata = f.GetKeyVal(key,"pai","");
		index = 0;
		for(int j=0;j<28;j+=2)
		{
			str = strdata.Mid(j,2);
			m_SrvLogic.m_UserData.m_byArHandPai[0][index] = ChangeStrToData(str);
			index++;
		}

		if(index%3 != 2)//牌数不对
		{
			f.SetKeyValString(key,"hutype","输入牌数不对，正确牌数：2,5,8,11,14");//输出糊牌类型
			continue;
		}

		//最后的牌
		lastpai = f.GetKeyVal(key,"lastpai",255);
		//是否自摸
		zimo = f.GetKeyVal(key,"zimo",0);
		//正精
		mainjing = f.GetKeyVal(key,"mainjing",255);
		//副精
		lessjing = f.GetKeyVal(key,"lessjing",255);
		m_SrvLogic.m_UserData.m_StrJing.AddJing(mainjing);//
		m_SrvLogic.m_UserData.m_StrJing.AddJing(lessjing);//
		//要检测的胡牌类型:最多支持同时检测3种牌型
		schecktype0 = f.GetKeyVal(key,"checktype0","");
		schecktype1 = f.GetKeyVal(key,"checktype1","");
		schecktype2 = f.GetKeyVal(key,"checktype2","");
		//检测  并输出结果
		if(m_SrvLogic.m_CheckHuPai.CanHu(0,lastpai,m_SrvLogic.m_GameData.T_CPGNotify[0],zimo,0))
		{//糊牌成功，输出糊牌类型
			GetHuPaiType(sHuType,hupaitype);
			f.SetKeyValString(key,"hutype",sHuType);//输出糊牌类型

			if(schecktype0 != "")//有检测牌型
			{
				if(sHuType.Find(schecktype0) != -1)//包含该糊牌类型
				{
					schecktype0 += " 存在";
					f.SetKeyValString(key,"outtype0",schecktype0);//输出糊牌类型
				}
				else
				{
					schecktype0 += " 不存在";
					f.SetKeyValString(key,"outtype0",schecktype0);//输出糊牌类型
				}
			}

			if(schecktype1 != "")//有检测牌型
			{
				if(sHuType.Find(schecktype1) != -1)//包含该糊牌类型
				{
					schecktype1 += " 存在";
					f.SetKeyValString(key,"outtype1",schecktype1);//输出糊牌类型
				}
				else
				{
					schecktype1 += " 不存在";
					f.SetKeyValString(key,"outtype1",schecktype1);//输出糊牌类型
				}
			}

			if(schecktype2 != "")//有检测牌型
			{
				if(sHuType.Find(schecktype2) != -1)//包含该糊牌类型
				{
					schecktype2 += " 存在";
					f.SetKeyValString(key,"outtype2",schecktype2);//输出糊牌类型
				}
				else
				{
					schecktype2 += " 不存在";
					f.SetKeyValString(key,"outtype2",schecktype2);//输出糊牌类型
				}
			}
		}
		else
		{//无法糊牌		
			f.SetKeyValString(key,"hutype","无法糊牌");//输出糊牌类型
		}
	}

}

//点炮
void CTestDlg::OnBnClickedCheckDiaopao()
{
	// TODO: 在此添加控件通知处理程序代码
}


//获取糊牌类型
void CTestDlg::GetHuPaiType(CString & str,BYTE hupaitype[])
{
	CString  strtype="";
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_HU_PAI,hupaitype))//胡牌
	{
		strtype = " 糊牌"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_ZI_MO,hupaitype))//自摸
	{
		strtype = " 自摸"; str += strtype;
	}

	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_QING_YI_SE,hupaitype))//清一色
	{
		strtype = " 清一色"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_PENG_PENG_HU,hupaitype))//碰碰胡
	{
		strtype = " 碰碰胡"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_QI_DUI,hupaitype))//七对
	{
		strtype = " 七对"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_HAO_HUA_QI_DUI,hupaitype))//豪华七对
	{
		strtype = " 豪华七对"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_TIAN_HU,hupaitype))//天糊
	{
		strtype = " 天糊"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_DI_HU,hupaitype))//地糊
	{
		strtype = " 地糊"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_QIANG_GANG,hupaitype))//抢杠
	{
		strtype = " 抢杠"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_GANG_KAI,hupaitype))//杠开
	{
		strtype = " 杠开"; str += strtype;
	}
	if(m_SrvLogic.m_CheckHuPai.CheckHupaiType(HUPAI_GANG_PAO,hupaitype))//杠后炮
	{
		strtype = " 杠后炮"; str += strtype;
	}
	
	
}


///名称：GameFinish
///描述：游戏结束（再此函数中调用计分函数）
bool CTestDlg::GameFinish(BYTE nDeskStation, BYTE bCloseFlag)
{
	return true;
}

///发送游戏状态
void CTestDlg::SendGameStationEx(BYTE nDeskStation, bool bWatchUser, void * pStationData, UINT uSize)
{
}
///发送数据函数 （发送消息给游戏者）
void CTestDlg::SendGameDataEx(BYTE nDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
}
///发送旁观数据 （发送消息给旁观者）
void CTestDlg::SendWatchDataEx(BYTE nDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
}
///发送数据函数 （发送消息给游戏者）
void CTestDlg::SendGameDataEx(BYTE nDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
}
///发送旁观数据 （发送消息给旁观者）
void CTestDlg::SendWatchDataEx(BYTE nDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
}

//获取玩家的同意状态
void CTestDlg::GetUserAgree(BYTE agree)
{
}

//是否在游戏中
bool CTestDlg::IsPlayGame(int bDeskStation)
{
	if(m_bGameStation>= 20 && m_bGameStation<23)
		return true;
	return false;
}

void CTestDlg::KillTimer(int nTimerID)
{
    KillTimer( nTimerID);
}
void CTestDlg::SetTimer(int nID, int nTime)
{
    SetTimer(nID, nTime);
}