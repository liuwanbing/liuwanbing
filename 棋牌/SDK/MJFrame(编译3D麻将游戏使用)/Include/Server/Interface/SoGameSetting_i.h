/*
    这里会有下面几种方式去使用此接口

    m_pGameSetting->GetItemValue("CardsFrame/bTileConfig", m_bTileConfig, 0);
	m_pGameSetting->GetItemValue("nGiveCardTime", m_stSettingData.nGiveCardTime, 15);
	m_pGameSetting->GetItemValue("test2", temp, sizeof(temp), "dddddddd");

    m_pGameSetting->GetItemValue("sjdc/", val, 0);
	m_pGameSetting->GetItemValue("nTrusteeTime", m_stSettingData.nTrusteeTime, 3);
	m_pGameSetting->GetItemValue("nBasePoint", m_stSettingData.nBasePoint, 1);

    m_pGameSetting->GetItemValue("Normal/", val, 0);

 */
#if !defined(SO_GAME_SETTING_I_H)
#define SO_GAME_SETTING_I_H

#include "Svrdef.h"
#include "gameevent_i.h"

namespace MyGame
{
	class ISoGameSetting :public IGameObject,public IEventProducer
	{
	public:
		//游戏Logic调用
		//直接传入配置文件的名字"zhaocha.cfg"，内部自动带上相对路径  (可不填后缀，默认后缀为.cfg格式)
		//例如：  "../config/zhaocha.cfg"
		virtual BOOL SetCfgPath(char *lpszCfgPath) = 0;

		// lpszName为NULL时，根据服务器的配置项读取 (请使用all/LogFlag)
		// 如果是要通过server的配置项的(如free/RoomName，free为房间配置项)，直接free)
		virtual BOOL GetItemValue(char *lpszName, int &nValue, int nDefaultValue) = 0;

        virtual BOOL GetItemValue(char *lpszName, char &nValue, int nDefaultValue) = 0;
		// lpszName为NULL时，根据服务器的配置项读取
		virtual BOOL GetItemValue(char *lpszName, char *lpszReturnedString, int nSize, char* pszDefaultValue) = 0;
	};

	class ISoGameSettingEvent : public IEvent
	{
	public:
		virtual int OnLoadCfg() = 0;
		virtual int OnReloadCfg() = 0;
	};
}
#endif

