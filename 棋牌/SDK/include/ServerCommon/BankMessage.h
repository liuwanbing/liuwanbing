#pragma once

#include "AFCDataBase.h"
#include "gameroommessage.h"

#define DTK_GR_BANK_NORMAL			80
#define DTK_GR_BANK_TRANSFER		81
#define DTK_GR_BANK_DEPOSIT			82
#define DTK_GR_BANK_WALLET			83
#define DTK_GR_BANK_QUERY_DEPOSIT	84

struct db_bank_game_wallet_in
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	bank_game_wallet					_data;
};

struct db_bank_game_wallet_out
{
	DataBaseResultLine					ResultHead;
	bank_game_wallet					_data;
};

struct db_bank_op_normal_in
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	bank_op_normal						_data;
};

struct db_bank_op_normal_out
{
	DataBaseResultLine					ResultHead;
	bank_op_normal						_data;
};

struct db_bank_op_transfer_in
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	bank_op_transfer					_data;
};
struct db_bank_op_transfer_out
{
	DataBaseResultLine					ResultHead;
	bank_op_transfer					_data;
};

struct db_bank_op_deposit_in
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	bank_op_deposit						_data;
};

struct db_bank_op_deposit_out
{
	DataBaseResultLine					ResultHead;
	bank_op_deposit						_data;
	__int64								_Money;
};

struct db_bank_op_wallet_in
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	int									_UserID;
};

struct db_bank_op_query_deposit_in
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	int									_UserID;
};

struct db_bank_op_query_deposit_out
{
	DataBaseResultLine					ResultHead;
	bank_op_deposit_node				_data;
};
