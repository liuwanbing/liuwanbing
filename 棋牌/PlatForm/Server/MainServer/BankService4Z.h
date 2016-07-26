#pragma once

#ifndef AFC_BANK_SERVICE_HEAD_FILE
#define AFC_BANK_SERVICE_HEAD_FILE

#include "Stdafx.h"
#include "AFCDataBase.h"

#include "BankMessage.h"
#include "Bank4ZMessage.h"
#include "Bank4ZDatabaseMessage.h"

class CGameMainManage;
class CGameLogonManage;

class CBankService4Z
{

public:

	CBankService4Z(CWnd *pWnd);

	~CBankService4Z(void);

public:

	//收到网络请求
	bool OnNetMessage(NetMessageHead* pNetHead,void* pData,UINT uSize,ULONG uAccessIP,UINT uIndex,DWORD dwHandleID);

	//处理网络请求结果处理
	bool OnDataBaseResult(DataBaseResultLine* pResultData); 

//所有网络请求的实现
public:

	//打开银行
	bool OnOpenBankRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//查询钱包
	bool OnGetGameWalletRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//存取钱
	bool OnCheckMoneyRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//转账
	bool OnTransferRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//查询账单
	bool OnTransRecordRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//存单业务
	bool OnDepositRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//查询存单
	bool OnQueryDepositsRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//修改银行密码
	bool OnChangePasswordRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

	//得到个人财务情况
	bool OnGetFinanceInfoRequest(void* pData,UINT uSize,UINT uIndex,DWORD dwHandleID);

//所有请求处理结果实现
public:

	//打开银行
	bool OnOpenBankResponse(DataBaseResultLine* pResultData);

	//查询钱包
	bool OnGetGameWalletResponse(DataBaseResultLine* pResultData);

	//存取钱
	bool OnCheckMoneyResponse(DataBaseResultLine* pResultData);

	//转账
	bool OnTransferReponse(DataBaseResultLine* pResultData);

	//查询账单
	bool OnTransRecordResponse(DataBaseResultLine* pResultData);

	//存单业务
	bool OnDepositResponse(DataBaseResultLine* pResultData);

	//查询存单
	bool OnQueryDepositsResponse(DataBaseResultLine* pResultData);

	//修改银行密码
	bool OnChangePasswordResponse(DataBaseResultLine* pResultData);

	//获取财务状况
	bool OnGetFinanceInfoResponse(DataBaseResultLine* pResultData);


private:

	//父管理模块
	CGameMainManage* m_pMainManage;

	//登陆管理模块
	CGameLogonManage* m_pLogonManage;

};

#endif

