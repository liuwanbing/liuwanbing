#pragma once
#ifndef CARDSET_HEAD_H
#define CARDSET_HEAD_H

struct SetCardStruct	//客户端到服务器
{
	BYTE pai[8][54];
	int	paicount[8];
};
class EXT_CLASS CCardSet: public CDialog
{
	DECLARE_DYNAMIC(CCardSet)

private:
	int						m_iCardCount;			//牌副数
	int						m_iCurrentDesk;			//当前位置
	int						m_iOldDesk;				//旧位置
	SetCardStruct			m_paistruct;			//牌结构
	int						m_RemainCard[54];		//剩牌数组
public:
	CCardSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCardSet();

	// Dialog Data
	//enum { IDD = IDD_CARD_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual void OnOK();
	//选牌数
	void OnChoiceCard(UINT nID);
	//选位置号
	void OnChoiceDesk(UINT nID);
	//设置当前数据
	void SetCurrentData(int iDesk);
	//更新控件
	BOOL UpdateComboBox();
	//从值得到牌
	BYTE GetCardByValue(int Value);
};

#endif
