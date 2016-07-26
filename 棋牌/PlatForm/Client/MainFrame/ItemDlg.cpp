// ItemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ItemDlg.h"

static char g_chClauseInfo[] = 
"\r\n			正易龙系列软件产品用户许可协议\r\n \
			 ZHENGYILONG SOFTWARE LICENSE\r\n \
\r\n\
\r\n\
重要须知——请认真阅读：\r\n\
\r\n\
        您下载并使用正易龙系列软件产品即视为同意本许可协议内容。\r\n\
　	  \r\n\
        本《正易龙系列软件产品最终用户许可协议》（以下简称《协议》）是您与深圳市正易龙科技有限公司（下称深圳正易龙）之间关于下载、安装、使用、复制正易龙系列软件产品（以下简称“软件”）的法律协议。本《协议》描述深圳正易龙与您之间关于“软件”许可使用及相关方面的权利义务。“用户”或“您”是指通过深圳正易龙提供的获取软件授权的途径而获得软件授权许可和软件产品的个人或单位。请您仔细阅读本《协议》中的各项条款，包括免除或者限制深圳正易龙责任的免责条款及对用户的权利限制。请您审阅并接受或不接受本《协议》（未成年人应在法定监护人陪同下审阅）。\r\n\
        除非您接受本《协议》条款，否则您无权下载、安装或使用本“软件”及其相关服务。您的安装使用行为将视为对本《协议》的接受，并同意接受本《协议》各项条款的约束。\r\n\
        未经深圳正易龙授权，任何销售、转让、出租、修改本“软件”的行为均被认为是侵权行为。\r\n\
		\r\n\
1     知识产权声明\r\n\
1.1 本“软件”是由深圳正易龙独立开发。“软件”的一切版权等知识产权，以及与“软件”相关的所有信息内容，包括但不限于：文字表述及其组合、商标、图标图饰、界面设计、版面框架、有关数据、印刷材料、电子文档等均受著作权法和国际著作权条约以及其他知识产权法律法规的保护。除了任何深圳正易龙授权许可协议所提供的明确书面许可外，拥有本“软件”并不赋予您任何有关这些专利、商标、版权或其它知识产权的许可。\r\n\
\r\n\
2     “软件”授权范围\r\n\
2.1   用户可以安装、使用、显示、运行本“软件”。\r\n\
2.2   保留权利：未明示授权的其他一切权利仍归深圳正易龙所有，用户使用其他权利时须另外取得深圳正易龙的书面同意。\r\n\
2.3   除本《协议》有明确规定外，本《协议》并未对本“软件”以外的深圳正易龙的其他服务的服务条款作出规定。对于这些服务，可能有单独的服务条款加以规范，请用户在使用有关服务时另行了解与确认。如用户使用该服务，视为对相关服务条款的接受。\r\n\
\r\n\
3     用户使用须知\r\n\
3.1   用户应保证注册登记软件时提供的资料真实无误。\r\n\
3.2   用户在遵守法律及本协议的前提下可依据本《协议》使用本“软件”。用户无权实施包括但不限于下列行为：\r\n\
3.2.1 不得删除本“软件”及其他副本上所有关于版权的信息、内容；\r\n\
3.2.2 不得对本“软件”进行反向工程、反向汇编、反向编译等；\r\n\
3.2.3 对于本“软件”的图像、文字等相关信息，未经深圳正易龙书面同意，用户不得擅自实施包括但不限于下列行为：使用、复制、修改、链接、转载、汇编、发表、出版，建立镜像站点、擅自借助“软件”发展与之有关的衍生产品、作品、服务等。\r\n\
3.2.4 用户不得利用本“软件”发表、传播、储存违反国家法律、危害国家安全、祖国统一、社会稳定的内容，或侮辱、诽谤、色情、暴力等任何违反国家法律法规政策的内容。\r\n\
3.2.5 用户不得利用本“软件”发表、传播、储存侵害他人知识产权、商业秘密权等合法权利的内容。\r\n\
3.3   使用本“软件”必须遵守国家有关法律和政策，维护国家利益，保护国家安全，对于用户违法或违反本协议的使用而引起的一切责任由用户负全部责任，一概与深圳正易龙无关，导致深圳正易龙损失的，深圳正易龙有权要求用户赔偿，并有权立即停止提供服务并保留相关记录，以配合司法机关追究法律责任的权利。\r\n\
3.4   本“软件”同大多数管理系统一样，可能受到各种安全问题的侵扰。如：下载安装其它软件中含有“特洛伊木马”等病毒，威胁到用户计算机信息和数据的安全，继而影响本“软件”的正常使用等。用户应加强信息安全意识，注意加强密码保护以免遭致损失。\r\n\
3.5   用户自行下载、安装、使用非经深圳正易龙开发并正式发布的其它任何由本“软件”衍生的软件将可能导致不可预知的风险，建议用户不要轻易下载、安装、使用，由此产生的一切法律责任与纠纷一概与深圳正易龙无关。\r\n\
3.6   深圳正易龙保留在任何时候为您提供本“软件”的修改、升级版本的权利，以及为修改或升级服务收取费用的权利。\r\n\
\r\n\
4     法律责任与免责\r\n\
4.1   深圳正易龙为了保障产品的技术领先，会对本软件进行优化和升级，更新后的产品用户可自行下载并免费使用。深圳正易龙对于免费产品不单独对客户进行技术支持和维护，用户可以在官网论坛上提交相关信息，我们评估后，以产品更新和升级的方式给予统一解决；\r\n\
4.2   用户违反本协议或相关的服务条款的规定，深圳正易龙有权视用户的行为性质采取包括但不限于中断使用许可、停止提供服务、限制使用、法律追究等措施。同时，深圳正易龙会视司法部门的要求，协助调查。\r\n\
4.3   使用本“软件”涉及到互联网服务因素，可能会受到各个环节不稳定因素的影响，存在因不可抗力、计算机病毒、黑客攻击、系统不稳定、用户所在位置、用户关机以及其他任何网络、技术、通信线路等原因造成的服务中断或不能满足用户要求的风险，用户须明白并自行承担以上风险。在适用法律允许的最大范围内，深圳正易龙在任何情况下不就因使用或不能使用本“软件”产品及所发生的特殊的、意外的、非直接或间接的损失承担赔偿责任，即使用户已事先被深圳正易龙告知该损害发生的可能性。\r\n\
4.4   用户因第三方如电信部门的通讯线路故障、技术问题、网络、电脑故障、系统不稳定性及其他各种不可抗力原因而遭受的经济损失，深圳正易龙不承担责任。\r\n\
4.5   因技术故障等不可抗事件影响到服务的正常运行的，深圳正易龙承诺在第一时间内与相关单位配合及时处理进行修复，但用户因此而遭受的经济损失，深圳正易龙不承担责任。\r\n\
\r\n\
5     其他条款\r\n\
5.1   本协议所规定的任何条款的部分或全部无效者，不影响其它条款的效力。\r\n\
5.2   本协议的解释、效力及纠纷的解决，适用于中华人民共和国法律。若用户和深圳正易龙之间发生任何纠纷或争议，首先应友好协商解决，协商不成的，用户在此完全同意将纠纷或争议提交深圳正易龙所在地法院管辖。\r\n\
\r\n\
深圳正易龙保留对本协议的解释权。\r\n\
		  \r\n\
						    深圳市正易龙科技有限公司\r\n\
								2011/1/20 ";

// CItemDlg 对话框

IMPLEMENT_DYNAMIC(CItemDlg, CDialog)

//CItemDlg::CItemDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CItemDlg::IDD_DIG_ITEM, pParent)
CItemDlg::CItemDlg() : CDialog(IDD_DIG_ITEM)
{

}

CItemDlg::~CItemDlg()
{
}

void CItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCLOSE,m_Bt1);	
	DDX_Control(pDX, IDOK,m_Bt2);	
}


BEGIN_MESSAGE_MAP(CItemDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCLOSE,OnClickedClose)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()


// CItemDlg 消息处理程序
BOOL CItemDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CString s=CBcfFile::GetAppPath ();/////本地路径
	m_strText = g_chClauseInfo;

	//CBcfFile bcf( s + "bzgame.bcf");
	//m_strText = bcf.GetKeyVal("BZW", "ItemText", "aa");

	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	// 2012.12.17 yyf 屏蔽
	//wsprintf(path,"%sdialog\\item.bmp",skinfolder);
	//m_bk.SetLoadInfo(path,TRUE);
	////m_bk.SetLoadInfo(AfxGetInstanceHandle(),IDB_BITMAP9,false);
	//HRGN hRgn2;
	//MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight(),0);
	//CGameImageHelper help(&m_bk);
	//HRGN hRgn;
	//hRgn = AFCBmpToRgn(help,m_bk.GetPixel(0,0),RGB(1,1,1));
	//if(hRgn)
	//	SetWindowRgn(hRgn,TRUE);

	
	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CItemDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	MoveWindow(0,0,cx,cy,0);
	//-----------------------------------------------------*/
	CenterWindow();

	HRGN hRgn2;
	CGameImage					m_bt;
	wsprintf(path,"%slogon\\log_exit_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_Bt1.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_Bt1.SetWindowRgn(hRgn2,true);
	//m_Bt1.MoveWindow(m_bk.GetWidth()-m_bt.GetWidth()/4-5,0,m_bt.GetWidth()/4,m_bk.GetHeight(),0);
	m_Bt1.MoveWindow(cx-m_bt.GetWidth()/4-5,0,m_bt.GetWidth()/4,cy,0);//2012.10.17 yyf

	wsprintf(path,"%sdialog\\AboutOk.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_Bt2.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_Bt2.SetWindowRgn(hRgn2,true);

	
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
	return TRUE;  
}
void CItemDlg::OnPaint()
{
	CPaintDC dc(this); 

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper	help(&m_bk);
	help.BitBlt(dc.GetSafeHdc(),0,0);*/
	
	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);

	SetDlgItemText( IDC_EDITTEXT , m_strText ) ;
}

//CString  CItemDlg::ReadFile(CString filename)
//{
//	CFile file(filename,CFile::modeRead);
//	char *buf;
//	int len=file.GetLength();
//	buf=new char[len+1];
//	file.Read(buf,len);
//	buf[len]=0;
//	CString resStr=buf;
//	delete buf;
//	return resStr;
//}


void CItemDlg::OnClickedClose()
{
	CDialog::OnOK();
}

LRESULT CItemDlg::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	//OnPaint(); 
	Invalidate();
	return LRESULT(0);
}

// 设置窗口圆角 2012.10.17 yyf
void CItemDlg::SetWndRgn(void)
{
	POINT m_arPoints[8];
	CRect winRect;
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);
 
    //左上角
	m_arPoints[0].x = winRect.left;
	m_arPoints[0].y = winRect.top+3;
	m_arPoints[1].x = winRect.left+3;
	m_arPoints[1].y = winRect.top;

	//右上角
	m_arPoints[2].x = winRect.right-3;
	m_arPoints[2].y = winRect.top;
	m_arPoints[3].x = winRect.right;
	m_arPoints[3].y = winRect.top+3;

	//右下角
	m_arPoints[4].x = winRect.right;
	m_arPoints[4].y = winRect.bottom-4;
	m_arPoints[5].x = winRect.right-4;
	m_arPoints[5].y = winRect.bottom;

	//左下角
	m_arPoints[6].x = winRect.left+4;
	m_arPoints[6].y = winRect.bottom;
	m_arPoints[7].x = winRect.left;
	m_arPoints[7].y = winRect.bottom-4;

	HRGN hRgn = CreatePolygonRgn(m_arPoints, 8, WINDING);
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
}