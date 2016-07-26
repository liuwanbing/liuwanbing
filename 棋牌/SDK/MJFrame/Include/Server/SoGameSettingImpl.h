#ifndef __SO_GAME_SETTING_H__
#define __SO_GAME_SETTING_H__

#include "afxwin.h"
#include "typeDef.h"
#include "SoGameSetting_i.h"
#include "SoFireEvent_t.h"

namespace MyGame
{
class CDeskLogicImpl;
class SoGameSettingImpl :public TSoEventProducerImpl<ISoGameSetting, 1> 
{
private:
        CDeskLogicImpl*     m_pDeskLogicImpl;
        char               m_SectName[32];
        BOOL               m_bSectBeable;
public:
        SoGameSettingImpl(CDeskLogicImpl* pDeskLogicImpl);
        ~SoGameSettingImpl(void);

        //游戏Logic调用
		//直接传入配置文件的名字"zhaocha.cfg"，内部自动带上相对路径  (可不填后缀，默认后缀为.cfg格式)
		//例如：  "../config/zhaocha.cfg"
		virtual BOOL SetCfgPath(char *lpszCfgPath);

		// lpszSectionName为NULL时，根据服务器的配置项读取 (请使用all/LogFlag；
		//如果是要通过server的配置项的(如free/RoomName，free为房间配置项)，直接free)
		virtual BOOL GetItemValue(char *lpszName, int &nValue, int nDefaultValue);
        virtual BOOL GetItemValue(char *lpszName, char &nValue, int nDefaultValue);
		// lpszSectionName为NULL时，根据服务器的配置项读取
		virtual BOOL GetItemValue(char *lpszName,  char *lpszReturnedString, int nSize, char* pszDefaultValue);

private:
        BOOL PaserSectName(char* lpszName, char* pSectName, char* pValueName);
};

};

#endif
