/* 
   实现：
	关于服务器的配置文件，只需要实现下面3个步骤即可
	首先，m_pGameSetting->SetCfgPath("../config/dgz.cfg");  设置服务器的配置文件路径
	然后，游戏所需要的配置 在XSETTING_DATA结构体中定义
	最后，在“void LoadCfg();”中利用sdk提供的读取配置的接口来读取即可，无需关心什么时候load与reload

   使用：
	程序中在SoCardsGameLogic中就可以利用 m_xGameSetting.GetSettingData(XSETTING_DATA)来获取配置内容了

注：
	1、是否使用配牌器  有一配置说明： [CardsFrame] 中的 bTileConfig （0：否 1：是）
	2、配置中有些是游戏房间规则性的，需要在CSoCardsGameLogic::OnCollectBaseMsg传送到客户端
 */
//////////////////////////////////////////////////////////////////////
#ifndef __SOMJGAME_SETTING_H____
#define __SOMJGAME_SETTING_H____

#include "gameobject_i.h"
#include "SoModuleimpl_t.h"
#include "gameeventimpl_t.h"

#include "SoGameSetting_i.h"
#include "SoGameProcessor_i.h"
#include "SoGameLogger_i.h"

#include "SoBaseLogic.h"
#include "XSoGameSetting_i.h"

#include "mjcfgdef.h"

using namespace MyGame;

namespace MyGame
{

typedef TModuleImpl<IXSoGameSetting> CMJSoGameSettingImp;

class CSoMJFrameSetting : 
			public CMJSoGameSettingImp,
			public TEventImpl<ISoGameSettingEvent>
{
public:
	CSoMJFrameSetting ();
	virtual ~CSoMJFrameSetting ();

	virtual void Initialize(CSoBaseLogic* pSoGameLogic, int nGameID);

	// ISoGameSettingEvent
	virtual int  OnLoadCfg();
	virtual int  OnReloadCfg();

	// IXSoGameSetting
	virtual void GetSettingData(XSETTING_DATA &rstSettingData) {rstSettingData = m_stSettingData;}

	virtual int  GetCoolTime(){return m_stSettingData.nCoolTime;}

	virtual void GetMJSettingData( MJ_SETTING_DATA &tagMjSettingData);

	virtual void LoadCfg();

protected:
	ISoGameLogger    *m_pSoGameLogger;
	ISoGameSetting   *m_pGameSetting;

	// 房间配置项
	XSETTING_DATA     m_stSettingData;
	MJ_SETTING_DATA	  m_tagMJSettingData;
 	BOOL			  m_bTileConfig;

    // 游戏ID与房间ID
    int               m_nGameID;
    int               m_nRoomID;
};

};


#endif 



