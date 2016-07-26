#ifndef __SO_GAME_FACTORY_H__
#define __SO_GAME_FACTORY_H__

#include "afxwin.h"
#include "SoGameFactory_i.h"

#define MAX_SO_GAME_INTERFACE  20

/*这个工厂是给写逻辑的用户使用的。*/
namespace MyGame
{
	class SoGameFactory : public ISoGameFactory
	{
        struct  FACTORY_ITEM
        {
            int          m_itemId;
            IGameObject* m_OjectPtr;
        };
	public:
		SoGameFactory(void);
		~SoGameFactory(void);

        static SoGameFactory* GetInstance(void);
		BOOL RegisterInterface(int nModuleID, IGameObject* pModule) ;
		BOOL UnRegisterInterface(int nModuleID);
		IGameObject* QueryInterface(int nModuleID);

    private:
        FACTORY_ITEM    m_Arrary[MAX_SO_GAME_INTERFACE];
	};

};

#endif