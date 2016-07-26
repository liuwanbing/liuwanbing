#pragma once

#include "..\..\..\..\sdk\include\GameBaseInterface\BUI.h"

class MUI : public BaseUI
{
public:
	MUI();
	virtual ~MUI();

	//发牌动画
	void PlayCardInAnima(const int iMoveActionID, 
		const  int StarPointID, 
		const  int EndPointID, 
		const  int iBeMoveID, 
		int bView, 
		BYTE iCard[], 
		int iNum, 
		int iTime, 
		bool bFlag);

	//梭哈（图片）控件移动动画
	void PlayChipInAnima_Image(const int iMoveActionID[], 
		const  int StarPointID, 
		const  int EndPointID[], 
		const  int iBeMoveID[], 
		int bView, 
		int iNum, 
		int iTime, 
		bool bFlag);

	//梭哈（图片）控件移动动画(赢家获得输家筹码用)
	void PlayChipInAnimaEx_Image(const int iMoveActionID[], 
		const  int StarPointID[], 
		const  int EndPointID, 
		const  int iBeMoveID[], 
		int bView, 
		int iNum, 
		int iTime, 
		bool bFlag);
};

#include "MUI.inl"