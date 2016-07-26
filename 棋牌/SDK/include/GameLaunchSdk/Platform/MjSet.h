#pragma once
#ifndef MJSET_HEAD_H
#define MJSET_HEAD_H
// CMjSet dialog
struct SetPaiStruct	//客户端到服务器
{
	int pai[8][20];
	int	paicount[8];
};

class EXT_CLASS CMjSet : public CDialog
{
	DECLARE_DYNAMIC(CMjSet)
private:
	int						m_iCurrentDesk;			//当前位置
	int						m_iOldDesk;				//旧位置
	SetPaiStruct			m_paistruct;			//牌结构
	int						m_RemainCard[42];		//剩牌数组

public:
	CMjSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMjSet();

// Dialog Data
	//enum { IDD = IDD_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//int	m_id[42];
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	//选位置号
	void OnChoiceDesk(UINT nID);
	//设置当前数据
	void SetCurrentData(int iDesk);
	//更新控件
	BOOL UpdateComboBox();
	//从值得到牌
	int GetCardByValue(int Value);

};

#endif