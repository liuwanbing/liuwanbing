#ifndef __SO_MAIN_FACTORY_H__
#define __SO_MAIN_FACTORY_H__

#include "IMainGameFactory_i.h"

#include <map>
using namespace std;
using namespace MyGame;

/*后面的 FactoryType = int 只是为了把他做成了模板吧*/
template<typename FactoryType = int>
class SoMainGameFactory : public IMainGameFactory
{
typedef map<UINT,  int*> GAME_OBJECT_LIST_MAP;
typedef GAME_OBJECT_LIST_MAP::iterator GAME_OBJECT_LIST_ITERATOR;
     GAME_OBJECT_LIST_MAP     m_GameObjetMap;

public:
    SoMainGameFactory(void)
    {
        
    }
    
    ~SoMainGameFactory(void)
    {
    
    }
    
    BOOL RegisterInterface(int nModuleID, IGameObject* pModule)
    {
         int* dwHanderAddress = reinterpret_cast<int*>(pModule);
         m_GameObjetMap.insert(make_pair(nModuleID, dwHanderAddress));
         return TRUE;
    }
    
    BOOL UnRegisterInterface(int nModuleID)
    {
        GAME_OBJECT_LIST_ITERATOR it = m_GameObjetMap.find(nModuleID);
 
        if (it != m_GameObjetMap.end())
        {
            m_GameObjetMap.erase(it);
            return TRUE;
        }
        return FALSE;
    }
    
    IGameObject* QueryInterface(int nModuleID)
    {
        GAME_OBJECT_LIST_ITERATOR it = m_GameObjetMap.find(nModuleID);

        if (it ==  m_GameObjetMap.end())
        {
            return NULL;
        }
        
        IGameObject* pGameObject = reinterpret_cast<IGameObject*>(it->second); 
        return pGameObject;
    }
};


#endif
