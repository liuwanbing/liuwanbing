#pragma once

//消息头文件
#include "..\..\..\SDK\include\common\BaseMessage.h"
#include "..\..\..\SDK\include\common\GameRoomMessage.h"
//#include "..\..\服务器组件\类库.2003\ItemDateMessage.h"

//数据类头文件
#include "UserPower.h"
#include "ClientComStruct.h"

//界面类
#include "AFCWeb.h"
//#include "GameCard.h"
#include "AFCRegistry.h"

//界面类
#include "AFCEdit.h"
#include "AFCImage.h"
#include "AFCFunction.h"
#include "AFCResource.h"
#include "AFCButton.h"
#include "AFCDialog.h"
#include "AFCListCtrl.h"
#include "AFCTabCtrl.h"
#include "AFCWindow.h"
#include "AFCLogoResource.h"
#include "..\..\..\SDK\include\socket\TCPClientSocket.h"
#include "AFCMenu.h"

//#include "SKinScrollWnd.h"
//#include "SKinScrollBar.h"
//功能类 
#include "AFCExpressionWnd.h"
#include "MessageHandle.h"
#include "GameEndView.h"

 //wushuqun 2008.9.24
#include "ConfirmDlg.h"

//游戏框架
#include "PlayView.h"
#include "GameFrameDlg.h"
#include "GetPoint.h"
//#include "BroadcastMsgInputDlg.h"
#include "ShortMessage.h"
//名字空间声明
//using namespace AFC;
//using namespace AFC::AFCEdit;
//using namespace AFC::AFCSocket;
//using namespace AFC::AFCDialog;
//using namespace AFC::AFCButton;
//using namespace AFC::AFCWindow;
//using namespace AFC::AFCListCtrl;
//using namespace AFC::AFCResource;
//using namespace AFC::AFCImageGoGoGo;

#ifndef AFC_FACE_DLL
//获取等级函数声明
extern "C" __declspec(dllimport) TCHAR * GetMoneyOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllexport) TCHAR * GetPointOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllimport) void SetGameID(int iGameID);
extern "C" __declspec(dllimport) TCHAR * GetMatchOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllimport) TCHAR * GetNormalOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllimport) int GetNormalOrderBasePoint(__int64 dwPoint);
extern "C" __declspec(dllimport) int GetNormalOrderLevel(__int64 dwPoint);
extern "C" __declspec(dllimport) int  GetMoneyGrade(__int64 dwPoint);
//魅力级别
extern "C" __declspec(dllimport) TCHAR * GetCharmGrade(__int64 dwCharm);

/// 获取指定数值的字符串
/// 本函数同时支持UNICODE和ANSI
/// @param[out] str 字符串数组，须由调用者分配空间
/// @param[in] nNum 须显示的整数数值
/// @param[in] nPower 以10的几次幂显示，如123显示成123000，则此参数为3
/// @param[in] bHasSpace 字符串是否分节显示，默认false表示数字不分节，true表示用空格每三位分一节
/// @return 返回str所在的位置
///
// PengJiLin, 2010-8-3, 增加可以输入分隔符
extern "C" __declspec(dllexport) TCHAR *GetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace);

///获取用户的形象图片索引(大厅使用)
/// @param[in] uLogoID 用户的LogoID;
/// @param[in] i64Bank 用户存款;
/// @param[in] i64Money 用户身上金币
/// @return 返回形象图片索引
extern "C" __declspec(dllimport) UINT GetAvatarIndexInMain(UINT uLogoID,__int64 i64Bank,__int64 i64Money);

///获取用户的形象图片索引(游戏房间使用)
/// @param[in] uLogoID     用户的LogoID;
/// @param[in] i64Bank     用户存款;
/// @param[in] i64Money    用户身上金币;
/// @param[in] dwPoint     用户积分;
/// @param[in] uGameNameID 游戏ID;
/// @param[in] uComType    房间类型，1-为积分场，3-为金币场
/// @return 返回形象图片索引
extern "C" __declspec(dllimport) UINT GetAvatarIndexInRoom(UINT uLogoID,__int64 i64Bank,__int64 i64Money,long dwPoint,UINT uGameNameID,UINT uComType);

#else // AFC_FACE_DLL

extern "C" __declspec(dllexport) TCHAR * GetMoneyOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllexport) TCHAR * GetPointOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllexport) void SetGameID(int iGameID);
extern "C" __declspec(dllexport) TCHAR * GetMatchOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllexport) TCHAR * GetNormalOrderName(__int64 dwPoint,UINT uGameNameID);
extern "C" __declspec(dllexport) int GetNormalOrderBasePoint(__int64 dwPoint);
extern "C" __declspec(dllexport) int GetNormalOrderLevel(__int64 dwPoint);
extern "C" __declspec(dllexport) int  GetMoneyGrade(__int64 dwPoint);
extern "C" __declspec(dllexport) TCHAR * GetCharmGrade(__int64 dwCharm);
extern "C" __declspec(dllexport) TCHAR *GetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace);

extern "C" __declspec(dllimport) UINT GetAvatarIndexInMain(UINT uLogoID,__int64 i64Bank,__int64 i64Money);
extern "C" __declspec(dllimport) UINT GetAvatarIndexInRoom(UINT uLogoID,__int64 i64Bank,__int64 i64Money,long dwPoint,UINT uGameNameID,UINT uComType);

#endif // AFC_FACE_DLL