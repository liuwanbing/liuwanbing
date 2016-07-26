#include "StdAfx.h"
#include "MUI.h"

MUI::MUI()
{

	return;
}
MUI::~MUI()
{

	return;
}

/*************************************************
*Function: 发牌动画(重写，发到牌控件中间)
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
void MUI::PlayCardInAnima(
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

		GETCTRL(ICardBase, pICardBase, GetpIUI(), EndPointID + bView);//以每个人的手牌位置为终点
		if (NULL != pICardBase)
		{
			int iw = 0;
			iw = pICardBase->GetGameW();
			EndPoint.x = pICardBase->GetGameX() + iw / 2 - 20;
			EndPoint.y = pICardBase->GetGameY();
		}

		GETCTRL(IBCInterFace, pControl, GetpIUI(), StarPointID);//以发牌器位置为起点
		if (NULL != pControl)
		{
			StarPoint.x = pControl->GetGameX();
			StarPoint.y = pControl->GetGameY();
		}

		ICardBase * _pICardBase = NULL;
		GETCTRL(ICardBase, _pICardBase, GetpIUI(), iBeMoveID);//移动的牌
		if (NULL != _pICardBase)
		{
			_pICardBase->SetCardList(iCard , iNum);
			_pICardBase->SetGameXY(StarPoint.x, StarPoint.y);
			_pICardBase->SetControlVisible(true);
		}


		IMoveAction* pMoveAction = NULL;
		GETCTRL(IMoveAction, pMoveAction, GetpIUI(), iMoveActionID);//移动动画
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

/*************************************************
*Function: 梭哈(图片)控件移动动画
*writer:
*		帅东坡
*Parameters:
*		[int]
*						iMoveActionID[2]MoveAction控件ID
*		[int]
*						StarPointID		起始点控件ID
*		[int]
*						EndPointID		终止点控件ID
*		[int]
*						iBeMoveID[2]	被移动控件ID(0 图片 1 文字)
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
void MUI::PlayChipInAnima_Image(const int iMoveActionID[], const  int StarPointID, const  int EndPointID[], const  int iBeMoveID[], int bView, int iNum, int iTime, bool bFlag)
{
	if (bFlag)
	{
		//起始位置
		POINT StarPoint;
		POINT EndPoint;
		ICardBase * pICardBase = NULL;
		IBCInterFace * pControl = NULL;
		IMoveAction* pMoveAction = NULL;
		GETCTRL(ICardBase, pICardBase, GetpIUI(), StarPointID + bView);//以每个人的手牌位置为起点
		if (NULL != pICardBase)
		{
			StarPoint.x = pICardBase->GetGameX();
			StarPoint.y = pICardBase->GetGameY();
		}

		for (int i = 0; i < 2; ++i)
		{
			GETCTRL(IBCInterFace, pControl, GetpIUI(), EndPointID[i] + bView);//以各玩家下注位置为终点
			if (NULL != pControl)
			{
				EndPoint.x = pControl->GetGameX();
				EndPoint.y = pControl->GetGameY();
			}

			SetControlGameXY(iBeMoveID[i], StarPoint.x, StarPoint.y);
			ShowControl(iBeMoveID[i], true);
			
			GETCTRL(IMoveAction, pMoveAction, GetpIUI(), iMoveActionID[i]);//移动动画
			if (NULL != pMoveAction)
			{
				pMoveAction->SetControlingCtrlID(iBeMoveID[i]);
				pMoveAction->SetCurveInstance(CT_STRAIGHT);

				pMoveAction->SetStartP(StarPoint);
				pMoveAction->SetEndP(EndPoint);
				pMoveAction->SetTotalMoveTime(iTime);

				pMoveAction->SetControlVisible(bFlag);
				pMoveAction->SetPlayState(bFlag);
			}
		}
	}

	return;
}

/*************************************************
*Function: 梭哈(图片)控件移动动画(赢家获得输家筹码用)
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
void MUI::PlayChipInAnimaEx_Image(const int iMoveActionID[], const  int StarPointID[], const  int EndPointID, const  int iBeMoveID[],  int bView, int iNum, int iTime, bool bFlag)
{
	if (bFlag)
	{
		//起始位置
		POINT StarPoint;
		POINT EndPoint;
		ICardBase * pICardBase = NULL;
		IBCInterFace * pControl = NULL;
		IMoveAction* pMoveAction = NULL;

		GETCTRL(ICardBase, pICardBase, GetpIUI(), EndPointID + bView);//以每个人的手牌位置为终点
		if (NULL != pICardBase)
		{
			EndPoint.x = pICardBase->GetGameX();
			EndPoint.y = pICardBase->GetGameY();
		}

		for (int i = 0; i < 2; ++i)
		{
			GETCTRL(IImage, pControl, GetpIUI(), StarPointID[i] + bView);//以各玩家下注位置为起点
			if (NULL != pControl)
			{
				StarPoint.x = pControl->GetGameX();
				StarPoint.y = pControl->GetGameY();
			}

			SetControlGameXY(iBeMoveID[i], StarPoint.x, StarPoint.y);
			ShowControl(iBeMoveID[i], true);

			GETCTRL(IMoveAction, pMoveAction, GetpIUI(), iMoveActionID[i]);//移动动画
			if (NULL != pMoveAction)
			{
				pMoveAction->SetControlingCtrlID(iBeMoveID[i]);
				pMoveAction->SetCurveInstance(CT_STRAIGHT);

				pMoveAction->SetStartP(StarPoint);
				pMoveAction->SetEndP(EndPoint);
				pMoveAction->SetTotalMoveTime(iTime);

				pMoveAction->SetControlVisible(bFlag);
				pMoveAction->SetPlayState(bFlag);
			}
		}
	}

	return;
}