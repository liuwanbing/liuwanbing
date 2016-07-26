#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "ServiceManage.h"

//服务器设置类
class CServiceSet : public CDialog
{
	//变量定义
protected:
	UINT								m_KindID;						//类型 ID
	TCHAR								m_szServiceIP[16];				//服务 IP
	SC_ServiceInfoStruct				m_ServiceInfo;					//组件参数
	SC_GameRoomInitStruct				m_GameRoomInit;					//初始化参数

	//控件变量
public:
	CComboBox							m_ComType;						//游戏类型
	CComboBox							m_GameKind;						//挂接类型
	CServiceManage						* m_pServiceManage;				//服务控制
	CIPAddressCtrl						m_CenterSQLIP;					//中心服务器 IP
	CIPAddressCtrl						m_LogonSQLIP;					//登陆服务器 IP
	CIPAddressCtrl						m_NativeSQLIP;					//本地服务器 IP

	//函数定义 
public:
	//构造函数
	CServiceSet(SC_ServiceInfoStruct & ServiceInfo, CServiceManage * pServiceManage);
	//析构函数
	virtual ~CServiceSet();

	DECLARE_MESSAGE_MAP()

	//重载函数
protected:
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();

	//功能函数
public:
	//获取配置参数
	SC_GameRoomInitStruct * GetGameRoomConfigData() { return &m_GameRoomInit; }
};
