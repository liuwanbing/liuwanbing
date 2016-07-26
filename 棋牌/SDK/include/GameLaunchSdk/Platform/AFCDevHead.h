#pragma once

//消息头文件
#include "../../common/basemessage.h"
#include "../../common/GameRoomMessage.h"
#include "../../common/commonuse.h"
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
#include "WndAnimate.h"
#include "AFCFunction.h"
#include "AFCResource.h"
#include "AFCButton.h"
#include "AFCDialog.h"
#include "AFCListCtrl.h"
//#include "AFCTabCtrl.h"
#include "AFCWindow.h"
#include "AFCLogoResource.h"
#include "AFCMenu.h"

//#include "TCPClientSocket.h"
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
//#include "GameFrameDlg.h"
#include "GetPoint.h"
//#include "BroadcastMsgInputDlg.h"
//#include "ShortMessage.h"
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

//获取等级函数声明
extern EXT_CLASS int GetMoneyOrderLevel(__int64 dwPoint);
extern EXT_CLASS TCHAR * GetMoneyOrderName(__int64 dwPoint);
extern EXT_CLASS TCHAR * GetPointOrderName(__int64 dwPoint);
extern EXT_CLASS TCHAR * GetMatchOrderName(__int64 dwPoint);
extern EXT_CLASS TCHAR * GetNormalOrderName(__int64 dwPoint);
extern EXT_CLASS int GetNormalOrderBasePoint(__int64 dwPoint);
extern EXT_CLASS int GetNormalOrderLevel(__int64 dwPoint);
extern EXT_CLASS int  GetMoneyGrade(__int64 dwPoint);
//魅力级别
extern EXT_CLASS TCHAR * GetCharmGrade(__int64 dwCharm);


///获取用户的形象图片索引(游戏房间使用)
/// @param[in] uLogoID     用户的LogoID;
/// @param[in] i64Bank     用户存款;
/// @param[in] i64Money    用户身上金币;
/// @param[in] dwPoint     用户积分;
/// @param[in] uGameNameID 游戏ID;
/// @param[in] uComType    房间类型，1-为积分场，3-为金币场
/// @return 返回形象图片索引
UINT GetAvatarIndex(UINT uLogoID,__int64 i64Bank,__int64 i64Money,long dwPoint,UINT uGameNameID,UINT uComType);

///是否VIP的有效时间
/// @param[in] lUserVipTime    VIP的有效时间;
/// @return 返回lUserVipTime是否到期
bool IsVipEffectTime(long lUserVipTime);
