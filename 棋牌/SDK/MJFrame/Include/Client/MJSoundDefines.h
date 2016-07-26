#ifndef _SOUND_FILE_DEFINES_H
#define _SOUND_FILE_DEFINES_H

// 麻将框架声音定义
////////////////////////////////////////////////////////////////////////////////
#define SOUND_OUTCARD           "OutcardWeight.ogg"
#define SOUND_BEGIN             "Begin.ogg"
#define SOUND_READY             "Ready.ogg"
#define SOUND_BLOCK             "Block.ogg"
#define SOUND_WARNING           "Warning.ogg"
#define SOUND_FAPAI             "Fapai.ogg"
#define SOUND_ZHUAPAI           "Get.ogg"
#define SOUND_LIUJU             "Liuju.ogg"
#define SOUND_BLOCKTIP          "blocktip.ogg"
#define SOUND_BAWANG			"Bawang.ogg"
#define SOUND_FANJIN			"Fanjin.ogg"
#define SOUND_HU				"Hu.ogg"
#define SOUND_WIN				"win.ogg"
#define SOUND_LOST				"lost.ogg"
#define SOUND_SEL               "Sel.ogg"
#define SOUND_SEZI              "Sezi.ogg"
#define SOUND_TIP               "Tip.ogg"
#define SOUND_CLOCK             "Clock.ogg"

#define SOUND_CHI				100 // 吃牌声音
#define SOUND_PENG				101 // 碰牌声音
#define SOUND_GANG				102 // 杠牌声音
#define SOUND_TING				103 // 听牌声音
#define SOUND_ZIMO				104 // 自摸声音
#define SOUND_DIAN_PAO			105 // 点炮声音

// 出牌时3种麻将音效随机播放
static char * SOUND_OUTCARDRAND[3] = 
{
	"Outcard3.ogg", 
	"OutcardSlight.ogg", 
	"OutcardWeight.ogg"
};

// 报警音效根据数值来确定播放
static char * SOUND_ALARM[5] = 
{
	"bj0.ogg",
	"bj1.ogg",
	"bj2.ogg",
	"bj3.ogg",
	"bj4.ogg",
};

#endif