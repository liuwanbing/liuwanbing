#pragma once

#ifndef AFC_BANK4Z_DATAMESSAGE_HEAD_FILE_
#define AFC_BANK4Z_DATAMESSAGE_HEAD_FILE_

#include "AFCDataBase.h"
#include "MainManage.h"

//数据库消息
#define DTK_GP_BANK_OPEN			80       //查询余额
#define DTR_GP_BANK_OPEN_SUC		81		 //打开钱柜成功
#define DTR_GP_BANK_OPEN_ERROR		82		 //打开钱柜错误
#define DTK_GP_BANK_GETGM_WALLET	83		 //打开钱包
#define DTK_GP_BANK_CHECK			84		 //存取
#define DTK_GP_BANK_TRANSFER		85		 //转账
#define DTK_GP_BANK_TRANS_RECORD	86		 //转账记录
#define DTK_GP_BANK_DEPOSIT			87		 //存单业务
#define DTK_GP_BANK_QUERY_DEPOSIT	88		 //查询存单
#define DTK_GP_BANK_CHPWD			95		 //修改银行密码
#define DTK_GP_BANK_FINANCE_INFO	96		 //获取个人财务情况

//打开银行
struct DL_GP_I_BankOpen
{
	DataBaseLineHead		DataBaseHead;
	long int				dwUserID;				//用户 ID
};

struct DL_GP_O_BankOpen
{
	DataBaseResultLine		ResultHead;
	TMSG_GP_BankInfo		_data;
};

//查询钱包
struct DL_GP_I_BankGetWallet
{
	DataBaseLineHead		DataBaseHead;
	int						_UserID;
};

struct DL_GP_O_BankGetWallet
{
	DataBaseResultLine		ResultHead;
	TMSG_GP_BankWallet		_data;
};

//存取钱
struct DL_GP_I_BankCheck
{
	DataBaseLineHead		DataBaseHead;
	TMSG_GP_BankCheck		_data;
};

struct DL_GP_O_BankCheck
{
	DataBaseResultLine		ResultHead;
	TMSG_GP_BankCheck		_data;
};

//转帐
struct DL_GP_I_BankTransfer
{
	DataBaseLineHead		DataBaseHead;
	TMSG_GP_BankTransfer    _data;
};

struct DL_GP_O_BankTransfer
{
	DataBaseResultLine		ResultHead;
	TMSG_GP_BankTransfer    _data;
};

///转帐记录
struct DL_GP_I_BankTransRecord
{
	DataBaseLineHead			DataBaseHead;
	TMSG_GP_BankTranRecordReq	_data;
};

struct DL_GP_O_BankTransRecord
{
	DataBaseResultLine			ResultHead;	
	TMSG_GP_BankTranRecord		_data;
};

//存单服务
struct DL_GP_I_BankDeposit
{
	DataBaseLineHead			DataBaseHead;
	TMSG_GP_BankDeposit			_data;
};

struct DL_GP_O_BankDeposit
{
	DataBaseResultLine			ResultHead;
	TMSG_GP_BankDeposit			_data;
	__int64						_Money;
};

//存单查询
struct DL_GP_I_BankQueryDeposit
{
	DataBaseLineHead			DataBaseHead;
	int							_UserID;
};

struct DL_GP_O_BankQueryDeposit
{
	DataBaseResultLine			ResultHead;
	TMSG_GP_BankQueryDeposit	_data;
};

//修改银行密码
struct DL_GP_I_BankChPwd
{
	DataBaseLineHead			DataBaseHead;
	TMSG_GP_BankChPwd			_data;
};

struct DL_GP_O_BankChPwd
{
	DataBaseResultLine			ResultHead;
};


#endif