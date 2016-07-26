#pragma once

#include "GameLanchSDK/bzwUiInclude/bzwUI.h"
#include "defines.h"
//#include "../../../../sdk/include/GameLanchSdk/LoveSendClassInExe.h"
#include "..\GameMessage\UpGradeMessage.h"

struct TResults
{
	int iBetPools[8];				//所有下注池
	int iUserBet[PLAY_COUNT][8];	//每个玩家下注数据
	int iScore[PLAY_COUNT];			//输赢积分
	int iMoney[PLAY_COUNT];			//输赢金币
	int iSelfMoney;					//自己的金币更新: 客户端获取的金币有可能还没有刷新, 所以在这里发下去
	int nUserID[PLAY_COUNT];		//每个玩家的ID

	TResults()
	{
		memset(this, 0, sizeof(TResults));
	}
};

class CFinishDlg : public BModalContainer
{
public:

	CFinishDlg(DWORD pId,CWnd* pParent=NULL);

	~CFinishDlg();

	void SetMessage(const TResult& Result);

	/// 处理当前包容器中的控件消息
	virtual bool OnPostMessage(UINT pId,BBaseParameter* pParam);

	/// 加载对话框
	virtual bool LoadConfig(std::string path);

public:

	//设置成只能退出的状态，因为没有玩家，只有自己在桌的话，只能退出
	void SetNotice(std::string path,bool bShowNextBnt);

	//设置成服务器游戏已Finish的状态
	void SetGameFinish(bool bFinish);

private:
	CFinishDlg();

private:

	CWnd* m_pWndParent;

	bool m_bGameFinished;

};