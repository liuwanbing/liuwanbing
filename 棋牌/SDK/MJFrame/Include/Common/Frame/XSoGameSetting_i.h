#ifndef __XSOGAMESETTING_H__
#define __XSOGAMESETTING_H__

#include "gameobject_i.h"

using namespace MyGame;

namespace MyGame
{

typedef struct tagXSettingData
{
	int nCoolTime;
	
}XSETTING_DATA;

class IXSoGameSetting :public IGameObject
{
public:
	virtual void GetSettingData(XSETTING_DATA &rstSettingData) = 0;
	virtual int  GetCoolTime() = 0;
};

}
#endif
