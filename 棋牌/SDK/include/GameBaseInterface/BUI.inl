#include <atlstr.h>
#include "BUI.h"

#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\

//构造函数
BaseUI::BaseUI()
{
	return;
}

BaseUI::~BaseUI()
{
	return;
}

//【公共部分】（所有控件都用得上）------------------------------------------------------

//设置指针m_pIUI 并获得游戏ID
void BaseUI::SetpIUI(IUserInterface *pIUI, int iGameID)
{
	m_pIUI = pIUI;
	m_iGameID = iGameID;
	return;
}

//获得指针m_pIUI
IUserInterface* BaseUI::GetpIUI()
{
	return m_pIUI;
}

/*************************************************
*Function: 加载UI.data【这里主要用来初始化一些游戏流程中的界面显示，变量等等】
*writer:
*		帅东坡
*Parameters:
*		void
*Return:
*		返回是否成功
*************************************************/
int BaseUI::Initial()
{
	if (m_pIUI == NULL)
	{
		return -1;
	}
	wchar_t wszUIFile[MAX_PATH];
#ifdef UI2_5D
	::swprintf_s(wszUIFile,L"%d_UI_3D.dat",NAME_ID);
#else
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
#endif
	m_pIUI->LoadData(wszUIFile);
	return 1;
}

/*************************************************
*Function: 加载UI.data【这里主要用来初始化一些游戏流程中的界面显示，变量等等】
*writer:
*		帅东坡
*Parameters:
*		[TCHAR*]
*						szDataFileName UI.data的文件名
*Return:
*		返回是否成功
*************************************************/
int BaseUI::Initial(TCHAR *szDataFileName)
{
	if ((m_pIUI == NULL)
		|| (szDataFileName == NULL))
	{
		return -1;
	}
	TCHAR pchar[MAX_PATH] = {0};
	sprintf(pchar, "%s", szDataFileName);
	m_pIUI->LoadData(CA2W(pchar));
	return 1;
}

//加载UI.data
int BaseUI::Initial(CString sFileName)
{
	if ((m_pIUI == NULL))
	{
		return -1;
	}
	m_pIUI->LoadData(CA2W(sFileName));
	return 1;
}

/*************************************************
*Function: 显示/隐藏 控件
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						bFlag	是否显示
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::ShowControl(const int iID, bool bFlag)
{
	IBCInterFace *pControl = NULL;
	GETCTRL(IBCInterFace,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetControlVisible(bFlag);
		return TRUE;
	}

	return FALSE;
}

/*************************************************
*Function: 查询控件是否显示/隐藏
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						bFlag	是否显示
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::GetControlShow(const int iID)
{
	IBCInterFace *pControl = NULL;
	GETCTRL(IBCInterFace,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		return pControl->GetVisible();
	}

	return FALSE;
}

/*************************************************
*Function: 设置控件所在位置
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						ix,iy	坐标位置
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::SetControlGameXY(const int iID, int ix, int iy)
{
	IBCInterFace *pControl = NULL;
	GETCTRL(IBCInterFace,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetGameXY(ix, iy);
		return TRUE;
	}

	return FALSE;
}

/*************************************************
*Function: 获得控件所在位置
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						ix,iy	坐标位置
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::GetControlGameXY(const int iID, int &ix, int &iy)
{
	ix = 0;
	iy = 0;

	IBCInterFace *pControl = NULL;
	GETCTRL(IBCInterFace,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		ix = pControl->GetGameX();
		iy = pControl->GetGameY();
		return TRUE;
	}

	return FALSE;
}

/*************************************************
*Function: 设置控件大小
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						iW,iH	控件宽和高
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::SetControlGameWH(const int iID, int iW, int iH)
{
	IBCInterFace *pControl = NULL;
	GETCTRL(IBCInterFace,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetGameW(iW);
		pControl->SetGameH(iH);
		return TRUE;
	}

	return FALSE;
}

/*************************************************
*Function: 获得控件大小
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						iW,iH	控件宽和高
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::GetControlGameWH(const int iID, int &iW, int &iH)
{
	iW = 0;
	iH = 0;

	IBCInterFace *pControl = NULL;
	GETCTRL(IBCInterFace,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		iW = pControl->GetGameW();
		iH = pControl->GetGameH();
		return TRUE;
	}

	return FALSE;
}

//【按钮部分】---------------------------------------------------------------------------

/*************************************************
*Function: 设置按钮上的字
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						num		数字
*Return:
*		void
*************************************************/
void BaseUI::SetTextForBut(const int iID, int num)
{
	IButton *pControl = NULL;
	GETCTRL(IButton,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		TCHAR pchar[MAX_PATH] = {0};
		sprintf(pchar, "%d",num);
		pControl->SetText(CA2W(pchar));
	}

	return;
}

/*************************************************
*Function: 设置按钮上的字
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[int]
*						num		数字
*Return:
*		void
*************************************************/
void BaseUI::SetTextForBut(const int iID, __int64 num)
{
	IButton *pControl = NULL;
	GETCTRL(IButton,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		TCHAR pchar[MAX_PATH] = {0};
		sprintf(pchar, "%I64d",num);
		pControl->SetText(CA2W(pchar));
	}

	return;
}

/*************************************************
*Function: 设置按钮上的字2
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[TCHAR]
*						Text[]	字符串
*Return:
*		void
*************************************************/
void BaseUI::SetTextForBut(const int iID, TCHAR Text[])
{
	IButton *pControl = NULL;
	GETCTRL(IButton,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetText(CA2W(Text));
	}

	return;
}

/*************************************************
*Function: 设置可用/禁用 按钮
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*		[bool]
*						bFlag	是否使能
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::EnableControl(const int iID, bool bFlag)
{
	IButton *pControl = NULL;
	GETCTRL(IButton,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetEnable(bFlag);
		return TRUE;
	}

	return FALSE;
}

/*************************************************
*Function: 查询按钮可用/禁用
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*Return:
*		返回是否成功
*************************************************/
BOOL BaseUI::GetControlEnable(const int iID)
{
	IButton *pControl = NULL;
	GETCTRL(IButton,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		return pControl->GetEnable();
	}

	return FALSE;
}

/*************************************************
*Function: 是否是按钮
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID		控件ID
*Return:
*		返回是否是按钮
*************************************************/
bool BaseUI::IsBut(const int iID)
{
	IButton *pControl = NULL;
	GETCTRL(IButton,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		return true;
	}
	return false;
}

//【图片部分】---------------------------------------------------------------------------

/*************************************************
*Function: 设置图片控件加载制定的图片
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[TCHAR]
*						TcNickName[]	路径
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetImagesLoadPic(const int iID, TCHAR TcNickName[], bool bFlag)
{
	IImage *pControl = NULL;
	GETCTRL(IImage,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->LoadPic(CA2W(TcNickName));
		pControl->SetControlVisible(bFlag);
	}

	return;
}

/*************************************************
*Function: 设置图片是否穿透
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[bool]
*						bTouFlag		是否穿透
*Return:
*		void
*************************************************/
void BaseUI::SetImagesPenetrate(const int iID, bool bTouFlag)
{
	IImage *pControl = NULL;
	GETCTRL(IImage,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetMouseThough(bTouFlag);
	}

	return;
}

/*************************************************
*Function: 设置仅显示图片部分区域
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[bool]
*						bFlag			是否显示
*		[int]
*						ix,iy			设置图片显示大小
*Return:
*		void
*************************************************/
void BaseUI::SetShowImageXY(const int iID, bool bFlag, int ix, int iy)
{
	IImage *pControl = NULL;
	GETCTRL(IImage,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetSrcX(ix);
		pControl->SetSrcY(iy);
		pControl->SetControlVisible(bFlag);
	}

	return;
}

/*************************************************
*Function: 克隆图片
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[int]
*						iGoalID			克隆的控件ID
*Return:
*		void
*************************************************/
void BaseUI::CloneImage(const int iID, int iGoalID)
{
	IImage *_pControl = NULL;
	GETCTRL(IImage,_pControl,m_pIUI,iID);
	IImage *pControl = NULL;
	if (NULL != _pControl)
	{
		pControl = dynamic_cast<IImage *>(_pControl->Clone(iGoalID));//克隆
		if (NULL != pControl)
		{
			if (NULL == m_pIUI->GetIControlbyID(pControl->GetGameControlID()))
			{
				m_pIUI->IInsertControl(pControl);
			}
		}
	}

	return;
}

//【静态文本部分】-----------------------------------------------------------------------

/*************************************************
*Function: 设置文本控件显示内容(1)
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[TCHAR]
*						TcNickName[]	字符串
*		[bool]
*						bFlag			是否显示
*		[FONTSHOWSTYLE]
*						showType		字体对齐模式
*Return:
*		void
*************************************************/
void BaseUI::SetTextInfo(const int iID, TCHAR TcNickName[], bool bFlag, FONTSHOWSTYLE showType)
{
	IText *pControl = NULL;
	GETCTRL(IText,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		pControl->SetText(CA2W(TcNickName));
		pControl->SetFontShowStyle(showType);
		pControl->SetControlVisible(bFlag);
	}

	return;
}

/*************************************************
*Function: 设置文本控件显示内容(2)
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[int]
*						iNum			数字
*		[bool]
*						bFlag			是否显示
*		[FONTSHOWSTYLE]
*						showType		字体对齐模式
*Return:
*		void
*************************************************/
void BaseUI::SetTextInfo(const int iID, int iNum, bool bFlag, FONTSHOWSTYLE showType)
{
	IText *pControl = NULL;
	GETCTRL(IText,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		TCHAR pchar[MAX_PATH] = {0};
		sprintf(pchar, TEXT("%d"), iNum);

		pControl->SetText(CA2W(pchar));
		pControl->SetFontShowStyle(showType);
		pControl->SetControlVisible(bFlag);
	}

	return;
}

/*************************************************
*Function: 设置文本控件显示内容(3)
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[__int64]
*						i64Num			数字
*		[bool]
*						bFlag			是否显示
*		[FONTSHOWSTYLE]
*						showType		字体对齐模式
*Return:
*		void
*************************************************/

void BaseUI::SetTextInfo(const int iID, __int64 i64Num, bool bFlag, bool bSymbol, FONTSHOWSTYLE showType)
{
	IText *pControl = NULL;
	GETCTRL(IText,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		TCHAR pchar[MAX_PATH] = {0};
		//是否带+-符号
		if (bSymbol&&i64Num>0)
		{
			sprintf(pchar, TEXT("+%I64d"), i64Num);
		}
		else
		{
			sprintf(pchar, TEXT("%I64d"), i64Num);
		}
		

		pControl->SetText(CA2W(pchar));
		pControl->SetFontShowStyle(showType);
		pControl->SetControlVisible(bFlag);
	}

	return;
}

/*************************************************
*Function: 得到文本控件的文字
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*Return:
*		返回字符串
*************************************************/
wchar_t* BaseUI::GetTextString(const int iID)
{
	IText *pControl = NULL;
	GETCTRL(IText,pControl,m_pIUI,iID);
	if (NULL != pControl)
	{
		return pControl->GetText();
	}

	return NULL;
}

/*************************************************
*Function: 得到文本控件的数字
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*Return:
*		返回字符串
*************************************************/
int BaseUI::GetTextNum(const int iID)
{
	wchar_t* pWchar = GetTextString(iID);

	return _wtoi(pWchar);
}

//【数字控件部分】-----------------------------------------------------------------------

/*************************************************
*Function: 设置各种数字控件显示
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[__int64]
*						byNum			数字
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetNumVisible(const int iID, __int64 byNum, bool bFlag)
{
	INum *pControl = NULL;
	GETCTRL(INum, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetNum(byNum);
		pControl->SetControlVisible(bFlag);
	}
	return;
}

//【计时器控件部分】---------------------------------------------------------------------

/*************************************************
*Function: 显示一个计时器控件
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[int]
*						iTime			时间
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetClock(const int iID, int iTime, bool bFlag)
{
	ITime *pControl = NULL;
	GETCTRL(ITime, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetShowMaxNum(iTime);
		pControl->SetShowMinNum(0);
		pControl->SetPLay(bFlag);
		pControl->SetControlVisible(bFlag);
	}
	return;
}

//【文本输入部分】-----------------------------------------------------------------------

/*************************************************
*Function: 获取文本输入框内容
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[wchar_t]
*						wText[]			字符串
*		[int]
*						iCount			字符串长度
*Return:
*		void
*************************************************/
void BaseUI::GetTextEditInfo(const int iID, wchar_t wText[], int iCount)
{
	wchar_t* Data = NULL;
	memset(wText, 0, sizeof(wText));

	IEditorText* pControl = NULL;
	GETCTRL(IEditorText, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		Data = pControl->GetText();

		if (Data != NULL)
		{
			memcpy(wText, Data, iCount * sizeof(wchar_t));
		}
	}

	return;
}

/*************************************************
*Function: 设置文本输入框内容
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[wchar_t]
*						wText[]			字符串
*Return:
*		void
*************************************************/
void BaseUI::SetTextEditInfo(const int iID, wchar_t wText[])
{
	memset(wText, 0, sizeof(wText));
	IEditorText* pControl = NULL;
	GETCTRL(IEditorText, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetText(wText);
	}

	return;
}

/*************************************************
*Function: 设置文本框只接受数字
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*Return:
*		void
*************************************************/
void BaseUI::SetTextEditOnlyNum(const int iID)
{
	IEditorText* pControl = NULL;
	GETCTRL(IEditorText, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetEnableASCII(1, '0', '9');
	}

	return;
}

/*************************************************
*Function: 设置文本框最小最大值
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[int]
*						iMin			最小值
*		[int]
*						iMax			最大值
*Return:
*		void
*************************************************/
void BaseUI::SetTextMin_Max(const int iID, int iMin, int iMax)
{
	IEditorText* pControl = NULL;
	GETCTRL(IEditorText, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetNumberLimit(true, iMin, iMax);
	}

	return;
}

//【牌控件部分】-------------------------------------------------------------------------

/*************************************************
*Function: 设置牌控件显示
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[BYTE]
*						iCard[]			牌数据
*		[int]
*						iNum			筹码值
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetCardInfo(const int iID, BYTE iCard[], int iNum, bool bFlag)
{
	ICardBase *pControl = NULL;
	GETCTRL(ICardBase, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetCardList(iCard, iNum);
		pControl->SetControlVisible(bFlag);
	}

	return;
}

/*************************************************
*Function: 获取升起的扑克
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[BYTE]
*						iCard[]			牌数据
*Return:
*		返回牌数目
*************************************************/
BYTE BaseUI::GetUpCard(int iID, BYTE iCard[])
{
	BYTE bUpCount=0;
	IOperateCard *pControl = NULL;
	GETCTRL(IOperateCard, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		return pControl->GetUpCardList(iCard);
	}
	return 0;
}

/*************************************************
*Function: 设置升起的扑克
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[BYTE]
*						iCard[]			牌数据
*		[int]
*						inum			牌数目
*Return:
*		返回牌数目
*************************************************/
void BaseUI::SetUpCard(int iID, BYTE iCard[], int inum)
{
	IOperateCard *pControl = NULL;
	GETCTRL(IOperateCard, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetUpCardList(iCard, inum);
	}
	return;
}

//【筹码部分】---------------------------------------------------------------------------

/*************************************************
*Function: 显示筹码控件
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[int]
*						iNum			筹码值
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetCouMa(const int iID, int iNum, bool bFlag)
{
	IShCtrl *pControl = NULL;
	GETCTRL(IShCtrl, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetShNumber(9);
		pControl->SetShNoteType(4);
		pControl->SetShNote(iNum);
		pControl->SetControlVisible(bFlag);
	}

	return;
}

//【单选复选框控件】---------------------------------------------------------------------

/*************************************************
*Function: 设置是否被选中
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*						bFlag			是否选中
*Return:
*		void
*************************************************/
void BaseUI::SetIsSelect(const int iID, bool bFlag)
{
	IRadioButton *pControl = NULL;
	GETCTRL(IRadioButton, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetIsSelect(bFlag);
	}

	return;
}

/*************************************************
*Function: 获取是否被选中
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*Return:
*		是否被选中
*************************************************/
bool BaseUI::GetIsSelect(const int iID)
{
	IRadioButton *pControl = NULL;
	GETCTRL(IRadioButton, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		return pControl->GetIsSelect();
	}

	return false;
}

//【动画控件部分】(外部动画)-------------------------------------------------------------

/*************************************************
*Function: 设置指定动画显示指定的帧数
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[int]
*						iFramNum		动画第几帧
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetAnimateShowFram(const int iID, int iFramNum, bool bFlag)
{
	IExAnimate *pControl = NULL;
	GETCTRL(IExAnimate, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetIsLoop(false);
		pControl->SetCurveInstance(CT_NONE);
		pControl->SetPlayState(false);
		//第几帧
		pControl->SetShowFrame(iFramNum);
		pControl->SetControlVisible(bFlag);
	}
	return;
}

/*************************************************
*Function: 设置播放外部动画 bLop-是否循环 iPlayCount-播放次数(可加载资源)
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[TCHAR]
*						szAnimaPath		动画资源路径
*		[bool]
*						bFlag			是否显示
*		[bool]
*						bLop			是否循环
*		[int]
*						iPlayCount		播放次数
*Return:
*		void
*************************************************/
void BaseUI::SetAnimatePlay(const int iID, TCHAR szAnimaPath[], bool bFlag, bool bLop, int iPlayCount)
{
	IExAnimate *pControl = NULL;
	GETCTRL(IExAnimate, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetExtFilePath(CA2W(szAnimaPath));
		if (pControl->LoadFromFile())
		{
			pControl->SetControlVisible(bFlag);
			pControl->SetCurveInstance(CT_NONE);//不移动
			pControl->SetIsLoop(bLop);
			if (!bLop)
			{
				pControl->SetPlayNum(iPlayCount);
			}
			pControl->SetPlayState(bFlag);
		}

	}

	return;
}

/*************************************************
*Function: 播放外部动画(单次播放)
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iID				控件ID
*		[bool]
*						bFlag			是否显示
*		[bool]
*						bIsLoopFlag		是否循环
*Return:
*		void
*************************************************/
void BaseUI::PlayAnimation(const int iID, bool bFlag, bool bIsLoopFlag)
{
	IExAnimate *pControl = NULL;
	GETCTRL(IExAnimate, pControl, m_pIUI, iID);
	if (NULL != pControl)
	{
		pControl->SetIsLoop(bIsLoopFlag);
		pControl->SetControlVisible(bFlag);
		pControl->SetPlayState(bFlag);//设置是否开始播放
	}

	return;
}

//【动画控件部分】(MoveAction)-----------------------------------------------------------

/*************************************************
*Function: 梭哈控件移动动画
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iMoveActionID	MoveAction控件ID
*		[int]
*						StarPointID		起始点控件ID
*		[int]
*						EndPointID		终止点控件ID
*		[int]
*						iBeMoveID		被移动控件ID
*		[int]
*						bView			玩家视图位置
*		[int]
*						iNum			数字
*		[int]
*						iNum			播放时间（毫秒）
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::PlayChipInAnima(const int iMoveActionID, const  int StarPointID, const  int EndPointID, const  int iBeMoveID, int bView, int iNum, int iTime, bool bFlag)
{
	if (bFlag)
	{
		//起始位置
		POINT StarPoint;
		POINT EndPoint;
		ICardBase * pICardBase = NULL;
		IShCtrl * pShCtrl1 = NULL;

		GETCTRL(ICardBase, pICardBase, m_pIUI, StarPointID + bView);//以每个人的手牌位置为起点
		if (NULL != pICardBase)
		{
			StarPoint.x = pICardBase->GetGameX();
			StarPoint.y = pICardBase->GetGameY();
		}

		GETCTRL(IShCtrl, pShCtrl1, m_pIUI, EndPointID);//以中心筹码位置为终点
		if (NULL != pShCtrl1)
		{
			EndPoint.x = pShCtrl1->GetGameX();
			EndPoint.y = pShCtrl1->GetGameY();
		}

		IShCtrl * pShCtrl = NULL;
		GETCTRL(IShCtrl, pShCtrl, m_pIUI, iBeMoveID + bView);
		if (NULL != pShCtrl)
		{
			pShCtrl->SetShNote(iNum);
			pShCtrl->SetGameXY(StarPoint.x, StarPoint.y);
			pShCtrl->SetControlVisible(true);
		}

		IMoveAction* pMoveAction = NULL;
		GETCTRL(IMoveAction, pMoveAction, m_pIUI, iMoveActionID + bView);//移动动画
		if (NULL != pMoveAction)
		{
			pMoveAction->SetControlingCtrlID(iBeMoveID + bView);
			pMoveAction->SetCurveInstance(CT_STRAIGHT);

			pMoveAction->SetStartP(StarPoint);
			pMoveAction->SetEndP(EndPoint);
			pMoveAction->SetTotalMoveTime(iTime);

			pMoveAction->SetControlVisible(bFlag);
			pMoveAction->SetPlayState(bFlag);
		}

	}

	return;
}

/*************************************************
*Function: 梭哈控件移动动画(赢家获得输家筹码用)
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iMoveActionID	MoveAction控件ID
*		[int]
*						StarPointID		起始点控件ID
*		[int]
*						EndPointID		终止点控件ID
*		[int]
*						iBeMoveID		被移动控件ID
*		[int]
*						bView			玩家视图位置
*		[int]
*						iNum			数字
*		[int]
*						iNum			播放时间（毫秒）
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::PlayChipInAnimaEx(const int iMoveActionID, const  int StarPointID, const  int EndPointID, const  int iBeMoveID, int bView, int iNum, int iTime, bool bFlag)
{
	if (bFlag)
	{
		//起始位置
		POINT StarPoint;
		POINT EndPoint;
		ICardBase * pICardBase = NULL;
		IShCtrl * pShCtrl1 = NULL;

		GETCTRL(ICardBase, pICardBase, m_pIUI, EndPointID + bView);//以每个人的手牌位置为终点
		if (NULL != pICardBase)
		{
			EndPoint.x = pICardBase->GetGameX();
			EndPoint.y = pICardBase->GetGameY();
		}

		GETCTRL(IShCtrl, pShCtrl1, m_pIUI, StarPointID);//以中心筹码位置为起点
		if (NULL != pShCtrl1)
		{
			StarPoint.x = pShCtrl1->GetGameX();
			StarPoint.y = pShCtrl1->GetGameY();
		}

		IShCtrl * pShCtrl = NULL;
		GETCTRL(IShCtrl, pShCtrl, m_pIUI, iBeMoveID + bView);
		if (NULL != pShCtrl)
		{
			pShCtrl->SetShNote(iNum);
			pShCtrl->SetGameXY(StarPoint.x, StarPoint.y);
			pShCtrl->SetControlVisible(true);
		}


		IMoveAction* pMoveAction = NULL;
		GETCTRL(IMoveAction, pMoveAction, m_pIUI, iMoveActionID + bView);//移动动画
		if (NULL != pMoveAction)
		{
			pMoveAction->SetControlingCtrlID(iBeMoveID + bView);
			pMoveAction->SetCurveInstance(CT_STRAIGHT);

			pMoveAction->SetStartP(StarPoint);
			pMoveAction->SetEndP(EndPoint);
			pMoveAction->SetTotalMoveTime(iTime);

			pMoveAction->SetControlVisible(bFlag);
			pMoveAction->SetPlayState(bFlag);
		}

	}

	return;
}

/*************************************************
*Function: 发牌动画
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iMoveActionID	MoveAction控件ID
*		[int]
*						StarPointID		起始点控件ID
*		[int]
*						EndPointID		终止点控件ID
*		[int]
*						iBeMoveID		被移动控件ID
*		[int]
*						bView			玩家视图位置
*		[BYTE]
*						iCard[]			牌值
*		[int]
*						iNum			牌张数
*		[int]
*						iNum			播放时间（毫秒）
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::PlayCardInAnima(
	const int iMoveActionID, 
	const  int StarPointID, 
	const  int EndPointID, 
	const  int iBeMoveID, 
	int bView, 
	BYTE iCard[], 
	int iNum, 
	int iTime, 
	bool bFlag)
{
	if (bFlag)
	{
		//起始位置
		POINT StarPoint;
		POINT EndPoint;
		ICardBase * pICardBase = NULL;
		IBCInterFace * pControl = NULL;

		GETCTRL(ICardBase, pICardBase, m_pIUI, EndPointID + bView);//以每个人的手牌位置为终点
		if (NULL != pICardBase)
		{
			EndPoint.x = pICardBase->GetGameX();
			EndPoint.y = pICardBase->GetGameY();
		}

		GETCTRL(IBCInterFace, pControl, m_pIUI, StarPointID);//以发牌器位置为起点
		if (NULL != pControl)
		{
			StarPoint.x = pControl->GetGameX();
			StarPoint.y = pControl->GetGameY();
		}

		ICardBase * _pICardBase = NULL;
		GETCTRL(ICardBase, _pICardBase, m_pIUI, iBeMoveID);//移动的牌
		if (NULL != _pICardBase)
		{
			_pICardBase->SetCardList(iCard , iNum);
			_pICardBase->SetGameXY(StarPoint.x, StarPoint.y);
			_pICardBase->SetControlVisible(true);
		}


		IMoveAction* pMoveAction = NULL;
		GETCTRL(IMoveAction, pMoveAction, m_pIUI, iMoveActionID);//移动动画
		if (NULL != pMoveAction)
		{
			pMoveAction->SetControlingCtrlID(iBeMoveID);
			pMoveAction->SetCurveInstance(CT_STRAIGHT);

			pMoveAction->SetStartP(StarPoint);
			pMoveAction->SetEndP(EndPoint);
			pMoveAction->SetTotalMoveTime(iTime);

			pMoveAction->SetControlVisible(bFlag);
			pMoveAction->SetPlayState(bFlag);
		}

	}

	return;
}

//【光标控件部分】-----------------------------------------------------------------------

/*************************************************
*Function: 设置光标
*writer:
*		帅东坡
*Parameters:
*		[wchar_t*]
*						pCurImg			鼠标图片资源
*		[bool]
*						bHide			设置显示时是否隐藏鼠标
*		[int]
*						PointMode		设置座标模式
*										0表左上角为鼠标
*										1表中央为鼠标
*										2表右上角为鼠标
*		[bool]
*						bFlag			是否显示
*Return:
*		void
*************************************************/
void BaseUI::SetICursor(const wchar_t* pCurImg, bool bHide, int PointMode, bool bFlag)
{
	m_pIUI->GetICursor()->SetCursorImage(pCurImg);
	m_pIUI->GetICursor()->SetPointMode(PointMode);
	m_pIUI->GetICursor()->SetHideCursor(bHide);
	m_pIUI->GetICursor()->SetControlVisible(bFlag);

	return;
}