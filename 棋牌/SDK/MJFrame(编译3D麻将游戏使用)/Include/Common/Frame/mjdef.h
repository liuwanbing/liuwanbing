/** @file	mjdef.h
 *	@brief	麻将结构的基础定义
 *	@author dxh
 *	@date 2010.10.08
 */
#ifndef __MJDEF_H___
#define __MJDEF_H___

#include "..\typeDef.h"

namespace MyGame
{

typedef unsigned char TILE;

#define TILE_BEGIN		0		// 空牌
#define TILE_CHAR_1		1		// 万
#define TILE_CHAR_2		2
#define TILE_CHAR_3		3
#define TILE_CHAR_4		4
#define TILE_CHAR_5		5
#define TILE_CHAR_6		6
#define TILE_CHAR_7		7
#define TILE_CHAR_8		8
#define TILE_CHAR_9		9
#define TILE_BAMBOO_1	11		// 条
#define TILE_BAMBOO_2	12
#define TILE_BAMBOO_3	13
#define TILE_BAMBOO_4	14
#define TILE_BAMBOO_5	15
#define TILE_BAMBOO_6	16
#define TILE_BAMBOO_7	17
#define TILE_BAMBOO_8	18
#define TILE_BAMBOO_9	19
#define TILE_BALL_1		21		// 筒
#define TILE_BALL_2		22
#define TILE_BALL_3		23
#define TILE_BALL_4		24
#define TILE_BALL_5		25
#define TILE_BALL_6		26
#define TILE_BALL_7		27
#define TILE_BALL_8		28
#define TILE_BALL_9		29
#define TILE_EAST		31		// 字
#define TILE_SOUTH		32
#define TILE_WEST		33
#define TILE_NORTH		34
#define TILE_ZHONG		35
#define TILE_FA			36
#define TILE_BAI		37
#define TILE_FLOWER_CHUN	41	// 花	// 春
#define TILE_FLOWER_XIA		42			// 夏
#define TILE_FLOWER_QIU		43			// 秋
#define TILE_FLOWER_DONG	44			// 冬
#define TILE_FLOWER_MEI		45			// 梅
#define TILE_FLOWER_LAN		46			// 兰
#define TILE_FLOWER_ZHU		47			// 竹
#define TILE_FLOWER_JU		48			// 菊

#define TILE_END			49	// 结束

#define TILE_ALL_NBR		34

#define TILE_GENRE_CHAR		0	// 类型：万
#define TILE_GENRE_BAMBOO	1	// 条
#define TILE_GENRE_BALL		2	// 筒
#define TILE_GENRE_WIND		3	// 字
#define TILE_GENRE_FLOWER	4	// 花
#define TILE_GENRE_OTHER	5	// 没得

#define MAX_TOTAL_TILES		144	// 总的牌张数
#define MAX_HAND_TILES		18	// 手牌的总数
#define MAX_GIVE_TILES		36	// 出牌的总数
#define MAX_SET_TILES		5	// 吃碰杠牌的总数
#define MAX_INCREASE_TILES	9	// 最大连续数

//方位定义

//////////////////////////////////////////////////////////////////////////
//  游戏中桌子座位方位的分布 (4人)		2人	
// 				 2						 1	
// 	             |						 |
// 				 |						 |
// 		  3 --———— 1				 |
// 				 |                       |
// 				 |                       |      
// 				 0	                     0
// 而游戏中逻辑的顺序却是顺时针的
//////////////////////////////////////////////////////////////////////////
#define SEAT_SELF			0	// 自家
#define SEAT_FOLLOW			1	// 下家
#define SEAT_OPPOSITE		2	// 对家
#define SEAT_AHEAD			3	// 上家

//定义吃、碰、杠
#define ACTION_EMPTY				0x00	// 没有行为
#define ACTION_COLLECT				0x10	// 吃牌
#define ACTION_TRIPLET				0x11	// 碰
#define ACTION_QUADRUPLET_PATCH		0x12	// 补杠
#define ACTION_QUADRUPLET_CONCEALED	0x13	// 暗杠
#define ACTION_QUADRUPLET_REVEALED	0x14	// 明杠
#define ACTION_QUADRUPLET_DNXB      0x15    // 东南西北杠
#define ACTION_QUADRUPLET_ZFB       0x16    // 中发白杠
#define ACTION_WIN					0x17	// 胡牌
#define ACTION_TING					0x18	// 听牌
#define ACTION_FLOWER				0x19	// 补花
#define ACTION_GIVE					0x1A	// 要出的牌（最右边的一张）


// 吃牌的结果位置标记
#define COLLECT_RES_OOX	1
#define COLLECT_RES_OXO	10
#define COLLECT_RES_XOO	100

/**
 * @ingroup MJFrame
 * @brief 麻将控件支持的数据结构
 */	
struct TILES_INFO
{
	TILE	tile;		// 牌型
	BOOL	bSelect;	// 是否被点击（是否弹起）
	BYTE	ucFlag;		// 标记（记录吃碰杠的牌用）
	BYTE	ucWitch;	// 吃的哪一张	(0, 1, 2)
	BYTE	ucChair;	// 吃碰杠谁的
    BYTE    ucType;     // 区分胡牌类型，0：自摸，1：点炮， 2：流局
	UINT	dwReserve1;	// 预留BUFF位	1位：在吃牌的情况下，0：不能选的牌，1：可以选的牌
						//              2位：牌上面图片标志，0：不显示图片，1：显示图片
						//				3位：判断是否播放暗杠字体动画，0：播放，1：不播放
						//              4位：特殊钢的开头，0：不是特殊杠，2：特殊杠的开头一张
	UINT	dwReserve2;	// 补杠时, 杠牌插入到拦牌区碰牌区域中的位置

	TILES_INFO()
    {
        memset(this, 0, sizeof(TILES_INFO));
    }
};

/**
* @ingroup MJFrame
* @brief 断线重回, 麻将出牌区域数据恢复
*/	
struct TILES_GIVE
{
    TILE	tile;	   // 牌值
    BOOL	bCPG;	   // 是否被吃碰杠
    BOOL    bTing;     // 是否为听牌后打出的那张牌
    
    TILES_GIVE()
    {
        memset(this, 0, sizeof(TILES_GIVE));
    }
};

}
#endif

