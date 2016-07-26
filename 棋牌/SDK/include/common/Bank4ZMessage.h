#pragma once

#ifndef AFC_BANK_MESSAGE_HEAD_FILE
#define AFC_BANK_MESSAGE_HEAD_FILE

#define MDM_GP_BANK									121

	#define ASS_GP_BANK_OPEN						21      //打开银行获取钱包信息
		#define HC_GP_BANK_OPEN_SUC					1		//打开钱柜成功
		#define HC_GP_BANK_OPEN_ERR					2		//打开钱柜错误
		#define HC_GP_BANK_OPEN_ERR_PSW				3		//打开钱框错误

	#define ASS_GP_BANK_GETGM_WALLET				22      //获取游戏钱包(二代银行专用)

	#define ASS_GP_BANK_CHECK						23		//银行存取钱
		#define HC_GP_BANK_CHECK_SUC				0		//操作成功
		#define HC_GP_BANK_CHECK_NO_USER			1		//没有找到用户
		#define HC_GP_BANK_CHECK_ERR_PWD			2		//错误密码
		#define HC_GP_BANK_CHECK_NOT_MONEY			4		//余额不足

	#define ASS_GP_BANK_TRANSFER					24		//银行转账
		#define HC_GP_BANK_TRANSFER_ERROR			1		//转帐错误
		#define HC_GP_BANK_TRANSFER_SUC				2		//转帐成功
		#define HC_GP_BANK_TRANSFER_NO_DEST			3		//转帐目标不存在
		#define HC_GP_BANK_TRANSFER_PASSWORD_ERROR	4		//转帐密码错误
		#define HC_GP_BANK_TRANSFER_TOTAL_LESS		5		//银行总额太小，不够资格
		#define HC_GP_BANK_TRANSFER_TOO_LESS		6		//单笔转帐数目太少
		#define HC_GP_BANK_TRANSFER_MULTIPLE		7		//单笔转帐数目必须是某数的倍数
		#define HC_GP_BANK_TRANSFER_NOT_ENOUGH		8		//银行金额不够本次转帐
		#define HC_GP_BANK_TRANSFER_TOO_MANY_TIME   9		//当天转账的次数太多了

	#define ASS_GP_BANK_TRANS_RECORD				25		//银行转账记录
		#define HC_GP_BANK_TRANS_RECORD_SUC			1		//银行转账记录正常获取

	#define ASS_GP_BANK_DEPOSIT						26		//存单业务
		#define HC_GP_BANK_DEPOSIT_SUC				0		//存单业务办理成功

	#define ASS_GP_BANK_QUERY_DEPOITS				27		//存单查询
	#define ASS_GP_BANK_CHPWD						28		//修改银行密码
		#define HC_GP_BANK_CHPWD_SUC				1		//银行转账记录正常获取
		#define HC_GP_BANK_CHPWD_ERR_PWD			2		//银行转账记录正常获取

	#define ASS_GP_BANK_FINANCE_INFO				29		//财务相关
		#define HC_GP_BANK_FINANCE_INFO_SUC			1		//财务数据正常获取
		#define HC_GP_BANK_FINANCE_INFO_ERR			2		///财务数据获取异常

//打开银行(使用二级密码)
struct TMSG_GP_BankOpenReq
{
	char szMD5Pass[50]; //加密密码
};

//钱柜数据
struct TMSG_GP_BankInfo
{
	long int        dwUserID;
	__int64         i64Bank;							///钱柜
	__int64			i64Wallet;							//钱包的钱
	char			szTwoPassword[50];					//银行密码
	int				nVer;								//银行版本
};

//游戏钱包结构
struct TMSG_GP_BankWallet
{
	int				_game_id;
	char			_game_name[30];
	__int64			_money;
};

//银行存取钱通讯包
struct TMSG_GP_BankCheck
{
	int				_operate_type;
	int				_user_id;
	int				_game_id;
	TCHAR			_szMD5Pass[50];  //银行密码
	__int64			_money;
};

//转帐
struct TMSG_GP_BankTransfer
{
	long int		UserID;					//用户ID
	long int		destUserID;				//转给谁
	__int64			i64Money;				//欲转帐金额
	__int64			i64ActualTransfer;		//实际到帐金额
	int				bTranTax;               //是否扣税，非0不扣税
	TCHAR			szMD5Pass[50];			//银行密码
	TCHAR			szNickName[50];			//用户昵称		add by lxl 2010-11-10
	TCHAR			szDestNickName[50];		//被转账用户昵称
	bool			bUseDestID;
	UINT            uHandleCode;			// PengJiLin, 2010-8-23, 用来区分是否是“全转”功能

	TMSG_GP_BankTransfer()
	{
		ZeroMemory(this,sizeof(TMSG_GP_BankTransfer));
	}
};

///转帐记录查询请求
struct TMSG_GP_BankTranRecordReq
{
	long int		dwUserID;	///用户 ID
};

//用户转帐记录结果
struct TMSG_GP_BankTranRecord
{
	int				nSeriNo;				///< 序号
	UINT			dwUserID;				///< 用户 ID
	UINT			destUserID;				///< 转给谁
	__int64			i64Money;				///< 欲转帐金额
	__int64			i64ActualTransfer;		///< 实际到帐金额
	COleDateTime	oleDateTime;			///< 时间

	TMSG_GP_BankTranRecord()
	{
		ZeroMemory(this, sizeof(TMSG_GP_BankTranRecord));
	}
};

//存单,充值
struct TMSG_GP_BankDeposit
{
	int		_operate_type;		//1为创建存单,2为充值
	int		_user_id;
	TCHAR	_szNickName[50];	//被转账用户昵称
	TCHAR	_szMD5Pass[50];		//银行密码
	int		_state;
	int		_csds;
	__int64	_money;
	char	_deposit_id[16];
	char	_deposit_password[16];
	char	_time[32];
};

//存单查询结构
struct TMSG_GP_BankQueryDeposit
{
	int				_serial_no;	    //序号
	char			_deposit_id[16];
	char			_deposit_password[16];
	__int64			_money;
	COleDateTime	_create_time;
	int				_csds;
	int				_user_id;
	int				_state;
	int				_used_user_id;
};

///修改密码
struct TMSG_GP_BankChPwd
{
	long int		_user_id;								///用户ID
	TCHAR			_MD5Pass_old[50];					///银行密码旧
	TCHAR			_MD5Pass_new[50];					///银行密码新
	TMSG_GP_BankChPwd()
	{
		ZeroMemory(this, sizeof(TMSG_GP_BankChPwd));
	}
};


#endif