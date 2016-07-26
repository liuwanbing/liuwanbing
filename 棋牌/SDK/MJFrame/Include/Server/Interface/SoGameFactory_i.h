#ifndef _SOGAMEFACTORY_I_H
#define _SOGAMEFACTORY_I_H

#include "Svrdef.h"
#include "gameobject_i.h"

namespace MyGame
{
	
	class ISoGameFactory
	{
	public:
		
		virtual BOOL RegisterInterface(int nModuleID, IGameObject* pModule) = 0;
		virtual BOOL UnRegisterInterface(int nModuleID) = 0;

		virtual IGameObject* QueryInterface(int nModuleID) = 0;


	};
}
#endif
