#ifndef _GAMESELECT_H_
#define _GAMESELECT_H_
// ----------------------------------------------------------------------------
//
// PengJiLin, 2011-4-2, 指定哪些游戏是有效的
//
// ----------------------------------------------------------------------------

#define MAX_NUM_OF_GAME     900     // 最大的游戏个数
#define DITS_NUM_OF_NAMEID  8       // NameID 的位数

class CGameSelecter
{
public:
    CGameSelecter();
    ~CGameSelecter();

public:
    // 游戏是否是可用的
    BOOL GameIsUsable(UINT uNameID);

private:
    // 免费游戏是否可用
    BOOL FreeGameIsUsable(UINT uNameID);

    // 收费游戏是否可用
    BOOL PayGameIsUsable(UINT uNameID);

    // 从表中获取免费NameID
    UINT GetNameIDFromFreeTable(UINT uIndex);

    // 从表中获取收费NameID
    UINT GetNameIDFromPayTable(UINT uIndex);

    // 加解密
    BYTE Encode(BYTE byData);
    BYTE Decode(BYTE byData);

private:

};


#endif