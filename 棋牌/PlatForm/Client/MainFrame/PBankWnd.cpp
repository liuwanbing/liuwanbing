#include "StdAfx.h"
#include "PBankWnd.h"
#include "MainRoomEx.h"
#include "MyDiskInfo.h"
#include "Bank4ZMessage.h"

using namespace BzDui;

#define PAGE_NUMS			5	//功能项目数量
#define RECORD_PAGE_SIZE	8	//转日志每页的记录数量

#define NICK_ID_USE_TO_TRANSFER	11 //用于银行转账
#define NICK_ID_USE_TO_DEPOSIT	12 //用于点卡充值
#define NICK_ID_BYID			0  //通过用户ID查询用户昵称
#define NICK_ID_BYNICKNAME		1  //通过玩家昵称查询用户ID

static char* szDigit[DIGITSIZE] = {"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖"}; //中文大写数字
static char* szUnit[UNITSIZE] = {"", "拾", "佰", "仟", "万", "拾", "佰", "仟", "亿", "拾","佰","仟","万", "拾","佰","仟","万","拾","佰"};	  //中文大写单位

//当前实例窗口句柄
HWND CPBankWnd::m_hInstanceWnd = NULL;

CPBankWnd::CPBankWnd(CWnd* pParent,bool bInMainRoom /*= false*/)
{
	//默认显示的页索引
	m_nDefaultPage = 0;

	bFlag = false;

	//充值的目标玩家，使用ID方式还是使用昵称方式
	m_bUseDestID = true;

	m_bTransferSuccess = true/*false*/;

	m_pGameRoom = pParent;

	m_strSaveID = "";

	m_iUseNickName = 0;

	m_uCheckInOrOut = 0;

	m_mapRecord.clear();

	//转账日志记录表显示位置
	m_iRecDispPos = 0;

	if (bInMainRoom)
	{
		//用户ID
		m_dwOpUserID = ((CMainRoomEx*)pParent)->m_PlaceUserInfo.dwUserID;
		m_emParWndType = IsMainRoom;

	}
	else
	{
		//用户ID
		m_dwOpUserID = ((CGameRoomEx*)pParent)->GetMeUserInfo()->dwUserID;
		m_emParWndType = ISGameRoom;
	}

}

CPBankWnd::~CPBankWnd(void)
{
	m_mapRecord.clear();
}

LPCTSTR CPBankWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CPBankWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CPBankWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CPBankWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);

	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("BankWnd\\BankWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	Init();

	CPBankWnd::m_hInstanceWnd = m_hWnd;

	//::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	return 0;
}

LRESULT CPBankWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPBankWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CPBankWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPBankWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPBankWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CPBankWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPBankWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CPBankWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

LPCTSTR CPBankWnd::GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
{
 	CString strBuffer;

	if (pControl->GetParent()->GetParent() == m_pRecordList) //转账记录表
	{
		std::map<int,MSG_GR_R_TransferRecord_t>::iterator it = m_mapRecord.find(m_iRecDispPos + iIndex);

		if (it == m_mapRecord.end())
		{
			return _T("");
		}

		MSG_GR_R_TransferRecord_t* pTRec = &it->second;

		if (pTRec == NULL)
		{
			return _T("");
		}

		if (pTRec->dwUserID == 0)
		{
			return _T("");
		}

		if (iSubItem == 0) //操作时间
		{
			strBuffer.Format(_T("%02d-%02d %02d:%02d"),
				pTRec->oleDateTime.GetMonth(),
				pTRec->oleDateTime.GetDay(),
				pTRec->oleDateTime.GetHour(),
				pTRec->oleDateTime.GetMinute());
			return strBuffer;
		}
		else if (iSubItem == 1)
		{
			strBuffer.Format(_T(" %d"),pTRec->dwUserID);
			return strBuffer;
		}
		else if (iSubItem == 2)
		{
			strBuffer.Format(_T(" %d"),pTRec->destUserID);
			return strBuffer;
		}
		else if (iSubItem == 3)
		{
			TCHAR szBuffer[32];
			GetNumString(szBuffer,pTRec->i64Money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			strBuffer.Format(_T(" %s"),szBuffer);
			return strBuffer;
		}
	}

	return _T("");
}

bool CPBankWnd::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//目前只限于大厅中的银行使用

	if (pNetHead->bMainID != MDM_GP_BANK && pNetHead->bMainID != MDM_BANK)
	{
		 return true;
	}

	switch (pNetHead->bAssistantID)
	{
	case ASS_BANK_NORMAL:
	case ASS_GP_BANK_CHECK:
		{
			bank_op_normal *p = (bank_op_normal *)pNetData;
			if (p != NULL)
			{
				if (p->_operate_type == 1) //取钱
				{
					if (pNetHead->bHandleCode == HC_BANK_OP_SUC)
					{
						TCHAR szNum[128];
						GetNumString(szNum,p->_money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);

// 						ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_Success",
// 							"您成功已取出 %s 金币到【%s】游戏的钱包中。",
// 							szNum,GetGameNameById(p->_game_id));

						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
						pPwdEdt->SetText("");

						//更新数据及显示数据
						UpdateUserMoney(0,p->_money);
						
						GetMainRoom()->m_PlaceUserInfo.i64Money += p->_money;

						return true;

					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_ERR_PWD)
					{
						DUIOkeyMsgBox(m_hWnd,"银行密码不正确");
						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
						pPwdEdt->SetFocus();

						return true;
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_CHECKMONEY_INV)
					{
						DUIOkeyMsgBox(m_hWnd,"您正在游戏中，不能取款。");
						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
						pPwdEdt->SetFocus();

						return true;
					}
				}
				else //存钱
				{
					if (pNetHead->bHandleCode == HC_BANK_OP_SUC)
					{
						TCHAR szNum[128];
						GetNumString(szNum,p->_money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);

// 						ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_Success",
// 							"您成功已从【%s】游戏钱包取出 %s 到钱柜中。",
// 							GetGameNameById(p->_game_id),szNum);

						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_OutCoints"));
						pPwdEdt->SetFocus();
						pPwdEdt->SetText("");

						//更新数据及显示数据
						UpdateUserMoney(1,p->_money);

						GetMainRoom()->m_PlaceUserInfo.i64Money -= p->_money;
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_CHECKMONEY_INV)
					{
						DUIOkeyMsgBox(m_hWnd,"您正在游戏中，不能存款。");
						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
						pPwdEdt->SetFocus();

						return true;
					}

					return true;
				}
			}
			else
			{
				if (pNetHead->bHandleCode == HC_BANK_OP_CHECKOUT_PLAYING) 
				{
					ShowTipMessage(NULL,"GameRoom","NoDrawForPlaying","您正在游戏中，不能取款。");
					return true;
				}
				else if (pNetHead->bHandleCode == HC_BANK_OP_CHECKIN_PLAYING) 
				{
					ShowTipMessage(NULL,"GameRoom","NoSaveForPlaying","您正在游戏中，不能存款。");
					return true;
				}
				else if (pNetHead->bHandleCode == HC_BANK_OP_CHECKIN_ZHUANG) 
				{
					ShowTipMessage(NULL,"GameRoom","NoSaveForZhuangjia","您正在上庄中，不能存款。");
					return true;
				}
			}

			break;
		}

	case ASS_GP_BANK_CHPWD:	//< 修改密码
		{
			if (pNetHead->bHandleCode == HC_GP_BANK_CHPWD_SUC)
			{
				ShowTipMessage(NULL,"GameRoom","ChangePasswdOK","修改银行密码成功！请妥善保管新密码。");
			}
			else if (pNetHead->bHandleCode == HC_GP_BANK_CHPWD_ERR_PWD)
			{
				ShowTipMessage(NULL,"GameRoom","ChangePasswdFailed","修改银行密码失败，旧银行不正确！");
			}
			break;
		}
	}

	return true;
}

void CPBankWnd::Init() 
{

	char szName[20];
	UINT uYPos = 40;

	for (int i = 0; i < PAGE_NUMS; ++i)
	{
		::sprintf_s(szName,sizeof(szName),_T("Ctn_Page_%d"),i + 1);
		m_pPageContainer[i] = static_cast<CContainerUI*>(m_pm.FindControl(szName));
	}

	for (int i = 0; i < PAGE_NUMS; ++i)
	{
		::sprintf_s(szName,sizeof(szName),_T("Bnt_Page_%d"),i + 1);
		CButtonUI* pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl(szName));
		if(pBtnCtrl)
		{
			SIZE sz = {pBtnCtrl->GetFixedXY().cx,uYPos};
			pBtnCtrl->SetFixedXY(sz);
			if(pBtnCtrl->GetName() == _T("Bnt_Page_2") || pBtnCtrl->GetName() == _T("Bnt_Page_3"))
			{
				if(CGamePlaceApp::GetModuleStatus("Bank","TransferMoney",1) == 0)
				{
					pBtnCtrl->SetVisible(false);
					continue;
				}
			}
			uYPos += 34;
		}
	}

	//记录表控件指针
	m_pRecordList = static_cast<CListUI*>(m_pm.FindControl("Lst_Ctn3_Record"));
	m_pRecordList->SetTextCallback(this);

	//翻页控件按钮
	m_pFirstPageBnt = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_Ctn3_First"));
	m_pPrePageBnt   = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_Ctn3_Previous"));
	m_pNextPageBnt  = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_Ctn3_Next"));
	m_pRearPageBnt  = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_Ctn3_Rear"));

	CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Bank"));
	pEdtCtrl->SetShowHtml(true);
	pEdtCtrl->SetReadOnly(true);

	LoadNormalCtrlSkin();

	//初始化财富信息
	InitMoneyInfo();

	//初始赠送操作UI
	InitGiftInfo();

	//初始存取款操作UI
	InitAccessInfo();

	//初始化日志操作UI
	InitRecordInfo();

	//初始化修改银行密码UI
	InitModifyPwdInfo();

	//开始默认显示第一页
	ShowPage(m_nDefaultPage);

}

//加载Combol的列表项
void CPBankWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CPBankWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

//金额转中文大写
void CPBankWnd::ChangeDigitToChinese(__int64 i64Num, CString &strResult)
{
	int i,size,high;

	strResult = "";
	digitArray.clear();
	i = 0;
	size = 0;

	//将数值的数字提出来放在数组中，逆序排序
	while(i64Num)
	{
		digitArray.push_back(i64Num%10);
		i64Num /=10;
		size++;
	}

	for(i = size-1; i >= 0; i--)
	{
		//加入数字
		if(digitArray[i] == 0 )//数字为零情况处理
		{
			if(i-1 == -1) break; 
			if(digitArray[i-1] != 0 && (szUnit[i] != "万" && szUnit[i] != "亿" )) //非亿、万位为0时处理，不连续时加入零。
				strResult += szDigit[digitArray[i]];
		}
		else 
			strResult += szDigit[digitArray[i]];		

		//加入单位
		if(digitArray[i] == 0 && (szUnit[i] != "万" && szUnit[i] != "亿" ) ) //非亿、万位为0时跳过
			continue; 
		strResult += szUnit[i];
	}
	if(strResult.Find("亿万") != -1)//字符串中有亿万连续的特殊处理
		strResult.Replace("亿万", "亿");
}

///
/// 设置财富信息
/// @param nBank 银行存款
/// @param nWallet 手中现金
/// @return void
///
void CPBankWnd::SetWealthInfor(__int64 i64Bank, __int64 i64Wallet)
{
	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	m_i64BankMoney = i64Bank;
	m_i64WalletMoney = i64Wallet;

	TCHAR szNum[128] = {0};
	TCHAR szBank[128] = {0};
	GetNumString(szBank, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	GetNumString(szNum, m_i64WalletMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

	CLabelUI* pLabCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;
	TCHAR szOut[128];
	
	//银行保险箱
	::memset(szOut,NULL,sizeof(szOut));
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn1_Wallet"));
	::sprintf(szOut,pLabCtrl->GetText().GetData(),szNum);
	pLabCtrl->SetText(szOut);

	//现在金币
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn1_Bank"));
	::sprintf(szOut,pLabCtrl->GetText().GetData(),szBank);
	pLabCtrl->SetText(szOut);

	::memset(szOut,NULL,sizeof(szOut));
	::sprintf(szOut,fMsg.GetKeyVal("PBankDlg","UHaveCoin","您钱包中有%s金币。"),szNum);
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_Tip1"));
	pLabCtrl->SetText(szOut);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Bank"));
	pEdtCtrl->SetText(szBank);

	::memset(szOut,NULL,sizeof(szOut));
	::sprintf(szOut,fMsg.GetKeyVal("PBankDlg","AccountCoin","现有金币：%s"),szNum);
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_Wallet"));
	pLabCtrl->SetText(szOut);

	::memset(szOut,NULL,sizeof(szOut));
	::sprintf(szOut,fMsg.GetKeyVal("PBankDlg","BoxCoin","银行存款：%s"),szBank);
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_Bank"));
	pLabCtrl->SetText(szOut);
	
}

// PengJiLin, 2010-8-23, 根据用户ID获取的昵称
// 设置获取的昵称
void CPBankWnd::SetNickName(TCHAR* szNickName)
{
	m_strNickName = szNickName;

	//m_Button_Trans_All.EnableWindow(TRUE);
	//m_btnTranOK.EnableWindow(TRUE);
}

///更新用户ID昵称的查询结果
void CPBankWnd::UpdateNicknameIDResult(GetNickNameID_t* pNID)
{
	CLabelUI* pLabCtrl = NULL;
	CLabelUI* pLabCtrl_temp = NULL;
	TCHAR szText[64];

	if (pNID->_nUseful == NICK_ID_USE_TO_TRANSFER)
	{
		pLabCtrl_temp = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID_temp"));
		pLabCtrl_temp->SetText("");

		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID"));

		if (pNID->_nType == NICK_ID_BYID)
		{
			if (strlen(pNID->_nickname) == 0)
			{
				pLabCtrl->SetText(_T("(玩家不存在)"));
			}
			else
			{
				_stprintf(szText,"(昵称：%s)",pNID->_nickname);
				pLabCtrl->SetText(szText);
				pLabCtrl_temp->SetText(pNID->_nickname);
			}
		}
		else if (pNID->_nType == NICK_ID_BYNICKNAME)
		{
			if (pNID->_user_id == 0)
			{
				pLabCtrl->SetText(_T("(玩家不存在)"));
			}
			else
			{
				_stprintf(szText,"(ID：%d)",pNID->_user_id);
				pLabCtrl->SetText(szText);
				_stprintf(szText,"%d",pNID->_user_id);
				pLabCtrl_temp->SetText(szText);
			}
		}
		
	}
	else if (pNID->_nUseful == NICK_ID_USE_TO_DEPOSIT)
	{
		pLabCtrl_temp = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID_temp"));
		pLabCtrl_temp->SetText("");

		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID"));

		if (pNID->_nType == NICK_ID_BYID)
		{
			if (strlen(pNID->_nickname) == 0)
			{
				pLabCtrl->SetText(_T("(玩家不存在)"));
			}
			else
			{
				_stprintf(szText,"(昵称：%s)",pNID->_nickname);
				pLabCtrl->SetText(szText);
				pLabCtrl_temp->SetText(pNID->_nickname);
			}
		}
		else if (pNID->_nType == NICK_ID_BYNICKNAME)
		{
			return;
		}
	}

	return;
}

/// 转帐成功后更新银行数据
/// @param Opt 0-表示转出给别人，1-表示由别人转入给自己
/// @param nMoney 表示本次所操作的金币数
void CPBankWnd::UpdateUserMoneyByTransfer(int OptType, __int64 i64Money)
{
	if( OptType == 0 ) //取出
	{
		m_i64BankMoney	-= i64Money;
	}
	else //存入
	{
		m_i64BankMoney	+= i64Money;
	}

	TCHAR szBank[128] = {0};
	GetNumString(szBank, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

	CEditUI* pEdtCtrl = NULL;
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Bank"));
	pEdtCtrl->SetText(szBank);

}

/// 存取钱后更新界面
/// @param Opt 0-表示取出，1-表示存入
/// @param nMoney 表示本次所操作的金币数
void CPBankWnd::UpdateUserMoney(int OptType, __int64 i64Money)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	TCHAR szNum[128];
	GetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	CString strMsg;
	CString strFormat = fMsg.GetKeyVal("PBankDlg", "CheckInOutTip", "成功%s %s 金币");
	if( OptType == 0 ) //取出
	{
		m_i64BankMoney	-= i64Money;
		m_i64WalletMoney += i64Money;
		strMsg.Format(strFormat, "取出", szNum);
	}
	else //存入
	{
		m_i64BankMoney	+= i64Money;
		m_i64WalletMoney -= i64Money;
		strMsg.Format(strFormat, "存入", szNum);
	}

	SetWealthInfor(m_i64BankMoney,m_i64WalletMoney);

// 	if (GetSafeHwnd()!=NULL)
// 	{
// 		ShowInterface();
// 		/// 提示玩家存取钱成功，
// 		SetDlgItemText(IDC_ETBREGPW, strMsg.GetBuffer());
// 		GetDlgItem(IDC_ETBREGPW)->ShowWindow(SW_SHOW);
// 	}

	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_Result"));
	pLabCtrl->SetText(strMsg);

	DUIOkeyMsgBox(m_hWnd,strMsg);

}

///刷新记录的页码
void CPBankWnd::UpdateRecordPageState()
{
	int iPageCount = ((m_mapRecord.size() % RECORD_PAGE_SIZE) == 0) ? 
		m_mapRecord.size() / RECORD_PAGE_SIZE : m_mapRecord.size() / RECORD_PAGE_SIZE + 1;
	int iCurrPage = m_iRecDispPos / RECORD_PAGE_SIZE + 1;

	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn3_4"));
	TCHAR szBuffer[32];
	_stprintf(szBuffer,_T("第%d/%d页"),iCurrPage,iPageCount);
	pLabCtrl->SetText(szBuffer);

	m_pFirstPageBnt->SetEnabled(iCurrPage > 1);
	m_pPrePageBnt->SetEnabled(iCurrPage > 1);
	m_pNextPageBnt->SetEnabled(iCurrPage < iPageCount);
	m_pRearPageBnt->SetEnabled(iCurrPage < iPageCount);
}

///
/// 提示
/// @param pToCtrl 在某一控件上提示
/// @param lpSecName ClientMessage.bcf中的段名
/// @param lpKeyName ClientMessage.bcf中的键值
/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
/// @return void
///
void CPBankWnd::ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...)
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
		DUIOkeyMsgBox(CPBankWndEx::m_hInstanceWnd,"<y 25><p 5>%s</p></y>",szMessage);
	}

	return;

}

void CPBankWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if (msg.pSender->GetName() == _T("Bnt_Page_1"))
		{
			ShowPage(0);
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_2"))
		{
			ShowPage(1);
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_3"))
		{
			ShowPage(2);
			OnBnClickedRecord();
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_4"))
		{
			ShowPage(3);
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Page_5"))
		{
			ShowPage(4);
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn2_OK"))
		{
			//确定转账
			OnBnClickedAccountOK();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn4_All"))
		{
			//存入取出全部
			OnBnClickedCoinAll();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn4_OK"))
		{
			//确定存取
			OnBnClickedBankOK();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn3_Refresh"))
		{
			//刷新转账记录
			m_bTransferSuccess = TRUE;
			OnBnClickedRecord();
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Ctn3_First"))
		{
			//记录翻到首页
			OnBnClickedRecordFirstPage();
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Ctn3_Previous"))
		{
			//记录翻上一页
			OnBnClickedRecordPreviousPage();
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Ctn3_Next"))
		{
			//记录翻下一页
			OnBnClickedRecordNextPage();
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_Ctn3_Rear"))
		{
			//记录翻到尾页
			OnBnClickedRecordRearPage();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Ctn5_OK"))
		{
			//修改银行密码
			OnBnClickedChPW();
			return;
		}
		else if(msg.pSender->GetName() == _T("Bnt_Close") ||
			msg.pSender->GetName() == _T("Btn_Ctn5_Close")) //关闭按钮
		{
			Close();
			m_pGameRoom->PostMessage(WM_BANK_CLOSE,0,0);
			return;
		}
	}
	else if (msg.sType == _T("textchanged"))
	{
		if (msg.pSender->GetName() == _T("Edt_Ctn2_OutCoint"))
		{
			//转账金额小写转换为中文大写
			OnEnChangeEtbconnoin();
			return;
		}
		else if (msg.pSender->GetName() == _T("Edt_Ctn4_OutCoints"))
		{
			//存取款金额小写转换为中文大写
			OnEnChangeMoney();
			return;
		}
	}
	else if(msg.sType == _T("selectchanged"))
	{
		if (msg.pSender->GetName() == _T("Opt_Ctn2_ID"))
		{
			//切换至目标用户ID输入方式
			OnBnClickedRTransId();
			return;
		}
		else if (msg.pSender->GetName() == _T("Opt_Ctn2_NickName"))
		{
			//切换至目标用户昵称输入方式
			OnBnClickedRTransNn();
			return;
		}

		if (msg.pSender->GetName() == _T("Opt_Ctn4_In"))
		{
			//切换至存入模式
			OnBnClickedAccessIn();
			return;
		}
		else if (msg.pSender->GetName() == _T("Opt_Ctn4_Out"))
		{
			//切换至取出模式
			OnBnClickedAccessOut();
			return;
		}

	}
	else if(msg.sType == _T("setfocus"))
	{
		if (msg.pSender->GetName() == _T("Edt_Ctn2_TargetUser"))
		{
			//输入转账目标用户，进行一些检测
			m_strFocusTemp = msg.pSender->GetText();
			return;
		}
	}
	else if(msg.sType == _T("killfocus"))
	{
// 		if (msg.pSender->GetName() == _T("Edt_Ctn2_TargetUser"))
// 		{
// 			//输入转账目标用户，进行一些检测
// 			OnIDKillFocus();
// 			return;
// 		}

		if (msg.pSender->GetName() == _T("Edt_Ctn2_TargetUser"))
		{
			//目标用户编辑框没有输入内容或为空都不触发OnIDKillFocus
			OnKillfocusTargetUser();

			return;
		}

	}
	else if(msg.sType == _T("return"))
	{
		if (msg.pSender->GetName() == _T("Edt_Ctn2_TargetUser"))
		{
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_OutCoint"));
			pEdtCtrl->SetFocus();
			return;
		}

		if (msg.pSender->GetName() == _T("Edt_Ctn2_OutCoint"))
		{
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Pwd"));
			pEdtCtrl->SetFocus();
			return;
		}
		
		if (msg.pSender->GetName() == _T("Edt_Ctn2_Pwd"))
		{
			//确定转账
			OnBnClickedAccountOK();
			return;
		}

		if (msg.pSender->GetName() == _T("Edt_Ctn4_OutCoints"))
		{
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
			if (pEdtCtrl->IsVisible())
			{
				pEdtCtrl->SetFocus();
				return;
			}
			//确定存取
			OnBnClickedBankOK();
			return;
		}

		if (msg.pSender->GetName() == _T("Edt_Ctn4_Pwd"))
		{
			//确定存取
			OnBnClickedBankOK();
			return;
		}

		//修改密码相关
		if (msg.pSender->GetName() == _T("Edt_Ctn5_OldPwd"))
		{
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_NewPwd"));
			pEdtCtrl->SetFocus();
			return;
		}

		if (msg.pSender->GetName() == _T("Edt_Ctn5_NewPwd"))
		{
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_PwdCfm"));
			pEdtCtrl->SetFocus();
			return;
		}

		if (msg.pSender->GetName() == _T("Edt_Ctn5_PwdCfm"))
		{
			//修改银行密码
			OnBnClickedChPW();
			return;
		}


	}
}

void CPBankWnd::CenterWindow()
{
	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	int iSrcW = GetSystemMetrics(SM_CXSCREEN);
	int iSrcH = GetSystemMetrics(SM_CYSCREEN);
	int iWndW = rc.right - rc.left;
	int iWndH = rc.bottom - rc.top;
	::MoveWindow(m_hWnd,(iSrcW - iWndW)/2,(iSrcH - iWndH)/2,iWndW,iWndH,true);
}


//加载常规控件皮肤
void CPBankWnd::LoadNormalCtrlSkin()
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
			if (!pEditUI->IsReadOnly())
			{
				pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
				pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
				pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
				pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
			}
// 			else
// 			{
// 				pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
// 				pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
// 				pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
// 				pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
// 			}
		}

	} while (pEditUI);

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

	//加载所有多选框/单选框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("OptionUI"));
	COptionUI* pOptionUI = NULL;
	do 
	{
		pOptionUI = static_cast<COptionUI*>(pCtrls->GetAt(idx++));
		if (pOptionUI)
		{
			if (lstrlen(pOptionUI->GetGroup()) == 0) //多选框
			{
				pOptionUI->SetNormalImage("file='dialog\\check.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
				pOptionUI->SetHotImage("file='dialog\\check.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
				pOptionUI->SetSelectedImage("file='dialog\\check.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
			}
			else  //单选框
			{
				pOptionUI->SetNormalImage("file='dialog\\radio.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
				pOptionUI->SetHotImage("file='dialog\\radio.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
				pOptionUI->SetSelectedImage("file='dialog\\radio.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
				RECT rc = {18,0,0,0};
				pOptionUI->SetTextPadding(rc);
			}
		}
	} while (pOptionUI);

}

//显示容器页面
void CPBankWnd::ShowPage(int nIndex)
{
	if (nIndex < 0 || nIndex >= PAGE_NUMS)
	{
		return;
	}

	CControlUI* pContainer = NULL;
	TCHAR szName[20];

	for (int i = 0; i < PAGE_NUMS; ++i)
	{
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

	return;
}

//初始化财富信息
void CPBankWnd::InitMoneyInfo()
{
	CLabelUI* pLabCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn1_1"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","UseDirectly","在游戏中使用。"));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn1_2"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransInfor","银行中的金币，不能在游戏中直接使用。"));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn1_3"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","NeedPassWord","银行中的存款可用来支付购买道具的费用。"));

}

//初始赠送操作UI
void CPBankWnd::InitGiftInfo()
{
	CLabelUI* pLabCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;
	COptionUI* pOptCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_1"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","UserCoin","银行金币："));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Bank"));
	//pEdtCtrl->SetEnabled(false);

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_2"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransCoinNum","赠送金币数："));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_OutCoint"));
	pEdtCtrl->SetMaxChar(23);

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_3"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","NumToChinese","金币数(大写)："));

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_Ctn2_ID"));
	pOptCtrl->SetText(fMsg.GetKeyVal("PBankDlg","RTransUserID","对方ID"));
	pOptCtrl->Selected(true);
	if (pOptCtrl->IsSelected())
	{
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_4"));
		pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserID","对方ID："));
	}

	m_bUseDestID = true;

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_Ctn2_NickName"));
	pOptCtrl->SetText(fMsg.GetKeyVal("PBankDlg","RTransUserNick","对方昵称"));
	if (pOptCtrl->IsSelected())
	{
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Opt_Ctn2_NickName"));
		pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserNick","对方昵称："));
	}
	
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_5"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","SecondPassword","二级密码："));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_TargetUser"));
	pEdtCtrl->SetMaxChar(32);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Pwd"));
	pEdtCtrl->SetMaxChar(32);

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_6"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","Tip","提示："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_Tip2"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","CoinToGameCoin","转帐须扣除一定的手续费。"));

	m_iUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);
}

//初始存取款操作UI
void CPBankWnd::InitAccessInfo()
{
	CLabelUI*  pLabCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;
	COptionUI* pOptCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_Ctn4_In"));
	pOptCtrl->SetText(fMsg.GetKeyVal("PBankDlg","SaveGameCoin","存入游戏币"));
	pOptCtrl->Selected(true);

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_Ctn4_Out"));
	pOptCtrl->SetText(fMsg.GetKeyVal("PBankDlg","GetGameCoin","取出游戏币"));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_5"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","InputGameCoin","请输入存取金额："));

	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Ctn4_All"));
	pBntCtrl->SetText(fMsg.GetKeyVal("PBankDlg","CoinAll","全部"));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_6"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","InputPW","请输入银行密码："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_7"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","NumToChinese","金币数(大写)："));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_OutCoints"));
	pEdtCtrl->SetMaxChar(23);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
	pEdtCtrl->SetMaxChar(32);

}

//初始化日志操作UI
void CPBankWnd::InitRecordInfo()
{
	
}

//初始化修改银行密码UI
void CPBankWnd::InitModifyPwdInfo()
{
	CLabelUI*  pLabCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn5_1"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","OldPW","旧银行密码："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn5_2"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","NewPW","新银行密码："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn5_3"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","ConfirmPW","确认新密码："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn5_4"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","ChangeYourPW","银行密码初始化为游戏登陆密码，请及时修改！"));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_OldPwd"));
	pEdtCtrl->SetMaxChar(20);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_NewPwd"));
	pEdtCtrl->SetMaxChar(20);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_PwdCfm"));
	pEdtCtrl->SetMaxChar(20);
}

// lxl 2010-11-19  转账金额小写转换为中文大写
void CPBankWnd::OnEnChangeEtbconnoin()
{
	CEditUI* pEdtCtrl = NULL;

	// 转账金额ID	
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_OutCoint"));
	CString strOutCoin = pEdtCtrl->GetText();

	//去除输入框中的分隔符号
	if(Glb().m_bUseSpace)
		strOutCoin.Replace(Glb().m_strSpaceChar, "");	//转帐页面		

	//验证字符串是否是0-9的数字
	for(int i = 0; i < strOutCoin.GetLength(); i++)
	{
		if(strOutCoin[i] < '0' || strOutCoin[i] > '9')
		{
			pEdtCtrl->SetText("");
			return;
		}
	}

	__int64 i64Num  = _tstoi64(strOutCoin.GetBuffer());
	TCHAR szNum[128];

	if(bFlag == false)	
	{
		//字符串中加入分隔符
		GetNumString(szNum, i64Num, 0, Glb().m_bUseSpace, Glb().m_strSpaceChar);
		bFlag = true;
		pEdtCtrl->SetText(szNum);
		//设置光标到最后面
		pEdtCtrl->SetSel(-1);
	}
	else
		bFlag = false;

	ChangeDigitToChinese(i64Num, strOutCoin);	

	CTextUI* pTxtCtrl = static_cast<CTextUI*>(m_pm.FindControl("Txt_Ctn2_Caps"));
	pTxtCtrl->SetText(strOutCoin);

	return;

}

//存取款金额小写转换为中文大写
void CPBankWnd::OnEnChangeMoney()
{
	CEditUI* pEdtCtrl = NULL;

	// 转账金额ID	
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_OutCoints"));
	CString strGetCoin = pEdtCtrl->GetText();

	//去除输入框中的分隔符号
	if(Glb().m_bUseSpace)
		strGetCoin.Replace(Glb().m_strSpaceChar, "");	//银行页面

	//验证字符串是否是0-9的数字
	for(int i = 0; i < strGetCoin.GetLength(); i++)
	{
		if(strGetCoin[i] < '0' || strGetCoin[i] > '9')
		{
			pEdtCtrl->SetText("");
			return;
		}
	}	

	__int64 i64Num  = _tstoi64(strGetCoin.GetBuffer());
	TCHAR szNum[128];

	if(bFlag == false)	
	{		
		//字符串中加入分隔符
		GetNumString(szNum, i64Num, 0, Glb().m_bUseSpace, Glb().m_strSpaceChar);
		bFlag = true;
		pEdtCtrl->SetText(szNum);
		//设置光标到最后面
		pEdtCtrl->SetSel(-1);
	}
	else 
		bFlag = false;

	ChangeDigitToChinese(i64Num, strGetCoin);

	CTextUI* pTxtCtrl = static_cast<CTextUI*>(m_pm.FindControl("Txt_Ctn4_Caps"));
	pTxtCtrl->SetText(strGetCoin);
}

//切换目标用户的输入类型
void CPBankWnd::OnBnClickedRTransId()
{
	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");
	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_4"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserID","对方ID："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID"));
	pLabCtrl->SetText("");

	CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_TargetUser"));
	pEdtCtrl->SetText("");
	pEdtCtrl->SetFocus();

	m_bUseDestID = true;
}

//切换目标用户的输入类型
void CPBankWnd::OnBnClickedRTransNn()
{
	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");
	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_4"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserNick","对方昵称："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID"));
	pLabCtrl->SetText("");

	CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_TargetUser"));
	pEdtCtrl->SetText("");
	pEdtCtrl->SetFocus();

	m_bUseDestID = false;
}

//切换存取类型，存款
void CPBankWnd::OnBnClickedAccessIn()
{
	//不显示输入密码框
	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_6"));
	pLabCtrl->SetVisible(false);

	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
	pEditCtrl->SetVisible(false);

	m_uCheckInOrOut = 0;
}

//切换存取类型，取款
void CPBankWnd::OnBnClickedAccessOut()
{
	//显示输入密码框
	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn4_6"));
	pLabCtrl->SetVisible(true);

	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
	pEditCtrl->SetVisible(true);

	m_uCheckInOrOut = 1;
}

///
/// 帐号转账 的确定按钮响应函数
/// @param void
/// @return void
void CPBankWnd::OnBnClickedAccountOK()
{
	// PengJiLin, 2010-9-27, 全转 按钮，金葫芦二代功能
// 	if (1&Glb().m_CenterServerPara.m_nFunction)
// 	{
// 		if(m_Check_TransAll.GetCheck())
// 		{
// 			return OnBnClickedTransAll();
// 		}
// 	}

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	CEditUI* pEdtOutCoint = NULL;
	pEdtOutCoint = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_OutCoint"));

	CEditUI* pEdtPwd = NULL;
	pEdtPwd = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Pwd"));

	/// 取得编辑框里的字符串
	TCHAR szMsg[256];
	CString strNum, str;
	strNum = pEdtOutCoint->GetText();

	//如果有分隔符，则去除字符串里的分隔符
	if(Glb().m_bUseSpace)
		strNum.Replace(Glb().m_strSpaceChar,"");

	TCHAR szTail[16];
	_tcscpy(szTail, TEXT(""));
	for (int i=0; i<Glb().m_nPowerOfGold; ++i)
	{
		_tcscat( szTail, TEXT("0"));
	}
	/// 如果没有输入
	if (strNum.GetLength() == 0)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotInputCoin","请输入您要存取款的数额。",
			Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		pEdtOutCoint->SetFocus();
		return;
	}
	/// 如果输入太短
	if (strNum.GetLength()<Glb().m_nPowerOfGold)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotGetCoin",
			"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
			Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold, szTail);
		pEdtOutCoint->SetFocus();
		return;
	}
	/// 如果输入太长
	if (strNum.GetLength()>18+Glb().m_nPowerOfGold)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NumberTooLarge",
			"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
			0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
		pEdtOutCoint->SetFocus();
		return;
	}

	/// 如果末几位不是零
	CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
	if (strSubNum != szTail)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotGetCoin2",
			"您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！",
			Glb().m_nPowerOfGold,szTail);
		pEdtOutCoint->SetFocus();
		return;
	}
	/// 截取前N位
	strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
	__int64 i64Num = _atoi64(strSubNum.GetBuffer());
	if (i64Num>0x3FFFFFFFFFFFFFFF)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NumberTooLarge",
			"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
			0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
		pEdtOutCoint->SetFocus();
		return;
	}

	//获得目标用户
	pEdtOutCoint = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_TargetUser"));
	strNum = pEdtOutCoint->GetText();

	//二级密码
	pEdtOutCoint = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_Pwd"));
	str = pEdtOutCoint->GetText();

	if (str.GetLength()<1)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","PasswdBytes","请输入密码！");
		pEdtPwd->SetFocus();
		return;
	}
	if (i64Num<=0)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotGetCoin",
			"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
			Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
		pEdtOutCoint->SetFocus();
		return;
	}

	if (0 == atol(strNum) && m_bUseDestID)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","WrongID","请输入正确的玩家ID！");
		CEditUI* pEdtTargetUser = NULL;
		pEdtTargetUser = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_TargetUser"));
		pEdtTargetUser->SetFocus();
		return;
	}

	TransferMoney_t *pTransferMoney = new TransferMoney_t();
	pTransferMoney->i64Money = i64Num;
	pTransferMoney->uDestUserID = atol(strNum);
	_tcscpy(pTransferMoney->szPWD, str.Left(20).GetBuffer());
	if (m_bUseDestID)
	{
		_tcscpy(pTransferMoney->szDestNickName, m_strNickName);
	}
	else
	{
		_tcscpy(pTransferMoney->szDestNickName, strNum.GetBuffer());
	}


	pTransferMoney->bUseDestID = m_bUseDestID;

	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID_temp"));

	/// 提示用户确认转帐
	TCHAR szNum[128];
	GetNumString(szNum, pTransferMoney->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	//int iUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);

	CString strFormat;
	CString test;
	test.Format("lxl is use nick name : %d",m_iUseNickName );
	OutputDebugString(test);

	if(m_bUseDestID)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		strFormat = fMsg.GetKeyVal("PBankDlg", "TransferConfirmId", 
			"<p 5>转账前请确认以下转账信息，是否确定转帐？<n><y 30>目标用户：<b>%s</b> (ID：<b>%d</b>)</y><n>转帐数额：<b>%s</b> 金币<n></p>");
		wsprintf( szMsg, strFormat,pLabCtrl->GetText().GetData(),pTransferMoney->uDestUserID,szNum);
	}
	else
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		// lxl, 2010-11-17, 增加昵称显示

		if (::atol(pLabCtrl->GetText().GetData()) == m_dwOpUserID)
		{
			DUIOkeyMsgBox(AfxGetMainWnd()->m_hWnd,
				fMsg.GetKeyVal("BankDlg","TransferToSelf","您打算转帐给自己？对不起，系统暂不提供此项服务。"));
			return;
		}

		strFormat = fMsg.GetKeyVal("PBankDlg", "TransferConfirmNn", 
			"<p 5>转账前请确认以下转账信息，是否确定转帐？<n><y 30>目标用户：<b>%s</b></y><n>转帐数额：<b>%s</b> 金币<n></p>");
		wsprintf( szMsg,strFormat,pLabCtrl->GetText().GetData(),szNum );
	}	

	if (IDYES != DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONQUESTION,"银行",szMsg))
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		delete pTransferMoney;
		pTransferMoney = NULL;
		return;
	}

	// PengJiLin, 2010-8-23, 此处认为已经转账成功，用于刷新列表
	m_bTransferSuccess = true;

	//清空密码框
	pEdtPwd->SetText("");
	pEdtPwd->SetFocus();


	m_pGameRoom->PostMessage(WM_TRANSFER_MONEY, 0, LPARAM(pTransferMoney));
}

//根据用户ID获取昵称,或根据用户昵称获取用户ID
// @param pEdtCtrl 户ID/昵称输入控件
// @param nType 操作类型，是ID获取昵称,还是昵称用户ID
// @param nUserful 用途，是指这个功能所在有场合，如转账，充值等。
// @return bool 是否操作成功
// JianGuankun, 2012-7-30
bool CPBankWnd::OnIDKillFocus(BzDui::CEditUI* pEdtCtrl,BYTE nType,BYTE nUserful)
{
	CLabelUI* pLabCtrl = NULL;
	CString strTemp = "";

	// 对方ID
	strTemp = pEdtCtrl->GetText();

	if (strTemp.GetLength() == 0)
	{
		return true;
	}

	GetNickNameID_t* p = new GetNickNameID_t;

	if (!p)
	{
		return true;
	}

	if(nType == NICK_ID_BYID) //通过ID
	{
		// 无效的输入
		if(atol(strTemp) <= 0)
		{
			return false;
		}

		p->_user_id = atol(strTemp);
	}
	else if(nType == NICK_ID_BYNICKNAME) //通过昵称
	{
		strcpy(p->_nickname,(const char*)strTemp.GetBuffer());
	}
	else
	{
		return true;
	}

	p->_nType = nType;
	p->_nUseful = nUserful;

	GetMainRoom()->PostMessage(WM_BANK_GET_NICKNAMEID,0,LPARAM(p));

	return true;
	//WM_BANK_GET_NICKNAME_ONID
}

///
/// 收到一条转账记录
/// @param void
/// @return void
///
void CPBankWnd::OnReceivedTransferRecord(MSG_GR_R_TransferRecord_t *pRecord)
{
	m_mapRecord[pRecord->nSeriNo] = *pRecord;
	m_pRecordList->NeedUpdate();

	UpdateRecordPageState();
}

//记录翻到首页
void CPBankWnd::OnBnClickedRecordFirstPage()
{
	m_iRecDispPos = 0;
	m_pRecordList->NeedUpdate();
	UpdateRecordPageState();
}

//记录翻到尾页
void CPBankWnd::OnBnClickedRecordRearPage()
{
	if (m_mapRecord.size() % RECORD_PAGE_SIZE == 0)
	{
		m_iRecDispPos = m_mapRecord.size() - RECORD_PAGE_SIZE;
	}
	else
	{
		m_iRecDispPos = m_mapRecord.size() - m_mapRecord.size() % RECORD_PAGE_SIZE;
	}
	
	m_pRecordList->NeedUpdate();
	UpdateRecordPageState();
}

//记录翻上一页
void CPBankWnd::OnBnClickedRecordPreviousPage()
{
	if (m_iRecDispPos - RECORD_PAGE_SIZE >= 0)
	{
		m_iRecDispPos -= RECORD_PAGE_SIZE;
		m_pRecordList->NeedUpdate();
		UpdateRecordPageState();
	}
}

//记录翻下一页
void CPBankWnd::OnBnClickedRecordNextPage()
{
	if (m_iRecDispPos + RECORD_PAGE_SIZE < m_mapRecord.size())
	{
		m_iRecDispPos += RECORD_PAGE_SIZE;
		m_pRecordList->NeedUpdate();
		UpdateRecordPageState();
	}
}

///
/// 修改密码 的确定按钮响应函数
/// @param void
/// @return void
///
void CPBankWnd::OnBnClickedChPW()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	CEditUI* pOldPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_OldPwd"));
	CEditUI* pNewPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_NewPwd"));
	CEditUI* pPwdCfmEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn5_PwdCfm"));

	CString strOld, strNew, strNewCfm, str;
	TCHAR szMsg[MAX_PATH];

	strOld = pOldPwdEdt->GetText();//旧密码

	if ((strOld.GetLength()<6) ||(strOld.GetLength()>20))
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NewPasswdBytes","密码由6-20字符组成，请重新输入旧密码！");
		pOldPwdEdt->SetFocus();
		return;
	}

	/// 先比较新密码和确认密码是否相同，如果不同，则直接返回，相同才继续
	strNew = pNewPwdEdt->GetText();//新密码
	strNewCfm = pPwdCfmEdt->GetText();//确认新密码
	if (strNew != strNewCfm)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","ConfirmPasswdNotSame","新密码两次输入不一致，请重新输入!");
		pNewPwdEdt->SetFocus();
		return;
	}
	if ((strNew.GetLength()<6) ||(strNew.GetLength()>20))
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NewPasswdBytes","密码由6-20字符组成，请重新输入新密码！");
		pNewPwdEdt->SetFocus();
		return;
	}

	if (ISGameRoom == m_emParWndType)
	{
		ChangePasswd_t *pChangePasswd = new ChangePasswd_t();
		_tcscpy( pChangePasswd->szPwdOld, strOld.Left(20).GetBuffer() );
		_tcscpy( pChangePasswd->szPwdNew, strNew.Left(20).GetBuffer() );

		m_pGameRoom->PostMessage(WM_CHANGE_PASSWD, 0, LPARAM(pChangePasswd));
	}
	else if (IsMainRoom == m_emParWndType)
	{
		TMSG_GP_BankChPwd* pChPwd = new TMSG_GP_BankChPwd();
		pChPwd->_user_id = m_dwOpUserID;

		CGameRoomEx::GetCryptedPasswd(pChPwd->_MD5Pass_old,(char*)strOld.Left(20).GetBuffer());
		CGameRoomEx::GetCryptedPasswd(pChPwd->_MD5Pass_new,(char*)strNew.Left(20).GetBuffer());

		GetMainRoom()->PostMessage(WM_CHANGE_PASSWD,0,LPARAM(pChPwd));
	}

	pOldPwdEdt->SetText("");//旧密码
	pNewPwdEdt->SetText("");//新密码
	pPwdCfmEdt->SetText("");//确认新密码

	return;
}

///
/// 银行 的确定按钮响应函数
/// 须判断，到底是取钱还是存钱
/// @param void
/// @return void
///
void CPBankWnd::OnBnClickedBankOK()
{
	
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	CEditUI* pOutCointEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_OutCoints"));
	CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));

	/// 取得编辑框里的字符串
	TCHAR szMsg[256];
	CString strNum, str;
	strNum = pOutCointEdt->GetText();

	//如果有分隔符，则去除字符串里的分隔符
	if(Glb().m_bUseSpace)
		strNum.Replace(Glb().m_strSpaceChar,"");

	TCHAR szTail[16];
	_tcscpy(szTail, TEXT(""));
	for (int i=0; i<Glb().m_nPowerOfGold; ++i)
	{
		_tcscat( szTail, TEXT("0"));
	}

	/// 如果没有输入
	if (strNum.GetLength() == 0)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotInputCoin","请输入您要存取款的数额。",
			Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
		pOutCointEdt->SetFocus();
		return;
	}
	/// 如果输入太短
	if (strNum.GetLength()<Glb().m_nPowerOfGold)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotGetCoin",
			"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
			Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
		pOutCointEdt->SetFocus();
		return;
	}
	/// 如果输入太长
	if (strNum.GetLength()>18 + Glb().m_nPowerOfGold)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NumberTooLarge",
			"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
			0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
		pOutCointEdt->SetFocus();
		return;
	}

	/// 如果末几位不是零
	CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
	if (strSubNum != szTail)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotGetCoin2",
			"您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！",
			Glb().m_nPowerOfGold,szTail);
		pOutCointEdt->SetFocus();
		return;
	}
	/// 截取前N位
	strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
	__int64 i64Num = _atoi64(strSubNum.GetBuffer());
	if (i64Num>0x3FFFFFFFFFFFFFFF)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NumberTooLarge",
			"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
			0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
		pOutCointEdt->SetFocus();
		return;
	}
	if (i64Num<=0)
	{
		//BZSoundPlay(this, "music/错误提示.mp3", 0);
		ShowTipMessage(NULL,"BankDlg","NotGetCoin",
			"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
			Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
		pOutCointEdt->SetFocus();
		return;
	}

	/// 发消息
	if (m_pGameRoom!=NULL)
	{
		/// 无论是取还是存，都只是往父窗口发送一个消息即可
		/// 为统一接口，存钱也把银行密码发送到父窗口
		/// 金额，二级密码，取还是存
		str = pPwdEdt->GetText();

		if ((m_uCheckInOrOut==1) && (str.GetLength()<1))
		{
			//BZSoundPlay(this, "music/错误提示.mp3", 0);
			ShowTipMessage(NULL,"BankDlg","PasswdBytes","请输入密码！");
			pPwdEdt->SetFocus();
			return;
		}

		//银行在游戏房间出现时的处理
		if (ISGameRoom == m_emParWndType)
		{
			CheckMoney_t *pCheckMoney = new CheckMoney_t;
			pCheckMoney->uType = m_uCheckInOrOut;
			pCheckMoney->i64Money = i64Num;
			_tcscpy(pCheckMoney->szPWD, str.Left(20).GetBuffer());
			m_pGameRoom->PostMessage(WM_CHECK_MONEY, 0, (LPARAM)pCheckMoney);
		}

		//银行在大厅出现时的处理
		else if (IsMainRoom == m_emParWndType)
		{
			bank_op_normal* p = new bank_op_normal;
			p->_user_id = m_dwOpUserID;
			p->_game_id = 0;
			p->_money = i64Num;

			if (m_uCheckInOrOut == 1) //取
			{
				p->_operate_type = 1;
				//提取输入的密码
				CGameRoomEx::GetCryptedPasswd(p->_szMD5Pass,(TCHAR*)str.GetBuffer());

				m_pGameRoom->PostMessage(WM_BANK,0,(LPARAM)p);
			}
			else //存
			{
				p->_operate_type = 2;
				m_pGameRoom->PostMessage(WM_BANK,0,(LPARAM)p);
			}
		}
		
	}
}

//输入用户ID/昵称
void CPBankWnd::OnKillfocusTargetUser()
{
	CLabelUI* pLabCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn2_TargetUser"));

	if (pEdtCtrl->GetText().GetLength() == 0)
	{
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID_temp"));
		pLabCtrl->SetText("");

		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID"));
		pLabCtrl->SetText("");
	}
	else
	{
		//输入转账目标用户，进行一些检测
		if (pEdtCtrl->GetText().Compare(m_strFocusTemp))
		{
			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Ctn2_NickNameID"));
			pLabCtrl->SetText("正在查询...");

			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_Ctn2_ID"));
			if(!OnIDKillFocus(pEdtCtrl,(pOptCtrl->IsSelected()?NICK_ID_BYID:NICK_ID_BYNICKNAME),NICK_ID_USE_TO_TRANSFER))
			{
				pLabCtrl->SetText("(玩家不存在)");
			}
		}

		m_strFocusTemp = "";
	}
}

void CPBankWnd::OnBnClickedCoinAll()
{
	TCHAR szNum[128];
	if(m_uCheckInOrOut == 0)
	{
		int nLess = 0;
		if(m_pGameRoom != NULL)
		{
			if (ISGameRoom == m_emParWndType)
			{
				nLess = ((CGameRoomEx*)m_pGameRoom)->m_GameInfo.uLessPoint;
			}
			else if (IsMainRoom == m_emParWndType)
			{
				nLess = 0;
			}			
		}
		GetNumString(szNum, m_i64WalletMoney-nLess, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	}
	else
	{
		GetNumString(szNum, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	}

	CEditUI* pEdtCtrl = NULL;
	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_OutCoints"));
	pEdtCtrl->SetFocus();
	pEdtCtrl->SetText(szNum);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_Ctn4_Pwd"));
	pEdtCtrl->SetFocus();

}

void CPBankWnd::OnBnClickedRecord()
{
	// PengJiLin, 2010-8-23, 转账成功，则刷新
	if(m_bTransferSuccess)
	{
		RefreshRecordList();
		m_bTransferSuccess = false;

		m_pFirstPageBnt->SetEnabled(false);
		m_pPrePageBnt->SetEnabled(false);
		m_pNextPageBnt->SetEnabled(false);
		m_pRearPageBnt->SetEnabled(false);
	}
}

///
/// 刷新转账记录
/// @return void
///
void CPBankWnd::RefreshRecordList()
{
	m_mapRecord.clear();
	m_iRecDispPos = 0;
	m_pGameRoom->PostMessage(WM_TRANSFER_RECORD, 0, 0);
}