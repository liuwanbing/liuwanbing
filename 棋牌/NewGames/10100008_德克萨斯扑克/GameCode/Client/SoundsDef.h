// ***************************************************************
//  SoundsDef   version:  1.0   ·  date: 03/29/2011
//  -------------------------------------------------------------

//	德克萨斯扑克 服务端，玩家游戏声音定义表\
	NameID：51300280

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#pragma once

#ifndef DZPKSOUND_DEF_HEAD_FILE_51300280

namespace DZPKSound
{
	//游戏声效ID定义
	enum BaseType
	{
		BT_GAME_MUSIC1		= 0,		// 背景音乐
		BT_SEND_BACK_CARD	= 1,		// 发底牌声音
		BT_FLOP_ROUND		= 2,		// 翻牌公共声音
		BT_TURN_ROUND		= 3,		// 转牌公共声音
		BT_RIVER_ROUND		= 4,		// 河牌公共声音
		BT_FLOP_PUBLIC_CARD	= 5,		// 翻公共牌声音
		BT_FLOP_BACK_CARD	= 6,		// 翻底牌声音
		BT_WIN				= 7,		// 胜利声调
		BT_LOSE				= 8,		// 失败声调
		BT_FOLD_MUSIC		= 9,		// 弃牌声调
		BT_CHIP_MUSIC		= 10,		// 筹码声调
		BT_TRANSFER_TOKEN	= 11,		// 移交令牌声调
		BT_OK				= 12,		// 确定按钮声调
		BT_MONEY_BUTTON		= 13,		// 金额选择声调
		BT_ALARM_CLOCK		= 14,		// 闹钟声调
		BT_CHIP_MONEY_MUSIC	= 15,		// 筹码音效
		BT_NUMS				= 16,		// 数量
	};

	//游戏声效表定义
	const static TCHAR* g_szBaseFiles[BT_NUMS] = 
	{
		_TEXT("背景音乐1.ogg"),	// 背景音乐
		_TEXT("发牌.ogg"),		// 发底牌声调
		_TEXT("flop.ogg"),		// 翻牌公共声音
		_TEXT("turn.ogg"),		// 转牌公共声音
		_TEXT("river.ogg"),		// 河牌公共声音
		_TEXT("中间翻牌.ogg"),	// 翻公共牌声音
		_TEXT("翻牌.ogg"),		// 翻底牌声调
		_TEXT("胜利.ogg"),		// 胜利声调
		_TEXT("失败.ogg"),		// 失败声调
		_TEXT("弃牌.ogg"),		// 弃牌声调
		_TEXT("筹码.ogg"),		// 筹码声调
		_TEXT("提示该谁.ogg"),	// 移交令牌声调
		_TEXT("确定按钮.ogg"),	// 确定按钮声调
		_TEXT("金额选择.ogg"),	// 金额选择声调
		_TEXT("闹钟.ogg"),		// 闹钟声调
		_TEXT("筹码音效.ogg"),		// 筹码音效
	};

	//地方语言ID定义
	enum LanguageType
	{
		LT_COMMON	= 0,	//标准语音
		LT_DIFANG01	= 1,	//地方语音
		LT_NUMS		= 2,    //数量
	};

	//语言包路径定义表
	const static TCHAR* g_szLangPath[LT_NUMS] = 
	{
		_TEXT("common"),		//标准语音
		_TEXT("difang_01"),		//地方语音
	};

	//游戏声效ID定义
	enum SpeakType
	{
		ST_CALL				= 0,		// 跟注声音
		ST_FOLD				= 1,		// 弃牌声音
		ST_CHECK			= 2,		// 过牌声音
		ST_RAISE			= 3,		// 加注声音
		ST_ALL_IN			= 4,		// 全下声音
		ST_NUMS				= 5,		// 数量
	};

	//游戏发音表定义
	const static TCHAR* g_szSpeakFiles[ST_NUMS][4] = 
	{
		_TEXT("Call_1.ogg"), _TEXT("Call_2.ogg"), _TEXT("Call_3.ogg"), _TEXT("Call_4.ogg"),     	// 跟注声音
		_TEXT("Fold_1.ogg"), _TEXT("Fold_2.ogg"),_TEXT("Fold_3.ogg"), _TEXT("Fold_4.ogg"),          // 弃牌声音
		_TEXT("Check_1.ogg"),_TEXT("Check_2.ogg"),_TEXT("Check_3.ogg"),_TEXT("Check_4.ogg"),	    // 过牌声音
		_TEXT("Raise_1.ogg"),_TEXT("Raise_2.ogg"),_TEXT("Raise_3.ogg"),_TEXT("Raise_4.ogg"),	    // 加注声音
		_TEXT("All_in_1.ogg"),_TEXT("All_in_2.ogg"),_TEXT("All_in_3.ogg"),_TEXT("All_in_4.ogg"),	// 全下声音
	};
}

#define DZPKSOUND_DEF_HEAD_FILE_51300280
#endif