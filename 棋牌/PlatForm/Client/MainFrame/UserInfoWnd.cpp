#include "StdAfx.h"
#include "UserInfoWnd.h"
#include "MainRoomEx.h"
#include "MyDiskInfo.h"

#define PAGE_NUMS				5	//Tab页的数量
#define PAGE_LOCK_MACHINE		2	//"锁机设置" Tab页索引
#define PAGE_BIND_MOBILE		3	//"手机绑定" Tab页索引

#define IDT_RE_GET_MOBILE_VCODE_1	1 //获取第手机验证码1
#define IDT_RE_GET_MOBILE_VCODE_2	2 //获取第手机验证码2
#define IDT_RE_GET_MOBILE_VCODE_3	3 //获取第手机验证码3

#define RE_VCODE_TIME	80	//重取手机验证码的时间

using namespace BzDui;

typedef map<char*,vector<char*>> AREA_MAP;

//省份地区关系
AREA_MAP CUserInfoWnd::m_mapArea;

//当前实例窗口句柄
HWND CUserInfoWnd::m_hInstanceWnd = NULL;

CUserInfoWnd::CUserInfoWnd()
{
	m_pUserInfo = NULL;
	//默认显示的页索引
	m_nDefaultPage = 0;
	//默认显示的子页索引
	m_nDefaultSubPage = 0;
	//重取手机验证码的剩余时间
	m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();
}

CUserInfoWnd::~CUserInfoWnd(void)
{
}

LPCTSTR CUserInfoWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CUserInfoWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CUserInfoWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CUserInfoWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);

	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("left\\uinfo\\UserInfoWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	CUserInfoWnd::m_hInstanceWnd = m_hWnd;

	Init();
	return 0;
}

LRESULT CUserInfoWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CUserInfoWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CUserInfoWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUserInfoWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUserInfoWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CUserInfoWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUserInfoWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//获取手机验证码按钮1倒计时(修改用户详细信息tab页中)
	if (wParam == IDT_RE_GET_MOBILE_VCODE_1)
	{
		if (m_iReVCodeTime == 0)
		{
			::KillTimer(m_hWnd,IDT_RE_GET_MOBILE_VCODE_1);
			CMainRoomEx::ClearMobileVCode();

			m_pVCodeBtn1->SetEnabled(true);
			m_pVCodeBtn1->SetText(_T("获取验证码"));
			m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();

			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_MobileNo")));
			pEdtCtrl->SetEnabled(true);

			return 0;
		}

		char szText[32] = {0};
		_stprintf(szText,"%2d秒后重取",m_iReVCodeTime--);
		m_pVCodeBtn1->SetText(szText);

		return 0;
	}
	//获取手机验证码按钮2倒计时(修改锁机设置的tab页中)
	else if (wParam == IDT_RE_GET_MOBILE_VCODE_2)
	{
		if (m_iReVCodeTime == 0)
		{
			::KillTimer(m_hWnd,IDT_RE_GET_MOBILE_VCODE_2);
			CMainRoomEx::ClearMobileVCode();

			m_pVCodeBtn2->SetEnabled(true);
			m_pVCodeBtn2->SetText(_T("获取验证码"));
			m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();
			return 0;
		}
		char szText[32] = {0};
		_stprintf(szText,"%2d秒后重取",m_iReVCodeTime--);
		m_pVCodeBtn2->SetText(szText);

		return 0;
	}

	//获取手机验证码按钮3倒计时(修改锁机设置的tab页中)
	else if (wParam == IDT_RE_GET_MOBILE_VCODE_3)
	{
		if (m_iReVCodeTime == 0)
		{
			::KillTimer(m_hWnd,IDT_RE_GET_MOBILE_VCODE_3);
			CMainRoomEx::ClearMobileVCode();

			m_pVCodeBtn3->SetEnabled(true);
			m_pVCodeBtn3->SetText(_T("获取验证码"));
			m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();
			return 0;
		}

		char szText[32] = {0};
		_stprintf(szText,"%2d秒后重取",m_iReVCodeTime--);
		m_pVCodeBtn3->SetText(szText);

		return 0;
	}

	return 0;
}

LRESULT CUserInfoWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch(uMsg) 
	{
		case WM_CREATE:
			{
				 lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
			}
			break;
		case WM_DESTROY:       
			{
				 lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
			}
			break;
		case WM_NCACTIVATE:    
			{
				 lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
			}
			break;
		case WM_NCCALCSIZE:    
			{
				 lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
			}
			break;
		case WM_NCPAINT:       
			{
				lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
			}
			break;
		case WM_NCHITTEST:     
			{
				 lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
			}
			break;
		case WM_SIZE:          
			{
				lRes = OnSize(uMsg, wParam, lParam, bHandled);
			}
			break;
		case WM_TIMER:
			{
				lRes = OnTimer(uMsg, wParam, lParam, bHandled);
			}
			break;
		case MSG_SET_LOGO:
			{
				OutputDebugString("chs 接收到消息MSG_SET_LOGO");
				int index = OnSetHeadPic(wParam,lParam);

				CString cs;
				cs.Format("../../log/UserLogos/Logo_%d.png",index);
				CButtonUI *pbtnheader = static_cast<CButtonUI*>(m_pm.FindControl("HeadPic"));
				pbtnheader->SetBkImage(cs);
				pbtnheader->SetNormalImage(cs);
			}
		default:
			bHandled = FALSE;
	}
	if(bHandled)
	{
		return lRes;
	}
	if(m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		return lRes;
	}
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CUserInfoWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			return true;
		}
		else if( wParam == VK_ESCAPE )
		{
			return true;
		}
	}
	return false;
}

void CUserInfoWnd::Init() 
{

	char szName[20];
	for (int i = 0; i < PAGE_NUMS; ++i)
	{
		::sprintf_s(szName,sizeof(szName),_T("Ctn_Page_%d"),i + 1);
		m_pPageContainer[i] = static_cast<CContainerUI*>(m_pm.FindControl(szName));
	}

	UINT uYPos = 40;

	for (int i = 0; i < PAGE_NUMS; ++i)
	{
		::sprintf_s(szName,sizeof(szName),_T("Bnt_Page_%d"),i + 1);
		CButtonUI* pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl(szName));
		if(pBtnCtrl)
		{
			SIZE sz = {pBtnCtrl->GetFixedXY().cx,uYPos};
			pBtnCtrl->SetFixedXY(sz);
			if(pBtnCtrl->GetName() == _T("Bnt_Page_4"))
			{
				if(CGamePlaceApp::GetModuleStatus("UserSecurity","MobileVerify",0) == 0)
				{
					pBtnCtrl->SetVisible(false);
					continue;
				}
			}
			uYPos += 35;
		}

		if (i < PAGE_NUMS - 1)
		{
			::sprintf_s(szName,sizeof(szName),_T("Img_Sep_%d"),i + 1);
			CControlUI* pSepImage = static_cast<CControlUI*>(m_pm.FindControl(szName));
			if(pSepImage)
			{
				SIZE sz = {pSepImage->GetFixedXY().cx,uYPos};
				pSepImage->SetFixedXY(sz);
			}
		}
		
	}

	//锁机设置页面窗口
	::sprintf_s(szName,sizeof(szName),_T("Ctn_Page_%d"),PAGE_LOCK_MACHINE + 1);
	m_pLockMachineCtn[0] = static_cast<CContainerUI*>(m_pm.FindControl(szName));
	::sprintf_s(szName,sizeof(szName),_T("Ctn_Page_%d_1"),PAGE_LOCK_MACHINE + 1);
	m_pLockMachineCtn[1] = static_cast<CContainerUI*>(m_pm.FindControl(szName));

	//手机绑定页面
	::sprintf_s(szName,sizeof(szName),_T("Ctn_Page_%d"),PAGE_BIND_MOBILE + 1);
	m_pBindMobileCtn[0] = static_cast<CContainerUI*>(m_pm.FindControl(szName));
	::sprintf_s(szName,sizeof(szName),_T("Ctn_Page_%d_1"),PAGE_BIND_MOBILE + 1);
	m_pBindMobileCtn[1] = static_cast<CContainerUI*>(m_pm.FindControl(szName));


	//获取手机验证码按钮1(修改用户详细信息tab页中)
	m_pVCodeBtn1 = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Ctn_2_1_GetVCode"));

	//获取手机验证码按钮2(修改锁机设置的tab页中)
	m_pVCodeBtn2 = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Ctn_3_1_GetVCode"));

	//获取手机验证码按钮3(手机绑定的tab页中)
	m_pVCodeBtn3 = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Ctn_4_1_GetVCode"));

	LoadNormalCtrlSkin();

	//初始化基本信息页面
	InitBaseInfo();

	//初始化详细信息页面
	InitDetail();

	//初始化锁机信息页面
	InitLockPCInfo();

	//初始化手机绑定页面
	InitBindMobile();

	//开始默认显示第一页
	ShowPage(m_nDefaultPage,m_nDefaultSubPage);

}

//初始化省份地区关系
void CUserInfoWnd::InitAreaMap()
{
	SafeClearAreaMap();

	vector<LPSTR> city;

	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("北京"),
		*PutInVector(&city,LPSTR("东城|西城|崇文|宣武|朝阳|丰台|石景山|海淀|门头沟|房山|通州|顺义|昌平|大兴|平谷|怀柔|密云|延庆|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("上海"),
		*PutInVector(&city,LPSTR("黄浦|卢湾|徐汇|长宁|静安|普陀|闸北|虹口|杨浦|闵行|宝山|嘉定|浦东|金山|松江|青浦|南汇|奉贤|崇明|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("天津"),
		*PutInVector(&city,LPSTR("和平|东丽|河东|西青|河西|津南|南开|北辰|河北|武清|红挢|塘沽|汉沽|大港|宁河|静海|宝坻|蓟县|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("重庆"),
		*PutInVector(&city,LPSTR("万州|涪陵|渝中|大渡口|江北|沙坪坝|九龙坡|南岸|北碚|万盛|双挢|渝北|巴南|黔江|长寿|綦江|潼南|铜梁|大足|荣昌|壁山|梁平|城口|丰都|垫江|武隆|忠县|开县|云阳|奉节|巫山|巫溪|石柱|秀山|酉阳|彭水|江津|合川|永川|南川|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("河北"),
		*PutInVector(&city,LPSTR("石家庄|邯郸|邢台|保定|张家口|承德|廊坊|唐山|秦皇岛|沧州|衡水|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("山西"),
		*PutInVector(&city,LPSTR("太原|大同|阳泉|长治|晋城|朔州|吕梁|忻州|晋中|临汾|运城|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("内蒙古"),
		*PutInVector(&city,LPSTR("呼和浩特|包头|乌海|赤峰|呼伦贝尔盟|阿拉善盟|哲里木盟|兴安盟|乌兰察布盟|锡林郭勒盟|巴彦淖尔盟|伊克昭盟|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("辽宁"),
		*PutInVector(&city,LPSTR("沈阳|大连|鞍山|抚顺|本溪|丹东|锦州|营口|阜新|辽阳|盘锦|铁岭|朝阳|葫芦岛|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("吉林"),
		*PutInVector(&city,LPSTR("长春|吉林|四平|辽源|通化|白山|松原|白城|延边|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("黑龙江"),
		*PutInVector(&city,LPSTR("哈尔滨|齐齐哈尔|牡丹江|佳木斯|大庆|绥化|鹤岗|鸡西|黑河|双鸭山|伊春|七台河|大兴安岭|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("江苏"),
		*PutInVector(&city,LPSTR("南京|镇江|苏州|南通|扬州|盐城|徐州|连云港|常州|无锡|宿迁|泰州|淮安|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("浙江"),
		*PutInVector(&city,LPSTR("杭州|宁波|温州|嘉兴|湖州|绍兴|金华|衢州|舟山|台州|丽水|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("安徽"),
		*PutInVector(&city,LPSTR("合肥|芜湖|蚌埠|马鞍山|淮北|铜陵|安庆|黄山|滁州|宿州|池州|淮南|巢湖|阜阳|六安|宣城|亳州|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("福建"),
		*PutInVector(&city,LPSTR("福州|厦门|莆田|三明|泉州|漳州|南平|龙岩|宁德|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("江西"),
		*PutInVector(&city,LPSTR("南昌市|景德镇|九江|鹰潭|萍乡|新馀|赣州|吉安|宜春|抚州|上饶|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("山东"),
		*PutInVector(&city,LPSTR("济南|青岛|淄博|枣庄|东营|烟台|潍坊|济宁|泰安|威海|日照|莱芜|临沂|德州|聊城|滨州|菏泽|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("河南"),
		*PutInVector(&city,LPSTR("郑州|开封|洛阳|平顶山|安阳|鹤壁|新乡|焦作|濮阳|许昌|漯河|三门峡|南阳|商丘|信阳|周口|驻马店|济源|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("湖北"),
		*PutInVector(&city,LPSTR("武汉|宜昌|荆州|襄樊|黄石|荆门|黄冈|十堰|恩施|潜江|天门|仙桃|随州|咸宁|孝感|鄂州"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("湖南"),
		*PutInVector(&city,LPSTR("长沙|常德|株洲|湘潭|衡阳|岳阳|邵阳|益阳|娄底|怀化|郴州|永州|湘西|张家界|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("广东"),
		*PutInVector(&city,LPSTR("广州|深圳|珠海|汕头|东莞|中山|佛山|韶关|江门|湛江|茂名|肇庆|惠州|梅州|汕尾|河源|阳江|清远|潮州|揭阳|云浮|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("广西"),
		*PutInVector(&city,LPSTR("南宁|柳州|桂林|梧州|北海|防城港|钦州|贵港|玉林|南宁地区|柳州地区|贺州|百色|河池|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("海南"),
		*PutInVector(&city,LPSTR("海口|三亚|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("四川"),
		*PutInVector(&city,LPSTR("成都|绵阳|德阳|自贡|攀枝花|广元|内江|乐山|南充|宜宾|广安|达川|雅安|眉山|甘孜|凉山|泸州|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("贵州"),
		*PutInVector(&city,LPSTR("贵阳|六盘水|遵义|安顺|铜仁|黔西南|毕节|黔东南|黔南|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("云南"),
		*PutInVector(&city,LPSTR("昆明|大理|曲靖|玉溪|昭通|楚雄|红河|文山|思茅|西双版纳|保山|德宏|丽江|怒江|迪庆|临沧"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("西藏"),
		*PutInVector(&city,LPSTR("拉萨|日喀则|山南|林芝|昌都|阿里|那曲|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("陕西"),
		*PutInVector(&city,LPSTR("西安|宝鸡|咸阳|铜川|渭南|延安|榆林|汉中|安康|商洛|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("甘肃"),
		*PutInVector(&city,LPSTR("兰州|嘉峪关|金昌|白银|天水|酒泉|张掖|武威|定西|陇南|平凉|庆阳|临夏|甘南|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("宁夏"),
		*PutInVector(&city,LPSTR("银川|石嘴山|吴忠|固原|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("青海"),
		*PutInVector(&city,LPSTR("西宁|海东|海南|海北|黄南|玉树|果洛|海西|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("新疆"),
		*PutInVector(&city,LPSTR("乌鲁木齐|石河子|克拉玛依|伊犁|巴音郭勒|昌吉|克孜勒苏柯尔克孜|博尔塔拉|吐鲁番|哈密|喀什|和田|阿克苏|"))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("香港"),
		*PutInVector(&city,LPSTR(""))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("澳门"),
		*PutInVector(&city,LPSTR(""))));
	m_mapArea.insert(pair<char*,vector<char*>>(LPSTR("台湾"),
		*PutInVector(&city,LPSTR("台北|高雄|台中|台南|屏东|南投|云林|新竹|彰化|苗栗|嘉义|花莲|桃园|宜兰|基隆|台东|金门|马祖|澎湖|"))));

}

//把城市名字符串初始到vector中
vector<LPSTR>* CUserInfoWnd::PutInVector(vector<LPSTR>* vec,LPSTR pszCitys)
{
	vec->clear();

	char* p = pszCitys;
	int iTotLen = strlen(p);

	if (iTotLen == 0)
	{
		return vec;
	}

	int nRead = 0;
	int nTempRead = 0;

	while (nRead < iTotLen)
	{
		nTempRead++;
		if (pszCitys[nRead] == '|')
		{
			char* pszTemp = new char[64];
			::memset(pszTemp,NULL,64);
			memcpy(pszTemp,p,nTempRead - 1);
			vec->push_back(pszTemp);

			p += nTempRead;
			nTempRead = 0;
		}
		nRead++;
	}


	return vec;
}

//安全清理地区关系
void CUserInfoWnd::SafeClearAreaMap()
{
	if (!m_mapArea.empty())
	{
		map<LPSTR,vector<LPSTR>>::iterator itMap = m_mapArea.begin();
		while(itMap != m_mapArea.end())
		{
			vector<LPSTR>* vecCitys = &itMap->second;
			if (!vecCitys->empty())
			{
				vector<LPSTR>::iterator itCity = vecCitys->begin();
				while(itCity != vecCitys->end())
				{
					char* p = *itCity;
					if (p)
					{
						delete[] p;
						p = NULL;
					}
					itCity++;
				}
			}
			itMap++;
		}
	}

	m_mapArea.clear();
	return;

}

///
/// 提示
/// @param pToCtrl 在某一控件上提示
/// @param lpSecName ClientMessage.bcf中的段名
/// @param lpKeyName ClientMessage.bcf中的键值
/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
/// @return void
///
void CUserInfoWnd::ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...)
{
	char szFormat[BUFSIZ];
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	strcpy(szFormat,fMsg.GetKeyVal(lpSecName,lpKeyName,lpDefault));

	va_list pArg;
	va_start(pArg,lpDefault);
	char szMessage[BUFSIZ];

#ifdef WIN32
	_vsnprintf(szMessage,BUFSIZ - 1,szFormat,pArg);
#else
	vsnprintf(szMessage,BUFSIZ - 1,szFormat,pArg);
#endif

	va_end(pArg);

	if (pToCtrl)
	{
		;
	}
	else
	{
		DUIOkeyMsgBox(CUserInfoWnd::m_hInstanceWnd,"%s",szMessage);
	}

	return;

}

//更新城市列表
void CUserInfoWnd::UpdateCitys(int nProvIdx)
{
	CComboUI* pCombol = NULL;
	pCombol	= static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Prov")));

	char* szPriName = (char*)pCombol->GetItemAt(nProvIdx)->GetText().GetData();
	AREA_MAP::iterator it = m_mapArea.begin();

	while(it != m_mapArea.end())
	{
		if (strcmp(it->first,szPriName) == 0)
		{
			break;
		}
		it++;
	}

	pCombol	= static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_City")));
	pCombol->RemoveAll();
	
	if (it != m_mapArea.end())
	{
		vector<LPSTR>::iterator itCity = it->second.begin();

		if (itCity != it->second.end())
		{
			while(itCity != it->second.end())
			{
				CListLabelElementUI* p = new CListLabelElementUI();
				p->SetText(*itCity);

				pCombol->Add(p);
				itCity++;
			}
			pCombol->SetEnabled(true);
			pCombol->SelectItem(0);
		}
		else //省级底下没有城市
		{
			pCombol->SetEnabled(false);
		}
	}
}

//初始化基本信息页面
void CUserInfoWnd::InitBaseInfo()
{
	if (!m_pUserInfo)
	{
		return;
	}

	CEditUI* pEdtCtrl = NULL;
	CComboUI* pCobCtrl = NULL;
	CRichEditUI* pREdtCtrl = NULL;
	TCHAR szTemp[BUFSIZ];
	/*CButtonUI *pBtnHeader = NULL;*/

	m_pPageContainer[0]->SetVisible(true);

	//用户名
	pEdtCtrl= static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn1_Name")));
	pEdtCtrl->SetText(m_pUserInfo->nickName);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->nickName) - 2);

	//用户ID
	pEdtCtrl= static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn1_ID")));
	::sprintf_s(szTemp,BUFSIZ,"%d",m_pUserInfo->dwUserID);
	pEdtCtrl->SetText(szTemp);

	//用户性别
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn1_Sex")));
	
	LPCTSTR szItems[2] = {_TEXT("女"),_TEXT("男")};
	LoadComboItems(pCobCtrl,szItems,2);
	pCobCtrl->SelectItem(m_pUserInfo->bBoy);

	//用户个性签名
	pREdtCtrl= static_cast<CRichEditUI*>(m_pm.FindControl(_T("Edt_Ctn1_Sign")));
	pREdtCtrl->SetText(m_pUserInfo->szSignDescr);

	//用户头像
	m_pBtnHeader = static_cast<CButtonUI*>(m_pm.FindControl(_T("HeadPic")));
	CString cs;
	cs.Format("../../log/UserLogos/Logo_%d.png",m_pUserInfo->bLogoID);
	m_pBtnHeader->SetBkImage(cs);
	m_pBtnHeader->SetNormalImage(cs);

	m_pPageContainer[0]->SetVisible(false);
}

//初始化详细信息页面
void CUserInfoWnd::InitDetail()
{
	if (!m_pUserInfo)
	{
		return;
	}

	CEditUI* pEdtCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;
	CComboUI* pCobCtrl = NULL;
	CContainerUI* pCtnCtrl = NULL;

	m_pPageContainer[1]->SetVisible(true);

	//真实姓名
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_RealName")));
	pEdtCtrl->SetText(m_pUserInfo->szRealName);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szRealName) - 2);

	//证件号码
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_IDCard")));
	//隐藏身份证号中间位
	char szTempID[36];
	sprintf_s(szTempID, sizeof(szTempID), "%s", m_pUserInfo->szIDCardNo);
	for (int i=0; i!=strlen(szTempID); ++i)
	{
		if (i >=3 && i<= 14)
			szTempID[i] = '*';
	}
	pEdtCtrl->SetText(szTempID);
	pEdtCtrl->SetEnabled(false);

	//手机号码
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_Ctn2_TelNo_Modify")));
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MobileNo")));
	//隐藏手机号中间4位
	char szTempPhone[50];
	sprintf_s(szTempPhone, sizeof(szTempPhone), "%s", m_pUserInfo->szMobileNo);
	for (int i=0; i!=strlen(szTempPhone); ++i)
	{
		if (i >=3 && i<= 6)
			szTempPhone[i] = '*';
	}
	pEdtCtrl->SetText(szTempPhone);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szMobileNo) - 2);

	if (CGamePlaceApp::GetModuleStatus(_T("UserSecurity"),_T("MobileVerify")) == 1)
	{
		pEdtCtrl->SetEnabled(false);
		pBntCtrl->SetVisible(true);
	}
	else
	{
		pEdtCtrl->SetEnabled(true);
		pBntCtrl->SetVisible(false);
	}

	//手机号码容器
	pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Page_2_1"));
	pCtnCtrl->SetVisible(false);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_MobileNo")));
	pEdtCtrl->SetMaxChar(18);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_VCode")));
	pEdtCtrl->SetMaxChar(6);

	//QQ号码
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_QQNo")));
	pEdtCtrl->SetText(m_pUserInfo->szQQNum);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szQQNum) - 2);

	//设置国籍
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Nat")));
	LPCTSTR szNats[1] = {_TEXT("中国")};
	LoadComboItems(pCobCtrl,szNats,1);
	pCobCtrl->SelectItem(0);

	int iSelect = 0;
	int iCounter = 0;

	//设置省份
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Prov")));
	map<LPSTR,vector<LPSTR>>::iterator itMap= m_mapArea.begin();
	while(itMap != m_mapArea.end())
	{
		CListLabelElementUI* p = new CListLabelElementUI();
		p->SetText(itMap->first);
		pCobCtrl->Add(p);
		itMap++;
	}
	SelectComboItem(pCobCtrl,m_pUserInfo->szAdrProvince);

	//设置城市
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_City")));
	SelectComboItem(pCobCtrl,m_pUserInfo->szAdrCity);

	//邮政编码
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_ZipCode")));
	pEdtCtrl->SetText(m_pUserInfo->szZipCode);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szZipCode) - 2);

	//联系方式
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_Address")));
	pEdtCtrl->SetText(m_pUserInfo->szAddress);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szAddress) - 2);

	//初始学历的显示
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Schooling")));
	LPCTSTR szSchooling[6] = {_TEXT("初中以下"),_TEXT("高中"),_TEXT("中专"),_TEXT("大专"),_TEXT("本科"),_TEXT("硕士以上")};
	LoadComboItems(pCobCtrl,szSchooling,6);
	SelectComboItem(pCobCtrl,m_pUserInfo->szSchooling);

	//电子邮件
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_EMail")));
	pEdtCtrl->SetText(m_pUserInfo->szEmail);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szEmail) - 2);

	//个人主页
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_HomePage")));
	pEdtCtrl->SetText(m_pUserInfo->szHomePage);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szHomePage) - 2);

	//固定电话
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_TelNo")));
	pEdtCtrl->SetText(m_pUserInfo->szTelNo);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szTelNo) - 2);

	//MSN帐号
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MSN")));
	pEdtCtrl->SetText(m_pUserInfo->szMSNID);
	pEdtCtrl->SetMaxChar(sizeof(m_pUserInfo->szMSNID) - 2);

	m_pPageContainer[1]->SetVisible(false);
}

//初始化锁机信息页面
void CUserInfoWnd::InitLockPCInfo()
{
	if (!m_pUserInfo)
	{
		return;
	}

	CLabelUI* pLabCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;

	//绑定操作按钮
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Bnt_Ctn3_PageNext")));
	if(m_pUserInfo->iLockMathine == 1)
	{
		pBntCtrl->SetText(_TEXT("解除绑定"));
		pBntCtrl->SetTag(0);
	}
	else
	{
		pBntCtrl->SetText(_TEXT("我要绑定"));
		pBntCtrl->SetTag(1);
	}


	//锁机标志
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl(_T("Lab_Ctn3_State")));
	if(m_pUserInfo->iLockMathine == 1)
	{
		pLabCtrl->SetShowHtml(false);
		pLabCtrl->SetText(_TEXT("当前状态：已绑定"));
	}
	else
	{
		pLabCtrl->SetShowHtml(true);
		pLabCtrl->SetText(_TEXT("<c #FF0000><b>当前状态：未绑定</b><c>"));
	}

	bool bMobileVerify = false;//(CGamePlaceApp::GetModualStatus(_T("UserSecurity"),_T("MobileVerify")) == 1);

	//二级密码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_Password")));
	pEdtCtrl->SetPasswordMode(true);
	pEdtCtrl->SetText(_T(""));
	pEdtCtrl->SetMaxChar(32);

	//手机号编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_PhoneNo")));
	pEdtCtrl->SetMaxChar(16);

	if (!bMobileVerify)
	{
		pEdtCtrl->SetVisible(false);
	}

	//手机验证码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_VCode")));
	pEdtCtrl->SetMaxChar(6);

	if (!bMobileVerify)
	{
		pEdtCtrl->SetVisible(false);
	}

	if (!bMobileVerify)
	{
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Page_3_1"));
		pLabCtrl->SetVisible(false);
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Page_3_2"));
		pLabCtrl->SetVisible(false);

		pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Ctn_3_1_GetVCode"));
		pBntCtrl->SetVisible(false);
	}

}

//初始化手机绑定页面
void CUserInfoWnd::InitBindMobile()
{
	if (!m_pUserInfo)
	{
		return;
	}

	CLabelUI* pLabCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;

	//绑定操作按钮
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Bnt_Ctn4_PageNext")));
	if(m_pUserInfo->iBindMobile == 1)
	{
		pBntCtrl->SetText(_TEXT("解除绑定"));
		pBntCtrl->SetTag(0);
	}
	else
	{
		pBntCtrl->SetText(_TEXT("我要绑定"));
		pBntCtrl->SetTag(1);
	}


	//锁机标志
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl(_T("Lab_Ctn4_State")));
	if(m_pUserInfo->iBindMobile == 1)
	{
		pLabCtrl->SetShowHtml(false);
		pLabCtrl->SetText(_TEXT("当前状态：已绑定"));
	}
	else
	{
		pLabCtrl->SetShowHtml(true);
		pLabCtrl->SetText(_TEXT("<c #FF0000><b>当前状态：未绑定</b><c>"));
	}

	//手机号编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn4_1_PhoneNo")));
	pEdtCtrl->SetMaxChar(16);

	//手机验证码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn4_1_VCode")));
	pEdtCtrl->SetMaxChar(6);
}

//加载Combol的列表项
void CUserInfoWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
{
	if (!pCtrl || nums <= 0)
	{
		return;
	}

	for (int i = 0; i < nums; ++i)
	{
		CListLabelElementUI* p = new CListLabelElementUI();
		p->SetText(items[i]);
		pCtrl->Add(p);
	}

	return;
}

//根据文本设置选择列表项
bool CUserInfoWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
{
	if (!pCtrl)
	{
		return false;
	}

	int itNums = pCtrl->GetCount();

	if (itNums <= 0)
	{
		return false;
	}

	for (int i = 0; i < itNums; ++i)
	{
		if (strcmp(szText,pCtrl->GetItemAt(i)->GetText().GetData()) == 0)
		{
			pCtrl->SelectItem(i);
			return true;
		}
	}

	return false;
}

void CUserInfoWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Bnt_Close")) 
		{
			Close();
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_1"))
		{
			ShowPage(0);
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_2"))
		{
			ShowPage(1);
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_3"))
		{
			ShowPage(2);
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_4"))
		{
			ShowPage(3);
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_5"))
		{
			ShowPage(4);
		}
		else if (msg.pSender->GetName() == _T("Bnt_Cnt1_Modify"))
		{
			OnUpdateBaseInfo();
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn2_Modify"))
		{
			OnUpdateDetailInfo();
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn_2_1_GetVCode"))
		{
			OnGetMobileVCode(3,m_pVCodeBtn1,
				static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn_2_1_MobileNo")),
				static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn_2_1_VCode")),
				IDT_RE_GET_MOBILE_VCODE_1);
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn2_TelNo_Modify"))
		{
			OnModifyMobileOpen();
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn_2_1_Cancel"))
		{
			OnModifyMobileCancel();
		}

		else if (msg.pSender->GetName() == _T("Btn_Ctn_2_1_OK"))
		{
			OnModifyMobileOK();
		}
		else if (msg.pSender->GetName() == _T("Bnt_Ctn3_PageNext"))
		{
			OnLockOperatePage(msg.pSender->GetTag() == 1);
		}
		
		else if (msg.pSender->GetName() == _T("Bnt_Ctn3_1_Band"))
		{
			OnLockPC(msg.pSender->GetTag() == 1);
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn_3_1_GetVCode"))
		{
			OnGetMobileVCode((m_pUserInfo->iLockMathine == 1 ? 2 : 1),m_pVCodeBtn2,
								static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn3_1_PhoneNo")),
								static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn3_1_VCode")),
								IDT_RE_GET_MOBILE_VCODE_2);
		}

		else if (msg.pSender->GetName() == _T("Bnt_Ctn4_PageNext"))
		{
			OnBindMobileOperatePage(msg.pSender->GetTag() == 1);
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn_4_1_GetVCode"))
		{
			OnGetMobileVCode((m_pUserInfo->iBindMobile == 1 ? 5 : 4),m_pVCodeBtn3,
				static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_1_PhoneNo")),
				static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_1_VCode")),
				IDT_RE_GET_MOBILE_VCODE_3);
		}
		else if (msg.pSender->GetName() == _T("Bnt_Ctn4_1_Band"))
		{
			OnBindMobile(msg.pSender->GetTag() == 1);
		}

		else if (msg.pSender->GetName() == _T("Btn_Ctn5_Modify"))
		{
			OnUpdatePassword();
		}
		else if(msg.pSender->GetName() == _T("HeadPic"))
		{
			//GetMainRoom()->ShowCharmPannel();
			//GetMainRoom()->m_pGameListWnd->OnLogoClick();
			OnChangeHeaderPic();
		}
		
	}
	else if (msg.sType == _T("itemselect"))
	{
		if( msg.pSender->GetName() == _T("Cob_Ctn2_Prov")) 
		{
			UpdateCitys(msg.wParam);
		}
	}
}

//设置用户数据
void CUserInfoWnd::SetPlaceUserInfo(MSG_GP_UserInfo* pUserInfo)
{
	m_pUserInfo = pUserInfo;
}

//加载常规控件皮肤
void CUserInfoWnd::LoadNormalCtrlSkin()
{
	int idx = 0;
	CStdPtrArray* pCtrls = NULL;

	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("EditUI"));
	CEditUI* pEditUI = NULL;
	RECT rcText = {4,1,4,0};
	do 
	{
		pEditUI = static_cast<CEditUI*>(pCtrls->GetAt(idx++));
		
		if (pEditUI)
		{
			pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
			pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
			pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
			pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
			pEditUI->SetTextPadding(rcText);
		}
	} while (pEditUI);

	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("RichEdit"));
	CRichEditUI* pREditUI = NULL;
	RECT rcRText = {4,1,4,0};
	do 
	{
		pREditUI = static_cast<CRichEditUI*>(pCtrls->GetAt(idx++));

		if (pREditUI)
		{
// 			pREditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
// 			pREditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
// 			pREditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
// 			pREditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
// 			pREditUI->SetTextPadding(rcRText);
		}
	} while (pREditUI);

	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("ComboUI"));
	CComboUI* pComboUI = NULL;
	RECT rc = {6,0,0,0};
	do 
	{
		pComboUI = static_cast<CComboUI*>(pCtrls->GetAt(idx++));
		if (pComboUI)
		{
			pComboUI->SetNormalImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,0,500,22' corner='5,0,22,0'");
			pComboUI->SetHotImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,22,500,44' corner='5,0,22,0'");
			pComboUI->SetPushedImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,44,500,66' corner='5,0,22,0'");
			pComboUI->SetDisabledImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,66,500,88' corner='5,0,22,0'");
			pComboUI->EnableScrollBar(true,false);
			pComboUI->SetItemTextPadding(rc);
		}
	} while (pComboUI);

}

//显示容器页面
void CUserInfoWnd::ShowPage(int nIndex,int nSubIndex/* = 0*/)
{
	if (nIndex < 0 || nIndex >= PAGE_NUMS)
	{
		return;
	}

	CControlUI* pContainer = NULL;
	TCHAR szName[20];
	
	for (int i = 0; i < PAGE_NUMS; ++i)
	{
		if (i == PAGE_LOCK_MACHINE) //锁机页面跳回第一场景
		{
			m_pPageContainer[i]->SetVisible(false);
			m_pPageContainer[i] = m_pLockMachineCtn[nSubIndex];
			if (nSubIndex == 1)
			{
				OnLockOperatePage((m_pUserInfo->iLockMathine == 1) ? false : true);
			}
		}
		else if (i == PAGE_BIND_MOBILE) //手机绑定页面跳回第一场景
		{
			m_pPageContainer[i]->SetVisible(false);
			m_pPageContainer[i] = m_pBindMobileCtn[nSubIndex];
			if (nSubIndex == 1)
			{
				OnBindMobileOperatePage((m_pUserInfo->iBindMobile == 1) ? false : true);
			}
		}

		m_pPageContainer[i]->SetVisible(i == nIndex);

		::sprintf_s(szName,sizeof(szName),_T("Bnt_Page_%d"),i + 1);
		pContainer = static_cast<CControlUI*>(m_pm.FindControl(szName));
		pContainer->SetEnabled(i != nIndex);

	}

	::sprintf_s(szName,sizeof(szName),_T("Bnt_Page_%d"),nIndex + 1);
	pContainer = static_cast<CControlUI*>(m_pm.FindControl(szName));
	SIZE sizeBnt = pContainer->GetFixedXY();

	pContainer = static_cast<CControlUI*>(m_pm.FindControl(_T("Img_ListBg")));
	SIZE size = {sizeBnt.cx,sizeBnt.cy - 22};
	pContainer->SetFixedXY(size);

	//翻页时，把所有获取手机验证码的按钮还原。验证码重取定时器关闭掉
	//验证码重取定时器正在计时时才做这个动作
	ResetReVCodeUI();

	return;
}

//更新用户的基本信息
void CUserInfoWnd::OnUpdateBaseInfo()
{
	MSG_GP_S_UserInfo tagUserInfo;

	CEditUI* pEdtCtrl = NULL;
	CRichEditUI* pREdtCtrl = NULL;
	CComboUI* pCobCtrl = NULL;

	//用户名
	pEdtCtrl= static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn1_Name")));
	CStdString str = pEdtCtrl->GetText();
	
	if (str.Find(" ",0) > 0)
	{
		DUIOkeyMsgBox(m_hWnd,"昵称不能含有空格。");
		pEdtCtrl->SetFocus();
		return;
	}

	if (str.GetLength() < 6)
	{
		DUIOkeyMsgBox(m_hWnd,"昵称至少由6个字符组成。");
		pEdtCtrl->SetFocus();
		return;
	}

	::strcpy_s(m_pUserInfo->nickName,sizeof(m_pUserInfo->nickName),str);
	CString cs(m_pUserInfo->nickName);
	//更新大厅用户头像和昵称
	::PostMessage(GetMainRoom()->m_TopDuWnd->GetHWND(),SMG_HEADPIC_CHANGED,m_pUserInfo->bLogoID,0);

	//用户性别
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn1_Sex")));
	m_pUserInfo->bBoy = (pCobCtrl->GetText() == _T("男"));

	//用户个性签名
	pREdtCtrl = static_cast<CRichEditUI*>(m_pm.FindControl(_T("Edt_Ctn1_Sign")));

	if (pREdtCtrl->GetText().GetLength() > 100)
	{
		DUIOkeyMsgBox(m_hWnd,"签名的字数不能超过100个英文字符(50个中文字符)");
		pREdtCtrl->SetFocus();
		return;
	}

	::strcpy_s(m_pUserInfo->szSignDescr,sizeof(m_pUserInfo->szSignDescr),pREdtCtrl->GetText());

	::memcpy(&tagUserInfo,m_pUserInfo,sizeof(MSG_GP_S_UserInfo));
	GetMainRoom()->SendData(&tagUserInfo,sizeof(MSG_GP_S_UserInfo),MDM_GP_USERINFO,ASS_GP_USERINFO_UPDATE_BASE,0);

	return;
}

//更新用户的详细信息
void CUserInfoWnd::OnUpdateDetailInfo()
{
	MSG_GP_S_UserInfo tagUserInfo;

	CEditUI* pEdtCtrl = NULL;
	CComboUI* pCobCtrl = NULL;

	//真实姓名
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_RealName")));
	::strcpy_s(m_pUserInfo->szRealName,sizeof(m_pUserInfo->szRealName),pEdtCtrl->GetText());

	//证件号码
//	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_IDCard")));
//	::strcpy_s(m_pUserInfo->szIDCardNo,sizeof(m_pUserInfo->szIDCardNo),pEdtCtrl->GetText());

	//手机号码
//	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MobileNo")));
//	::strcpy_s(m_pUserInfo->szMobileNo,sizeof(m_pUserInfo->szMobileNo),pEdtCtrl->GetText());

	//QQ号码
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_QQNo")));
	::strcpy_s(m_pUserInfo->szQQNum,sizeof(m_pUserInfo->szQQNum),pEdtCtrl->GetText());

	//设置国籍
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Nat")));
	::strcpy_s(m_pUserInfo->szAdrNation,sizeof(m_pUserInfo->szAdrNation),pCobCtrl->GetText());

	//设置省份
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Prov")));
	::strcpy_s(m_pUserInfo->szAdrProvince,sizeof(m_pUserInfo->szAdrProvince),pCobCtrl->GetText());

	//设置城市
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_City")));
	::strcpy_s(m_pUserInfo->szAdrCity,sizeof(m_pUserInfo->szAdrCity),pCobCtrl->GetText());

	//邮政编码
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_ZipCode")));
	::strcpy_s(m_pUserInfo->szZipCode,sizeof(m_pUserInfo->szZipCode),pEdtCtrl->GetText());

	//联系方式
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_Address")));
	::strcpy_s(m_pUserInfo->szAddress,sizeof(m_pUserInfo->szAddress),pEdtCtrl->GetText());

	//初始学历的显示
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_Ctn2_Schooling")));
	::strcpy_s(m_pUserInfo->szSchooling,sizeof(m_pUserInfo->szSchooling),pCobCtrl->GetText());

	//电子邮件
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_EMail")));
	::strcpy_s(m_pUserInfo->szEmail,sizeof(m_pUserInfo->szEmail),pEdtCtrl->GetText());

	//个人主页
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_HomePage")));
	::strcpy_s(m_pUserInfo->szHomePage,sizeof(m_pUserInfo->szHomePage),pEdtCtrl->GetText());

	//固定电话
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_TelNo")));
	::strcpy_s(m_pUserInfo->szTelNo,sizeof(m_pUserInfo->szTelNo),pEdtCtrl->GetText());

	//MSN帐号
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MSN")));
	::strcpy_s(m_pUserInfo->szMSNID,sizeof(m_pUserInfo->szMSNID),pEdtCtrl->GetText());

	::memcpy(&tagUserInfo,m_pUserInfo,sizeof(MSG_GP_S_UserInfo));
	GetMainRoom()->SendData(&tagUserInfo,sizeof(MSG_GP_S_UserInfo),MDM_GP_USERINFO,ASS_GP_USERINFO_UPDATE_DETAIL,0);

	return;
}

//点击更新手机号
void CUserInfoWnd::OnModifyMobileOpen()
{
	ResetReVCodeUI();

	CContainerUI* pCtnCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;

	pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Page_2_1"));
	pCtnCtrl->SetVisible(true);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_MobileNo")));
	pEdtCtrl->SetFocus();
	pEdtCtrl->SetText(_TEXT(""));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_VCode")));
	pEdtCtrl->SetText(_TEXT(""));
}

//取消更新手机号
void CUserInfoWnd::OnModifyMobileCancel()
{
	ResetReVCodeUI();

	CContainerUI* pCtnCtrl = NULL;

	pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Page_2_1"));
	pCtnCtrl->SetVisible(false);
}

//确定更新手机号
void CUserInfoWnd::OnModifyMobileOK()
{
	CContainerUI* pCtnCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_VCode")));
	CStdString strVCode = pEdtCtrl->GetText();

	if(strVCode.GetLength() == 0)
	{
		ShowTipMessage(NULL,"","","请输入手机验证码");
		pEdtCtrl->SetFocus();
		return;
	}

	//在此添加手机验证
	if (!CMainRoomEx::ValidMobileVCode((char*)strVCode.GetData()))
	{
		ShowTipMessage(NULL,"","","手机验证码不正确");
		pEdtCtrl->SetFocus();
		return;
	}

	pEdtCtrl->SetText(_T(""));

	//手机验证通过
	//把所有获取手机验证码的按钮还原。验证码重取定时器关闭掉
	ResetReVCodeUI();

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_MobileNo")));
	CStdString strMobileNo = pEdtCtrl->GetText();

	pEdtCtrl->SetText(_T(""));

	//隐藏手机号修改框
	pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl(_T("Ctn_Page_2_1")));
	pCtnCtrl->SetVisible(false);

	//把验证通过的新手机号更新到上层编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MobileNo")));
	//隐藏手机号中间4位
	char szTempPhone[50];
	sprintf_s(szTempPhone, sizeof(szTempPhone), "%s", strMobileNo);
	for (int i=0; i!=strlen(szTempPhone); ++i)
	{
		if (i >=3 && i<= 6)
			szTempPhone[i] = '*';
	}
	pEdtCtrl->SetText(szTempPhone);

	ShowTipMessage(NULL,"","","更改改手机号码后，注意按下方“修改”按钮提交更改。");

}

//锁机/解锁按扭响应
void CUserInfoWnd::OnLockOperatePage(bool bLock)
{
	//GetMainRoom()->OnSwitchLockButton();
	m_pPageContainer[PAGE_LOCK_MACHINE]->SetVisible(false);
	m_pPageContainer[PAGE_LOCK_MACHINE] = m_pLockMachineCtn[1];
	m_pPageContainer[PAGE_LOCK_MACHINE]->SetVisible(true);

	CEditUI* pEdtCtrl = NULL;
	CButtonUI* pBtnCtrl = NULL;

	//二级密码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_Password")));
	pEdtCtrl->SetText(_T(""));
	pEdtCtrl->SetFocus();

	//绑定操作按钮
	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Bnt_Ctn3_1_Band")));
	if (bLock)
	{
		if (CMainRoomEx::m_bLockQuit)
		{
			pBtnCtrl->SetText(_TEXT("绑定并退出"));	
		}
		else
		{
			pBtnCtrl->SetText(_TEXT("绑定"));
		}
		pBtnCtrl->SetTag(1);
	}
	else
	{
		pBtnCtrl->SetText(_TEXT("解除绑定"));
		pBtnCtrl->SetTag(0);
	}
	
	//手机号显示，如果已有手机号，则不用填写
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_PhoneNo")));
	if (::strlen(m_pUserInfo->szMobileNo) > 0)
	{
		pEdtCtrl->SetText(m_pUserInfo->szMobileNo);
		pEdtCtrl->SetEnabled(false);
	}
	else
	{
		pEdtCtrl->SetEnabled(true);
		pEdtCtrl->SetText(_T(""));
	}
	
	//手机验证码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_VCode")));
	pEdtCtrl->SetText(_T(""));
}

//进入手机锁绑定导向
void CUserInfoWnd::OnBindMobileOperatePage(bool bLock)
{
	//GetMainRoom()->OnSwitchLockButton();
	m_pPageContainer[PAGE_BIND_MOBILE]->SetVisible(false);
	m_pPageContainer[PAGE_BIND_MOBILE] = m_pBindMobileCtn[1];
	m_pPageContainer[PAGE_BIND_MOBILE]->SetVisible(true);

	CEditUI* pEdtCtrl = NULL;
	CButtonUI* pBtnCtrl = NULL;

	//绑定操作按钮
	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Bnt_Ctn4_1_Band")));
	if (bLock)
	{
		pBtnCtrl->SetText(_TEXT("绑定"));
		pBtnCtrl->SetTag(1);
	}
	else
	{
		pBtnCtrl->SetText(_TEXT("解除绑定"));
		pBtnCtrl->SetTag(0);
	}

	//手机号显示，如果已有手机号，则不用填写
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn4_1_PhoneNo")));
	if (::strlen(m_pUserInfo->szMobileNo) > 0)
	{
		//隐藏手机号中间4位
		char szTempPhone[50];
		sprintf_s(szTempPhone, sizeof(szTempPhone), "%s", m_pUserInfo->szMobileNo);
		for (int i=0; i!=strlen(szTempPhone); ++i)
		{
			if (i >=3 && i<= 6)
				szTempPhone[i] = '*';
		}
		pEdtCtrl->SetText(szTempPhone);
		pEdtCtrl->SetEnabled(false);
	}
	else
	{
		pEdtCtrl->SetEnabled(true);
		pEdtCtrl->SetText(_T(""));
	}

	//手机验证码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn4_1_VCode")));
	pEdtCtrl->SetText(_T(""));
}

//锁机/解锁按扭响应
void CUserInfoWnd::OnLockPC(bool bLock)
{
	CEditUI* pEdtCtrl = NULL;

	//二级密码编辑框
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_Password")));
	CStdString strPassword = pEdtCtrl->GetText();

	if(strPassword.GetLength() == 0 || strPassword.GetLength() > 32)
	{
		ShowTipMessage(NULL,"LockAccountDlg","InputBankPW","请输入银行密码");
		pEdtCtrl->SetFocus();
		return;
	}

	if(false/*CGamePlaceApp::GetModualStatus(_T("UserSecurity"),_T("MobileVerify")) == 1*/)
	{
		pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_VCode")));
		CStdString strVCode = pEdtCtrl->GetText();

		if(strVCode.GetLength() == 0)
		{
			ShowTipMessage(NULL,"","","请输入手机验证码");
			pEdtCtrl->SetFocus();
			return;
		}

		//在此添加手机验证
		if (!CMainRoomEx::ValidMobileVCode((char*)strVCode.GetData()))
		{
			ShowTipMessage(NULL,"","","手机验证码不正确");
			pEdtCtrl->SetFocus();
			return;
		}

		//手机验证通过
		//把所有获取手机验证码的按钮还原。验证码重取定时器关闭掉
		ResetReVCodeUI();

		pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn3_1_PhoneNo")));
		CStdString strMobileNo = pEdtCtrl->GetText();
		if (::strlen(m_pUserInfo->szMobileNo) == 0) //第一次设置手机号
		{
			//已修改过手机号
			pEdtCtrl->SetEnabled(false);
			::lstrcpy(m_pUserInfo->szMobileNo,strMobileNo.GetData());

			//同步手机号到详细设置Tab的“手机号”项中
			pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MobileNo")));
			pEdtCtrl->SetText(strMobileNo);
		}
	}

	//修改支持MD5 和SHA 加密方式
	CenterServerMsg& par=Glb().m_CenterServerPara;

	//md5方式
	char szMD5Pass[50] = {0};
	if( par.m_nEncryptType == 1)
	{
		unsigned char szMDTemp[16];
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char*)strPassword.GetData(),strPassword.GetLength());
		Md5.MD5Final(szMDTemp);
		for (int i = 0; i < 16; i ++) 
			wsprintf(&szMD5Pass[i * 2], "%02x", szMDTemp[i] );
	}

	//sha加密方式
	if(par.m_nEncryptType == 2)
	{
		unsigned szMDTemp[5] = {0};
		CString strSha = strPassword;
		SHA1 sha;
		sha.Reset();
		sha<<strSha;
		sha.Result(szMDTemp);

		CString strSingle;
		strSha="";
		for (int i=0;i<5;i++) 
		{
			strSingle.Format("%.8x",szMDTemp[i]);
			strSha+=strSingle;
		}
		wsprintf(szMD5Pass,"%s",strSha);
	}


	MSG_GP_S_LockAccount lockMsg;
	::memset(&lockMsg,NULL,sizeof(MSG_GP_S_LockAccount));

	lockMsg.dwUserID = m_pUserInfo->dwUserID;
	lockMsg.dwCommanType = bLock ? 1 : 0;
	::lstrcpy(lockMsg.szMathineCode,CMainRoomEx::coreGetCode());
	::lstrcpy(lockMsg.szMD5Pass,szMD5Pass);
	::lstrcpy(lockMsg.szMobileNo,m_pUserInfo->szMobileNo);

	//发出锁定机器命令
	GetMainRoom()->SendData(&lockMsg,sizeof(MSG_GP_S_LockAccount),
		MDM_GP_LOCK_ACCOUNT,ASS_LOCK_ACCOUNT,0);

}

//更新用户密码
void CUserInfoWnd::OnUpdatePassword()
{
	CEditUI* pEdtOldPwd = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn5_oldpw")));
	CEditUI* pEdtNewPwd = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn5_newpw")));
	CEditUI* pEdtCfmPwd = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn5_cfmpw")));

	if (pEdtOldPwd->GetText().GetLength() == 0)
	{
		DUIOkeyMsgBox(m_hWnd,"请填写旧密码");
		pEdtOldPwd->SetFocus();
		return;
	}

	if (pEdtNewPwd->GetText().GetLength() == 0)
	{
		DUIOkeyMsgBox(m_hWnd,"请填写新密码");
		pEdtNewPwd->SetFocus();
		return;
	}

	if (pEdtCfmPwd->GetText().GetLength() == 0)
	{
		DUIOkeyMsgBox(m_hWnd,"请填写新密码确认");
		pEdtCfmPwd->SetFocus();
		return;
	}

	CenterServerMsg& par=Glb().m_CenterServerPara;

	char szMD5Input[64];
	::strcpy_s(szMD5Input,sizeof(szMD5Input),pEdtOldPwd->GetText());

	if( par.m_nEncryptType == 1)
	{

		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)szMD5Input,lstrlen(szMD5Input));
		unsigned char szMDTemp[16];
		Md5.MD5Final(szMDTemp);
		for (int i=0;i<16;i++)
		{
			wsprintf(&szMD5Input[i*2],"%02x",szMDTemp[i]);
		}
	}
	else if(par.m_nEncryptType == 2)
	{
		//JMod-加密处理-20090429
		unsigned szMDTemp[5]={0};
		CString strSha= pEdtOldPwd->GetText();
		SHA1 sha;
		sha.Reset();
		sha<<strSha;
		sha.Result(szMDTemp);

		CString strSingle;
		strSha="";
		for (int i=0;i<5;i++) 
		{
			strSingle.Format("%.8x",szMDTemp[i]);
			strSha+=strSingle;
		}
		wsprintf(szMD5Input,"%s",strSha);
	}


	if (strcmp((const char*)szMD5Input,m_pUserInfo->szMD5Pass) != 0)
	{
		DUIOkeyMsgBox(m_hWnd,"旧密码不正确");
		pEdtOldPwd->SetFocus();
		return;
	}

	if (pEdtNewPwd->GetText().GetLength() < 6 || pEdtNewPwd->GetText().GetLength() > 12)
	{
		DUIOkeyMsgBox(m_hWnd,"新密码的长度为6-12位字符");
		pEdtNewPwd->SetFocus();
		return;
	}

	if (strcmp(pEdtNewPwd->GetText(),pEdtCfmPwd->GetText()) != 0)
	{
		DUIOkeyMsgBox(m_hWnd,"新密码与确认不一致");
		pEdtCfmPwd->SetFocus();
		return;
	}

	::strcpy_s(szMD5Input,sizeof(szMD5Input),pEdtNewPwd->GetText());

	if( par.m_nEncryptType == 1)
	{

		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)szMD5Input,lstrlen(szMD5Input));
		unsigned char szMDTemp[16];
		Md5.MD5Final(szMDTemp);
		for (int i=0;i<16;i++)
		{
			wsprintf(&szMD5Input[i*2],"%02x",szMDTemp[i]);
		}
	}
	else if(par.m_nEncryptType == 2)
	{
		//JMod-加密处理-20090429
		unsigned szMDTemp[5]={0};
		CString strSha= pEdtNewPwd->GetText();
		SHA1 sha;
		sha.Reset();
		sha<<strSha;
		sha.Result(szMDTemp);

		CString strSingle;
		strSha="";
		for (int i=0;i<5;i++) 
		{
			strSingle.Format("%.8x",szMDTemp[i]);
			strSha+=strSingle;
		}
		wsprintf(szMD5Input,"%s",strSha);
	}

	CMyDiskInfo diskInfo;
	diskInfo.GetDiskInfo();

	MSG_GP_S_ChPassword tagChPwd;
	tagChPwd.dwUserID = m_pUserInfo->dwUserID;
	lstrcpy(tagChPwd.szHardID,diskInfo.szSerialNumber);

	::memcpy_s(tagChPwd.szMD5OldPass,sizeof(tagChPwd.szMD5OldPass),m_pUserInfo->szMD5Pass,sizeof(tagChPwd.szMD5OldPass));
	::memcpy_s(tagChPwd.szMD5NewPass,sizeof(tagChPwd.szMD5NewPass),szMD5Input,sizeof(szMD5Input));

	GetMainRoom()->SendData(&tagChPwd,sizeof(MSG_GP_S_ChPassword),MDM_GP_USERINFO,ASS_GP_USERINFO_UPDATE_PWD,0);

	::memcpy_s(m_pUserInfo->szMD5Pass,sizeof(m_pUserInfo->szMD5Pass),szMD5Input,sizeof(m_pUserInfo->szMD5Pass));
	
}

//点击获取验证码的按钮
void CUserInfoWnd::OnGetMobileVCode(UINT uType,BzDui::CButtonUI* pBtn,BzDui::CEditUI* pMobileNoEdt,BzDui::CEditUI* pVCodeEdt,UINT uTimerID)
{
	CStdString strMobileNo;
	if (pMobileNoEdt->IsEnabled())
		strMobileNo = pMobileNoEdt->GetText();
	else
		strMobileNo = m_pUserInfo->szMobileNo;
	if (strMobileNo.GetLength() == 0)
	{
		ShowTipMessage(NULL,"","","请输入手机号码");
		pMobileNoEdt->SetFocus();
		return;
	}

	pMobileNoEdt->SetEnabled(false);

	pVCodeEdt->SetText(_T(""));
	pVCodeEdt->SetFocus();
	CMainRoomEx::ClearMobileVCode();
	GetMainRoom()->OnSendMobileVCodeSms((char*)strMobileNo.GetData(),uType);

	pBtn->SetEnabled(false);
	pBtn->SetText("获取...");

	::SetTimer(m_hWnd,uTimerID,1000,NULL);
}

//清理所有重取验证码的定时器
void CUserInfoWnd::ResetReVCodeUI()
{
	//if (m_iReVCodeTime < CMobileVerifyWnd::GetVCodeValidTime();)
	{
		::KillTimer(m_hWnd,IDT_RE_GET_MOBILE_VCODE_1);
		::KillTimer(m_hWnd,IDT_RE_GET_MOBILE_VCODE_2);
		::KillTimer(m_hWnd,IDT_RE_GET_MOBILE_VCODE_3);
		CMainRoomEx::ClearMobileVCode();

		m_pVCodeBtn1->SetEnabled(true);
		m_pVCodeBtn1->SetText(_T("获取验证码"));

		m_pVCodeBtn2->SetEnabled(true);
		m_pVCodeBtn2->SetText(_T("获取验证码"));

		m_pVCodeBtn3->SetEnabled(true);
		m_pVCodeBtn3->SetText(_T("获取验证码"));

		CEditUI* pEdtCtrl = NULL;
		pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn_2_1_MobileNo")));
		pEdtCtrl->SetEnabled(true);

		m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();
	}
}

//锁机/解锁信息
void CUserInfoWnd::OnLockResult(bool bLock)
{
	bool bShowing = m_pPageContainer[PAGE_LOCK_MACHINE]->IsVisible();
	m_pPageContainer[PAGE_LOCK_MACHINE]->SetVisible(false);
	m_pPageContainer[PAGE_LOCK_MACHINE] = m_pLockMachineCtn[0];
	if (bShowing)
	{
		m_pPageContainer[PAGE_LOCK_MACHINE]->SetVisible(true);
	}
	

	CLabelUI* pLabCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;

	//绑定操作按钮
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Bnt_Ctn3_PageNext")));
	if(bLock)
	{
		pBntCtrl->SetText(_TEXT("解除绑定"));
		pBntCtrl->SetTag(0);
	}
	else
	{
		pBntCtrl->SetText(_TEXT("我要绑定"));
		pBntCtrl->SetTag(1);
	}

	//锁机标志
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl(_T("Lab_Ctn3_State")));
	if(bLock)
	{
		pLabCtrl->SetShowHtml(false);
		pLabCtrl->SetText(_TEXT("当前状态：已绑定"));
	}
	else
	{
		pLabCtrl->SetShowHtml(true);
		pLabCtrl->SetText(_TEXT("<c #FF0000><b>当前状态：未绑定</b><c>"));
	}

}

//绑定手机号码按扭响应
void CUserInfoWnd::OnBindMobile(bool bLock)
{
	CEditUI* pEdtCtrl = NULL;

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn4_1_VCode")));
	CStdString strVCode = pEdtCtrl->GetText();

	if(strVCode.GetLength() == 0)
	{
		ShowTipMessage(NULL,"","","请输入手机验证码");
		pEdtCtrl->SetFocus();
		return;
	}

	//在此添加手机验证
	if (!CMainRoomEx::ValidMobileVCode((char*)strVCode.GetData()))
	{
		ShowTipMessage(NULL,"","","手机验证码不正确");
		pEdtCtrl->SetFocus();
		return;
	}

	//手机验证通过
	//把所有获取手机验证码的按钮还原。验证码重取定时器关闭掉
	ResetReVCodeUI();

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn4_1_PhoneNo")));
	CStdString strMobileNo = pEdtCtrl->GetText();
	if (::strlen(m_pUserInfo->szMobileNo) == 0) //第一次设置手机号
	{
		//已修改过手机号
		pEdtCtrl->SetEnabled(false);
		::lstrcpy(m_pUserInfo->szMobileNo,strMobileNo.GetData());

		//同步手机号到详细设置Tab的“手机号”项中
		pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_Ctn2_MobileNo")));
		pEdtCtrl->SetText(strMobileNo);
	}


	MSG_GP_S_BindMobile BindMsg;
	::memset(&BindMsg,NULL,sizeof(MSG_GP_S_BindMobile));

	BindMsg.dwUserID = m_pUserInfo->dwUserID;
	BindMsg.dwCommanType = bLock ? 1 : 0;
	::lstrcpy(BindMsg.szMobileNo,m_pUserInfo->szMobileNo);

	//发出锁定机器命令
	GetMainRoom()->SendData(&BindMsg,sizeof(MSG_GP_S_BindMobile),
		MDM_GP_BIND_MOBILE,ASS_GP_BIND_MOBILE,0);

}

//绑定手机号码信息
void CUserInfoWnd::OnBindMobileResult(bool bLock)
{
	bool bShowing = m_pPageContainer[PAGE_BIND_MOBILE]->IsVisible();
	m_pPageContainer[PAGE_BIND_MOBILE]->SetVisible(false);
	m_pPageContainer[PAGE_BIND_MOBILE] = m_pBindMobileCtn[0];
	if (bShowing)
	{
		m_pPageContainer[PAGE_BIND_MOBILE]->SetVisible(true);
	}


	CLabelUI* pLabCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;

	//绑定操作按钮
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(_T("Bnt_Ctn4_PageNext")));
	if(bLock)
	{
		pBntCtrl->SetText(_TEXT("解除绑定"));
		pBntCtrl->SetTag(0);
	}
	else
	{
		pBntCtrl->SetText(_TEXT("我要绑定"));
		pBntCtrl->SetTag(1);
	}

	//锁机标志
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl(_T("Lab_Ctn4_State")));
	if(bLock)
	{
		pLabCtrl->SetShowHtml(false);
		pLabCtrl->SetText(_TEXT("当前状态：已绑定"));
	}
	else
	{
		pLabCtrl->SetShowHtml(true);
		pLabCtrl->SetText(_TEXT("<c #FF0000><b>当前状态：未绑定</b><c>"));
	}
}

//点击更换头像按钮
void CUserInfoWnd::OnChangeHeaderPic()
{
	m_pLogoChangeWnd = new CLogoChangeDlg();
	m_pLogoChangeWnd->Create(this->GetHWND(),NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_pLogoChangeWnd->CenterWindow();
	m_pLogoChangeWnd->ShowWindow(true);

}

//改变玩家图像
int CUserInfoWnd::OnSetHeadPic(WPARAM wParam, LPARAM lParam)
{
	int iIndex = wParam;
	if (0 > iIndex)
		return 0;	
	if (m_pLogoChangeWnd)
	{
		m_pLogoChangeWnd->ShowWindow(SW_HIDE);
	}
	//设置用户头像
	if (iIndex == m_pUserInfo->bLogoID)
	{
		return 0;
	}
	m_pUserInfo->bLogoID = iIndex;
	//SetUserInfoWnd(m_pUserInfo);

	//将数据上传到服务器
	GetMainRoom()->SendSetLogoInfo(m_pUserInfo->dwUserID, iIndex);
	return iIndex;
}